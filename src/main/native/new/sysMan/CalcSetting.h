#ifndef __CALC_SETTING_H__
#define __CALC_SETTING_H__

#include "utils/FakeXEvent.h"
#include "utils/IniFile.h"
#include "utils/Utils.h"

#include "patient/ViewArchive.h"
#include "sysMan/ConfigToHost.h"

class CalcSetting {
public:
  static CalcSetting* GetInstance();

public:
  CalcSetting();
  ~CalcSetting();

  GtkWidget* GetWindow();
  GtkWidget* CreateCalcWindow(GtkWidget* parent);

  vector<string> GetExamItemsCalc();

  void ClearAll();

  void ChangeModel2();

  string GetExamName();
  string GetDepartmentName();
  bool GetSelectPath();

  string CustomItemTransName(int item_num);
  string ItemMenuTransEnglish(int item_num);

  void ChangeExamBox(string check_part);
  void ChangeExamBoxDelete();
  void ChangeModelAndLight(const string name);

  int GetCalcMaxEtype();
  int GetMeasureSequence(const string exam_type);
  void GetCustomCalcMeasure(int Etype, int& measure_type, string& calc_name);
  void GetCalcListEtype(string exam_type, vector<int>& vecItemCalc);
  void GetDepartmentForCustomMeasure(int Etype, string& department);

  vector<string> GetSelectedVec() {
    return m_vecPath;
  }

private:
  // signal

  static void signal_combobox_changed_exam_type(GtkComboBox* combobox, CalcSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedExamType(combobox);
    }
  }

  static void signal_combobox_changed_department(GtkComboBox* combobox, CalcSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDepartment(combobox);
    }
  }

  static void signal_combobox_changed_sequence(GtkComboBox* combobox, CalcSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedSequence(combobox);
    }
  }

  static void signal_button_clicked_selectone(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedSelectOne(button);
    }
  }

  static void signal_button_clicked_selectall(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedSelectAll(button);
    }
  }

  static void signal_button_clicked_backone(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedBackOne(button);
    }
  }

  static void signal_button_clicked_backall(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedBackAll(button);
    }
  }

  static void signal_button_clicked_add(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAdd(button);
    }
  }

  static void signal_button_clicked_delete(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDelete(button);
    }
  }

  static void signal_button_clicked_up(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedUp(button);
    }
  }

  static void signal_button_clicked_down(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDown(button);
    }
  }

  static void signal_button_clicked_export(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedExport(button);
    }
  }

  static void signal_button_clicked_import(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedImport(button);
    }
  }

  static void signal_button_clicked_default(GtkButton* button, CalcSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefault(button);
    }
  }

  static int signal_callback_add(gpointer data) {
    CalcSetting::GetInstance()->AddItem();

    return 0;
  }

  static int signal_callback_delete(gpointer data) {
    CalcSetting::GetInstance()->DeleteItem();

    return 0;
  }

  static int signal_callback_default(gpointer data) {
    CalcSetting::GetInstance()->UpdateAll();

    return 0;
  }

  // signal

  void ComboBoxChangedExamType(GtkComboBox* combobox);
  void ComboBoxChangedDepartment(GtkComboBox* combobox);
  void ComboBoxChangedSequence(GtkComboBox* combobox);

  void ButtonClickedSelectOne(GtkButton* button);
  void ButtonClickedSelectAll(GtkButton* button);
  void ButtonClickedBackOne(GtkButton* button);
  void ButtonClickedBackAll(GtkButton* button);
  void ButtonClickedAdd(GtkButton* button);
  void ButtonClickedDelete(GtkButton* button);

  void ButtonClickedUp(GtkButton* button);
  void ButtonClickedDown(GtkButton* button);
  void ButtonClickedExport(GtkButton* button);
  void ButtonClickedImport(GtkButton* button);

  void ButtonClickedDefault(GtkButton* button);

  void AddItem();
  void DeleteItem();
  void UpdateAll();

