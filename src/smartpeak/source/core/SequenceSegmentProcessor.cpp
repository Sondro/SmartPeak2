// TODO: Add copyright

#include <SmartPeak/core/SequenceSegmentProcessor.h>
#include <SmartPeak/core/Filenames.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/Utilities.h>
#include <OpenMS/ANALYSIS/QUANTITATION/AbsoluteQuantitation.h>
#include <OpenMS/METADATA/AbsoluteQuantitationStandards.h>
#include <OpenMS/ANALYSIS/OPENSWATH/MRMFeatureFilter.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationStandardsFile.h>
#include <OpenMS/FORMAT/AbsoluteQuantitationMethodFile.h>
#include <OpenMS/FORMAT/MRMFeatureQCFile.h>  // load featureFilter and featureQC
#include <plog/Log.h>

namespace SmartPeak
{
  void SequenceSegmentProcessor::getSampleIndicesBySampleType(
    const SequenceSegmentHandler& sequenceSegmentHandler,
    const SequenceHandler& sequenceHandler,
    const SampleType sampleType,
    std::vector<size_t>& sampleIndices
  )
  {
    sampleIndices.clear();
    for (const size_t index : sequenceSegmentHandler.getSampleIndices()) {
      if (sequenceHandler.getSequence().at(index).getMetaData().getSampleType() == sampleType) {
        sampleIndices.push_back(index);
      }
    }
  }

  void CalculateCalibration::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START optimizeCalibrationCurves";

