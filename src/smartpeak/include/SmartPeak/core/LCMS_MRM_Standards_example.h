#include <SmartPeak/core/SequenceHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>
#include <SmartPeak/io/SequenceParser.h>

using namespace SmartPeak;

void example_LCMS_MRM_Standards(
  const std::string& dir_I,
  const std::string& delimiter_I = ",",
  const bool verbose_I = false
)
{
  SequenceHandler sequenceHandler;

  sequenceHandler.setDirStatic(dir_I);
  sequenceHandler.setDirDynamic(dir_I);

  SequenceProcessor::createSequence(sequenceHandler, delimiter_I);

  std::vector<std::string> raw_data_processing_methods = {
    "load_raw_data",
    // # "load_features",
    "pick_features",
    "filter_features",
    "filter_features",
    "select_features",
    "check_features",
    "store_features",
    // # "plot_features"
  };

  SequenceProcessor::processSequence(
    sequenceHandler,
    std::vector<std::string>(),
    raw_data_processing_methods,
    true
  );

  const std::vector<std::string> sequence_segment_processing_methods = {
    "calculate_calibration",
  // "plot_calibrators",
    "store_quantitation_methods",
  // # "load_quantitation_methods",
  // # "store_components_to_concentrations"
  };

  SequenceProcessor::processSequenceSegments(
    sequenceHandler,
    std::set<std::string>(),
    sequence_segment_processing_methods,
    true
  );

  raw_data_processing_methods = {
    "quantify_features",
    "check_features",
    "store_features",
    // # "plot_features"
  };

  SequenceProcessor::processSequence(
    sequenceHandler,
    std::vector<std::string>(),
    raw_data_processing_methods,
    true
  );

  const std::string sequenceSummary_csv_i = dir_I + "/SequenceSummary.csv";

  SequenceParser::writeDataMatrixFromMetaValue(
    sequenceHandler,
    sequenceSummary_csv_i,
    {"calculated_concentration"},
    {MetaDataHandler::SampleType::Standard}
  );

  const std::string featureSummary_csv_i = dir_I + "/FeatureSummary.csv";

  SequenceParser::writeDataTableFromMetaValue(
    sequenceHandler,
    featureSummary_csv_i,
    {
      "peak_apex_int", "total_width", "width_at_50", "tailing_factor", "asymmetry_factor",
      "baseline_delta_2_height", "points_across_baseline", "points_across_half_height",
      "logSN", "calculated_concentration", "QC_transition_message", "QC_transition_pass",
      "QC_transition_score", "QC_transition_group_message", "QC_transition_group_score"
    },
    {MetaDataHandler::SampleType::Standard}
  );
}