private:
  void CreateDefineItemCalc(vector<string>& vecExamItem);

  void AddColumnsCalc(GtkTreeView* treeview);
  void AddColumnsCalc1(GtkTreeView* treeview);

  GtkTreeModel* CreateItemCalcModel1();
  GtkTreeModel* CreateItemCalcModel2();

  void ChangeModel();

  GtkTreeIter InsertUniqueCalc(GtkTreeModel* model, const string str);

  int GetSequence(const string exam_type);
  int ItemNameTransEtype(string select_name);

  void CreateItemList_Calc(string department, vector<string>& vecItemCalc);
  void CreateItemList_Calc1(string probe_exam, vector<string>& vecItemCalc);
  void CreateItemList_Calc2(string probe_exam, vector<int>& vecItemCalc);
  void CreateItemList_Delete_Calc(string select_name, string department, vector<int>& vecDeleteCalc, IniFile* ini);
  void CreateItemList_Delete_Calc1(string select_name, string probe_exam, vector<int>& vecItemCalc);

private:
  static CalcSetting* m_instance;

private:
  GtkWidget* m_parent;
  GtkDialog* m_dialog;

  GtkComboBoxText* m_combobox_exam_type;
  GtkComboBoxText* m_combobox_department;
  GtkScrolledWindow* m_scrolled_window_item;
  GtkTreeView* m_treeview_item;

  GtkComboBoxText* m_combobox_sequence;
  GtkScrolledWindow* m_scrolled_window_selected_item;
  GtkTreeView* m_treeview_selected_item;

  vector<string> m_vecPath;
};

class CustomCalc: public FakeXEvent {
public:
  static CustomCalc* GetInstance();

public:
  CustomCalc();
  ~CustomCalc();

  void CreateCalcSettingWin(GtkWidget* parent);
  void CreateExportCalcSettingWin(GtkWidget* parent);

  void SetProgressBar(double fraction);
  void ExportLoadInfoNotice(string result);
  void ExportRightInfoNotice(string result);
  void ExportErrorInfoNotice(string result);

  void ImportSuccess();
  void ImportWrite(string item_name, int& item_num);

  void ButtonImportNameOK();

private:
  // signal

  static void signal_button_clicked_ok(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonClickedOK(button);
    }
  }

  static void signal_button_clicked_export(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonClickedExport(button);
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonClickedCancel(button);
    }
  }

  static void signal_entry_focus_in(GtkEditable* editable, GdkEventFocus* event, CustomCalc* data) {
    if (data != NULL) {
      data->EntryItemFocusIn(editable, event);
    }
  }

  static void signal_combobox_changed_type(GtkComboBox* combobox, CustomCalc* data) {
    if (data != NULL) {
      data->ComboboxChangedType(combobox);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, CustomCalc* data) {
    if (data != NULL) {
      data->DestroyWin();
    }

    return FALSE;
  }

  static gboolean signal_load_data(gpointer data) {
    CustomCalc* calc = (CustomCalc*)data;

    if (calc != NULL) {
      calc->LoadData();
    }

    return FALSE;
  }

  static gboolean signal_exit_customecalc(gpointer data) {
    CustomCalc* calc = (CustomCalc*)data;

    if (calc != NULL) {
      calc->DestroyWin();
    }

    return FALSE;
  }

  static void signal_callback_progress(goffset current, goffset total, gpointer data) {
    g_cancellable_is_cancelled(m_cancellable);
  }

  static gboolean Destroy(gpointer data) {
    ConfigToHost::GetInstance()->DestroyWindow();

    return FALSE;
  }

  // signal

  void ButtonClickedOK(GtkButton* button);
  void ButtonClickedExport(GtkButton* button);
  void ButtonClickedCancel(GtkButton* button);
  void EntryItemFocusIn(GtkEditable* editable, GdkEventFocus* event);
  void ComboboxChangedType(GtkComboBox* combobox);

  void LoadData();

private:
  void KeyEvent(unsigned char keyValue);

  void ClickedOKAndCancel();
  void HideClickedOKAndCancel();

  void DestroyWin();
  void DelayDestroyWin();

  void ImportRenameCopy(string item_name);
  bool RenameCompare(string name_copy);

private:
  static CustomCalc* m_instance;
  static GCancellable* m_cancellable;

private:
  GtkDialog* m_dialog;
  GtkEntry* m_entry_name;
  GtkComboBoxText* m_combobox_type;
  GtkComboBoxText* m_combobox_method;

  GtkEntry* m_entry_export_name;
  GtkProgressBar* m_progress_bar;
  GtkImage* m_image;
  GtkLabel* m_label_notice;

  string m_export_name;
};

#endif
