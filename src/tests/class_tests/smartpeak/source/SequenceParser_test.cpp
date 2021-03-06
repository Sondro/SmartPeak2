// TODO: Add copyright

#include <SmartPeak/test_config.h>

#define BOOST_TEST_MODULE SequenceParser test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/MetaDataHandler.h>
#include <SmartPeak/core/RawDataProcessor.h>
#include <SmartPeak/core/SampleType.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(sequenceparser)

BOOST_AUTO_TEST_CASE(readSequenceFile)
{
  string pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1.csv");
  SequenceHandler sequenceHandler;
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ",");
  const vector<InjectionHandler>& sequence1 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence1.size(), 4);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc2_1x");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence1[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().inj_number, 2);
  BOOST_CHECK_EQUAL(sequence1[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence1[3].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence1[3].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_semicolon.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, ";");
  const vector<InjectionHandler>& sequence2 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence2.size(), 5);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence2[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().inj_number, 1);
  BOOST_CHECK_EQUAL(sequence2[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence2[4].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence2[4].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");

  sequenceHandler.clear();
  pathname = SMARTPEAK_GET_TEST_DATA_PATH("SequenceParser_sequence_1_tab.csv");
  SequenceParser::readSequenceFile(sequenceHandler, pathname, "\t");
  const vector<InjectionHandler>& sequence3 = sequenceHandler.getSequence();
  BOOST_CHECK_EQUAL(sequence3.size(), 5);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleName(), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence3[0].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().inj_number, 1);
  BOOST_CHECK_EQUAL(sequence3[0].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleName(), "170808_Jonathan_yeast_Yarr2_1x");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleGroupName(), "Test01");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSequenceSegmentName(), "Group01");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getSampleType() == SampleType::Unknown, true);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getFilename(), "/home/user/code/test/data/mzML/170808_Jonathan_yeast_Sacc1_1x.mzML");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().acq_method_name, "LCMS");
  BOOST_CHECK_CLOSE(sequence3[4].getMetaData().inj_volume, 10.0, 1e-3);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().inj_volume_units, "uL");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().batch_name, "FluxTest");
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().inj_number, 5);
  BOOST_CHECK_EQUAL(sequence3[4].getMetaData().getAcquisitionDateAndTimeAsString(), "2015-07-07_153300");
}

