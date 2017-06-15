#ifndef __VIEW_SYSTEM_H__
#define __VIEW_SYSTEM_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

#include <sqlite3.h>
#include "probe/ExamItem.h"

#define MAXNUMBER 100

extern string g_user_configure;

class SysCalculateSetting;
class SysGeneralSetting;
class SysMeasurementSetting;
class SysNoteSetting;
class SysOptions;

struct CommonPrinter {
  string brand;
  string model;
};

struct ExamPara {
  string dept_name;
  string name;
  ExamItem::EItem index;
};

int _system_(const string cmd);

class ViewCustomOB: public FakeXEvent {
public:
  static ViewCustomOB* GetInstance();

public:
  ~ViewCustomOB();

  void CreateWindow(GtkWidget* parent);
  void DestroyWindow();

private:
  // signal

  static void signal_button_clicked_save(GtkButton* button, ViewCustomOB* data) {
    if (data != NULL) {
      data->ButtonClickedSave(button);
    }
  }

  static void signal_button_clicked_clear(GtkButton* button, ViewCustomOB* data) {
    if (data != NULL) {
      data->ButtonClickedClear(button);
    }
  }

  static void signal_button_clicked_exit(GtkButton* button, ViewCustomOB* data) {
    if (data != NULL) {
      data->ButtonClickedExit(button);
    }
  }

  static void signal_combobox_changed(GtkComboBox *combobox, ViewCustomOB *data) {
    data->ComboBoxChanged(combobox);
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewCustomOB* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static void signal_cell_edited(GtkCellRenderer* cell, gchar* path_string, gchar* new_text, ViewCustomOB* data) {
    if (data != NULL) {
      data->CellEdited(cell, path_string, new_text);
    }
  }

  static void signal_cell_editing_started(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path, ViewCustomOB* data) {
    if (data != NULL) {
      data->CellEditingStarted(cell, editable, path);
    }
  }

  static void signal_entry_insert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewCustomOB* data) {
    if (data != NULL) {
      data->EntryInsert(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_callback_exitwindow(gpointer data) {
    ViewCustomOB* ob = (ViewCustomOB*)data;

    if (ob != NULL) {
      ob->DestroyWindow();
    }

    return FALSE;
  }

  // signal

  void ButtonClickedSave(GtkButton* button);
  void ButtonClickedClear(GtkButton* button);
  void ButtonClickedExit(GtkButton* button);
  void ComboBoxChanged(GtkComboBox* combobox);

  void CellEdited(GtkCellRenderer* cell, gchar* path_string, gchar* new_text);
  void CellEditingStarted(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path);
  void EntryInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);

private:
  ViewCustomOB();

  void KeyEvent(unsigned char keyValue);

  GtkWidget* CreateCustomOB();
  void InitCustomOB();

  void UpdateList(int type);
  void InsertList(GtkTreeModel* model, int table[]);

  void AddToTable(gint type, int temp1, float temp2);

private:
  static ViewCustomOB* m_instance;

private:
  struct CustomData {
    char* gw;
    char* gw1;
    char* gw2;
    char* gw3;
    char* gw4;
    char* gw5;
    char* gw6;
    char* gw7;
    char* gw8;
    char* gw9;
    char* gw10;
  };

  GtkDialog* m_dialog;
  GtkComboBoxText* m_combobox_text;

  GtkTreeModel* m_modelCustomOB;
  GtkTreeView* m_treeCustomOB;

  GArray* m_arrayGW;
  CustomData m_data;
};

class ViewSystem: public FakeXEvent {
public:
  static ViewSystem* GetInstance();

public:
  ~ViewSystem();

public:
  void CreateWindow();
  void DestroyWindow();

  GtkWidget* GetWindow();
  string GetUserName();

  bool CheckFlagFromReportTemplet(int id);

  void CreateDefineItemFormUsbToHost(string name);
  void ChangeNoteBookPage(int page);
  void SetActiveUser(gint num);
  void ShowList(const string name);

  void UpdateUserItem();
  void UpdateSpecificPrinterModel();

private:
  // signal

  static void signal_button_clicked_apply(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonClickedApply(button);
    }
  }

  static void signal_button_clicked_save(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonClickedSave(button);
    }
  }

