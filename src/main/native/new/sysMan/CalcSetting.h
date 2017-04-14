#ifndef __CALC_SETTING_H__
#define __CALC_SETTING_H__

#include <gtk/gtk.h>
#include <string>

#include "utils/FakeXEvent.h"

class MessageDialog;
class CalcSetting;

using std::string;

class CustomCalc:public FakeXEvent {
public:
  static CustomCalc* GetInstance();

public:
  CustomCalc();
  ~CustomCalc();

  void CreateCalcSettingWin(GtkWidget* parent);
  void CreateExportCalcSettingWin(GtkWidget* parent);

  void DestroyWin(void);
  void DelayDestroyWin(void);

  void OKAndCancelClicked();
  void SetProgressBar(double fraction);

  void ExportErrorInfoNotice(char *result);
  void ExportLoadInfoNotice(char *result);
  void ExportRightInfoNotice(char *result);

  void ButtonImportNameOK();
  void ImportWrite(string item_name, int &item_num);
  void ImportSuccess(void);

private:
  // signal

  static void signal_button_clicked_ok(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonClickedOK(button);
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonClickedCancel(button);
    }
  }

  static void signal_entry_focus_in(GtkEditable* editable, GdkEventFocus* event, CustomCalc* data) {
    if (data != NULL) {
      data->EntryFocusIn(editable, event);
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

  static void signal_button_clicked_export(GtkButton* button, CustomCalc* data) {
    if (data != NULL) {
      data->ButtonExportNameOKClicked(button);
    }
  }

private:
  GtkWidget* GetWindow();

  MessageDialog* GetMessageDialog();
  CalcSetting* GetCalcSetting();

  void ButtonClickedOK(GtkButton* button);
  void ButtonClickedCancel(GtkButton* button);
  void EntryFocusIn(GtkEditable* editable, GdkEventFocus* event);
  void ComboboxChangedType(GtkComboBox* combobox);
  void ButtonExportNameOKClicked(GtkButton *button);

private:
  GtkWidget* m_window;

  GtkEntry* m_entry_name;
  GtkComboBoxText* m_combobox_type;
  GtkComboBoxText* m_combobox_method;

  GtkEntry* m_entry_export_name;
  GtkProgressBar* m_progress_bar;
  GtkImage* m_image;

private:


static void HandleButtonRenameCancelClicked(GtkButton* button, CustomCalc* data) {
  if (data != NULL) {
    data->ButtonRenameCancelClicked(button);
  }
}



private:

    void HideOKAndCancelClicked();

    bool RenameCompare(char * name_copy);

    bool ImportCopy(int j);
    void ImportRenameCopy(string item_name);



private:

    GtkWidget *label_type;
    GtkWidget *label_method;

    GtkWidget *fixed1;
    GtkWidget *m_frame_new_notice;
    GtkWidget *m_label_notice;
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

    GtkTreeModel *CreateComboModel(char name[][20], int n);
    void KeyEvent(unsigned char keyValue);
    static CustomCalc* m_ptrInstance;

    void ButtonRenameCancelClicked(GtkButton *button);


    //      void ButtonImportNameOK();
    void EntryItemInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);



};





#include <gtk/gtk.h>
#include "calcPeople/MeasureDef.h"
#include <vector>
#include "probe/ExamItem.h"

#ifdef VET
static const int EXAM_NUM = 9;
static const int SECTION_NUM =22;
#else
#ifdef EMP_322
static const int EXAM_NUM = 18;
static const int SECTION_NUM =12;
static const int ETYPE_NUM =28;
static const int MAX_METHOD = 15;
#else
static const int EXAM_NUM = 22;
static const int SECTION_NUM =18;
static const int ETYPE_NUM =35;
static const int MAX_METHOD = 17;
#endif
#endif
struct CustomEtype {
    std::string name;
    int etype;
};

struct CustomTypeAndMethod {
    int type;
    std::string name;
    int etype;
};



enum {
    NAME_COLUMN,
    N_COLUMNS
};

class CalcSetting {
public:
    CalcSetting();
    ~CalcSetting();
    GtkWidget* GetWindow(void);
    GtkWidget* CreateExportCalcWindow(GtkWidget *parent);
    GtkWidget* CreateCalcWindow(GtkWidget *parent);
    GtkWidget *m_win_parent;
    GtkWidget* m_treeView_all;
    static CalcSetting* GetInstance();
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
    const gchar* GetExamName(void);
    const gchar* GetDepartmentName(void);
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

    static CalcSetting* m_ptrInstance;
    CustomCalc m_custom_calc_win;
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
    //GtkTreeModel* create_item_calc_model();
    GtkTreeModel* create_item_calc_model2();
    void CreateItemList_Calc1(char *probe_exam,vector<string>& vecItemCalc1);
    void CreateItemList_Calc2(char *probe_exam,vector<int>& vecItemCalc1);
    void CreateItemList_Delete_Calc1(char *select_name, string probe_exam,vector<int>& vecItemCalc1);
    void CreateItemList_Delete_Calc(char *select_name, char *department, vector<int>& vecDeleteCalc, IniFile *ini);
    GtkTreeIter InsertUniqueCalc(GtkTreeModel *model, const char *str);
    void CreateDefineItem_calc(vector<string>& vecExamItem_calc);
    int  create_exam_calc_model();
    int ItemNameTransEtype(char *select_name);
    // signal handle
    void ExamCalcChanged(GtkComboBox *widget);
    void ButtonBackOneClicked(GtkButton *button);
    void ButtonBackAllClicked(GtkButton *button);
    void ButtonSelectOneCalcClicked(GtkButton *button);
    void ButtonSelectAllCalcClicked(GtkButton *button);
    void ButtonAddClicked(GtkButton *button);
    void ButtonDeleteSelectClicked(GtkButton *button);
    void ButtonDeleteAllClicked(GtkButton *button);
    void ButtonUpClicked(GtkButton *button);
    void ButtonDownClicked(GtkButton *button);
    void ButtonExportClicked(GtkButton *button);
    void ButtonImportClicked(GtkButton *button);
    void ButtonDefaultClicked(GtkButton *button);
    void MeasureSequenceChanged(GtkComboBox *widget);
    void DepartmentCalcChanged(GtkComboBox *widget);
    static void HandleDepartmentCalcChanged(GtkComboBox *widget, CalcSetting *data) {
        data->DepartmentCalcChanged(widget);
    }

    static void HandleMeasureSequenceChanged(GtkComboBox *widget, CalcSetting *data) {
        data->MeasureSequenceChanged(widget);
    }

    static void HandleExamCalcChanged(GtkComboBox *widget, CalcSetting *data) {
        data->ExamCalcChanged(widget);
    }

    static void HandleButtonSelectOneCalcClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonSelectOneCalcClicked(button);
    }
    static void HandleButtonSelectAllCalcClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonSelectAllCalcClicked(button);
    }

    static void HandleButtonDeleteSelectClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonDeleteSelectClicked(button);
    }
    static void HandleButtonAddClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonAddClicked(button);
    }

    static void HandleButtonBackOneClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonBackOneClicked(button);
    }

    static void HandleButtonBackAllClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonBackAllClicked(button);
    }

    static void HandleButtonDownClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonDownClicked(button);
    }
    static void HandleButtonUpClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonUpClicked(button);
    }

    static void HandleButtonExportClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonExportClicked(button);
    }
    static void HandleButtonImportClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonImportClicked(button);
    }

    static void HandleButtonDefaultClicked(GtkButton *button, CalcSetting *data) {
        if (data)
            data->ButtonDefaultClicked(button);
    }

};
#endif
