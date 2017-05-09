#ifndef __CALC_SETTING_H__
#define __CALC_SETTING_H__

#include "utils/FakeXEvent.h"
#include "utils/IniFile.h"
#include "utils/Utils.h"

#include "patient/ViewArchive.h"

class CalcSetting {
public:
  static CalcSetting* GetInstance();

public:
  CalcSetting();
  ~CalcSetting();

  GtkWidget* CreateCalcWindow(GtkWidget* parent);
  GtkWidget* GetWindow();

  vector<string> GetExamItemsCalc();

private:
  // signal

  static void HandleDepartmentCalcChanged(GtkComboBox* combobox, CalcSetting* data) {
        data->DepartmentCalcChanged(combobox);
    }

    static void HandleMeasureSequenceChanged(GtkComboBox* combobox, CalcSetting* data) {
        data->MeasureSequenceChanged(combobox);
    }

    static void HandleExamCalcChanged(GtkComboBox* combobox, CalcSetting* data) {
        data->ExamCalcChanged(combobox);
    }

    static void HandleButtonSelectOneCalcClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonSelectOneCalcClicked(button);
    }
    static void HandleButtonSelectAllCalcClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonSelectAllCalcClicked(button);
    }

    static void HandleButtonDeleteSelectClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonDeleteSelectClicked(button);
    }
    static void HandleButtonAddClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonAddClicked(button);
    }

    static void HandleButtonBackOneClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonBackOneClicked(button);
    }

    static void HandleButtonBackAllClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonBackAllClicked(button);
    }

    static void HandleButtonDownClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonDownClicked(button);
    }
    static void HandleButtonUpClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonUpClicked(button);
    }

    static void HandleButtonExportClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonExportClicked(button);
    }
    static void HandleButtonImportClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonImportClicked(button);
    }

    static void HandleButtonDefaultClicked(GtkButton* button, CalcSetting* data) {
        if (data)
            data->ButtonDefaultClicked(button);
    }

    // signal
    void ExamCalcChanged(GtkComboBox* combobox);
    void MeasureSequenceChanged(GtkComboBox* combobox);
    void DepartmentCalcChanged(GtkComboBox* combobox);

    void ButtonBackOneClicked(GtkButton* button);
    void ButtonBackAllClicked(GtkButton* button);
    void ButtonSelectOneCalcClicked(GtkButton* button);
    void ButtonSelectAllCalcClicked(GtkButton* button);
    void ButtonAddClicked(GtkButton* button);
    void ButtonDeleteSelectClicked(GtkButton* button);
    void ButtonDeleteAllClicked(GtkButton* button);
    void ButtonUpClicked(GtkButton* button);
    void ButtonDownClicked(GtkButton* button);
    void ButtonExportClicked(GtkButton* button);
    void ButtonImportClicked(GtkButton* button);
    void ButtonDefaultClicked(GtkButton* button);

private:
  static CalcSetting* m_instance;

private:
  GtkDialog* m_dialog;

private:

////////////////////////////////////////////////////////////

public:
    GtkWidget *m_win_parent;
    GtkWidget* m_treeView_all;

    GtkWidget *m_treeview_item_calc;
    GtkWidget *m_treeview_item_calc1;
    GtkCellRenderer *m_cellrenderer_calc_text;
    GtkCellRenderer *m_cellrenderer_calc_text1;
    GtkWidget *m_combobox_department_calc;
    GtkWidget *m_combobox_sequence_calc;
    void ChangeModel(void);
    void ChangeModel2(void);
    void ChangeExamBox(char *check_part);
    void ChangeExamBoxDelete(void);
    void ChangeExamBoxToDefault(void);
    void ChangeModelAndLight(const char *name);
    void GetCalcListEtype(char *exam_type, vector<int> &vecItemCalc);
    int GetCalcListNum(char *exam_type);
    int GetMeasureSequence(const char *exam_type);
    void GetCustomCalcMeasure(int Etype, int &measure_type, string &calc_name);
    int GetCalcMaxEtype();
    void GetDepartmentForCustomMeasure(int Etype, string &department);
    vector<string> GetSelectedVec() {
        return m_vecPath;
    }
    vector<string> GetSelectedVec1() {
        return m_vecPath1;
    }
    gboolean GetSelectPath(void);
    const string GetExamName(void);
    const string GetDepartmentName(void);
    string ItemMenuTransEnglish(int item_num);
    string CustomItemTransName(int item_num);
    void DeleteItem(void);
    void AddItem(void);
    void UpdateAllCalc(void);
    void ClearAllCalc(void);

