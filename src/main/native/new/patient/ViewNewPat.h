#ifndef __VIEW_NEW_PAT_H__
#define __VIEW_NEW_PAT_H__

#include "AbsCalendarOpr.h"
#include "patient/PatientInfo.h"
#include "thirdparty/DCMDef.h"
#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

class ViewNewPat: public FakeXEvent, public AbsCalendarOpr {
public:
  static ViewNewPat* GetInstance();

public:
  ~ViewNewPat();

  void CreateWindow();
  void DestroyWindow();

  GtkWidget* GetWindow();

  void UpdateNameLength();
  void UpdateStudyInfo();

  void ClearData();
  void ClearExamData();
  void ClearStudyInfo();

  bool GetClearStatus();

  bool GetMPPSFlag();
  void SetMPPSFlag(bool flag);

  void InsertPatientInfo(const string id, PatientInfo::Name patientName,
    string birthDate, string age, const string sex, const string size, const string weight,
    const string address, const string doctor, const string description);

  void SetStudyInfo(DCMWORKLISTELEMENT element);
  void SetStudyInfo(DCMSTUDYELEMENT element);

  void FillNewPat(const PatientInfo::Info &info);
  void SetSeneitive(bool sensitive);

public:

private:
  // signal

  static void signal_button_clicked_end_exam(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedEndExam(button);
    }
  }

  static void signal_button_clicked_new_patient(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedNewPatient(button);
    }
  }

  static void signal_button_clicked_new_exam(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedNewExam(button);
    }
  }

  static void signal_button_clicked_worklist(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedWorkList(button);
    }
  }

  static void signal_button_clicked_ok(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedOk(button);
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedCancel(button);
    }
  }

  static void signal_entry_insert_patient_id(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertPatientId(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_patient_id(GtkWidget* widget, GdkEventFocus* event, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryFocusOutPatientId(widget, event);
    }

    return FALSE;
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewNewPat* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static void signal_checkbutton_clicked_patient_id(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->CheckButtonClickedPatientId(button);
    }
  }

  static void signal_button_clicked_search(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedSearch(button);
    }
  }

  static void signal_entry_insert_name(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertName(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_insert_birth_date_year(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertBirthDateYear(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_year(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryDeleteBirthDateYear(editable, start_pos, end_pos);
    }
  }

  static void signal_entry_insert_birth_date_month(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertBirthDateMonth(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_month(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryDeleteBirthDateMonth(editable, start_pos, end_pos);
    }
  }

  static void signal_entry_insert_birth_date_day(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertBirthDateDay(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_day(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryDeleteBirthDateDay(editable, start_pos, end_pos);
    }
  }

  static void signal_button_clicked_calendar(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedCalendar(button);
    }
  }

  static void signal_entry_age(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static void signal_textview_insert_comment(GtkTextBuffer* textbuffer, GtkTextIter* location, gchar* text, gint len, ViewNewPat* data) {
    if (data != NULL) {
      data->TextViewInsertComment(textbuffer, location, text, len);
    }
  }

  static void signal_entry_insert_stature(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_stature(GtkWidget* widget, GdkEventFocus* event, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryFocusOutStature(widget, event);
    }

    return FALSE;
  }

  static void signal_entry_insert_weight(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_weight(GtkWidget* widget, GdkEventFocus* event, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryFocusOutWeight(widget, event);
    }

    return FALSE;
  }

  static void signal_combobox_changed_ob_date(GtkComboBox* combobox, ViewNewPat* data) {
    if (data != NULL) {
      data->ComboboxChangedOBDate(combobox);
    }
  }

  static void signal_entry_insert_ob_year(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_insert_ob_month(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_insert_ob_day(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_ob(GtkWidget* widget, GdkEventFocus* event, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryFocusOutOB(widget, event);
    }

    return FALSE;
  }

  static void signal_entry_insert_hr(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryInsertNum(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_callback_exitwindow(gpointer data) {
    ViewNewPat* viewNewPat = (ViewNewPat*)data;

    if (viewNewPat != NULL) {
      viewNewPat->DestroyWindow();
    }

    return FALSE;
  }


  static int signal_callback_load_patdata(gpointer data) {
    ViewNewPat::GetInstance()->LoadPatData();

    return 0;
  }

  static int signal_callback_auto_patid(gpointer data) {
    ViewNewPat::GetInstance()->AutoPatID();

    return 0;
  }

  // signal

  void ButtonClickedEndExam(GtkButton* button);
  void ButtonClickedNewPatient(GtkButton* button);
  void ButtonClickedNewExam(GtkButton* button);
  void ButtonClickedWorkList(GtkButton* button);
  void ButtonClickedOk(GtkButton* button);
  void ButtonClickedCancel(GtkButton* button);

  void EntryInsertPatientId(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void EntryFocusOutPatientId(GtkWidget* widget, GdkEventFocus* event);
  void CheckButtonClickedPatientId(GtkButton* button);
  void ButtonClickedSearch(GtkButton* button);
  void EntryInsertName(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);

  void EntryInsertBirthDateYear(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void EntryDeleteBirthDateYear(GtkEditable* editable, gint start_pos, gint end_pos);
  void EntryInsertBirthDateMonth(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void EntryDeleteBirthDateMonth(GtkEditable* editable, gint start_pos, gint end_pos);
  void EntryInsertBirthDateDay(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void EntryDeleteBirthDateDay(GtkEditable* editable, gint start_pos, gint end_pos);
  void ButtonClickedCalendar(GtkButton* button);
  void EntryInsertNum(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);

  void TextViewInsertComment(GtkTextBuffer* textbuffer, GtkTextIter* location, gchar* text, gint len);

  void EntryFocusOutStature(GtkWidget* widget, GdkEventFocus* event);
  void EntryFocusOutWeight(GtkWidget* widget, GdkEventFocus* event);
  void ComboboxChangedOBDate(GtkComboBox* combobox);

  void EntryFocusOutOB(GtkWidget* widget, GdkEventFocus* event);

  void LoadPatData();
  void AutoPatID();

private:
  ViewNewPat();

  void KeyEvent(unsigned char keyValue);

  GtkWidget* create_note_general();
  GtkWidget* create_note_ob();
  GtkWidget* create_note_card();
  GtkWidget* create_note_uro();
  GtkWidget* create_note_other();

  void CalcBSA();
  void Calc_GW_EDD(const string year, const string month, const string day);
  void CheckPatientId(const string pat_id);

  string GenPatID();

  void FillPatInfo(const PatientInfo::Patient& pat_info);
  void FillExamInfo(const PatientInfo::ExamGeneral& exam_info);
  void FillObInfo(const PatientInfo::ObExam& ob);
  void FillCarInfo(const PatientInfo::CarExam& car);
  void FillUroInfo(const PatientInfo::UroExam& uro);

  void GetPatInfo(PatientInfo::Info &info);
  const string GetLMP();
  DCMMPPSELEMENT GetMPPSElement(PatientInfo::Info &info);

  bool AutoCalcAge(const int year, const int month, const int day);
  void EditStudyInfo(PatientInfo::Info& info);

private:
  static ViewNewPat* m_instance;

private:
  GtkDialog* m_dialog;
  GtkEntry* m_entry_patient_id;
  GtkCheckButton* m_checkbutton_patient_id;

  GtkEntry* m_entry_last_name;
  GtkEntry* m_entry_first_name;
  GtkEntry* m_entry_mid_name;

  GtkEntry* m_entry_birth_year;
  GtkEntry* m_entry_birth_month;
  GtkEntry* m_entry_birth_day;
  GtkEntry* m_entry_age;

  GtkComboBoxText* m_combobox_text_age;
  GtkComboBoxText* m_combobox_text_gender;

  GtkTextView* m_textview_comment;
  GtkComboBoxEntry* m_comboboxentry_diagnostician;
  GtkComboBoxEntry* m_comboboxentry_physician;

  // General
  GtkEntry* m_entry_stature;
  GtkEntry* m_entry_weight;
  GtkEntry* m_entry_bsa;

  // OB
  GtkComboBoxText* m_combobox_ob_date;
  GtkEntry* m_entry_ob_year;
  GtkEntry* m_entry_ob_month;
  GtkEntry* m_entry_ob_day;
  GtkEntry* m_entry_ob_gw;
  GtkEntry* m_entry_ob_edd;
  GtkLabel* m_warming_label;

  GtkEntry* m_entry_ob_gravida;
  GtkEntry* m_entry_ob_ectopic;
  GtkEntry* m_entry_ob_gestations;
  GtkEntry* m_entry_ob_para;
  GtkEntry* m_entry_ob_aborta;

  // CARD
  GtkEntry* m_entry_hr;
  GtkEntry* m_entry_bp_high;
  GtkEntry* m_entry_bp_low;

  // URO
  GtkEntry* m_entry_uro_psa;

  // Other
  GtkEntry* m_entry_other_tel;
  GtkEntry* m_entry_other_address;

  DCMSTUDYELEMENT m_studyInfo;

  bool m_langCN;
  int m_curYear;
  int m_curMonth;
  int m_curDay;

  int m_patient_name_first;
  int m_patient_name_middle;
  int m_patient_name_last;

  int m_date_format;

  bool m_clearAll;  // clear all or examinfo
  bool m_flagMPPS;
};

#endif
