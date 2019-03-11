#pragma once

#include "imgui.h"

#include <string>
#include <vector>

namespace SmartPeak
{
  class Widget
  {
  public:
    Widget() = default;
    ~Widget() = default;

    /**
      Interface to show the widget
    */
    virtual void show() = 0

    /**
      Helper method to filter a popup

      @param[in] popup_id Sequence handler
      @param[in] filter Text filters
      @param[in] colum Column of text items to filter
      @param[in] checked Vector of boolean values indicating if the column is filtered or not
    */
    static void TableFilterPopup(const char* popuop_id, ImGuiTextFilter& filter, std::vector<std::string>& column, bool* checked);
  };

  class GenericTableWidget : public Widget
  {
  public:
    void show();
  };

  class GenericPlotWidget : public Widget
  {
  public:
    void show();
  };

  class SequenceWidget : public Widget
  {
  public:
    void show();
  };

  class FileBrowserWidget : public Widget
  {
  public:
    void show();
  };

  class WorkflowWidget : public Widget
  {
  public:
    void show();
  };
}