// TODO: Add copyright

#include <SmartPeak/io/SequenceParser.h>
#include <SmartPeak/core/FeatureMetadata.h>
#include <SmartPeak/core/SampleType.h>
#include <SmartPeak/core/SequenceHandler.h>
#ifndef CSV_IO_NO_THREAD
#define CSV_IO_NO_THREAD
#endif
#include <SmartPeak/io/csv.h>
#include <SmartPeak/io/CSVWriter.h>
#include <SmartPeak/io/InputDataValidation.h>
#include <ctime>

#include <plog/Log.h>

namespace SmartPeak
{
  void SequenceParser::readSequenceFile(
    SequenceHandler& sequenceHandler,
    const std::string& pathname,
    const std::string& delimiter
  )
  {
    LOGD << "START readSequenceFile";
    LOGD << "Delimiter: " << delimiter;

    LOGI << "Loading: " << pathname;

    if (pathname.empty()) {
      LOGE << "Pathname is empty";
      LOGD << "END readSequenceFile";
      return;
    }

    if (!InputDataValidation::fileExists(pathname)) {
      LOGE << "File not found";
      LOGD << "END readSequenceFile";
      return;
    }

    const std::string s_sample_name {"sample_name"};
    const std::string s_sample_group_name {"sample_group_name"};
    const std::string s_sequence_segment_name {"sequence_segment_name"};
    const std::string s_sample_type {"sample_type"};
    const std::string s_original_filename {"original_filename"};
    const std::string s_proc_method_name {"proc_method_name"};
    const std::string s_rack_number {"rack_number"};
    const std::string s_plate_number {"plate_number"};
    const std::string s_pos_number {"pos_number"};
    const std::string s_inj_number {"inj_number"};
    const std::string s_dilution_factor {"dilution_factor"};
    const std::string s_acq_method_name {"acq_method_name"};
    const std::string s_operator_name {"operator_name"};
    const std::string s_acquisition_date_and_time {"acquisition_date_and_time"};
    const std::string s_inj_volume {"inj_volume"};
    const std::string s_inj_volume_units {"inj_volume_units"};
    const std::string s_batch_name {"batch_name"};

    const std::string s_comma {","};
    const std::string s_semicolon {";"};
    const std::string s_tab {"\t"};

    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<','>> in_comma(pathname);
    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<';'>> in_semicolon(pathname);
    io::CSVReader<17, io::trim_chars<>, io::no_quote_escape<'\t'>> in_tab(pathname);

    if (delimiter == s_comma) {
      in_comma.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else if (delimiter == s_semicolon) {
      in_semicolon.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else if (delimiter == s_tab) {
      in_tab.read_header(
        io::ignore_extra_column/* | io::ignore_missing_column*/,
        s_sample_name,
        s_sample_group_name,
        s_sequence_segment_name,
        s_sample_type,
        s_original_filename,
        s_proc_method_name,
        s_rack_number,
        s_plate_number,
        s_pos_number,
        s_inj_number,
        s_dilution_factor,
        s_acq_method_name,
        s_operator_name,
        s_acquisition_date_and_time,
        s_inj_volume,
        s_inj_volume_units,
        s_batch_name
      );
    } else {
      throw std::invalid_argument("Delimiter \"" + delimiter + "\" is not supported.");
    }

    while (true) {
      MetaDataHandler t; // as in temporary
      std::string t_date;
      std::string t_sample_type;
      std::string t_rack_number;
      std::string t_plate_number;
      std::string t_pos_number;
      std::string t_inj_number;
      std::string t_dilution_factor;
      std::string t_inj_volume;
      bool is_valid = false;

      if (delimiter == s_comma) {
        is_valid = in_comma.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t_rack_number, t_plate_number, t_pos_number,
          t_inj_number, t_dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t_inj_volume, t.inj_volume_units, t.batch_name);
      } else if (delimiter == s_semicolon) {
        is_valid = in_semicolon.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t_rack_number, t_plate_number, t_pos_number,
          t_inj_number, t_dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t_inj_volume, t.inj_volume_units, t.batch_name);
      } else if (delimiter == s_tab) {
        is_valid = in_tab.read_row(t.sample_name, t.sample_group_name,
          t.sequence_segment_name, t_sample_type, t.original_filename,
          t.proc_method_name, t_rack_number, t_plate_number, t_pos_number,
          t_inj_number, t_dilution_factor, t.acq_method_name, t.operator_name,
          t_date, t_inj_volume, t.inj_volume_units, t.batch_name);
      }

      if (!is_valid)
        break;

      if (false == validateAndConvert(t_inj_number, t.inj_number)) {
        LOGW << "Warning: Empty cell in column '" << s_inj_number << "'. Skipping entire row";
        continue;
      }

      if (t.inj_number <= 0) {
        LOGW << "Warning: Value '" << t.inj_number << "' is not valid for column '" << s_inj_number << "'. Skipping entire row";
        continue;
      }

      validateAndConvert(t_rack_number,     t.rack_number);
      validateAndConvert(t_plate_number,    t.plate_number);
      validateAndConvert(t_pos_number,      t.pos_number);
      validateAndConvert(t_dilution_factor, t.dilution_factor);
      validateAndConvert(t_inj_volume,      t.inj_volume);

      if (stringToSampleType.count(t_sample_type)) {
        t.sample_type = stringToSampleType.at(t_sample_type);
      } else {
        t.sample_type = SampleType::Unrecognized;
      }

      std::tm& adt = t.acquisition_date_and_time;
      std::istringstream iss(t_date);
      iss.imbue(std::locale(""));
      iss >> std::get_time(&adt, "%d/%m/%Y %H:%M:%S");
      if (adt.tm_mday < 1 || adt.tm_mday > 31) {
        LOGD << "Invalid value for std::tm::tm_mday: " << adt.tm_mday << ". Setting to 1.";
        adt.tm_mday = 1;
      }

      sequenceHandler.addSampleToSequence(t, OpenMS::FeatureMap());
    }

    LOGD << "END readSequenceFile";
  }