  static void signal_button_clicked_exit(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonClickedExit(button);
    }
  }

  static void signal_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewSystem *data) {
    if (data != NULL) {
      data->NotebookChanged(notebook, page, page_num);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewSystem* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_callback_poweroff(gpointer data) {
    execlp("poweroff", "poweroff", NULL);
    perror("PowerOff Failed");

    return FALSE;
  }

  static gboolean signal_callback_exit_window_system(gpointer data) {
    ViewSystem* system = (ViewSystem*)data;

    if (system != NULL) {
      system->DestroyWindow();
    }

    return FALSE;
  }

  // General
  static void signal_entry_insert_hospital(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewSystem* data) {
    if (data != NULL) {
      data->EntryHospitalInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_spinbutton_insert_hour(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewSystem* data) {
    if (data != NULL) {
      data->SpinbuttonInsertHour(editable, new_text, new_text_length, position);
    }
  }

  static void signal_spinbutton_insert_time(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewSystem* data) {
    if (data != NULL) {
      data->SpinbuttonInsertTime(editable, new_text, new_text_length, position);
    }
  }

  static void signal_button_clicked_adjust_time(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnAdjustTimeClicked(button);
    }
  }

  static void signal_combobox_changed_language(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->LanguageChanged(combobox);
    }
  }

  static void signal_combobox_changed_vga(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->VgaChanged(combobox);
    }
  }

  static void signal_button_clicked_general_default(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnGeneralDefaultClicked(button);
    }
  }

  // Options

  static void signal_button_clicked_options_default(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnOptionsDefaultClicked(button);
    }
  }

  // Image Preset

  static void signal_entry_insert_text_userselect(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewSystem* data) {
    if (data != NULL) {
      data->EntryNameInsert(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_userselect(GtkWidget* widget, GdkEventFocus* event, ViewSystem* data) {
    if (data != NULL) {
      data->UserSelectFocusOut(widget, event);
    }

    return FALSE;
  }

  static void signal_button_clicked_delete_user(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnDeleteUserClicked(button);
    }
  }

  static void signal_combobox_changed_user_select(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->UserChanged(combobox);
    }
  }

  static void signal_combobox_changed_probe_type(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->ProbeTypeChanged(combobox);
    }
  }

  static void signal_renderer_insert_user_item(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path, ViewSystem* data) {
    if (data != NULL) {
      data->EntryItemInsert(cell, editable, path);
    }
  }

  static void signal_renderer_rename(GtkCellRendererText* renderer, gchar* path, gchar* new_text, ViewSystem* data) {
    if (data != NULL) {
      data->CellRendererRename(renderer, path, new_text);
    }
  }

  static void signal_combobox_changed_exam_type(GtkTreeSelection* selection, ViewSystem* data) {
    if (data != NULL) {
      data->ExamTypeChanged(selection);
    }
  }

  static void signal_treeview_button_release_exam_department(GtkWidget* widget, GdkEventButton* event, ViewSystem* data) {
    if (data != NULL) {
      data->BtnExamDepartmentClicked(widget, event);
    }
  }

  static void signal_button_clicked_add_item(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->AddItemClicked(button);
    }
  }

  static void signal_button_clicked_delete_item(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->DeleteItemClicked(button);
    }
  }

  static void signal_button_clicked_image_save(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnImageSaveClicked(button);
    }
  }

  static void signal_button_clicked_image_new(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnImageNewClicked(button);
    }
  }

  static void signal_button_clicked_image_export(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnImageExportToUSBClicked(button);
    }
  }

  static void signal_button_clicked_image_import(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnImageImportFromUSBClicked(button);
    }
  }

  static void signal_button_clicked_image_default(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnImageDefaultClicked(button);
    }
  }

  // Measure

  static void signal_togglebutton_toggled_tamax(GtkToggleButton* togglebutton, ViewSystem* data) {
    if (data != NULL) {
      data->ChkBtnTAmaxToggled(togglebutton);
    }
  }

  static void signal_togglebutton_toggled_pi(GtkToggleButton* togglebutton, ViewSystem* data) {
    if (data != NULL) {
      data->ChkBtnPIToggled(togglebutton);
    }
  }

  static void signal_togglebutton_toggled_hr(GtkToggleButton* togglebutton, ViewSystem* data) {
    if (data != NULL) {
      data->ChkBtnHRToggled(togglebutton);
    }
  }

  static void signal_button_clicked_measure_default(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnMeasureDefaultClicked(button);
    }
  }

  // Calc And Caliper

  static void on_calc_notebook_switch_page(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num, ViewSystem* data) {
    if (data != NULL) {
      data->CalcnotebookChanged(notebook, page, page_num);
    }
  }

  // Calculate

  static void signal_button_clicked_ob_custom(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnOBCustomClicked(button);
    }
  }

  static void signal_button_clicked_calc_default(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnCalculateDefaultClicked(button);
    }
  }

  // Comment

  static void signal_combobox_changed_probe_comment(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->ProbeCommentChanged(combobox);
    }
  }

  static void signal_combobox_changed_exam_comment(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->ExamCommentChanged(combobox);
    }
  }

  static void signal_combobox_changed_department_comment(GtkComboBox* combobox, ViewSystem* data) {
    if (data != NULL) {
      data->DepartmentCommentChanged(combobox);
    }
  }

  static void signal_button_clicked_selectone_comment(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonSelectOneCommentClicked(button);
    }
  }
  static void signal_button_clicked_selectall_comment(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonSelectAllCommentClicked(button);
    }
  }
  static void signal_button_clicked_add(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonAddClicked(button);
    }
  }
  static void signal_button_clicked_deleteselect(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonDeleteSelectClicked(button);
    }
  }

  static void signal_button_clicked_up(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonUpClicked(button);
    }
  }

  static void signal_button_clicked_down(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonDownClicked(button);
    }
  }

  static void signal_button_clicked_delete(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonDeleteClicked(button);
    }
  }

  static void signal_button_clicked_deleteall(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->ButtonDeleteAllClicked(button);
    }
  }

  static void on_button_comment_default_clicked(GtkButton* button, ViewSystem* data) {
    if (data != NULL) {
      data->BtnCommentDefaultClicked(button);
    }
  }

  static void signal_renderer_rename_selectcomment(GtkCellRendererText* cellrenderer, gchar* path, gchar* new_text, ViewSystem* data) {
    if (data != NULL) {
      data->CellRendererRenameSelectComment(cellrenderer, path, new_text);
    }
  }

  static void signal_renderer_rename_comment(GtkCellRendererText* cellrenderer, gchar* path, gchar* new_text, ViewSystem* data) {
    if (data != NULL) {
      data->CellRendererRenameComment(cellrenderer, path, new_text);
    }
  }

  // Peripheral

  // signal

  void ButtonClickedApply(GtkButton* button);
  void ButtonClickedSave(GtkButton* button);
  void ButtonClickedExit(GtkButton* button);
  void NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num);

  // General
  void EntryHospitalInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void SpinbuttonInsertTime(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void SpinbuttonInsertHour(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void BtnAdjustTimeClicked(GtkButton* button);
  void LanguageChanged(GtkComboBox* combobox);
  void VgaChanged(GtkComboBox* combobox);
  void BtnGeneralDefaultClicked(GtkButton* button);

  // Options
  void BtnOptionsDefaultClicked(GtkButton* button);

  // Image Preset
  void EntryNameInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void UserSelectFocusOut(GtkWidget* widget, GdkEventFocus* event);
  void BtnDeleteUserClicked(GtkButton* button);
  void UserChanged(GtkComboBox* combobox);
  void ProbeTypeChanged(GtkComboBox* combobox);
  void EntryItemInsert(GtkCellRenderer* cell, GtkCellEditable* editable, const gchar* path);
  void CellRendererRename(GtkCellRendererText* renderer, gchar* path, gchar* new_text);
  void ExamTypeChanged(GtkTreeSelection* selection);
  void BtnExamDepartmentClicked(GtkWidget* widget, GdkEventButton* event);
  void AddItemClicked(GtkButton* button);
  void DeleteItemClicked(GtkButton* button);
  void BtnImageSaveClicked(GtkButton* button);
  void BtnImageNewClicked(GtkButton* button);
  void BtnImageExportToUSBClicked(GtkButton* button);
  void BtnImageImportFromUSBClicked(GtkButton* button);
  void BtnImageDefaultClicked(GtkButton* button);

  // Measure
  void ChkBtnTAmaxToggled(GtkToggleButton* togglebutton);
  void ChkBtnPIToggled(GtkToggleButton* togglebutton);
  void ChkBtnHRToggled(GtkToggleButton* togglebutton);
  void BtnMeasureDefaultClicked(GtkButton* button);

  // Calc And Caliper
  void CalcnotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num);

  // Calculate
  void BtnOBCustomClicked(GtkButton* button);
  void BtnCalculateDefaultClicked(GtkButton* button);

  // Comment
  void ProbeCommentChanged(GtkComboBox* combobox);
  void ExamCommentChanged(GtkComboBox* combobox);
  void DepartmentCommentChanged(GtkComboBox* combobox);
  void ButtonSelectOneCommentClicked(GtkButton* button);
  void ButtonSelectAllCommentClicked(GtkButton* button);
  void ButtonAddClicked(GtkButton* button);
  void ButtonDeleteSelectClicked(GtkButton* button);
  void ButtonUpClicked(GtkButton* button);
  void ButtonDownClicked(GtkButton* button);
  void ButtonDeleteClicked(GtkButton* button);
  void ButtonDeleteAllClicked(GtkButton* button);
  void BtnCommentDefaultClicked(GtkButton* button);
  void CellRendererRenameSelectComment(GtkCellRendererText* cellrenderer, gchar* path_str, gchar* new_text);
  void CellRendererRenameComment(GtkCellRendererText* cellrenderer, gchar* path_str, gchar* new_text);

  // Peripheral

  // Custom Report

  // DICOM

  // P1-P3

  // System Info

private:
  ViewSystem();

  void KeyEvent(unsigned char keyValue);

  GtkWidget* create_note_general();
  void init_general_setting();
  void save_general_setting();

  GtkWidget* create_note_options();
  void init_option_setting(SysOptions* sysOptions);
  void save_option_setting();

  GtkWidget* create_note_image();
  void init_image_setting();
  void save_image_setting();

  GtkWidget* create_note_measure();
  void init_measure_setting(SysMeasurementSetting* sysMeasure);
  void save_measure_setting();

  GtkWidget* create_note_calc_measure();

  GtkWidget* create_note_calc();
  void init_calc_setting(SysCalculateSetting* sysCalc);
  void save_calc_setting();

  GtkWidget* create_note_comment();
  void init_comment_setting(SysNoteSetting* sysNote);
  void save_comment_setting();

  void create_note_tvout();
  void init_tvout_setting(SysGeneralSetting* sysGeneralSetting);
  void save_tvout_setting();

  GtkWidget* create_set_report();

  GtkWidget* create_note_dicom();
  void init_dicom_setting();
  void save_dicom_setting();

  GtkWidget* create_note_key_config();
  void init_key_config();
  void save_key_config();

  GtkWidget* create_note_info();
  void init_info_setting();

private:
  static ViewSystem* m_instance;

private:
  GtkDialog* m_dialog;
  GtkButton* m_button_apply;
  GtkButton* m_button_save;
  GtkButton* m_button_exit;
  GtkNotebook* m_notebook;

  GtkBox* m_box_peripheral;

  // General
  GtkEntry* m_entry_hospital;
  GtkWidget* m_calendar;
  GtkSpinButton* m_spinbutton_hour;
  GtkSpinButton* m_spinbutton_minute;
  GtkSpinButton* m_spinbutton_second;
  GtkComboBoxText* m_combobox_language;
  GtkComboBoxText* m_combobox_vga;
  GtkComboBoxText* m_combobox_date_format;
  GtkComboBoxText* m_combobox_screen_saver;

  // Options
  GtkRadioButton* m_radiobutton_tic;
  GtkRadioButton* m_radiobutton_tib;
  GtkRadioButton* m_radiobutton_tis;
  GtkRadioButton* m_radiobutton_keyboard_sound_on;
  GtkRadioButton* m_radiobutton_keyboard_sound_off;
  GtkRadioButton* m_radiobutton_center_line_on;
  GtkRadioButton* m_radiobutton_center_line_off;
  GtkRadioButton* m_radiobutton_display_format_m_1;
  GtkRadioButton* m_radiobutton_display_format_m_2;
  GtkRadioButton* m_radiobutton_display_format_m_3;
  GtkRadioButton* m_radiobutton_display_format_m_4;
  GtkRadioButton* m_radiobutton_display_format_m_5;
  GtkRadioButton* m_radiobutton_display_format_pw_cw_1;
  GtkRadioButton* m_radiobutton_display_format_pw_cw_2;
  GtkRadioButton* m_radiobutton_display_format_pw_cw_3;
  GtkRadioButton* m_radiobutton_display_format_pw_cw_4;
  GtkRadioButton* m_radiobutton_display_format_pw_cw_5;
  GtkRadioButton* m_radiobutton_mouse_speed_high;
  GtkRadioButton* m_radiobutton_mouse_speed_middle;
  GtkRadioButton* m_radiobutton_mouse_speed_low;
  GtkComboBoxText* m_combobox_img_format;
  GtkComboBoxText* m_combobox_img_file;
  GtkComboBoxText* m_combobox_video_format;
  GtkComboBoxText* m_combobox_video_file;

  // Image Preset
  GtkComboBoxText* m_combobox_probe_type;
  GtkTreeView* m_treeview_exam_type;
  GtkComboBoxEntry* m_comboboxentry_user_select;
  GtkFrame* m_frame_new_check_part;
  GtkLabel* m_label_check_part;
  GtkEntry* m_entry_new_check_part;

  string m_str_index;

  // Measure
  GtkComboBoxText* m_combobox_unit_dist;
  GtkComboBoxText* m_combobox_unit_area;
  GtkComboBoxText* m_combobox_unit_vol;
  GtkComboBoxText* m_combobox_unit_time;
  GtkComboBoxText* m_combobox_unit_vel;
  GtkComboBoxText* m_combobox_unit_accel;
  GtkComboBoxText* m_combobox_unit_efw;
  GtkComboBoxText* m_combobox_heart_beat;
  GtkCheckButton* m_checkbutton_autocalc_ps;
  GtkCheckButton* m_checkbutton_autocalc_ed;
  GtkCheckButton* m_checkbutton_autocalc_ri;
  GtkCheckButton* m_checkbutton_autocalc_sd;
  GtkCheckButton* m_checkbutton_autocalc_tamax;
  GtkCheckButton* m_checkbutton_autocalc_pi;
  GtkCheckButton* m_checkbutton_autocalc_time;
  GtkCheckButton* m_checkbutton_autocalc_hr;
  GtkCheckButton* m_checkbutton_autocalc_pgmax;
  GtkCheckButton* m_checkbutton_autocalc_pgmean;
  GtkComboBoxText* m_combobox_current_line;
  GtkComboBoxText* m_combobox_confirmed_line;
  GtkRadioButton* m_radiobutton_report_last;
  GtkRadioButton* m_radiobutton_report_average;
  GtkRadioButton* m_radiobutton_result_small;
  GtkRadioButton* m_radiobutton_result_big;
  GtkRadioButton* m_radiobutton_trace_point;
  GtkRadioButton* m_radiobutton_trace_track;
  GtkRadioButton* m_radiobutton_trace_auto;
  GtkRadioButton* m_radiobutton_cursor_big;
  GtkRadioButton* m_radiobutton_cursor_mid;
  GtkRadioButton* m_radiobutton_cursor_small;
  GtkRadioButton* m_radiobutton_measure_line_on;
  GtkRadioButton* m_radiobutton_measure_line_off;

  // Calculate
  GtkComboBoxText* m_combobox_ob_cer;
  GtkComboBoxText* m_combobox_ob_hl;
  GtkComboBoxText* m_combobox_ob_bpd;
  GtkComboBoxText* m_combobox_ob_fl;
  GtkComboBoxText* m_combobox_ob_crl;
  GtkComboBoxText* m_combobox_ob_gs;
  GtkComboBoxText* m_combobox_ob_ac;
  GtkComboBoxText* m_combobox_ob_hc;
  GtkComboBoxText* m_combobox_ob_tad;
  GtkComboBoxText* m_combobox_ob_apad;
  GtkComboBoxText* m_combobox_ob_thd;
  GtkComboBoxText* m_combobox_ob_ofd;
  GtkComboBoxText* m_combobox_ob_efw;
  GtkComboBoxText* m_combobox_bsa;

  // Comment
  GtkComboBoxText* m_combobox_probe_comment;
  GtkComboBoxText* m_combobox_exam_comment;
  GtkComboBoxText* m_combobox_department_comment;
  GtkScrolledWindow* m_scrolledwindow_item_comment;
  GtkScrolledWindow* m_scrolledwindow_item_comment_selected;
  GtkTreeView* m_treeview_item_comment;
  GtkTreeView* m_treeview_item_comment_selected;
  GtkCellRenderer* m_cellrenderer_comment_text;
  GtkCellRenderer* m_cellrenderer_comment_text_selected;
  GtkRadioButton* m_radiobutton_font_big;
  GtkRadioButton* m_radiobutton_font_mid;
  GtkRadioButton* m_radiobutton_font_small;
  GtkRadioButton* m_radiobutton_bodymark_big;
  GtkRadioButton* m_radiobutton_bodymark_mid;
  GtkRadioButton* m_radiobutton_bodymark_small;
  GtkComboBoxText* m_combobox_font_color;
  GtkComboBoxText* m_combobox_bodymark_color;




public:
    static const int MAX_KEY = 10;//8;//9;

    void DoDicomTest();
    int GetProbeType();
    int GetCommentProbeType();
    int GetKbType();
    void UpdateHospitalandpart(int date_format, const string hospital_name);

    int get_active_user();
    void save_itemIndex(int itemIndex);
    int  get_itemIndex();
    void save_itemName(char *itemName);

    void get_itemName(char *itemName);
    //char *get_itemName();

    void SetImagePara(const ExamItem::ParaItem &item);
    void GetImagePara(ExamItem::ParaItem &item);
    void save_image_para(ExamItem::ParaItem &item);
    //void GetItemListOfKb(char* KbModel, vector<enum PItem> *ptrItemList);
    void tree_auto_scroll(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, gpointer user_data);

    ////>option fuction
    void UpdateOptionStatus(bool status);
private:


    int m_kbIndex;
    bool m_vgaInterl;
    //PItem m_itemIndex;




    bool m_imageItemSensitive;
    int m_itemIndex;
    char *m_itemName;
    GtkCellRenderer *m_renderer1;
    //GtkWidget *m_button_add;
    GtkWidget *m_button_rename;
    //GtkWidget *m_button_delete_item;
    //GtkWidget *m_frame_new_check_part;
    //GtkWidget *m_label_check_part;
    int m_save_or_new_flag;


    int m_current_note_page;

    // GtkWidget *m_notebookPrinter;
    GtkWidget *m_radiobutton_common;
    GtkWidget *m_radiobutton_specific;
    GtkWidget *m_scrolledwindow_common;
    GtkWidget *m_fixed_specific;

    GtkTreeSelection *m_selected_common_printer;
    GtkTreeSelection *m_selected_specific_printer;
    GtkWidget *m_treeview_common_print;
    GtkWidget *m_treeview_specific_print;




    //comment



    GtkWidget* m_scrolled_all;
    GtkWidget* m_treeView_all;


    //vector<string> vecItemComment;
    vector<string> vecExamItem_comment;
    // measure



    GtkWidget *m_combobox_unit_slope;

    GtkTreeIter topIter;
    GtkCellRenderer *m_renderer;
    GtkWidget *m_combobox_childsection;
    GtkWidget *m_entry_templet;

    GtkWidget* show_window;
    GtkWidget* m_WindowABD;
    GtkWidget* m_WindowUR;
    GtkWidget* m_WindowAE;
    GtkWidget* m_WindowOB;
    GtkWidget* m_WindowGYN;
    GtkWidget* m_WindowSP;
    GtkWidget* m_WindowFE;
    GtkWidget* m_WindowOTHERS;
    GtkWidget* m_WindowVES;
    GtkWidget* m_WindowTD;

    GtkWidget* m_WindowANOB;
    GtkWidget *m_FixedReportSet;
    GtkWidget *m_TreeviewReportSet;

    GtkWidget *button_add, *button_save, *button_delete, *button_ok, *button_cancel;
    GtkWidget *label_templet, *label_childsection;
    int init_language;
    guint m_flag_notebook_coustomreport;
    guint m_flag_notebook_image;
    //GtkWidget *m_entry_new_check_part;

    ///> optional
    GtkWidget *m_treeOption ;
    GtkWidget *m_frameRegisterInfo;
    GtkWidget *m_labelExport;
    GtkWidget *m_entryRegisterKey;
    GtkWidget *m_labelRegister;
    bool m_powerOffFlag;

    bool StrCmpSectionString(int section, const string string, int* language);
    bool StrCmpCustomString(const string str, int* language);
    bool StrCmpTemplet1String(const string str, int* language);

    void InitReportVar();
    bool OpenDB();
    bool CloseDB();
    bool InitDB();
    bool LocalizationDB();

    void AddTempletBySection( sqlite3 *db);
    void BtnRecoveryClicked(GtkButton *button);

    void ChkBtnClicked(GtkButton *button);

    GtkWidget *AddFrameByTitle(char *title, int section, int startid, int endid, int *h);
    GtkWidget *AddFrameByTitle2(char *title, int section, int startid, int endid, int *h);
    GtkWidget *AddFrameForOthers(const char *title, int *h);

    GtkWidget *CreatWindowABD();
    GtkWidget *CreatWindowAE();
    GtkWidget *CreatWindowUR();
    GtkWidget *CreatWindowOB();
    GtkWidget *CreatWindowGYN();
    GtkWidget *CreatWindowSP();
    GtkWidget *CreatWindowFE();
    GtkWidget *CreatWindowOTHERS();
    GtkWidget *CreatWindowVES();
    GtkWidget *CreatWindowTD();
    GtkWidget *CreatWindowAnOB();

    GtkTreeModel *CreateTreeModel();
    void TreeSelectionChanged(GtkTreeSelection *selection);
    void BtnAddClicked(GtkButton *button);
    void BtnSave2Clicked(GtkButton *button);
    void BtnDeleteClicked(GtkButton *button);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);


    void ChangeCommentExamBox(int probe_type, char *check_part);
    void ChangeCommentExamBoxDelete(int probe_type);

    void EntryItemLengthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void CreateDefineItem_comment(int probeIndex, vector<string>& vecExamItem);
    void CreateDefineItem(int probeIndex, vector<ExamPara>& vecExamItem);
    bool RenameNotice(int probe_type_index, char *new_text, char *dialognotice, int group_length);
    void TransEnglish(char *strname, char str[256],char *str_indexname,char str_index[256]);

    std::vector <string> GetUserGroup();
    static void  on_combobox_video_changed(GtkComboBox *widget,ViewSystem *data) {
        data->BtnComboVideoChanged();
    }

    static void on_entry_insert_custom_item(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->EntryItemLengthInsert(editable, new_text, new_text_length, position);
    }

    void BtnComboVideoChanged();




    void RenameItemClicked(GtkButton *button);
    static void HandleRenameItemClicked(GtkButton *button, ViewSystem *data) {
        data->RenameItemClicked(button);
    }





    bool ExamTypeTestRowExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path);


    static void HandleTreeSelectionChanged(GtkTreeSelection *selection, ViewSystem *data) {
        data->TreeSelectionChanged(selection);
    }
    static void HandleChkBtnClicked(GtkButton *button, ViewSystem *data) {
        data->ChkBtnClicked(button);
    }
    static void on_button_recovery_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnRecoveryClicked(button);
    }
    static void on_button_add_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnAddClicked(button);
    }
    static void on_button_save2_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnSave2Clicked(button);
    }
    static void on_button_delete_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnDeleteClicked(button);
    }
    static void on_button_ok_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnOkClicked(button);
    }
    static void on_button_cancel_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnCancelClicked(button);
    }



    bool UniqueItem(const string sections, const string templet = "%", const string childsection = "%");
    GtkTreeIter InsertUnique(GtkTreeModel* model, GtkTreeIter* iter, const string str);
    GtkTreeIter InsertUniqueComment(GtkTreeModel* model, const string str);

    bool InitRecordFromShowArr(const string childsection, int* start, int* end);
    bool ReadRecordFromDB(const string sections, const string templet, const string childsection);
    void apply_report_setting();

    void EntrytempletInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void BtnNewCheckPartOkClicked(GtkButton *button);
    void BtnNewCheckPartCancelClicked(GtkButton *button);

    static void on_entry_templet_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->EntrytempletInsert(editable, new_text, new_text_length, position);
    }

    void GetImageNoteSelection(int &probeIndex, int &itemIndex, char *&itemName);

    //signal handle


    //signal

    static void HandleNewCheckPartBtnOk(GtkButton *button, ViewSystem *data) {
        data->BtnNewCheckPartOkClicked(button);
    }
    static void HandleNewCheckPartBtnCancel(GtkButton *button, ViewSystem *data) {
        data->BtnNewCheckPartCancelClicked(button);
    }


    // TV Out
    GtkWidget *m_fixed_tvout;
    GtkWidget *m_combobox_video;
    GtkWidget *m_combobox_connect_type;
    GtkWidget *m_radiobutton_format_1;
    GtkWidget *m_radiobutton_format_2;
    GtkWidget *m_radiobutton_format_3;
    int m_flag_notebook_tvout;

    //dicom
    GtkWidget *m_dicom_notebook;
    int  m_page_num ;

    // key define
    bool m_p1Selected;
    GtkWidget *m_entry_function_p1;
    GtkWidget *m_entry_function_p2;
    GtkWidget *m_entry_function_p3;
    int m_p1_func_index;
    int m_p2_func_index;
    int m_p3_func_index;

    GtkWidget *m_radiobutton_key_p1[MAX_KEY];
    GtkWidget *m_radiobutton_key_p2[MAX_KEY];
    GtkWidget *m_radiobutton_key_p3[MAX_KEY];


    //info
    GtkWidget *m_textview_version;

    //image
    /*GtkWidget *m_combobox_mbp;
    GtkWidget *m_combobox_ao_power;
    GtkWidget *m_spinbutton_2d_gain;
    GtkWidget *m_combobox_agc;
    GtkWidget *m_combobox_edge_enh;
    GtkWidget *m_combobox_steer;
    GtkWidget *m_combobox_focus_sum;
    GtkWidget *m_combobox_focus_pos;
    GtkWidget *m_combobox_depth;
    GtkWidget *m_combobox_chroma;
    GtkWidget *m_combobox_lr;
    GtkWidget *m_combobox_ud;*/

    //comment

    GtkWidget *m_combobox_polarity;
    GtkWidget *m_combobox_frame;
    GtkWidget *m_combobox_line_aver;
    GtkWidget *m_combobox_gray_trans; //lhm
    GtkWidget *m_combobox_space_compound;
    GtkWidget *m_combobox_freq_compound;
    GtkWidget *m_combobox_restric;
    GtkWidget *m_combobox_thi;
    GtkWidget *m_combobox_tsi;
    GtkWidget *m_combobox_imgEnh;
    GtkWidget *m_spinbutton_m_gain;
    GtkWidget *m_combobox_scan_range;
    GtkWidget *m_combobox_dynamic_range;
    GtkWidget *m_combobox_2d_freq;
    GtkWidget* m_combobox_thi_freq;
    GtkWidget *m_combobox_2d_line_density;
    GtkWidget *m_combobox_2d_smooth;
    GtkWidget *m_spinbutton_pw_gain;
    GtkWidget *m_spinbutton_pw_angle;
    GtkWidget *m_combobox_pw_scale_prf;
    GtkWidget *m_combobox_time_resolution;
    GtkWidget *m_combobox_pw_freq;
    GtkWidget *m_combobox_pw_wallfilter;
    GtkWidget *m_combobox_pw_invert;
    GtkWidget *m_combobox_cfm_line_density;
    GtkWidget *m_combobox_cfm_scale_prf;
    GtkWidget *m_spinbutton_cfm_gain;
    GtkWidget *m_combobox_cfm_wallfilter;
    GtkWidget *m_combobox_variance;
    GtkWidget *m_combobox_cfm_invert;
    GtkWidget *m_combobox_persistence;
    GtkWidget *m_combobox_cfm_smooth;
    GtkWidget *m_combobox_color_rejection;
    // GtkWidget *m_spinbutton_pdi_gain;
    GtkWidget *m_combobox_sensitivity;

    GtkTreeIter m_optionIter;








    void save_new_item();
    void init_printer(SysGeneralSetting *sysGeneralSetting);
    void CreatePrinter();











    GtkTreeModel* create_common_print_model();
    GtkTreeModel* create_specific_print_model();
    GtkWidget* create_common_print_treeview();
    GtkWidget* create_specific_print_treeview();

    int common_printer_selection();
    std::string specific_printer_selection();

    // void add_print_column(GtkTreeView *treeview);
    GtkWidget * create_key_function_treeview(const std::string function_list[], unsigned int size);
    GtkTreeModel* create_exam_item_model(std::vector<ExamItem::EItem> index);
    GtkTreeModel* create_item_comment_model(int index);
    GtkTreeModel* create_item_comment_model_selected();

    int  DepartmentIndex();
    void create_exam_comment_model(std::vector<ExamItem::EItem> index);
    string DepartmentName(int index);
    void CreateItemList_Comment(string department,vector<ExamPara>& vecItemComment, int number);
    void CreateItemList_Note(string department,vector<ExamPara>& vecItemComment, int number);
    void CreateItemList_Comment1(string probe_exam,vector<ExamPara>& vecItemComment1);
    void CreateItemList_Delete_Comment1(string select_name, string probe_exam,vector<ExamPara>& vecItemComment1);
    void CreateItemList_Delete_Comment(string select_name, string department, vector<ExamPara>& vecDeleteComment);
    void add_exam_item_column(GtkTreeView *treeview);
    void init_image_para();
    void set_image_item_sensitive(bool status);
    void image_para_combo_box_set(GtkWidget *combobox, int value);
    void image_para_restrict(int probeIndex);
    // signal handle

    void CommonRadioToggled(GtkToggleButton *togglebutton);
    void SpecificRadioToggled(GtkToggleButton *togglebutton);
    void BtnAddPrinterClicked(GtkButton *button);
    void BtnDelPrinterClicked(GtkButton *button);






    void BtnTVOutDefaultClicked(GtkButton *button);
    void ComboNetMethodChanged(GtkComboBox *widget);
    void BtnDicomTest(GtkButton *button);
    void BtnDicomReset(GtkButton *button);
    void EntryIPInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    //    void EntryIPChanged(GtkEditable *editable);
    void EntryDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void EntryFracDigitInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);




    //void ChkBtnTotalToggled(GtkToggleButton *togglebutton);

    void AddCheckPart(char *checkpart);

    void RadioP1Toggled(GtkToggleButton *togglebutton);

    void RadioP2Toggled(GtkToggleButton *togglebutton);
    void TreeFuncChanged(GtkTreeSelection *selection);





    void BtnUpgradeClicked(GtkButton *button);

    void image_default_setting();
    void BtnImageGetCurrentClicked(GtkButton *button);

    void get_current_and_save_image_para();



    void ComboFocusSum(GtkComboBox *box);
    void ComboPwPrf(GtkComboBox *box);
    void ComboCfmPrf(GtkComboBox *box);
    void ComboSensitivity(GtkComboBox *box);
    void ClearComboBox(GtkComboBox *box);

    void SpinbuttonInsertGain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    void add_columns_comment(GtkTreeView* treeview);
    void add_columns_comment_selected(GtkTreeView* treeview);

    void SpinbuttonInsertAngle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);
    gboolean SpinbuttonOutputAngle(GtkSpinButton *spin);
    void CommonTreeviewSelectionChanged(GtkTreeSelection *treeselection);
    void SpecificTreeviewSelectionChanged(GtkTreeSelection *treeselection);
    void ModebmRadioToggled(GtkToggleButton *togglebutton);
    void ModebpwRadioToggled(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_total(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud11(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud21(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_ud12(GtkToggleButton *togglebutton);
    void OnRadiobtnDisplay_lr11(GtkToggleButton *togglebutton);


    void DicomnotebookChanged(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num);









    ///>optional function
    void TreeCursorChanged(GtkTreeView *treeview);
    int OptionOptEvent(GtkWidget *widget, GdkEvent *event);
    void MenuInstallActivate(GtkMenuItem *menuitem);
    void MenuUninstallActivate(GtkMenuItem *menuitem);
    void BtnExportClicked(GtkButton *button);
    void BtnRegisterClicked(GtkButton *button);


    // signal connect



    static void on_common_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->CommonRadioToggled(togglebutton);
    }
    static void on_specific_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->SpecificRadioToggled(togglebutton);
    }
    static void on_button_add_printer_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnAddPrinterClicked(button);
    }
    static void on_button_del_printer_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnDelPrinterClicked(button);
    }






    static void on_button_tvout_default_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnTVOutDefaultClicked(button);
    }
    //static void HandleNetMethodChanged(GtkComboBox *widget, ViewSystem *data) { data->ComboNetMethodChanged(widget); }
    static void HandleDicomTest(GtkButton *button, ViewSystem *data) {
        data->BtnDicomTest(button);
    }
    static void HandleDicomReset(GtkButton *button, ViewSystem *data) {
        data->BtnDicomReset(button);
    }
    static void on_entry_ip_insert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->EntryFracDigitInsert(editable, new_text, new_text_length, position);
    }
    //static void on_entry_ip_changed(GtkEditable *editable, ViewSystem *data) { data->EntryIPChanged(editable); }
    static void on_entry_host_port(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->EntryDigitInsert(editable, new_text, new_text_length, position);
    }

    static void on_button_upgrade_clicked(GtkButton *button, ViewSystem *data) {
        data->BtnUpgradeClicked(button);
    }



    static void HandleP1Toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->RadioP1Toggled(togglebutton);
    }

    static void HandleP2Toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->RadioP2Toggled(togglebutton);
    }

    static void HandleTreeFuncChanged(GtkTreeSelection *selection, ViewSystem *data) {
        data->TreeFuncChanged(selection);
    }






    static bool HandleExamTypeExpandBefore(GtkTreeView *tree_view, GtkTreeIter *iter, GtkTreePath *path, ViewSystem *data) {
        return data->ExamTypeTestRowExpandBefore(tree_view, iter, path);
    }



    static void HandleImageBtnGetCurrent(GtkButton *button, ViewSystem *data) {
        data->BtnImageGetCurrentClicked(button);
    }




    static void HandleComboFocusSum(GtkComboBox *box, ViewSystem *data) {
        data->ComboFocusSum(box);
    }
    static void HandleComboPwPrf(GtkComboBox *box, ViewSystem *data) {
        data->ComboPwPrf(box);
    }
    static void HandleComboCfmPrf(GtkComboBox *box, ViewSystem *data) {
        data->ComboCfmPrf(box);
    }
    static void HandleComboCfmSensitivity(GtkComboBox *box, ViewSystem *data) {
        data->ComboSensitivity(box);
    }

    static void on_spinbutton_insert_gain(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->SpinbuttonInsertGain(editable, new_text, new_text_length, position);
    }
    static void on_spinbutton_insert_angle(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewSystem *data) {
        data->SpinbuttonInsertAngle(editable, new_text, new_text_length, position);
    }
    static gboolean on_spinbutton_output_angle(GtkSpinButton *spin, ViewSystem *data) {
        return data->SpinbuttonOutputAngle(spin);
    }

    static void on_common_treeview_selection_changed(GtkTreeSelection *treeselection, ViewSystem *data) {
        data->CommonTreeviewSelectionChanged(treeselection);
    }
    static void on_specific_treeview_selection_changed(GtkTreeSelection *treeselection, ViewSystem *data) {
        data->SpecificTreeviewSelectionChanged(treeselection);
    }



    static void on_modebm_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->ModebmRadioToggled(togglebutton);
    }
    static void on_modebpw_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->ModebpwRadioToggled(togglebutton);
    }
    static void on_display_ud11_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->OnRadiobtnDisplay_ud11(togglebutton);
    }
    static void on_display_ud21_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->OnRadiobtnDisplay_ud21(togglebutton);
    }
    static void on_display_ud12_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->OnRadiobtnDisplay_ud12(togglebutton);
    }
    static void on_display_total_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->OnRadiobtnDisplay_total(togglebutton);
    }
    static void on_display_lr11_radio_button_toggled(GtkToggleButton *togglebutton, ViewSystem *data) {
        data->OnRadiobtnDisplay_lr11(togglebutton);
    }





    static void on_dicom_notebook_switch_page(GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, ViewSystem *data) {
        data->DicomnotebookChanged(notebook, page, page_num);
    }












    ///>optional functions
    static void HandleOptionCursorChanged(GtkTreeView *treeview, ViewSystem *data) {
        return data->TreeCursorChanged(treeview);
    }
    static int HandleOptionOptEvent(GtkWidget *widget, GdkEvent *event, ViewSystem *data) {
        return data->OptionOptEvent(widget, event);
    }
    static void HandleMenuInstallActivate(GtkMenuItem *menuitem, ViewSystem *data) {
        data->MenuInstallActivate(menuitem);
    }
    static void HandleMenuUninstallActivate(GtkMenuItem *menuitem, ViewSystem *data) {
        data->MenuUninstallActivate(menuitem);
    }
    static void HandleBtnExportClicked(GtkButton *widget, ViewSystem *data) {
        return data->BtnExportClicked(widget);
    }
    static void HandleBtnRegisterClicked(GtkButton *widget, ViewSystem *data) {
        return data->BtnRegisterClicked(widget);
    }

};

#endif
