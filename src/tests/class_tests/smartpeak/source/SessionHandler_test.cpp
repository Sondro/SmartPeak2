// TODO: Add copyright

#include <SmartPeak/test_config.h>
#define BOOST_TEST_MODULE SessionHandler test suite
#include <boost/test/included/unit_test.hpp>
#include <SmartPeak/core/SessionHandler.h>
#include <SmartPeak/core/SequenceProcessor.h>

using namespace SmartPeak;
using namespace std;

BOOST_AUTO_TEST_SUITE(SessionHandler1)

struct TestData {
  TestData(const bool& load_sequence = true, const bool& load_features = false){
    // Handle the filenames
    const std::string pathname = SMARTPEAK_GET_TEST_DATA_PATH("workflow_csv_files");
    // Load the sequence
    if (load_sequence) {
      Filenames filenames = Filenames::getDefaultStaticFilenames(pathname);
      CreateSequence cs(sequenceHandler);
      cs.filenames = filenames;
      cs.delimiter = ",";
      cs.checkConsistency = false;
      cs.process();
    }
    // Load the picked features
    if (load_features) {
      LoadFeatures loadFeatures;
      for (auto& injection : sequenceHandler.getSequence()) {
        Filenames filenames = Filenames::getDefaultDynamicFilenames(
          pathname + "/mzML",
          pathname + "/features",
          pathname + "/features",
          injection.getMetaData().getInjectionName(),
          injection.getMetaData().getInjectionName());
        loadFeatures.process(injection.getRawData(), {}, filenames);
      }
    }
  }
  SequenceHandler sequenceHandler;
};

BOOST_AUTO_TEST_CASE(setSequenceTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setSequenceTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers(0), "inj#");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_headers(session_handler.sequence_table_headers.size() - 1), "acquisition_date_and_time");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body.dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body(0, 0), "1");
  BOOST_CHECK_EQUAL(session_handler.sequence_table_body(session_handler.sequence_table_body.dimension(0) - 1, session_handler.sequence_table_body.dimension(1) - 1), "1900-01-01_000000");

  session_handler.setInjectionExplorer();
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers.size(), 3);
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers(0), "workflow");
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_headers(session_handler.injection_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_body.dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.injection_explorer_checkbox_body.dimension(1), 3);
  BOOST_CHECK(session_handler.injection_explorer_checkbox_body(0, 0));
  BOOST_CHECK(!session_handler.injection_explorer_checkbox_body(1, 1));
  BOOST_CHECK(session_handler.injection_explorer_checkbox_body(session_handler.injection_explorer_checkbox_body.dimension(0) - 1, session_handler.injection_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setTransitionsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setTransitionsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers.size(), 9);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers(0), "transition_group");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_headers(session_handler.transitions_table_headers.size() - 1), "detecting_transition");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body.dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body.dimension(1), 9);
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.transitions_table_body(session_handler.transitions_table_body.dimension(0) - 1, session_handler.transitions_table_body.dimension(1) - 1), "1");

  session_handler.setTransitionExplorer();
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers.size(), 2);
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers(0), "plot");
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_headers(session_handler.transition_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_body.dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.transition_explorer_checkbox_body.dimension(1), 2);
  BOOST_CHECK(session_handler.transition_explorer_checkbox_body(0, 0));
  BOOST_CHECK(!session_handler.transition_explorer_checkbox_body(1, 0));
  BOOST_CHECK(session_handler.transition_explorer_checkbox_body(session_handler.transition_explorer_checkbox_body.dimension(0) - 1, session_handler.transition_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setFeatureExplorer1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setFeatureExplorer();
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_headers.size(), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_headers(0), "name");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body.dimension(0), 16);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body.dimension(1), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body(0, 0), "asymmetry_factor");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_body(session_handler.feature_explorer_body.dimension(0) - 1, session_handler.feature_explorer_body.dimension(1) - 1), "rightWidth");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers.size(), 2);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers(0), "plot");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_headers(session_handler.feature_explorer_checkbox_headers.size() - 1), "table");
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_body.dimension(0), 16);
  BOOST_CHECK_EQUAL(session_handler.feature_explorer_checkbox_body.dimension(1), 2);
  BOOST_CHECK(!session_handler.feature_explorer_checkbox_body(0, 0));
  BOOST_CHECK(session_handler.feature_explorer_checkbox_body(2, 0));
  BOOST_CHECK(session_handler.feature_explorer_checkbox_body(session_handler.feature_explorer_checkbox_body.dimension(0) - 1, session_handler.feature_explorer_checkbox_body.dimension(1) - 1));
}

BOOST_AUTO_TEST_CASE(setWorkflowTable1)
{
  TestData testData;
  //SessionHandler session_handler; session_handler.setWorkflowTable();
}