BOOST_AUTO_TEST_CASE(makeDataTableFromMetaValue)
{
  SequenceHandler sequenceHandler;

  const vector<string> sample_names = {
    "170808_Jonathan_yeast_Sacc1_1x",
    "170808_Jonathan_yeast_Sacc2_1x",
    "170808_Jonathan_yeast_Sacc3_1x",
    "170808_Jonathan_yeast_Yarr1_1x",
    "170808_Jonathan_yeast_Yarr2_1x",
    "170808_Jonathan_yeast_Yarr3_1x"
  };

  for (const string& sample_name : sample_names) {
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.acq_method_name = "6";
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "9";

    Filenames filenames;
    filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(sample_name + ".featureXML");
    RawDataHandler rawDataHandler;
    LoadFeatures loadFeatures;
    loadFeatures.process(rawDataHandler, {}, filenames);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  vector<vector<string>> data_out;
  vector<string> headers_out;
  const vector<string> meta_data {
    "peak_apex_int",
    "logSN",
    "QC_transition_message",
    "QC_transition_group_message",
    "leftWidth",
    "rightWidth",
  };
  const set<SampleType> sample_types = {SampleType::Unknown};

  SequenceParser::makeDataTableFromMetaValue(sequenceHandler, data_out, headers_out, meta_data, sample_types, std::set<std::string>(), std::set<std::string>());

  BOOST_CHECK_EQUAL(data_out.size(), 1657);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "23dpg");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "23dpg.23dpg_1.Heavy");
  BOOST_CHECK_EQUAL(data_out.at(0).at(19), std::to_string(235.0));
  BOOST_CHECK_EQUAL(data_out.at(0).at(20), std::to_string(3.52866193485212));
  BOOST_CHECK_EQUAL(data_out.at(0).at(23), std::to_string(15.605367));
  BOOST_CHECK_EQUAL(data_out.at(0).at(24), std::to_string(15.836817));
  BOOST_CHECK_EQUAL(headers_out.size(), 25);
  BOOST_CHECK_EQUAL(headers_out[0], "sample_name");
  BOOST_CHECK_EQUAL(headers_out[1], "sample_type");
  BOOST_CHECK_EQUAL(headers_out[2], "component_group_name");
  BOOST_CHECK_EQUAL(headers_out[3], "component_name");
  BOOST_CHECK_EQUAL(headers_out[4], "batch_name");
  BOOST_CHECK_EQUAL(headers_out[5], "rack_number");
  BOOST_CHECK_EQUAL(headers_out[6], "plate_number");
  BOOST_CHECK_EQUAL(headers_out[7], "pos_number");
  BOOST_CHECK_EQUAL(headers_out[8], "inj_number");
  BOOST_CHECK_EQUAL(headers_out[9], "dilution_factor");
  BOOST_CHECK_EQUAL(headers_out[10], "inj_volume");
  BOOST_CHECK_EQUAL(headers_out[11], "inj_volume_units");
  BOOST_CHECK_EQUAL(headers_out[12], "operator_name");
  BOOST_CHECK_EQUAL(headers_out[13], "acq_method_name");
  BOOST_CHECK_EQUAL(headers_out[14], "proc_method_name");
  BOOST_CHECK_EQUAL(headers_out[15], "original_filename");
  BOOST_CHECK_EQUAL(headers_out[16], "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(headers_out[17], "injection_name");
  BOOST_CHECK_EQUAL(headers_out[18], "used_");
  // metadata
  BOOST_CHECK_EQUAL(headers_out[19], "peak_apex_int");
  BOOST_CHECK_EQUAL(headers_out[20], "logSN");
  BOOST_CHECK_EQUAL(headers_out[21], "QC_transition_message");
  BOOST_CHECK_EQUAL(headers_out[22], "QC_transition_group_message");
  BOOST_CHECK_EQUAL(headers_out[23], "leftWidth");
  BOOST_CHECK_EQUAL(headers_out[24], "rightWidth");

  SequenceParser::makeDataTableFromMetaValue(sequenceHandler, data_out, headers_out, std::vector<std::string>({ "leftWidth" }), sample_types,
    std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }), std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  BOOST_CHECK_EQUAL(data_out.size(), 1);
  BOOST_CHECK_EQUAL(data_out.at(0).at(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(data_out.at(0).at(1), "Unknown");
  BOOST_CHECK_EQUAL(data_out.at(0).at(2), "23dpg");
  BOOST_CHECK_EQUAL(data_out.at(0).at(3), "23dpg.23dpg_1.Light");
  BOOST_CHECK_EQUAL(data_out.at(0).at(19), std::to_string(15.605367));
  BOOST_CHECK_EQUAL(headers_out.size(), 20);
  BOOST_CHECK_EQUAL(headers_out[0], "sample_name");
  BOOST_CHECK_EQUAL(headers_out[1], "sample_type");
  BOOST_CHECK_EQUAL(headers_out[2], "component_group_name");
  BOOST_CHECK_EQUAL(headers_out[3], "component_name");
  BOOST_CHECK_EQUAL(headers_out[4], "batch_name");
  BOOST_CHECK_EQUAL(headers_out[5], "rack_number");
  BOOST_CHECK_EQUAL(headers_out[6], "plate_number");
  BOOST_CHECK_EQUAL(headers_out[7], "pos_number");
  BOOST_CHECK_EQUAL(headers_out[8], "inj_number");
  BOOST_CHECK_EQUAL(headers_out[9], "dilution_factor");
  BOOST_CHECK_EQUAL(headers_out[10], "inj_volume");
  BOOST_CHECK_EQUAL(headers_out[11], "inj_volume_units");
  BOOST_CHECK_EQUAL(headers_out[12], "operator_name");
  BOOST_CHECK_EQUAL(headers_out[13], "acq_method_name");
  BOOST_CHECK_EQUAL(headers_out[14], "proc_method_name");
  BOOST_CHECK_EQUAL(headers_out[15], "original_filename");
  BOOST_CHECK_EQUAL(headers_out[16], "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(headers_out[17], "injection_name");
  BOOST_CHECK_EQUAL(headers_out[18], "used_");
  // metadata
  BOOST_CHECK_EQUAL(headers_out[19], "leftWidth");

  // write sequence to output
  // const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("output/SequenceParser_writeDataTableFromMetaValue.csv");
  // SequenceParser::writeDataTableFromMetaValue(sequenceHandler, pathname_output, meta_data, sample_types);
}

BOOST_AUTO_TEST_CASE(makeDataMatrixFromMetaValue)
{
  SequenceHandler sequenceHandler;

  const vector<string> sample_names = {
    "170808_Jonathan_yeast_Sacc1_1x",
    "170808_Jonathan_yeast_Sacc2_1x",
    "170808_Jonathan_yeast_Sacc3_1x",
    "170808_Jonathan_yeast_Yarr1_1x",
    "170808_Jonathan_yeast_Yarr2_1x",
    "170808_Jonathan_yeast_Yarr3_1x"
  };

  for (const string& sample_name : sample_names) {
    MetaDataHandler metaDataHandler;
    metaDataHandler.setSampleName(sample_name);
    metaDataHandler.setFilename(sample_name + ".mzML");
    metaDataHandler.setSampleType(SampleType::Unknown);
    metaDataHandler.setSampleGroupName("sample_group");
    metaDataHandler.setSequenceSegmentName("sequence_segment");
    metaDataHandler.acq_method_name = "6";
    metaDataHandler.inj_volume = 7.0;
    metaDataHandler.inj_volume_units = "8";
    metaDataHandler.batch_name = "9";

    Filenames filenames;
    filenames.featureXML_i = SMARTPEAK_GET_TEST_DATA_PATH(sample_name + ".featureXML");
    RawDataHandler rawDataHandler;
    LoadFeatures loadFeatures;
    loadFeatures.process(rawDataHandler, {}, filenames);

    sequenceHandler.addSampleToSequence(metaDataHandler, rawDataHandler.getFeatureMap());
  }

  Eigen::Tensor<float, 2> data_out;
  Eigen::Tensor<std::string, 1> columns_out;
  Eigen::Tensor<std::string, 2> rows_out;

  const vector<string> meta_data = {
    "calculated_concentration",
    "leftWidth",
    "rightWidth"
  };
  // const vector<string> meta_data = {"calculated_concentration"};
  const set<SampleType> sample_types = {SampleType::Unknown};

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out, meta_data, sample_types, std::set<std::string>(), std::set<std::string>());

  BOOST_CHECK_EQUAL(columns_out.size(), 6);
  BOOST_CHECK_EQUAL(columns_out(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(rows_out.dimension(0), 636);
  BOOST_CHECK_EQUAL(rows_out.dimension(1), 3);
  BOOST_CHECK_EQUAL(rows_out(0,1), "23dpg");
  BOOST_CHECK_CLOSE(data_out(0,0), 15.6053667, 1e-3);
  BOOST_CHECK_CLOSE(data_out(rows_out.dimension(0)-1,columns_out.size()-1), 1.66744995, 1e-3);

  SequenceParser::makeDataMatrixFromMetaValue(sequenceHandler, data_out, columns_out, rows_out, std::vector<std::string>({ "leftWidth" }), sample_types,
    std::set<std::string>({ "170808_Jonathan_yeast_Sacc1_1x" }), std::set<std::string>({ "23dpg.23dpg_1.Light" }));

  BOOST_CHECK_EQUAL(columns_out.size(), 1);
  BOOST_CHECK_EQUAL(columns_out(0), "170808_Jonathan_yeast_Sacc1_1x");
  BOOST_CHECK_EQUAL(rows_out.dimension(0), 1);
  BOOST_CHECK_EQUAL(rows_out.dimension(1), 3);
  BOOST_CHECK_EQUAL(rows_out(0, 1), "23dpg");
  BOOST_CHECK_CLOSE(data_out(0, 0), 15.6053667, 1e-3);

  // write sequence to output
  // const std::string pathname_output = SMARTPEAK_GET_TEST_DATA_PATH("output/SequenceParser_writeDataMatrixFromMetaValue.csv");
  // SequenceParser::writeDataMatrixFromMetaValue(sequenceHandler, pathname_output);
}

BOOST_AUTO_TEST_SUITE_END()