private:
    GtkWidget *fixed_calc;
    GtkWidget *m_combobox_exam_calc;
    GtkWidget *scrolledwindow_item_calc1;
    GtkWidget *scrolledwindow_item_calc;
    GtkWidget* m_button_select_one;
    GtkWidget *m_button_select_all;
    GtkWidget *m_button_calc_add;
    GtkWidget *m_button_calc_delete;
    GtkWidget *m_button_calc_delete_all;
    GtkWidget *m_button_calc_delete_select;
    GtkWidget *m_button_calc_up;
    GtkWidget *m_button_calc_down;
    GtkWidget *m_button_calc_export;
    GtkWidget *m_button_calc_import;
    GtkWidget *m_button_calc_default;

    GtkWidget* m_section_view;
    GtkWidget* m_section_combobox;
    vector<string> m_vecPath;
    vector<string> m_vecPath1;

    int GetSequence(const char *exam_type);

    int  create_exam_calc_model(vector<string> &vecExamItem_calc);
    void add_columns_calc(GtkTreeView *treeview);
    void add_columns_calc1(GtkTreeView *treeview);
    GtkTreeModel* create_item_calc_model1();
    void CreateDefaultItemList_Calc(char *department,vector<string>& vecItemCalc);
    void CreateItemList_Calc(char *department,vector<string>& vecItemCalc);

    GtkTreeModel* create_item_calc_model2();
    void CreateItemList_Calc1(char *probe_exam,vector<string>& vecItemCalc1);
    void CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1);
    void CreateItemList_Delete_Calc1(char *select_name, string probe_exam,vector<int>& vecItemCalc1);
    void CreateItemList_Delete_Calc(char *select_name, char *department, vector<int>& vecDeleteCalc, IniFile *ini);
    GtkTreeIter InsertUniqueCalc(GtkTreeModel *model, const char *str);
    void CreateDefineItem_calc(vector<string>& vecExamItem_calc);
    int  create_exam_calc_model();
    int ItemNameTransEtype(char *select_name);
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

  static void signal_progress_callback(goffset current, goffset total, gpointer data) {
    g_cancellable_is_cancelled(m_cancellable);
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

////////////////////////////////////////////////////////////

public:
    void DestroyWin(void);

    void DelayDestroyWin(void);

    void HideOKAndCancelClicked();
    void OKAndCancelClicked();
    bool RenameCompare(char * name_copy);
    void ImportSuccess(void);
    bool ImportCopy(int j);
    void ImportRenameCopy(string item_name);
    void ButtonImportNameOK();
    void ImportWrite(string item_name, int &item_num);
private:
  GtkWidget* GetWindow(void);

    /*GtkWidget *label_type;
    GtkWidget *label_method;
    GtkWidget *fixed1;
    GtkWidget *m_frame_new_notice;
    GtkWidget *fixed_new_notice;
    GtkWidget *m_label_notice1;
    GtkWidget *m_label_notice2;
    GtkWidget *m_label_notice3;
    GtkWidget *button_right;
    GtkWidget *img_right;
    GtkWidget *button_error;
    GtkWidget *img_error;
    GtkWidget *button_load;
    GtkWidget *img_load;
    GtkWidget *button_ok;
    GtkWidget *button_cancel;
    */
};

#endif