BOOST_AUTO_TEST_CASE(setParametersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setParametersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers.size(), 4);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers(0), "function");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_headers(session_handler.parameters_table_headers.size() - 1), "value");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body.dimension(0), 106);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body.dimension(1), 4);
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body(0, 0), "AbsoluteQuantitation");
  BOOST_CHECK_EQUAL(session_handler.parameters_table_body(session_handler.parameters_table_body.dimension(0) - 1, session_handler.parameters_table_body.dimension(1) - 1), "TRUE");
}

BOOST_AUTO_TEST_CASE(setQuantMethodTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setQuantMethodTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers.size(), 20);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_headers(session_handler.quant_method_table_headers.size() - 1), "transformation_model_param_y_datum_max");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body.dimension(1), 20);
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body(0, 0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.quant_method_table_body(session_handler.quant_method_table_body.dimension(0) - 1, session_handler.quant_method_table_body.dimension(1) - 1), "1000000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setStdsConcsTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setStdsConcsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers.size(), 7);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_headers(session_handler.stds_concs_table_headers.size() - 1), "dilution_factor");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body.dimension(0), 8);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body.dimension(1), 7);
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.stds_concs_table_body(session_handler.stds_concs_table_body.dimension(0) - 1, session_handler.stds_concs_table_body.dimension(1) - 1), "1.000000");

}

BOOST_AUTO_TEST_CASE(setComponentFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setComponentFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers.size(), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_headers(session_handler.comp_filters_table_headers.size() - 1), "metaValue_peak_apex_int_u");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body.dimension(1), 11);
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_filters_table_body(session_handler.comp_filters_table_body.dimension(0) - 1, session_handler.comp_filters_table_body.dimension(1) - 1), "1000000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupFiltersTable1)
{
  TestData testData;
  SessionHandler session_handler; 
  session_handler.setComponentGroupFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_headers(session_handler.comp_group_filters_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_filters_table_body(session_handler.comp_group_filters_table_body.dimension(0) - 1, session_handler.comp_group_filters_table_body.dimension(1) - 1), "");

}

BOOST_AUTO_TEST_CASE(setComponentQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers.size(), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers(0), "component_name");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_headers(session_handler.comp_qcs_table_headers.size() - 1), "metaValue_var_xcorr_shape_weighted_u");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body.dimension(1), 19);
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body(0, 0), "arg-L.arg-L_1.Heavy");
  BOOST_CHECK_EQUAL(session_handler.comp_qcs_table_body(session_handler.comp_qcs_table_body.dimension(0) - 1, session_handler.comp_qcs_table_body.dimension(1) - 1), "1000000000.000000");

}