    std::vector<size_t> standards_indices;
    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      standards_indices
    );

    // check if there are any standards to calculate the calibrators from
    if (standards_indices.empty()) {
      LOGE << "standards_indices argument is empty. Returning";
      LOGD << "END optimizeCalibrationCurves";
      return;
    }

    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    if (params_I.at("AbsoluteQuantitation").empty()) {
      LOGE << "Parameters not found for AbsoluteQuantitation. Returning";
      LOGD << "END optimizeCalibrationCurves";
      return;
    }

    // add in the method parameters
    OpenMS::AbsoluteQuantitation absoluteQuantitation;
    OpenMS::Param parameters = absoluteQuantitation.getParameters();
    Utilities::updateParameters(parameters, params_I.at("AbsoluteQuantitation"));
    absoluteQuantitation.setParameters(parameters);

    absoluteQuantitation.setQuantMethods(sequenceSegmentHandler_IO.getQuantitationMethods());
    std::map<std::string, std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration>> components_to_concentrations;
    for (const OpenMS::AbsoluteQuantitationMethod& row : sequenceSegmentHandler_IO.getQuantitationMethods()) {
      // map standards to features
      OpenMS::AbsoluteQuantitationStandards absoluteQuantitationStandards;
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations;

      absoluteQuantitationStandards.getComponentFeatureConcentrations(
        sequenceSegmentHandler_IO.getStandardsConcentrations(),
        standards_featureMaps,
        row.getComponentName(),
        feature_concentrations
      );
      // remove features with an actual concentration of 0.0 or less
      std::vector<OpenMS::AbsoluteQuantitationStandards::featureConcentration> feature_concentrations_pruned;
      for (const OpenMS::AbsoluteQuantitationStandards::featureConcentration& feature : feature_concentrations) {
        if (feature.actual_concentration > 0.0) {
          feature_concentrations_pruned.push_back(feature);
        }
      }

      // remove components without any points
      if (feature_concentrations_pruned.empty()) {
        continue;
      }

      try
      {
        absoluteQuantitation.optimizeSingleCalibrationCurve(
          row.getComponentName(),
          feature_concentrations_pruned
        );
      }
      catch (OpenMS::Exception::DivisionByZero& )
      {
        LOGW << "Warning: '" << row.getComponentName() << "' cannot be analysed - division by zero\n";
        continue;
      }
      // find the optimal calibration curve for each component

      components_to_concentrations.erase(row.getComponentName());
      components_to_concentrations.insert({row.getComponentName(), feature_concentrations_pruned});
    }
    // store results
    sequenceSegmentHandler_IO.setComponentsToConcentrations(components_to_concentrations);
    sequenceSegmentHandler_IO.getQuantitationMethods() = absoluteQuantitation.getQuantMethods();
    //sequenceSegmentHandler_IO.setQuantitationMethods(absoluteQuantitation.getQuantMethods());
    LOGD << "END optimizeCalibrationCurves";
  }

  void LoadStandardsConcentrations::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadStandardsConcentrations";
    LOGI << "Loading: " << filenames.standardsConcentrations_csv_i;

    if (filenames.standardsConcentrations_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.standardsConcentrations_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadStandardsConcentrations";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationStandardsFile AQSf;
      AQSf.load(filenames.standardsConcentrations_csv_i, sequenceSegmentHandler_IO.getStandardsConcentrations());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getStandardsConcentrations().clear();
      LOGI << "Standards concentrations clear";
    }

    LOGD << "END loadStandardsConcentrations";
  }

  void LoadQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadQuantitationMethods";
    LOGI << "Loading: " << filenames.quantitationMethods_csv_i;

    if (filenames.quantitationMethods_csv_i.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    if (!InputDataValidation::fileExists(filenames.quantitationMethods_csv_i)) {
      LOGE << "File not found";
      LOGD << "END loadQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile AQMf;
      AQMf.load(filenames.quantitationMethods_csv_i, sequenceSegmentHandler_IO.getQuantitationMethods());
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getQuantitationMethods().clear();
      LOGI << "quantitation methods clear";
    }

    LOGD << "END loadQuantitationMethods";
  }

  void StoreQuantitationMethods::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeQuantitationMethods";
    LOGI << "Storing: " << filenames.quantitationMethods_csv_o;

    if (filenames.quantitationMethods_csv_o.empty()) {
      LOGE << "Filename is empty";
      LOGD << "END storeQuantitationMethods";
      return;
    }

    try {
      OpenMS::AbsoluteQuantitationMethodFile aqmf;
      aqmf.store(
        filenames.quantitationMethods_csv_o,
        sequenceSegmentHandler_IO.getQuantitationMethods()
      );
    }
    catch (const std::exception& e) {
      LOGE << e.what();
    }

    LOGD << "END storeQuantitationMethods";
  }

  void LoadFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureFilter";
    LOGI << "Loading: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
      filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponents_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    if (filenames.featureFilterComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureFilterComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureFilterComponentGroups_csv_i;
      LOGD << "END loadFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureFilter";
  }

  void LoadFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
      filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponents_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    if (filenames.featureQCComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureQCComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureQCComponentGroups_csv_i;
      LOGD << "END loadFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }

    LOGD << "END loadFeatureQC";
  }

  void StoreFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureFilter";
    LOGI << "Storing: " << filenames.featureFilterComponents_csv_i << " and " <<
      filenames.featureFilterComponentGroups_csv_i;

    if (filenames.featureFilterComponents_csv_i.empty() &&
      filenames.featureFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), false);
      }
      if (filenames.featureFilterComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureFilter";
  }

  void StoreFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureQC";
    LOGI << "Loading: " << filenames.featureQCComponents_csv_i << " and " <<
      filenames.featureQCComponentGroups_csv_i;

    if (filenames.featureQCComponents_csv_i.empty() &&
      filenames.featureQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), false);
      }
      if (filenames.featureQCComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureQC";
  }

  void LoadFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureRSDFilter";
    LOGI << "Loading: " << filenames.featureRSDFilterComponents_csv_i << " and " <<
      filenames.featureRSDFilterComponentGroups_csv_i;

    if (filenames.featureRSDFilterComponents_csv_i.empty() &&
      filenames.featureRSDFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    if (filenames.featureRSDFilterComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureRSDFilterComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureRSDFilterComponents_csv_i;
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    if (filenames.featureRSDFilterComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureRSDFilterComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureRSDFilterComponentGroups_csv_i;
      LOGD << "END loadFeatureRSDFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureRSDFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureRSDFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureRSDFilter(), false);
      }
      if (filenames.featureRSDFilterComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureRSDFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureRSDFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureRSDFilter";
  }

  void LoadFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureRSDQC";
    LOGI << "Loading: " << filenames.featureRSDQCComponents_csv_i << " and " <<
      filenames.featureRSDQCComponentGroups_csv_i;

    if (filenames.featureRSDQCComponents_csv_i.empty() &&
      filenames.featureRSDQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    if (filenames.featureRSDQCComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureRSDQCComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureRSDQCComponents_csv_i;
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    if (filenames.featureRSDQCComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureRSDQCComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureRSDQCComponentGroups_csv_i;
      LOGD << "END loadFeatureRSDQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureRSDQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureRSDQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureRSDQC(), false);
      }
      if (filenames.featureRSDQCComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureRSDQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureRSDQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureRSDQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }

    LOGD << "END loadFeatureRSDQC";
  }

  void StoreFeatureRSDFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureRSDFilter";
    LOGI << "Storing: " << filenames.featureRSDFilterComponents_csv_i << " and " <<
      filenames.featureRSDFilterComponentGroups_csv_i;

    if (filenames.featureRSDFilterComponents_csv_i.empty() &&
      filenames.featureRSDFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureRSDFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureRSDFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureRSDFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureRSDFilter(), false);
      }
      if (filenames.featureRSDFilterComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureRSDFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureRSDFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureRSDFilter";
  }

  void StoreFeatureRSDQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureRSDQC";
    LOGI << "Loading: " << filenames.featureRSDQCComponents_csv_i << " and " <<
      filenames.featureRSDQCComponentGroups_csv_i;

    if (filenames.featureRSDQCComponents_csv_i.empty() &&
      filenames.featureRSDQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureRSDQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureRSDQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureRSDQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureRSDQC(), false);
      }
      if (filenames.featureRSDQCComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureRSDQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureRSDQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureRSDQC";
  }

  void LoadFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureBackgroundFilter";
    LOGI << "Loading: " << filenames.featureBackgroundFilterComponents_csv_i << " and " <<
      filenames.featureBackgroundFilterComponentGroups_csv_i;

    if (filenames.featureBackgroundFilterComponents_csv_i.empty() &&
      filenames.featureBackgroundFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    if (filenames.featureBackgroundFilterComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureBackgroundFilterComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureBackgroundFilterComponents_csv_i;
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    if (filenames.featureBackgroundFilterComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureBackgroundFilterComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureBackgroundFilterComponentGroups_csv_i;
      LOGD << "END loadFeatureBackgroundFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureBackgroundFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureBackgroundFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), false);
      }
      if (filenames.featureBackgroundFilterComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureBackgroundFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundFilter().component_group_pair_qcs.clear();
      LOGI << "feature filter clear";
    }

    LOGD << "END loadFeatureBackgroundFilter";
  }

  void LoadFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START loadFeatureBackgroundQC";
    LOGI << "Loading: " << filenames.featureBackgroundQCComponents_csv_i << " and " <<
      filenames.featureBackgroundQCComponentGroups_csv_i;

    if (filenames.featureBackgroundQCComponents_csv_i.empty() &&
      filenames.featureBackgroundQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    if (filenames.featureBackgroundQCComponents_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureBackgroundQCComponents_csv_i)) {
      LOGE << "File not found: " << filenames.featureBackgroundQCComponents_csv_i;
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    if (filenames.featureBackgroundQCComponentGroups_csv_i.size() &&
      !InputDataValidation::fileExists(filenames.featureBackgroundQCComponentGroups_csv_i)) {
      LOGE << "File not found: " << filenames.featureBackgroundQCComponentGroups_csv_i;
      LOGD << "END loadFeatureBackgroundQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureBackgroundQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.load(filenames.featureBackgroundQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundQC(), false);
      }
      if (filenames.featureBackgroundQCComponentGroups_csv_i.size()) {
        featureQCFile.load(filenames.featureBackgroundQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_group_qcs.clear();
      sequenceSegmentHandler_IO.getFeatureBackgroundQC().component_group_pair_qcs.clear();
      LOGI << "Feature qc clear";
    }

    LOGD << "END loadFeatureBackgroundQC";
  }

  void StoreFeatureBackgroundFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureBackgroundFilter";
    LOGI << "Storing: " << filenames.featureBackgroundFilterComponents_csv_i << " and " <<
      filenames.featureBackgroundFilterComponentGroups_csv_i;

    if (filenames.featureBackgroundFilterComponents_csv_i.empty() &&
      filenames.featureBackgroundFilterComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureBackgroundFilter";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureBackgroundFilterComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureBackgroundFilterComponents_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), false);
      }
      if (filenames.featureBackgroundFilterComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureBackgroundFilterComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundFilter(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "feature filter store exception";
    }

    LOGD << "END storeFeatureBackgroundFilter";
  }

  void StoreFeatureBackgroundQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START storeFeatureBackgroundQC";
    LOGI << "Loading: " << filenames.featureBackgroundQCComponents_csv_i << " and " <<
      filenames.featureBackgroundQCComponentGroups_csv_i;

    if (filenames.featureBackgroundQCComponents_csv_i.empty() &&
      filenames.featureBackgroundQCComponentGroups_csv_i.empty()) {
      LOGE << "Filenames are both empty";
      LOGD << "END storeFeatureBackgroundQC";
      return;
    }

    try {
      OpenMS::MRMFeatureQCFile featureQCFile;
      if (filenames.featureBackgroundQCComponents_csv_i.size()) { // because we don't know if either of the two names is empty
        featureQCFile.store(filenames.featureBackgroundQCComponents_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundQC(), false);
      }
      if (filenames.featureBackgroundQCComponentGroups_csv_i.size()) {
        featureQCFile.store(filenames.featureBackgroundQCComponentGroups_csv_i, sequenceSegmentHandler_IO.getFeatureBackgroundQC(), true);
      }
    }
    catch (const std::exception& e) {
      LOGE << e.what();
      LOGI << "Feature qc store exception";
    }

    LOGD << "END storeFeatureBackgroundQC";
  }

  void EstimateFeatureFilterValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START estimateFeatureFilterValues";

    std::vector<size_t> standards_indices, qcs_indices;

    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      standards_indices
    );

    // get all QCs
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (standards_indices.empty() && qcs_indices.empty()) {
      LOGE << "standards_indices and/or qcs_indices argument is empty. Returning";
      LOGD << "END estimateFeatureFilterValues";
      return;
    }

    // OPTIMIZATION: it would be prefered to only use those standards that are part of the optimized calibration curve for each component
    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }
    for (const size_t index : qcs_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.EstimateDefaultMRMFeatureQCValues(
      standards_featureMaps,
      sequenceSegmentHandler_IO.getFeatureFilter(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment(), // Targeted experiment used by all injections in the sequence
      true
    );

    LOGD << "END estimateFeatureFilterValues";
  }

  void EstimateFeatureQCValues::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START estimateFeatureQCValues";

    std::vector<size_t> standards_indices, qcs_indices;

    // get all standards
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Standard,
      standards_indices
    );

    // get all QCs
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (standards_indices.empty() && qcs_indices.empty()) {
      LOGE << "standards_indices and/or qcs_indices argument is empty. Returning";
      LOGD << "END estimateFeatureQCValues";
      return;
    }

    // OPTIMIZATION: it would be prefered to only use those standards that are part of the optimized calibration curve for each component
    std::vector<OpenMS::FeatureMap> standards_featureMaps;
    for (const size_t index : standards_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }
    for (const size_t index : qcs_indices) {
      standards_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.EstimateDefaultMRMFeatureQCValues(
      standards_featureMaps,
      sequenceSegmentHandler_IO.getFeatureQC(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment(), // Targeted experiment used by all injections in the sequence
      true
    );

    LOGD << "END estimateFeatureQCValues";
  }

  void TransferLOQToFeatureFilters::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START TransferLOQToFeatureFilters";

    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      LOGE << "quantitation methods is empty. Returning";
      LOGD << "END TransferLOQToFeatureFilters";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureFilter()
    );

    LOGD << "END TransferLOQToFeatureFilters";
  }

  void TransferLOQToFeatureQCs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START TransferLOQToFeatureQCs";

    // check if there are any quantitation methods
    if (sequenceSegmentHandler_IO.getQuantitationMethods().empty()) {
      LOGE << "quantitation methods is empty. Returning";
      LOGD << "END TransferLOQToFeatureQCs";
      return;
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.TransferLLOQAndULOQToCalculatedConcentrationBounds(
      sequenceSegmentHandler_IO.getQuantitationMethods(),
      sequenceSegmentHandler_IO.getFeatureQC()
    );

    LOGD << "END TransferLOQToFeatureQCs";
  }
  void EstimateFeatureRSDs::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START EstimateFeatureRSDs";

    // get all QCs
    std::vector<size_t> qcs_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::QC,
      qcs_indices
    );

    // check if there are any standards or QCs to estimate the feature filter parameters from
    if (qcs_indices.empty()) {
      LOGE << "qcs_indices argument is empty. Returning";
      LOGD << "END EstimateFeatureRSDs";
      return;
    }

    std::vector<OpenMS::FeatureMap> qcs_featureMaps;
    for (const size_t index : qcs_indices) {
      qcs_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.EstimatePercRSD(
      qcs_featureMaps, 
      sequenceSegmentHandler_IO.getFeatureRSDEstimations(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment() // Targeted experiment used by all injections in the sequence
    );

    LOGD << "END EstimateFeatureRSDs";
  }

  void EstimateFeatureBackgroundInterferences::process(
    SequenceSegmentHandler& sequenceSegmentHandler_IO,
    const SequenceHandler& sequenceHandler_I,
    const std::map<std::string, std::vector<std::map<std::string, std::string>>>& params_I,
    const Filenames& filenames
  ) const
  {
    LOGD << "START EstimateFeatureBackgroundInterferences";

    // get all Blanks
    std::vector<size_t> blanks_indices;
    this->getSampleIndicesBySampleType(
      sequenceSegmentHandler_IO,
      sequenceHandler_I,
      SampleType::Blank,
      blanks_indices
    );

    // check if there are any Blanks to estimate the background interference from
    if (blanks_indices.empty()) {
      LOGE << "blanks_indices argument is empty. Returning";
      LOGD << "END EstimateFeatureBackgroundInterferences";
      return;
    }

    std::vector<OpenMS::FeatureMap> blanks_featureMaps;
    for (const size_t index : blanks_indices) {
      blanks_featureMaps.push_back(sequenceHandler_I.getSequence().at(index).getRawData().getFeatureMap());
    }

    // Initialize with a zero filter
    OpenMS::MRMFeatureFilter featureFilter;
    featureFilter.zeroFilterValues(sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(), sequenceSegmentHandler_IO.getFeatureBackgroundEstimations());

    // Then estimate the background interferences
    featureFilter.EstimateBackgroundInterferences(
      blanks_featureMaps,
      sequenceSegmentHandler_IO.getFeatureBackgroundEstimations(),
      sequenceHandler_I.getSequence().front().getRawData().getTargetedExperiment() // Targeted experiment used by all injections in the sequence
    );

    LOGD << "END EstimateFeatureBackgroundInterferences";
  }
}