  void SequenceParser::makeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    std::vector<std::vector<std::string>>& rows_out,
    std::vector<std::string>& headers_out,
    const std::vector<std::string>& meta_data,
    const std::set<SampleType>& sample_types,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& component_names) {
    std::vector<std::string> headers = {
      "sample_name", "sample_type", "component_group_name", "component_name", "batch_name",
      "rack_number", "plate_number", "pos_number", "inj_number", "dilution_factor", "inj_volume",
      "inj_volume_units", "operator_name", "acq_method_name", "proc_method_name",
      "original_filename", "acquisition_date_and_time", "injection_name", "used_"
    };
    headers.insert(headers.end(), meta_data.cbegin(), meta_data.cend());
    for (size_t i = 0; i < headers.size() - 1; ++i) { // checking headers are unique, stable (maintaining the same positions)
      for (size_t j = i + 1; j < headers.size(); ) {
        if (headers[i] == headers[j])
          headers.erase(headers.begin() + j);
        else
          ++j;
      }
    }
    headers_out = headers;

    const std::string delimiter {"_____"};

    rows_out.clear();
    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      if (sample_types.count(mdh.getSampleType()) == 0)
        continue;
      if (sample_names.size() > 0 && sample_names.count(mdh.getSampleName()) == 0)
        continue;

      // feature_map_history_ is needed in order to export all "used_" = true and false features
      for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMapHistory()) {
        if (!feature.metaValueExists(s_PeptideRef) || feature.getMetaValue(s_PeptideRef).isEmpty()) {
          LOGV << "component_group_name is absent or empty. Skipping this feature";
          continue;
        }
        const std::string component_group_name = feature.getMetaValue(s_PeptideRef);
        for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
          std::vector<std::string> row;
          row.push_back(mdh.getSampleName());
          row.push_back(sampleTypeToString.at(mdh.getSampleType()));
          row.push_back(component_group_name);
          if (!subordinate.metaValueExists(s_native_id) ||
              subordinate.getMetaValue(s_native_id).isEmpty() ||
              subordinate.getMetaValue(s_native_id).toString().empty()) {
            LOGV << "component_name is absent or empty. Skipping this subordinate";
            continue;
          }
          const std::string component_name = subordinate.getMetaValue(s_native_id);
          if (component_names.size() > 0 && component_names.count(component_name) == 0)
            continue;
          row.push_back(component_name);
          row.push_back(mdh.batch_name);
          row.push_back(std::to_string(mdh.rack_number));
          row.push_back(std::to_string(mdh.plate_number));
          row.push_back(std::to_string(mdh.pos_number));
          row.push_back(std::to_string(mdh.inj_number));
          row.push_back(std::to_string(mdh.dilution_factor));
          row.push_back(std::to_string(mdh.inj_volume));
          row.push_back(mdh.inj_volume_units);
          row.push_back(mdh.operator_name);
          row.push_back(mdh.acq_method_name);
          row.push_back(mdh.proc_method_name);
          row.push_back(mdh.original_filename);
          row.push_back(mdh.getAcquisitionDateAndTimeAsString());
          row.push_back(mdh.getInjectionName());
          row.push_back(subordinate.metaValueExists("used_") ? subordinate.getMetaValue("used_").toString() : "");
          for (const std::string& meta_value_name : meta_data) {
            if (subordinate.metaValueExists(meta_value_name) && meta_value_name == "QC_transition_message") {
              OpenMS::StringList messages = subordinate.getMetaValue(meta_value_name).toStringList();
              row.push_back(
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            }
            else if (feature.metaValueExists(meta_value_name) && meta_value_name == "QC_transition_group_message") {
              OpenMS::StringList messages = feature.getMetaValue(meta_value_name).toStringList();
              row.push_back(
                Utilities::join(messages.begin(), messages.end(), delimiter)
              );
            }
            else {
              CastValue datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
              if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0) {
                // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
                row.push_back(std::to_string(datum.f_));
              } else {
                row.push_back("");
              }
            }
          }
          rows_out.push_back(row);
        }
      }
    }
  }

  bool SequenceParser::writeDataTableFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types
  )
  {
    LOGD << "START writeDataTableFromMetaValue";
    LOGI << "Storing: " << filename;

    std::vector<std::vector<std::string>> rows;
    std::vector<std::string> headers;
    std::vector<std::string> meta_data_strings;
    for (const FeatureMetadata& m : meta_data) {
      meta_data_strings.push_back(metadataToString.at(m));
    }
    makeDataTableFromMetaValue(sequenceHandler, rows, headers, meta_data_strings, sample_types, std::set<std::string>(), std::set<std::string>());

    CSVWriter writer(filename, ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeDataTableFromMetaValue";
      return false;
    }

    for (const std::vector<std::string>& line : rows) {
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeDataTableFromMetaValue";
    return true;
  }

  void SequenceParser::makeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    Eigen::Tensor<float, 2>& data_out,
    Eigen::Tensor<std::string, 1>& columns_out,
    Eigen::Tensor<std::string, 2>& rows_out,
    const std::vector<std::string>& meta_data,
    const std::set<SampleType>& sample_types,
    const std::set<std::string>& sample_names,
    const std::set<std::string>& component_names
  )
  {
    std::set<std::string> columns;
    std::set<Row,Row_less> rows;
    std::map<std::string,std::map<Row,float,Row_less>> data_dict;

    for (const InjectionHandler& sampleHandler : sequenceHandler.getSequence()) {
      const MetaDataHandler& mdh = sampleHandler.getMetaData();
      std::map<std::string, float> validation_metrics = sampleHandler.getRawData().getValidationMetrics();
      const std::string& sample_name = mdh.getSampleName();
      if (sample_names.size() && sample_names.count(sample_name) == 0) 
        continue;
      if (sample_types.count(mdh.getSampleType()) == 0) 
        continue;
      data_dict.insert({ sample_name, std::map<Row,float,Row_less>() });
      for (const std::string& meta_value_name : meta_data) {
        // feature_map_history is not needed here as we are only interested in the current/"used" features
        for (const OpenMS::Feature& feature : sampleHandler.getRawData().getFeatureMap()) {
          for (const OpenMS::Feature& subordinate : feature.getSubordinates()) {
            if (component_names.size() && component_names.count(subordinate.getMetaValue(s_native_id).toString()) == 0) 
              continue;
            if (subordinate.metaValueExists("used_")) {
              const std::string used = subordinate.getMetaValue("used_").toString();
              if (used.empty() || used[0] == 'f' || used[0] == 'F') 
                continue;
            }
            const Row row_tuple_name(
              feature.getMetaValue(s_PeptideRef).toString(),
              subordinate.getMetaValue(s_native_id).toString(),
              meta_value_name
            );
            CastValue datum;
            if (meta_value_name == "accuracy" || meta_value_name == "n_features") {
              datum = validation_metrics.at(meta_value_name);
            }
            else {
              datum = SequenceHandler::getMetaValue(feature, subordinate, meta_value_name);
            }
            if (meta_value_name == "validation") {
              if (datum.s_ == "TP") datum = static_cast<float>(1.0);
              else if (datum.s_ == "FP") datum = static_cast<float>(-1.0);
              else datum = static_cast<float>(-2.0);
            }
            if (datum.getTag() == CastValue::Type::FLOAT && datum.f_ != 0.0 && !std::isnan(datum.f_)) { // Skip NAN (replaced by 0 later)
              data_dict[sample_name].emplace(row_tuple_name, datum.f_);
              columns.insert(sample_name);
              rows.insert(row_tuple_name);
            }
          }
        }
      }
    }

    // Copy over the rows
    rows_out.resize((int)rows.size(), 3);
    int row = 0;
    for (const auto& r: rows) {
      rows_out(row, 0) = r.component_name;
      rows_out(row, 1) = r.component_group_name;
      rows_out(row, 2) = r.meta_value_name;
      ++row;
    }

    // Copy over the columns
    columns_out.resize((int)columns.size());
    int col = 0;
    for (const auto& c : columns) {
      columns_out(col) = c;
      ++col;
    }

    // Copy over the data
    data_out.resize((int)rows.size(), (int)columns.size());
    data_out.setConstant(0.0); // for now, initialize to 0 instead of NAN even though there are clear benefits to using NAN in packages that support NAN
    col = 0;
    for (const auto& c : columns) {
      row = 0;
      for (const auto& r : rows) {
        if (data_dict.count(c) && data_dict[c].count(r)) {
          data_out(row,col) = data_dict[c][r];
        }
        ++row;
      }
      ++col;
    }
  }

  bool SequenceParser::writeDataMatrixFromMetaValue(
    const SequenceHandler& sequenceHandler,
    const std::string& filename,
    const std::vector<FeatureMetadata>& meta_data,
    const std::set<SampleType>& sample_types
  )
  {
    LOGD << "START writeDataMatrixFromMetaValue";

    LOGI << "Storing: " << filename;

    Eigen::Tensor<float,2> data;
    Eigen::Tensor<std::string,1> columns;
    Eigen::Tensor<std::string, 2> rows;
    std::vector<std::string> meta_data_strings;
    for (const FeatureMetadata& m : meta_data) {
      meta_data_strings.push_back(metadataToString.at(m));
    }
    makeDataMatrixFromMetaValue(sequenceHandler, data, columns, rows, meta_data_strings, sample_types, std::set<std::string>(), std::set<std::string>());

    std::vector<std::string> headers = {"component_group_name", "component_name", "meta_value"};
    for (int i=0;i<columns.size();++i) headers.push_back(columns(i));

    CSVWriter writer(filename, ",");
    const size_t cnt = writer.writeDataInRow(headers.cbegin(), headers.cend());

    if (cnt < headers.size()) {
      LOGD << "END writeDataMatrixFromMetaValue";
      return false;
    }

    for (size_t i = 0; i < rows.dimension(0); ++i) {
      std::vector<std::string> line;
      for (size_t j = 0; j < rows.dimension(1); ++j) {
        line.push_back(rows(i,j));
      }
      for (size_t j = 0; j < data.dimension(1); ++j) {
        // NOTE: to_string() rounds at 1e-6. Therefore, some precision might be lost.
        line.emplace_back(std::to_string(data(i,j)));
      }
      writer.writeDataInRow(line.cbegin(), line.cend());
    }

    LOGD << "END writeDataMatrixFromMetaValue";
    return true;
  }
}