BOOST_AUTO_TEST_CASE(setComponentGroupQCsTable1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setComponentGroupQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers.size(), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers(0), "component_group_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_headers(session_handler.comp_group_qcs_table_headers.size() - 1), "ion_ratio_feature_name");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body.dimension(0), 10);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body.dimension(1), 24);
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body(0, 0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.comp_group_qcs_table_body(session_handler.comp_group_qcs_table_body.dimension(0) - 1, session_handler.comp_group_qcs_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(sessionHandlerGetters1)
{
  TestData testData;
  SessionHandler session_handler;
  session_handler.setMinimalDataAndFilters(testData.sequenceHandler);
  // Dynamic headers and body
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader()(0), "inj#");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerHeader()(session_handler.getInjectionExplorerHeader().dimension(0) - 1), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody().dimension(0), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody().dimension(1), 2);
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody()(0,0), "1");
  BOOST_CHECK_EQUAL(session_handler.getInjectionExplorerBody()(session_handler.getInjectionExplorerBody().dimension(0) - 1,session_handler.getInjectionExplorerBody().dimension(1) - 1), "150516_CM1_Level10");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader()(0), "transition_group");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerHeader()(session_handler.getTransitionExplorerHeader().dimension(0) - 1), "transition_name");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody().dimension(0), 6);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody().dimension(1), 2);
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody()(0,0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.getTransitionExplorerBody()(session_handler.getTransitionExplorerBody().dimension(0) - 1, session_handler.getTransitionExplorerBody().dimension(1) - 1), "ser-L.ser-L_2.Light");
  // N selected counts
  BOOST_CHECK_EQUAL(session_handler.getNSelectedSampleNamesTable(), 2);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedSampleNamesPlot(), 1);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedTransitionsTable(), 6);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedTransitionsPlot(), 1);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedFeatureMetaValuesTable(), 16);
  BOOST_CHECK_EQUAL(session_handler.getNSelectedFeatureMetaValuesPlot(), 1);
  // Selected string values
  BOOST_CHECK(session_handler.getSelectInjectionNamesWorkflow(testData.sequenceHandler) == std::set<std::string>({"150516_CM1_Level10_2_BatchName_1900-01-01_000000", "150516_CM1_Level1_1_BatchName_1900-01-01_000000"}));
  BOOST_CHECK(session_handler.getSelectSequenceSegmentNamesWorkflow(testData.sequenceHandler) == std::set<std::string>({ "segment1" }));
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable()(0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesTable()(session_handler.getSelectSampleNamesTable().dimension(0)-1), "150516_CM1_Level10");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot().size(), 2);
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot()(0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.getSelectSampleNamesPlot()(session_handler.getSelectSampleNamesPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable()(0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsTable()(session_handler.getSelectTransitionsTable().dimension(0) - 1), "ser-L.ser-L_2.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable()(0), "arg-L");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionGroupsTable()(session_handler.getSelectTransitionGroupsTable().dimension(0) - 1), "ser-L");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot().size(), 6);
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot()(0), "arg-L.arg-L_1.Light");
  BOOST_CHECK_EQUAL(session_handler.getSelectTransitionsPlot()(session_handler.getSelectTransitionsPlot().dimension(0) - 1), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable().size(), 16);
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable()(0), "asymmetry_factor");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesTable()(session_handler.getSelectFeatureMetaValuesTable().dimension(0) - 1), "rightWidth");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot().size(), 16);
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot()(0), "");
  BOOST_CHECK_EQUAL(session_handler.getSelectFeatureMetaValuesPlot()(session_handler.getSelectFeatureMetaValuesPlot().dimension(0) - 1), "");
  // Table filters
  BOOST_CHECK_EQUAL(session_handler.getSequenceTableFilters().size(), 2);
  BOOST_CHECK(session_handler.getSequenceTableFilters()(0));
  BOOST_CHECK(session_handler.getSequenceTableFilters()(session_handler.getSequenceTableFilters().dimension(0) - 1));
  BOOST_CHECK_EQUAL(session_handler.getTransitionsTableFilters().size(), 6);
  BOOST_CHECK(session_handler.getTransitionsTableFilters()(0));
  BOOST_CHECK(session_handler.getTransitionsTableFilters()(session_handler.getTransitionsTableFilters().dimension(0) - 1));
  session_handler.setQuantMethodTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getQuantMethodsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getQuantMethodsTableFilters()(0));
  BOOST_CHECK(!session_handler.getQuantMethodsTableFilters()(session_handler.getQuantMethodsTableFilters().dimension(0) - 1));
  session_handler.setComponentFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentFiltersTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentFiltersTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentFiltersTableFilters()(session_handler.getComponentFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentQCsTableFilters().size(), 10);
  BOOST_CHECK(!session_handler.getComponentQCsTableFilters()(0));
  BOOST_CHECK(session_handler.getComponentQCsTableFilters()(session_handler.getComponentQCsTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupFiltersTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupFiltersTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupFiltersTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupFiltersTableFilters()(session_handler.getComponentGroupFiltersTableFilters().dimension(0) - 1));
  session_handler.setComponentGroupQCsTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.getComponentGroupQCsTableFilters().size(), 10);
  BOOST_CHECK(session_handler.getComponentGroupQCsTableFilters()(0));
  BOOST_CHECK(!session_handler.getComponentGroupQCsTableFilters()(session_handler.getComponentGroupQCsTableFilters().dimension(0) - 1));
}

BOOST_AUTO_TEST_CASE(setFeatureTable1)
{
  TestData testData(true, true);
  SessionHandler session_handler;
  session_handler.setFeatureTable(testData.sequenceHandler);
  std::cout << session_handler.feature_table_body << std::endl;
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers.size(), 19);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(session_handler.feature_table_headers.size() - 1), "used_");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(0), 13);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(1), 19);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(session_handler.feature_table_body.dimension(0) - 1, session_handler.feature_table_body.dimension(1) - 1), "true");
  session_handler.setMinimalDataAndFilters(testData.sequenceHandler);
  session_handler.setFeatureTable(testData.sequenceHandler);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers.size(), 20);
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(0), "sample_name");
  BOOST_CHECK_EQUAL(session_handler.feature_table_headers(session_handler.feature_table_headers.size() - 1), "calculated_concentration");
  std::cout << session_handler.feature_table_body << std::endl;
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(0), 1);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body.dimension(1), 20);
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(0, 0), "150516_CM1_Level1");
  BOOST_CHECK_EQUAL(session_handler.feature_table_body(session_handler.feature_table_body.dimension(0) - 1, session_handler.feature_table_body.dimension(1) - 1), "");
}

BOOST_AUTO_TEST_CASE(setFeatureMatrix1)
{
  TestData testData;
  SessionHandler session_handler; session_handler.setFeatureMatrix(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setChromatogramScatterPlot1)
{
  TestData testData;
  SessionHandler session_handler; session_handler.setChromatogramScatterPlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_CASE(setCalibratorsScatterLinePlot1)
{
  TestData testData;
  SessionHandler session_handler; session_handler.setCalibratorsScatterLinePlot(testData.sequenceHandler);
}

BOOST_AUTO_TEST_SUITE_END()
