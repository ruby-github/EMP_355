#ifndef __MEASURE_SETTING_H__
#define __MEASURE_SETTING_H__

#include "utils/Utils.h"

class MeasureSetting {
public:
  static MeasureSetting* GetInstance();

public:
  ~MeasureSetting();

  GtkWidget* CreateMeasureWindow(GtkWidget* parent);

  int GetMeasureSequence(string exam_type);
  void GetMeasureListEtype(string exam_type, vector<int>& vecItemCalc);

  void ChangeExamBox(string check_part);
  void ChangeExamBoxDelete();

private:
  // signal

  static void signal_combobox_changed_exam_type(GtkComboBox* combobox, MeasureSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedExamType(combobox);
    }
  }

  static void signal_combobox_changed_sequence(GtkComboBox* combobox, MeasureSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedMeasureSequence(combobox);
    }
  }

  static void signal_button_clicked_selectone(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedSelectOne(button);
    }
  }

  static void signal_button_clicked_selectall(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedSelectAll(button);
    }
  }

  static void signal_button_clicked_backone(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedBackOne(button);
    }
  }

  static void signal_button_clicked_backall(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedBackAll(button);
    }
  }

  static void signal_button_clicked_up(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedUp(button);
    }
  }

  static void signal_button_clicked_down(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDown(button);
    }
  }

  static void signal_button_clicked_default(GtkButton* button, MeasureSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefault(button);
    }
  }

  // signal

  void ComboBoxChangedExamType(GtkComboBox* combobox);
  void ComboBoxChangedMeasureSequence(GtkComboBox* combobox);


  void ButtonClickedSelectOne(GtkButton* button);
  void ButtonClickedSelectAll(GtkButton* button);
  void ButtonClickedBackOne(GtkButton* button);
  void ButtonClickedBackAll(GtkButton* button);
  void ButtonClickedUp(GtkButton* button);
  void ButtonClickedDown(GtkButton* button);
  void ButtonClickedDefault(GtkButton* button);

private:
  MeasureSetting();

  void CreateDefineItemCalc(vector<string>& vec);

  GtkTreeModel* CreateItemCalcModel1();
  GtkTreeModel* CreateItemCalcModel2();

  void AddColumnsCalc(GtkTreeView* treeview);
  void AddColumnsCalc1(GtkTreeView* treeview);

  void ChangeModel();
  void ChangeModel2();

  int GetSequence(const string exam_type);
  string ItemMenuTransEnglish(int item_num);
  int ItemNameTransEtype(string select_name);
  GtkTreeIter InsertUniqueCalc(GtkTreeModel* model, const string str);

  void CreateItemListCalc2(string probe_exam, vector<int>& vec);
  void CreateItemListForMeasure(vector<string>& vec);
  void CreateItemListForMeasureed(string probe_exam, vector<string>& vec);
  void CreateItemListForDeleteMeasureed(string select_name, string probe_exam, vector<int>& vec);

private:
  static MeasureSetting* m_instance;

private:
  GtkWidget* m_parent;

  GtkComboBoxText* m_combobox_exam_type;
  GtkComboBoxText* m_combobox_sequence;

  GtkScrolledWindow* m_scrolled_window_selected_item;

  GtkTreeView* m_treeview_item;
  GtkTreeView* m_treeview_selected_item;
};

#endif
