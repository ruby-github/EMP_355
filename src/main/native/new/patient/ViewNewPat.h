#ifndef __VIEW_NEW_PAT_H__
#define __VIEW_NEW_PAT_H__

#include "AbsCalendarOpr.h"
#include "patient/PatientInfo.h"
#include "periDevice/DCMDef.h"
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
    bool GetClearStatus() {
        return m_clearAll;
    }

  bool GetMPPSFlag() {
        return m_flagMPPS;
    }
    void SetMPPSFlag(bool flag) {
        m_flagMPPS = flag;
    }

  void ClearStudyInfo();

  void InsertPatientInfo(const char *ID, PatientInfo::Name patientName, string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description);

    void SetStudyInfo(DCMWORKLISTELEMENT element);
    void SetStudyInfo(DCMSTUDYELEMENT element);

    void FillNewPat(const PatientInfo::Info &info);
    void SetSeneitive(bool sensitive);

public:

private:
  // signal

  static void signal_button_clicked_end_exam(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnExamEndClicked(button);
    }
  }

  static void signal_button_clicked_new_patient(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnNewPatClicked(button);
    }
  }

  static void signal_button_clicked_new_exam(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnNewExamClicked(button);
    }
  }

  static void signal_button_clicked_worklist(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnWorkListClicked(button);
    }
  }

  static void signal_button_clicked_ok(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnOkClicked(button);
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnCancelClicked(button);
    }
  }

  static void signal_entry_insert_patient_id(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryAlNumInsert(editable, new_text, new_text_length, position);
    }
  }

  static gboolean signal_entry_focusout_patient_id(GtkWidget* widget, GdkEventFocus* event, ViewNewPat* data) {
    if (data != NULL) {
      data->PatIDFocusOut(widget, event);
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
      data->ChkBtnPatIDClicked(button);
    }
  }

  static void signal_button_clicked_search(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->BtnSearchClicked(button);
    }
  }

  static void signal_entry_insert_name(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryNameInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_insert_birth_date_year(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateYearInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_year(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateYearDelete(editable, start_pos, end_pos);
    }
  }

  static void signal_entry_insert_birth_date_month(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateMonthInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_month(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateMonthDelete(editable, start_pos, end_pos);
    }
  }

  static void signal_entry_insert_birth_date_day(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateDayInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_entry_delete_birth_date_day(GtkEditable* editable, gint start_pos, gint end_pos, ViewNewPat* data) {
    if (data != NULL) {
      data->BirthDateDayDelete(editable, start_pos, end_pos);
    }
  }

  static void signal_button_clicked_calendar(GtkButton* button, ViewNewPat* data) {
    if (data != NULL) {
      data->ButtonClickedCalendar(button);
    }
  }

  static void signal_entry_age(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewNewPat* data) {
    if (data != NULL) {
      data->EntryNumInsert(editable, new_text, new_text_length, position);
    }
  }

  static void signal_textview_insert_comment(GtkTextBuffer* textbuffer, GtkTextIter* location, gchar* text, gint len, ViewNewPat* data) {
    if (data != NULL) {
      data->CommentInsert(textbuffer, location, text, len);
    }
  }

  // signal

  void BtnExamEndClicked(GtkButton* button);
  void BtnNewPatClicked(GtkButton* button);
  void BtnNewExamClicked(GtkButton* button);
  void BtnWorkListClicked(GtkButton* button);
  void BtnOkClicked(GtkButton* button);
  void BtnCancelClicked(GtkButton* button);

  void EntryAlNumInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void PatIDFocusOut(GtkWidget* widget, GdkEventFocus* event);
  void ChkBtnPatIDClicked(GtkButton* button);
  void BtnSearchClicked(GtkButton* button);
  void EntryNameInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);

  void BirthDateYearInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void BirthDateYearDelete(GtkEditable* editable, gint start_pos, gint end_pos);
  void BirthDateMonthInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void BirthDateMonthDelete(GtkEditable* editable, gint start_pos, gint end_pos);
  void BirthDateDayInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void BirthDateDayDelete(GtkEditable* editable, gint start_pos, gint end_pos);
  void ButtonClickedCalendar(GtkButton* button);
  void EntryNumInsert(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);

  void CommentInsert(GtkTextBuffer* textbuffer, GtkTextIter* location, gchar* text, gint len);

private:
  ViewNewPat();

  GtkWidget* create_note_general();
  GtkWidget* create_note_ob();
  GtkWidget* create_note_card();
  GtkWidget* create_note_uro();
  GtkWidget* create_note_other();

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

  bool m_langCN;
  int m_curYear;
  int m_curMonth;
  int m_curDay;

  int m_patient_name_first;
  int m_patient_name_middle;
  int m_patient_name_last;

  int m_date_format;

public:
  void LoadPatData();
    void AutoPatID();


private:

    void FillPatInfo(const PatientInfo::Patient &pat_info);
    void GetPatInfo(PatientInfo::Info &info);

    virtual void SetStartDate(int year, int month, int day);



    bool IsErrorChar(gchar *new_text) {
        return (0 == strcmp(new_text, "'"));
    }

private:
    bool AutoCalcAge(const int year, const int month, const int day);
    void KeyEvent(unsigned char keyValue);
    void CalcBSA();
    std::string GenPatID();
    void Calc_GA_EDD(const gchar *year, const gchar *month, const gchar *day);
    void CheckPatID(const gchar *pat_id);
    void FillExamInfo(const PatientInfo::ExamGeneral &exam_info);
    void FillObInfo(const PatientInfo::ObExam &ob);
    void FillCarInfo(const PatientInfo::CarExam &car);
    void FillUroInfo(const PatientInfo::UroExam &uro);

    bool m_clearAll;            // clear all or examinfo

    DCMSTUDYELEMENT m_studyInfo;

    bool m_flagMPPS;


    DCMMPPSELEMENT GetMPPSElement(PatientInfo::Info &info);
    void EditStudyInfo(PatientInfo::Info &info);

    const gchar* GetLMP();

    void ComboboxOBDateChanged(GtkComboBox *widget);

    void OBFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void StatureFocusOut(GtkWidget *widget, GdkEventFocus *event);
    void WeightFocusOut(GtkWidget *widget, GdkEventFocus *event);

    // signal connect
    static gboolean HandleOBFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data) {
        data->OBFocusOut(widget, event);
        return FALSE;
    }
    static gboolean HandleStatureFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data) {
        data->StatureFocusOut(widget, event);
        return FALSE;
    }
    static gboolean HandleWeightFocusOut(GtkWidget *widget, GdkEventFocus *event, ViewNewPat *data) {
        data->WeightFocusOut(widget, event);
        return FALSE;
    }

    static void on_entry_stature(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }
    static void on_entry_weight(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }

    static void on_combobox_ob_date_changed(GtkComboBox *widget, ViewNewPat *data) {
        data->ComboboxOBDateChanged(widget);
    }
    static void on_entry_ob_year(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }
    static void on_entry_ob_month(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }
    static void on_entry_ob_day(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }

    static void on_entry_hr(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, ViewNewPat *data) {
        data->EntryNumInsert(editable, new_text, new_text_length, position);
    }

};



#endif
