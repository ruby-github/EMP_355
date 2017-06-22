#include "patient/ViewNewPat.h"

#include "calcPeople/MeaCalcFun.h"
#include "display/TopArea.h"
#include "display/ViewCalendar.h"
#include "keyboard/KeyDef.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/MultiFuncFactory.h"
#include "patient/Database.h"
#include "patient/ViewPatSearch.h"
#include "patient/ViewWorkList.h"
#include "periDevice/DCMMan.h"
#include "periDevice/DCMRegister.h"
#include "sysMan/SysCalculateSetting.h"
#include "sysMan/SysDicomSetting.h"
#include "sysMan/SysGeneralSetting.h"
#include "sysMan/UpgradeMan.h"
#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"
#include "utils/MessageDialog.h"
#include "ViewMain.h"

#include "display/gui_func.h"

#define PATIENT_NAME_MAX_LEN  31

ViewNewPat* ViewNewPat::m_instance = NULL;

// ---------------------------------------------------------

ViewNewPat* ViewNewPat::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewNewPat();
  }

  return m_instance;
}

ViewNewPat::ViewNewPat() {
  m_dialog = NULL;

  m_langCN = 0;
  m_curYear = 0;
  m_curMonth = 0;
  m_curDay = 0;
  m_flagMPPS = false;

  UpdateNameLength();
}

ViewNewPat::~ViewNewPat() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewNewPat::CreateWindow() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  SysGeneralSetting setting;

  if (ZH == setting.GetLanguage() || FR == setting.GetLanguage()) {
    m_langCN = true;
  } else {
    m_langCN = false;
  }

  Utils::GetCurrentDate(m_curYear, m_curMonth, m_curDay);

  m_dialog = Utils::create_dialog(
    GTK_WINDOW(ViewMain::GetInstance()->GetMainWindow()),
    _("Patient Information"), 900, 600);

  GtkButton* button_end_exam = Utils::add_dialog_button(m_dialog, _("End Exam"), GTK_RESPONSE_NONE, GTK_STOCK_MEDIA_STOP);
  GtkButton* button_new_patient = Utils::add_dialog_button(m_dialog, _("New Patient"), GTK_RESPONSE_NONE, GTK_STOCK_ORIENTATION_PORTRAIT);
  GtkButton* button_new_exam = Utils::add_dialog_button(m_dialog, _("New Exam"), GTK_RESPONSE_NONE, GTK_STOCK_NEW);

  g_signal_connect(button_end_exam, "clicked", G_CALLBACK(signal_button_clicked_end_exam), this);
  g_signal_connect(button_new_patient, "clicked", G_CALLBACK(signal_button_clicked_new_patient), this);
  g_signal_connect(button_new_exam, "clicked", G_CALLBACK(signal_button_clicked_new_exam), this);

  if (CDCMRegister::GetMe()->IsAuthorize()) {
    GtkButton* button_worklist = Utils::add_dialog_button(m_dialog, _("Worklist"), GTK_RESPONSE_NONE, GTK_STOCK_INFO);
    g_signal_connect(button_worklist, "clicked", G_CALLBACK(signal_button_clicked_worklist), this);
  }

  GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK, GTK_STOCK_OK);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  // table

  GtkTable* table = Utils::create_table(13, 9);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  // General Information

  GtkFrame* frame = Utils::create_frame(_("General Information"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame), 0, 9, 0, 4);

  GtkTable* table_frame = Utils::create_table(3, 18);
  gtk_container_set_border_width(GTK_CONTAINER(table_frame), 5);
  gtk_table_set_col_spacings(table_frame, 5);

  gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(table_frame));

  // Patient ID

  GtkLabel* label_patient_id = Utils::create_label(_("Patient ID:"));
  m_entry_patient_id = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_patient_id), 0, 2, 0, 1);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_patient_id), 2, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_patient_id, 15);

  g_signal_connect(m_entry_patient_id, "insert_text", G_CALLBACK(signal_entry_insert_patient_id), this);
  g_signal_connect(m_entry_patient_id, "focus-out-event", G_CALLBACK(signal_entry_focusout_patient_id), this);

  // Auto Generated ID

  GtkCheckButton* m_checkbutton_patient_id = Utils::create_check_button(_("Auto Generated ID"));
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_checkbutton_patient_id), 6, 12, 0, 1);

  g_signal_connect(m_checkbutton_patient_id, "clicked", G_CALLBACK(signal_checkbutton_clicked_patient_id), this);

  // Search

  GtkButton* button_search_patient_info = Utils::create_button(_("Search"));
  gtk_table_attach(table_frame, GTK_WIDGET(button_search_patient_info), 14, 18, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_search_patient_info, "clicked", G_CALLBACK (signal_button_clicked_search), this);

  // Last Name

  GtkLabel* label_last_name = Utils::create_label(_("Last Name:"));
  m_entry_last_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_last_name), 0, 2, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_last_name), 2, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_last_name, m_patient_name_last);

  g_signal_connect(G_OBJECT(m_entry_last_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // First Name

  GtkLabel* label_first_name = Utils::create_label(_("First Name:"));
  m_entry_first_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_first_name), 6, 8, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_first_name), 8, 12, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_first_name, m_patient_name_first);

  g_signal_connect(G_OBJECT(m_entry_first_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // Middle Name

  GtkLabel* label_mid_name = Utils::create_label(_("Middle Name:"));
  m_entry_mid_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_mid_name), 12, 14, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_mid_name), 14, 18, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_mid_name, m_patient_name_middle);

  g_signal_connect(G_OBJECT(m_entry_mid_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // Date of Birth

  GtkLabel* label_birth_date = Utils::create_label(_("Date of Birth:"));

  m_entry_birth_year = Utils::create_entry(9679);
  GtkLabel* label_birth_year = Utils::create_label(_("Y"));
  gtk_entry_set_max_length(m_entry_birth_year, 4);

  m_entry_birth_month = Utils::create_entry(9679);
  GtkLabel* label_birth_month = Utils::create_label(_("M"));
  gtk_entry_set_max_length(m_entry_birth_month, 2);

  m_entry_birth_day = Utils::create_entry(9679);
  GtkLabel* label_birth_day = Utils::create_label(_("D"));
  gtk_entry_set_max_length(m_entry_birth_day, 2);

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_date), 0, 2, 2, 3);

  m_date_format = setting.GetDateFormat();

  switch (m_date_format) {
  case 1:
    // M/D/Y
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 2, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 3, 4, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 4, 5, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 5, 6, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 6, 7, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 7, 8, 2, 3);
    }

    break;
  case 2:
    // D/M/Y
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 2, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 3, 4, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 4, 5, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 5, 6, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 6, 7, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 7, 8, 2, 3);
    }

    break;
  default:
    // Y/M/D
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 2, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 3, 4, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 4, 5, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 5, 6, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 6, 7, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 7, 8, 2, 3);
    }

    break;
  }

  g_signal_connect(G_OBJECT(m_entry_birth_year), "insert_text", G_CALLBACK(signal_entry_insert_birth_date_year), this);
  g_signal_connect(G_OBJECT(m_entry_birth_year), "delete_text", G_CALLBACK(signal_entry_delete_birth_date_year), this);
  g_signal_connect(G_OBJECT(m_entry_birth_month), "insert_text", G_CALLBACK(signal_entry_insert_birth_date_month), this);
  g_signal_connect(G_OBJECT(m_entry_birth_month), "delete_text", G_CALLBACK(signal_entry_delete_birth_date_month), this);
  g_signal_connect(G_OBJECT(m_entry_birth_day), "insert_text", G_CALLBACK(signal_entry_insert_birth_date_day), this);
  g_signal_connect(G_OBJECT(m_entry_birth_day), "delete_text", G_CALLBACK(signal_entry_delete_birth_date_day), this);

  // calendar

  GtkButton* button_birth_calendar = Utils::create_button();
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(button_birth_calendar), 8, 9, 2, 3);

  Utils::set_button_image(button_birth_calendar, Utils::create_image("./res/icon/Calendar.png"));
  g_signal_connect(button_birth_calendar, "clicked", G_CALLBACK(signal_button_clicked_calendar), this);

  // Age

  GtkLabel* label_age = Utils::create_label(_("Age:"));
  m_entry_age = Utils::create_entry();
  m_combobox_text_age = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_age), 9, 10, 2, 3);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_age), 10, 11, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(m_combobox_text_age), 11, 12, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_misc_set_alignment(GTK_MISC(label_age), 0.5, 0.5);

  gtk_entry_set_max_length(m_entry_age, 3);
  g_signal_connect(m_entry_age, "insert_text", G_CALLBACK(signal_entry_age), this);

  gtk_combo_box_text_append_text(m_combobox_text_age, _("Y"));
  gtk_combo_box_text_append_text(m_combobox_text_age, _("M"));
  gtk_combo_box_text_append_text(m_combobox_text_age, _("D"));

  // Gender

  GtkLabel* label_gender = Utils::create_label(_("Gender:"));
  m_combobox_text_gender = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_gender), 12, 14, 2, 3);
  gtk_table_attach(table_frame, GTK_WIDGET(m_combobox_text_gender), 14, 18, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_combo_box_text_append_text(m_combobox_text_gender, _("Female"));
  gtk_combo_box_text_append_text(m_combobox_text_gender, _("Male"));
  gtk_combo_box_text_append_text(m_combobox_text_gender, _("Other"));

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_gender), -1);

  // Notebook

  GtkNotebook* notebook = Utils::create_notebook();
  gtk_table_attach_defaults(table, GTK_WIDGET(notebook), 0, 9, 4, 10);

  gtk_notebook_append_page(notebook, create_note_general(), GTK_WIDGET(Utils::create_label(_("General"))));
  gtk_notebook_append_page(notebook, create_note_ob(), GTK_WIDGET(Utils::create_label(_("OB"))));
  gtk_notebook_append_page(notebook, create_note_card(), GTK_WIDGET(Utils::create_label(_("CARD"))));
  gtk_notebook_append_page(notebook, create_note_uro(), GTK_WIDGET(Utils::create_label(_("URO"))));
  gtk_notebook_append_page(notebook, create_note_other(), GTK_WIDGET(Utils::create_label(_("Other"))));

  // Comment

  GtkLabel* label_comment = Utils::create_label(_("Comment:"));
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_comment), 0, 1, 10, 12);
  gtk_table_attach_defaults(table, GTK_WIDGET(scrolled_window), 1, 9, 10, 12);

  gtk_misc_set_alignment(GTK_MISC(label_comment), 0, 0);

  m_textview_comment = Utils::create_text_view();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_textview_comment));

  g_signal_connect(G_OBJECT(gtk_text_view_get_buffer(m_textview_comment)), "insert-text", G_CALLBACK(signal_textview_insert_comment), this);

  // Diagnostician

  GtkLabel* label_diagnostician = Utils::create_label(_("Diagnostician:"));
  m_comboboxentry_diagnostician = Utils::create_combobox_entry();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_diagnostician), 0, 1, 12, 13);
  gtk_table_attach(table, GTK_WIDGET(m_comboboxentry_diagnostician), 1, 3, 12, 13, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkEntry* bin_entry_diagnostician = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxentry_diagnostician)));
  gtk_entry_set_max_length(bin_entry_diagnostician, 45);

  // Physician

  GtkLabel* label_physician = Utils::create_label(_("Physician:"));
  m_comboboxentry_physician = Utils::create_combobox_entry();

  gtk_table_attach_defaults(table, GTK_WIDGET(label_physician), 3, 4, 12, 13);
  gtk_table_attach(table, GTK_WIDGET(m_comboboxentry_physician), 4, 6, 12, 13, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkEntry* bin_entry_physician = GTK_ENTRY(gtk_bin_get_child(GTK_BIN(m_comboboxentry_physician)));
  gtk_entry_set_max_length(bin_entry_physician, 45);

  g_signal_connect(bin_entry_physician, "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  Database db;
  vector<string> doc_name;

  if (db.DoctorSearch(&doc_name)) {
    for (int i = 0; i < doc_name.size(); i++) {
      gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxentry_diagnostician), doc_name[i].c_str());
      gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxentry_physician), doc_name[i].c_str());
    }
  }

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();

  if (m_langCN) {
    gtk_widget_hide(GTK_WIDGET(label_mid_name));
    gtk_widget_hide(GTK_WIDGET(m_entry_mid_name));
  }

  PatientInfo::Info info;
  g_patientInfo.GetInfo(info);
  FillNewPat(info);

  if (info.p.id.empty()) {
    if (setting.GetAutoGeneratedID()) {
      AutoPatID();
    }
  } else {
    gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_patient_id), FALSE);
  }
}

void ViewNewPat::DestroyWindow() {
  if (GTK_IS_WIDGET(m_dialog))	{
    g_keyInterface.Pop();
    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }

    m_dialog = NULL;
  }
}

GtkWidget* ViewNewPat::GetWindow() {
  return GTK_WIDGET(m_dialog);
}

void ViewNewPat::UpdateNameLength() {
  SysGeneralSetting sgs;

  if(ZH == sgs.GetLanguage()) {
    m_patient_name_last = 31;
    m_patient_name_middle = 0;
    m_patient_name_first = 31;
  } else if(RU == sgs.GetLanguage()) {
    m_patient_name_last = 30;
    m_patient_name_middle = 16;
    m_patient_name_first = 16;
  } else if(FR == sgs.GetLanguage()) {
    m_patient_name_last = 31;
    m_patient_name_middle = 0;
    m_patient_name_first = 31;
  } else {
    m_patient_name_last = 20;
    m_patient_name_middle = 20;
    m_patient_name_first = 20;
  }
}

void ViewNewPat::UpdateStudyInfo() {
  Database db;
  string examNum;
  db.GetExamIDCurrent(examNum);
  m_studyInfo.stStudyNo = atoi(examNum.c_str());
  CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
}

void ViewNewPat::ClearData() {
  if (!GTK_IS_WIDGET(m_dialog)) {
    return;
  }

  gtk_entry_set_text(m_entry_patient_id, "");
  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_patient_id), TRUE);

  gtk_entry_set_text(m_entry_first_name, "");
  gtk_entry_set_text(m_entry_mid_name, "");
  gtk_entry_set_text(m_entry_last_name, "");

  gtk_entry_set_text(m_entry_age, "");
  gtk_entry_set_text(m_entry_birth_year, "");
  gtk_entry_set_text(m_entry_birth_month, "");
  gtk_entry_set_text(m_entry_birth_day, "");
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_gender), -1);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);

  ClearExamData();
}

void ViewNewPat::ClearExamData() {
  if (!GTK_IS_WIDGET(m_dialog)) {
    return;
  }

  // general
  if(m_clearAll) {
    gtk_entry_set_text(m_entry_stature, "");
    gtk_entry_set_text(m_entry_weight, "");
    gtk_entry_set_text(m_entry_bsa, "");
  }

  // ob
  gtk_entry_set_text(m_entry_ob_year, "");
  gtk_entry_set_text(m_entry_ob_month, "");
  gtk_entry_set_text(m_entry_ob_day, "");
  gtk_entry_set_text(m_entry_ob_gw, "");
  gtk_entry_set_text(m_entry_ob_edd, "");
  gtk_entry_set_text(m_entry_ob_gravida, "");
  gtk_entry_set_text(m_entry_ob_ectopic, "");
  gtk_entry_set_text(m_entry_ob_gestations, "");
  gtk_entry_set_text(m_entry_ob_para, "");
  gtk_entry_set_text(m_entry_ob_aborta, "");

  // card
  gtk_entry_set_text(GTK_ENTRY(m_entry_hr), "");
  gtk_entry_set_text(GTK_ENTRY(m_entry_bp_high), "");
  gtk_entry_set_text(GTK_ENTRY(m_entry_bp_low), "");

  // URO
  gtk_entry_set_text(GTK_ENTRY(m_entry_uro_psa), "");

  // Other
  if (m_clearAll) {
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_tel), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_address), "");
  }

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
  gtk_text_buffer_set_text(buffer, "", -1);

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_diagnostician), -1);
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_physician), -1);
}

void ViewNewPat::ClearStudyInfo() {
  m_studyInfo.stSpecificCharacterSet ="";
  m_studyInfo.stPatientName = "";
  m_studyInfo.stPatientID = "";
  m_studyInfo.stPatientBirthDate = "";
  m_studyInfo.stPatientSex = "";
  m_studyInfo.stPatientAge = "";
  m_studyInfo.stPatientSize = "";
  m_studyInfo.stPatientWeight = "";
  m_studyInfo.stStudyInstanceUID = "";
  m_studyInfo.stAccessionNumber = "";
  m_studyInfo.stStudyDoctor = "";
  m_studyInfo.stStudyDescription = "";
  m_studyInfo.stStudyDate = "";
  m_studyInfo.stStudyTime = "";
  m_studyInfo.stFactoryName = "";
  m_studyInfo.stHospital = "";
  m_studyInfo.stMechineType = "";
  m_studyInfo.stMechineSN = "";
  m_studyInfo.stStudyNo = 0;
}

bool ViewNewPat::GetClearStatus() {
  return m_clearAll;
}

bool ViewNewPat::GetMPPSFlag() {
  return m_flagMPPS;
}

void ViewNewPat::SetMPPSFlag(bool flag) {
  m_flagMPPS = flag;
}

void ViewNewPat::InsertPatientInfo(const string id, PatientInfo::Name patientName,
  string birthDate, string age, const string sex, const string size, const string weight,
  const string address, const string doctor, const string description) {
  m_clearAll = true;
  ClearData();

  gtk_entry_set_text(m_entry_patient_id, id.c_str());

  Database db;

  if (!((db.GetPatIDExist(id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id))) {
    gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), false);
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_QUESTION, _("Patient ID exist, load data?"), signal_callback_load_patdata, signal_callback_auto_patid);
  } else {
    gtk_entry_set_text(m_entry_last_name, patientName.last.c_str());
    gtk_entry_set_text(m_entry_first_name, patientName.first.c_str());
    gtk_entry_set_text(m_entry_mid_name, patientName.mid.c_str());

    // Age
    if (!age.empty()) {
      char tmp = age[age.size() - 1];

      if (tmp == 'Y' || tmp == 'M' || tmp == 'D') {
        if (tmp == 'M') {
          gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age), 1);
        } else if (tmp == 'D') {
          gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age), 2);
        } else {
          gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age), 0);
        }

        age = age.substr(0, age.size() - 1);
      }

      gtk_entry_set_text(m_entry_age, age.c_str());
    } else {
      gtk_entry_set_text(m_entry_age, "");
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), -1);
    }

    // sex
    if (!sex.empty()) {
      if (sex == "F") {
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender), 0);
      } else if (sex == "M") {
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender), 1);
      } else {
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender), 2);
      }
    } else {
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_gender), -1);
    }

    // birthDate
    if (birthDate.size() >= 8) {
      int pos = 0;
      gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_year), birthDate.substr(0, 4).c_str(), 4, &pos);

      pos = 0;
      gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_month), birthDate.substr(4, 2).c_str(), 2, &pos);

      pos = 0;
      gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_day), birthDate.substr(6, 2).c_str(), 2, &pos);
    } else {
      gtk_entry_set_text(m_entry_birth_year, "");
      gtk_entry_set_text(m_entry_birth_month, "");
      gtk_entry_set_text(m_entry_birth_day, "");
    }

    char buf[100] = {0};

    string size_tmp = Utils::DotToCommaLocaleNumeric(size);

    float stature = atof(size_tmp.c_str());
    int stature_real = stature * 100;
    sprintf(buf, "%d", stature_real);
    gtk_entry_set_text(m_entry_stature, buf);

    string weight_tmp = Utils::DotToCommaLocaleNumeric(weight);

    float tmp = atof(weight_tmp.c_str());
    sprintf(buf,"%.1f", tmp);
    gtk_entry_set_text(m_entry_weight, buf);

    CalcBSA();
    gtk_entry_set_text(m_entry_other_address, address.c_str());
  }

  GtkWidget* entry = gtk_bin_get_child(GTK_BIN(m_comboboxentry_diagnostician));
  gtk_entry_set_text(GTK_ENTRY(entry), doctor.c_str());
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(m_textview_comment);
  gtk_text_buffer_set_text(buffer, description.c_str(), -1);
}

void ViewNewPat::SetStudyInfo(DCMWORKLISTELEMENT element) {
  m_studyInfo.stSpecificCharacterSet = element.wlSpecificCharacterSet;
  m_studyInfo.stPatientName = element.wlPatientName;
  m_studyInfo.stPatientID = element.wlPatientID;
  m_studyInfo.stPatientBirthDate = element.wlPatientBirthDate;
  m_studyInfo.stPatientSex = element.wlPatientSex;
  m_studyInfo.stPatientAge = element.wlPatientAge;
  m_studyInfo.stPatientSize = element.wlPatientSize;
  m_studyInfo.stPatientWeight = element.wlPatientWeight;
  m_studyInfo.stStudyInstanceUID = element.wlStudyInstanceUID;
  m_studyInfo.stAccessionNumber = element.wlAccessionNumber;
  m_studyInfo.stStudyDoctor = element.wlStudyDoctor;
  m_studyInfo.stStudyDescription = element.wlStudyDescription;
}

void ViewNewPat::SetStudyInfo(DCMSTUDYELEMENT element) {
  m_studyInfo = element;
}

void ViewNewPat::FillNewPat(const PatientInfo::Info& info) {
  FillPatInfo(info.p);
  FillExamInfo(info.e);
  FillObInfo(info.ob);
  FillCarInfo(info.car);
  FillUroInfo(info.uro);
}

void ViewNewPat::SetSeneitive(bool sensitive) {
  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), sensitive);
  gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_patient_id), sensitive);
}

// ---------------------------------------------------------

void ViewNewPat::ButtonClickedEndExam(GtkButton* button) {
  PatientInfo::Info info;
  GetPatInfo(info);

  Database db;
  if (!db.GetPatIDExist(info.p.id.c_str()).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id)) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please enter patient ID again!"), NULL);

    return ;
  }

  db.GetExamIDNext(info.e.examNum);

  g_patientInfo.SetInfo(info);

  if (CDCMRegister::GetMe()->IsAuthorize()) {
    SysDicomSetting sysDicomSetting;

    if(!g_patientInfo.GetExist()) {
      if(sysDicomSetting.GetMPPS()) {
        if(!m_flagMPPS) {
          if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="") {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
              MessageDialog::DLG_ERROR, _("Please Set the default MPPS service in system setting"), NULL);

            return ;
          }

          CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
          m_flagMPPS = true;
        }
      }
    }
  }

  // end exam
  m_clearAll = true;
  KeyEndExam kee;
  kee.Execute();
}

void ViewNewPat::ButtonClickedNewPatient(GtkButton* button) {
  if (g_patientInfo.GetExist()) {
    // end exam
    m_clearAll = true;
    KeyEndExam kee;
    kee.Execute();
  } else {
    //clear patient and exam data in dialog
    ClearData();
    // clear data in patient management
    g_patientInfo.ClearAll();
    ClearStudyInfo();
  }

  gtk_widget_grab_focus(GTK_WIDGET(m_entry_patient_id));
}

void ViewNewPat::ButtonClickedNewExam(GtkButton* button) {
  if (g_patientInfo.GetExist()) {
    // end exam
    m_clearAll = false;
    KeyEndExam kee;
    kee.Execute();
  } else {
    //clear exam data in dialog
    ClearExamData();
    // clear exam data in patient management
    g_patientInfo.ClearExam();
  }
}

void ViewNewPat::ButtonClickedWorkList(GtkButton* button) {
  string device = CDCMMan::GetMe()->GetDefaultWorklistServiceDevice();

  if (device.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()),
      MessageDialog::DLG_ERROR, _("Please Set the default worklist service in system setting"), NULL);

    return ;
  }

  ViewWorkList::GetInstance()->CreateWorkListWin(GTK_WIDGET(m_dialog));
}

void ViewNewPat::ButtonClickedOk(GtkButton* button) {
  PatientInfo::Info info;
  GetPatInfo(info);

  Database db;
  if (!(db.GetPatIDExist(info.p.id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id)) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_ERROR, _("Please enter patient ID again!"), NULL);
    return ;
  }

  db.GetExamIDNext(info.e.examNum);

  if(CDCMRegister::GetMe()->IsAuthorize()) {
    EditStudyInfo(info);

    SysDicomSetting sysDicomSetting;

    if(!g_patientInfo.GetExist()) {
      if(sysDicomSetting.GetMPPS()) {
        if(!m_flagMPPS) {
          if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="") {
            MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
              MessageDialog::DLG_ERROR, _("Please Set the default MPPS service in system setting"), NULL);
            return;
          }

          CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
          m_flagMPPS = true;
        }
      }
    }
  }

  g_patientInfo.SetInfo(info);
  TopArea* ptrTopArea = TopArea::GetInstance();

  if(ptrTopArea->GetReadImg()) {
    ptrTopArea->SetReadImg(true);
  }

  DestroyWindow();
}

void ViewNewPat::ButtonClickedCancel(GtkButton* button) {
  DestroyWindow();
}

void ViewNewPat::EntryInsertPatientId(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isalnum(*new_text)) {
    if(g_ascii_strcasecmp(new_text, "-") != 0 && g_ascii_strcasecmp(new_text, "_") != 0 && g_ascii_strcasecmp(new_text, "/") != 0) {
      g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }
  }
}

void ViewNewPat::EntryFocusOutPatientId(GtkWidget* widget, GdkEventFocus* event) {
  string pat_id = gtk_entry_get_text(GTK_ENTRY(widget));
  CheckPatientId(pat_id);
}

void ViewNewPat::CheckButtonClickedPatientId(GtkButton* button) {
  SysGeneralSetting sys;

  gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));

  if (value) {
    string ID = GenPatID();
    gtk_entry_set_text(m_entry_patient_id, ID.c_str());
    sys.SetAutoGeneratedID(1);
    sys.SyncFile();
  } else {
    gtk_entry_set_text(m_entry_patient_id, "");
    gtk_widget_grab_focus(GTK_WIDGET(m_entry_patient_id));
    sys.SetAutoGeneratedID(0);
    sys.SyncFile();
  }
}

void ViewNewPat::ButtonClickedSearch(GtkButton* button) {
  string id;
  string name_last;
  string name_first;
  string name_mid;
  string birth_year;
  string birth_month;
  string birth_day;
  string age;

  GetEntryTextForDB(GTK_WIDGET(m_entry_patient_id), id);
  GetEntryTextForDB(GTK_WIDGET(m_entry_last_name), name_last);
  GetEntryTextForDB(GTK_WIDGET(m_entry_first_name), name_first);
  GetEntryTextForDB(GTK_WIDGET(m_entry_mid_name), name_mid);
  GetEntryTextForDB(GTK_WIDGET(m_entry_birth_year), birth_year);
  GetEntryTextForDB(GTK_WIDGET(m_entry_birth_month), birth_month);
  GetEntryTextForDB(GTK_WIDGET(m_entry_birth_day), birth_day);
  GetEntryTextForDB(GTK_WIDGET(m_entry_age), age);

  stringstream ss;

  int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_gender));

  if (gender_index == -1) {
    ss << "%";
  } else {
    ss << gender_index;
  }

  string gender = ss.str();

  if (birth_month != "%") {
    char buf[10] = {0};
    sprintf(buf, "%02d", atoi(birth_month.c_str()));

    birth_month = buf;
  }

  if (birth_day != "%") {
    char buf[10] = {0};
    sprintf(buf, "%02d", atoi(birth_day.c_str()));

    birth_day = buf;
  }

  ss.str("");

  if (age != "%") {
    int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_age));

    ss << age_unit_index;
  } else {
    ss << "%";
  }

  string age_unit = ss.str();

  Database::NewPatSearchTerm term;

  term.id = id;
  term.name.last = name_last;
  term.name.first = name_first;
  term.name.mid = name_mid;
  term.gender = gender;
  term.birthYear = birth_year;
  term.birthMonth = birth_month;
  term.birthDay = birth_day;
  term.age = age;
  term.ageUnit = age_unit;

  Database db;
  vector<Database::NewPatSearchResult> vecResult;
  db.NewPatSearch(term, vecResult);

  if (vecResult.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
      MessageDialog::DLG_INFO, _("No result found!"), NULL);
    return;
  }

  ViewPatSearch::GetInstance()->CreateWindow(GTK_WINDOW(m_dialog), vecResult);
}

void ViewNewPat::EntryInsertName(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

  if(old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)) || g_ascii_strcasecmp(new_text, "'") == 0) {
    gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
  }
}

void ViewNewPat::EntryInsertBirthDateYear(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }

  string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
  string new_str = new_text;
  old_str.insert(*position, new_str);

  if (old_str.length() == 4) {
      int year = atoi(old_str.c_str());

      if (year < 1900) {
        *position = 4;  // 移动光标之输入框末尾
        gtk_entry_set_text(m_entry_birth_year, "1900");
        year = 1900;
      } else if (year > m_curYear) {
        *position = 4;
        char curYear[5];
        sprintf(curYear, "%d", m_curYear);
        gtk_entry_set_text(m_entry_birth_year, curYear);
        year = m_curYear;
      }

      string month_text = gtk_entry_get_text(m_entry_birth_month);
      string day_text = gtk_entry_get_text(m_entry_birth_day);

      AutoCalcAge(year, atoi(month_text.c_str()), atoi(day_text.c_str()));
      gtk_widget_grab_focus(GTK_WIDGET(m_entry_birth_month));
  }
}

void ViewNewPat::EntryDeleteBirthDateYear(GtkEditable* editable, gint start_pos, gint end_pos) {
  string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
  new_str.erase(start_pos, end_pos);

  if (new_str.length() != 4) {
    gtk_entry_set_text(m_entry_age, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
  }
}

void ViewNewPat::EntryInsertBirthDateMonth(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }

  string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
  string new_str = new_text;
  old_str.insert(*position, new_str);

  if (old_str.length() != 0) {
    int month = atoi(old_str.c_str());

    if (month < 1) {
      g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
      return ;
    } else if (month > 12) {
      *position = 2;  // 移动光标之输入框末尾
      gtk_entry_set_text(m_entry_birth_month, "12");
      month = 12;
    }

    string year_text = gtk_entry_get_text(m_entry_birth_year);
    if (year_text.length() != 4) {
      return;
    }

    string day_text = gtk_entry_get_text(m_entry_birth_day);
    AutoCalcAge(atoi(year_text.c_str()), month, atoi(day_text.c_str()));

    if (old_str.length() == 2) {
      gtk_widget_grab_focus(GTK_WIDGET(m_entry_birth_day));
    }
  }
}

void ViewNewPat::EntryDeleteBirthDateMonth(GtkEditable* editable, gint start_pos, gint end_pos) {
  string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
  new_str.erase(start_pos, end_pos);

  if (new_str.length() != 0) {
    int month = atoi(new_str.c_str());

    if (month < 1) {
      g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
      return ;
    }

    string year_text = gtk_entry_get_text(m_entry_birth_year);
    if (year_text.length() != 4) {
      return;
    }

    string day_text = gtk_entry_get_text(m_entry_birth_day);
    AutoCalcAge(atoi(year_text.c_str()), month, atoi(day_text.c_str()));
  } else {
    gtk_entry_set_text(m_entry_age, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
  }
}

void ViewNewPat::EntryInsertBirthDateDay(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }

  string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
  string new_str = new_text;
  old_str.insert(*position, new_str);

  int day = atoi(old_str.c_str());

  if (day < 1) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return ;
  }

  string year_text = gtk_entry_get_text(m_entry_birth_year);
  string month_text = gtk_entry_get_text(m_entry_birth_month);

  if (year_text.length() < 4 || month_text.length() == 0) {
    if (day > 31) {
      *position = 2;  // 移动光标之输入框末尾
      gtk_entry_set_text(m_entry_birth_day, "31");
    }

    return ;
  } else {
    int year = atoi(year_text.c_str());
    int month = atoi(month_text.c_str());
    int last_day = g_date_get_days_in_month (GDateMonth(month), GDateYear(year));

    if (day > last_day) {
      *position = 2;  // 移动光标之输入框末尾
      char str_last_day[3];
      sprintf(str_last_day, "%d", last_day);
      gtk_entry_set_text(m_entry_birth_day, str_last_day);
      day = last_day;
    }

    AutoCalcAge(year, month, day);
  }
}

void ViewNewPat::EntryDeleteBirthDateDay(GtkEditable* editable, gint start_pos, gint end_pos) {
  string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
  new_str.erase(start_pos, end_pos);

  if (new_str.length() != 0) {
    int day = atoi(new_str.c_str());

    if (day < 1) {
      g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
      return ;
    }

    string year_text = gtk_entry_get_text(m_entry_birth_year);
    string month_text = gtk_entry_get_text(m_entry_birth_month);

    if (year_text.length() < 4 || month_text.length() == 0) {
      return ;
    } else {
      int year = atoi(year_text.c_str());
      int month = atoi(month_text.c_str());

      AutoCalcAge(year, month, day);
    }
  } else {
    gtk_entry_set_text(m_entry_age, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
  }
}

void ViewNewPat::ButtonClickedCalendar(GtkButton* button) {
  ViewCalendar::GetInstance()->CreateWindow(GTK_WIDGET(m_dialog), ViewCalendar::START, this);
}

void ViewNewPat::EntryInsertNum(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0) {
    g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
  }
}

void ViewNewPat::TextViewInsertComment(GtkTextBuffer* textbuffer, GtkTextIter* location, gchar* text, gint len) {
  if (g_ascii_strcasecmp(text, "'") == 0) {
    g_signal_stop_emission_by_name((gpointer)textbuffer, "insert_text");
  }
}

void ViewNewPat::EntryFocusOutStature(GtkWidget* widget, GdkEventFocus* event) {
  CalcBSA();
}

void ViewNewPat::EntryFocusOutWeight(GtkWidget* widget, GdkEventFocus* event) {
  CalcBSA();
}

void ViewNewPat::ComboboxChangedOBDate(GtkComboBox* combobox) {
  string year;
  string month;
  string day;

  if(m_date_format == 0) {
    // 年/月/日
    year = gtk_entry_get_text(m_entry_ob_year);
    month = gtk_entry_get_text(m_entry_ob_month);
    day = gtk_entry_get_text(m_entry_ob_day);
  } else if(m_date_format == 1) {
    // 月/日/年
    month = gtk_entry_get_text(m_entry_ob_year);
    day = gtk_entry_get_text(m_entry_ob_month);
    year = gtk_entry_get_text(m_entry_ob_day);
  } else {
    // 日/月/年
    day = gtk_entry_get_text(m_entry_ob_year);
    month = gtk_entry_get_text(m_entry_ob_month);
    year = gtk_entry_get_text(m_entry_ob_day);
  }

  Calc_GW_EDD(year, month, day);
}

void ViewNewPat::EntryFocusOutOB(GtkWidget* widget, GdkEventFocus* event) {
  time_t now;
  time(&now);

  tm* timeinfo;
  timeinfo = localtime(&now);

  int year_now = timeinfo->tm_year + 1900;
  int month_now = timeinfo->tm_mon + 1;
  int day_now = timeinfo->tm_mday;

  string year;
  string month;
  string day;

  if(m_date_format == 0) {
    // 年/月/日
    year = gtk_entry_get_text(m_entry_ob_year);
    month = gtk_entry_get_text(m_entry_ob_month);
    day = gtk_entry_get_text(m_entry_ob_day);
  } else if(m_date_format == 1) {
    // 月/日/年
    month = gtk_entry_get_text(m_entry_ob_year);
    day = gtk_entry_get_text(m_entry_ob_month);
    year = gtk_entry_get_text(m_entry_ob_day);
  } else {
    // 日/月/年
    day = gtk_entry_get_text(m_entry_ob_year);
    month = gtk_entry_get_text(m_entry_ob_month);
    year = gtk_entry_get_text(m_entry_ob_day);
  }

  unsigned char monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if(!(atoi(year.c_str()) % 4)) {
    monthdays[1] = 29;  // 闰年
  }

  string warming;

  if(atoi(month.c_str()) > 12) {
    warming = _("Month out of range");

    gtk_label_set_text(m_warming_label, warming.c_str());
    gtk_widget_show(GTK_WIDGET(m_warming_label));

    return;
  } else {
    gtk_label_set_text(m_warming_label, "");
  }

  if(atoi(day.c_str()) > monthdays[atoi(month.c_str()) - 1]) {
    warming = _("Day out of range");

    gtk_label_set_text(m_warming_label, warming.c_str());
    gtk_widget_show(GTK_WIDGET(m_warming_label));

    return;
  } else {
    gtk_label_set_text(m_warming_label, "");
  }

  if (!year.empty()) {
    if (g_date_valid_year((GDateYear)atoi(year.c_str())) == FALSE ||
      atoi(year.c_str()) > year_now || abs(atoi(year.c_str()) - year_now) > 1 ||
      (abs(atoi(year.c_str()) - year_now) > 0 && atoi(month.c_str()) != 0 && atoi(month.c_str()) < month_now)) {
      gtk_entry_set_text(m_entry_ob_gw, "");
      gtk_entry_set_text(m_entry_ob_edd, "");

      // The entered date of LMP is under the range:

      stringstream ss;

      if (GetLMP() == _("LMP")) {
        if(m_date_format == 0) {
            // 年/月/日
            ss << _("The entered date of LMP is under the range: ")
              << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
            // 月/日/年
            ss << _("The entered date of LMP is under the range: ")
              << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
            // 日/月/年
            ss << _("The entered date of LMP is under the range: ")
              << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      } else {
        if(m_date_format == 0) {
            // 年/月/日
            ss << _("The entered date of Ovul. Date is under the range: ")
              << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
            // 月/日/年
            ss << _("The entered date of Ovul. Date is under the range: ")
              << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
            // 日/月/年
            ss << _("The entered date of Ovul. Date is under the range: ")
              << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      }

      gtk_label_set_text(m_warming_label, ss.str().c_str());
      gtk_widget_show(GTK_WIDGET(m_warming_label));

      return;
    } else {
      // clear
      gtk_label_set_text(m_warming_label, "");
    }
  } else {
    gtk_entry_set_text(m_entry_ob_gw, "");
    gtk_entry_set_text(m_entry_ob_edd, "");

    return;
  }

  if (!month.empty()) {
    if (g_date_valid_month((GDateMonth)atoi(month.c_str())) == FALSE ||
      (atoi(year.c_str()) == year_now && atoi(month.c_str()) > month_now) ||
      (atoi(year.c_str()) < year_now && atoi(month.c_str()) < month_now)) {
      stringstream ss;


      if(GetLMP() == _("LMP")) {
        if(m_date_format == 0) {
          // 年/月/日
          ss << _("The entered date of LMP is under the range: ")
            << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
          // 月/日/年
          ss << _("The entered date of LMP is under the range: ")
            << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
          // 日/月/年
          ss << _("The entered date of LMP is under the range: ")
            << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      } else {
        if(m_date_format == 0) {
          // 年/月/日
          ss << _("The entered date of Ovul. Date is under the range: ")
            << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
          // 月/日/年
          ss << _("The entered date of Ovul. Date is under the range: ")
            << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
          // 日/月/年
          ss << _("The entered date of Ovul. Date is under the range: ")
            << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      }

      gtk_label_set_text(m_warming_label, ss.str().c_str());
      gtk_entry_set_text(m_entry_ob_gw, "");
      gtk_entry_set_text(m_entry_ob_edd, "");

      return;
    } else {
      gtk_label_set_text(m_warming_label, "");
    }
  } else {
    gtk_label_set_text(m_warming_label, "");
    gtk_entry_set_text(m_entry_ob_gw, "");
    gtk_entry_set_text(m_entry_ob_edd, "");

    return;
  }

  if (!day.empty()) {
    if (g_date_valid_day((GDateDay)atoi(day.c_str())) == FALSE ||
      (atoi(year.c_str()) < year_now && atoi(month.c_str()) == month_now && atoi(day.c_str()) < day_now) ||
      (atoi(year.c_str()) == year_now && atoi(month.c_str()) == month_now && atoi(day.c_str()) > day_now)) {
      stringstream ss;

      if(GetLMP() == _("LMP")) {
        if(m_date_format == 0) {
          // 年/月/日
          ss << _("The entered date of LMP is under the range: ")
            << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
          // 月/日/年
          ss << _("The entered date of LMP is under the range: ")
            << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
          // 日/月/年
          ss << _("The entered date of LMP is under the range: ")
            << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      } else {
        if(m_date_format == 0) {
          // 年/月/日
          ss << _("The entered date of Ovul. Date is under the range: ")
            << year_now - 1 << month_now << day_now << "-" << year_now << month_now << day_now;
        } else if(m_date_format == 1) {
          // 月/日/年
          ss << _("The entered date of Ovul. Date is under the range: ")
            << month_now << day_now << year_now - 1 << "-" << month_now << day_now << year_now;
        } else {
          // 日/月/年
          ss << _("The entered date of Ovul. Date is under the range: ")
            << day_now << month_now << year_now - 1 << "-" << day_now << month_now << year_now;
        }
      }

      gtk_label_set_text(m_warming_label, ss.str().c_str());
      gtk_entry_set_text(m_entry_ob_gw, "");
      gtk_entry_set_text(m_entry_ob_edd, "");

      return;
    } else {
      gtk_label_set_text(m_warming_label, "");
    }
  } else {
    gtk_label_set_text(m_warming_label, "");
    gtk_entry_set_text(m_entry_ob_gw, "");
    gtk_entry_set_text(m_entry_ob_edd, "");

    return;
  }

  gtk_widget_show(GTK_WIDGET(m_warming_label));
  Calc_GW_EDD(year, month, day);
}

void ViewNewPat::KeyEvent(unsigned char keyValue) {
  FakeXEvent::KeyEvent(keyValue);

  switch(keyValue) {
  case KEY_ESC:
  case KEY_PATIENT:
    g_timeout_add(100, signal_callback_exitwindow, this);
    FakeEscKey();
    break;
  default:
    break;
  }
}

GtkWidget* ViewNewPat::create_note_general() {
  GtkTable* table = Utils::create_table(5, 6);

  // Stature

  GtkLabel* label_stature = Utils::create_label(_("Stature:"));
  m_entry_stature = Utils::create_entry();
  GtkLabel* label_cm = Utils::create_label(_("cm"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_stature), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_stature), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_cm), 3, 4, 0, 1);

  gtk_entry_set_max_length(m_entry_stature, 3);
  g_signal_connect(m_entry_stature, "insert_text", G_CALLBACK(signal_entry_insert_stature), this);
  g_signal_connect(m_entry_stature, "focus-out-event", G_CALLBACK(signal_entry_focusout_stature), this);

  // Weight

  GtkLabel* label_weight = Utils::create_label(_("Weight:"));
  m_entry_weight = Utils::create_entry();
  GtkLabel* label_kg = Utils::create_label(_("kg"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_weight), 0, 1, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_weight), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_kg), 3, 4, 1, 2);

  gtk_entry_set_max_length(m_entry_weight, 6);
  g_signal_connect(m_entry_weight, "insert_text", G_CALLBACK(signal_entry_insert_weight), this);
  g_signal_connect(m_entry_weight, "focus-out-event", G_CALLBACK(signal_entry_focusout_weight), this);

  // BSA

  GtkLabel* label_bsa = Utils::create_label(_("BSA:"));
  m_entry_bsa = Utils::create_entry();
  GtkLabel* label_m2 = Utils::create_label(_("m²"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_bsa), 0, 1, 2, 3);
  gtk_table_attach(table, GTK_WIDGET(m_entry_bsa), 1, 3, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_m2), 3, 4, 2, 3);

  gtk_editable_set_editable(GTK_EDITABLE(m_entry_bsa), FALSE);

  return GTK_WIDGET(table);
}

GtkWidget* ViewNewPat::create_note_ob() {
  GtkTable* table = Utils::create_table(5, 30);

  // Date

  m_combobox_ob_date = Utils::create_combobox_text();

  m_entry_ob_year = Utils::create_entry(9679);
  GtkLabel* label_ob_year = Utils::create_label(_("Y"));
  gtk_entry_set_max_length(m_entry_ob_year, 4);

  m_entry_ob_month = Utils::create_entry(9679);
  GtkLabel* label_ob_month = Utils::create_label(_("M"));
  gtk_entry_set_max_length(m_entry_ob_month, 2);

  m_entry_ob_day = Utils::create_entry(9679);
  GtkLabel* label_ob_day = Utils::create_label(_("D"));
  gtk_entry_set_max_length(m_entry_ob_day, 2);

  gtk_table_attach(table, GTK_WIDGET(m_combobox_ob_date), 0, 5, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  switch (m_date_format) {
  case 1:
    // M/D/Y
    {
      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_month), 5, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_month), 7, 8, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_day), 8, 10, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_day), 10, 11, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_year), 11, 14, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_year), 14, 15, 0, 1);
    }

    break;
  case 2:
    // D/M/Y
    {
      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_day), 5, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_day), 7, 8, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_month), 8, 10, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_month), 10, 11, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_year), 11, 14, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_year), 14, 15, 0, 1);
    }

    break;
  default:
    // Y/M/D
    {
      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_year), 5, 8, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_year), 8, 9, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_month), 9, 11, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_month), 11, 12, 0, 1);

      gtk_table_attach(table, GTK_WIDGET(m_entry_ob_day), 12, 14, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_day), 14, 15, 0, 1);
    }

    break;
  }

  gtk_combo_box_text_append_text(m_combobox_ob_date, _("LMP"));
  gtk_combo_box_text_append_text(m_combobox_ob_date, _("Ovul. Date"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);

  g_signal_connect(m_combobox_ob_date, "changed", G_CALLBACK(signal_combobox_changed_ob_date), this);
  g_signal_connect(m_entry_ob_year, "insert_text", G_CALLBACK(signal_entry_insert_ob_year), this);
  g_signal_connect(m_entry_ob_year, "focus-out-event", G_CALLBACK(signal_entry_focusout_ob), this);
  g_signal_connect(m_entry_ob_month, "insert_text", G_CALLBACK(signal_entry_insert_ob_month), this);
  g_signal_connect(m_entry_ob_month, "focus-out-event", G_CALLBACK(signal_entry_focusout_ob), this);
  g_signal_connect(m_entry_ob_day, "insert_text", G_CALLBACK(signal_entry_insert_ob_day), this);
  g_signal_connect(m_entry_ob_day, "focus-out-event", G_CALLBACK(signal_entry_focusout_ob), this);

  // GW

  GtkLabel* label_ob_gw = Utils::create_label(_("GW:"));
  m_entry_ob_gw = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_gw), 15, 20, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_gw), 20, 30, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_editable_set_editable(GTK_EDITABLE(m_entry_ob_gw), FALSE);

  // EDD

  GtkLabel* label_ob_edd = Utils::create_label(_("EDD:"));
  m_entry_ob_edd = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_edd), 0, 5, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_edd), 5, 15, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_editable_set_editable(GTK_EDITABLE(m_entry_ob_edd), FALSE);

  // Gravida

  GtkLabel* label_ob_gravida = Utils::create_label(_("Gravida:"));
  m_entry_ob_gravida = Utils::create_entry(8226);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_gravida), 15, 20, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_gravida), 20, 30, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  // Ectopic

  GtkLabel* label_ob_ectopic = Utils::create_label(_("Ectopic:"));
  m_entry_ob_ectopic = Utils::create_entry(8226);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_ectopic), 0, 5, 2, 3);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_ectopic), 5, 15, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  // Gestations

  GtkLabel* label_ob_gestations = Utils::create_label(_("Gestations:"));
  m_entry_ob_gestations = Utils::create_entry(8226);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_gestations), 15, 20, 2, 3);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_gestations), 20, 30, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  // Para

  GtkLabel* label_ob_para = Utils::create_label(_("Para:"));
  m_entry_ob_para = Utils::create_entry(8226);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_para), 0, 5, 3, 4);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_para), 5, 15, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  // Aborta

  GtkLabel* label_ob_aborta = Utils::create_label(_("Aborta:"));
  m_entry_ob_aborta = Utils::create_entry(8226);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_ob_aborta), 15, 20, 3, 4);
  gtk_table_attach(table, GTK_WIDGET(m_entry_ob_aborta), 20, 30, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  // warming

  m_warming_label = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_warming_label), 0, 15, 4, 5);

  gtk_widget_modify_fg(GTK_WIDGET(m_warming_label), GTK_STATE_NORMAL, Utils::get_color("red"));

  return GTK_WIDGET(table);
}

GtkWidget* ViewNewPat::create_note_card() {
  GtkTable* table = Utils::create_table(5, 30);

  // HR

  GtkLabel* label_hr = Utils::create_label(_("HR:"));
  m_entry_hr = Utils::create_entry(8226);
  GtkLabel* label_bpm = Utils::create_label(_("bpm"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_hr), 0, 5, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_hr), 5, 15, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_bpm), 15, 20, 0, 1);

  gtk_entry_set_max_length(m_entry_hr, 4);
  g_signal_connect(m_entry_hr, "insert_text", G_CALLBACK(signal_entry_insert_hr), this);

  // BP

  GtkLabel* label_bp = Utils::create_label(_("BP(High/Low):"));
  m_entry_bp_high = Utils::create_entry(8226);
  GtkLabel* label_bp_div = Utils::create_label("/");
  m_entry_bp_low = Utils::create_entry(8226);
  GtkLabel* label_mmhg = Utils::create_label(_("mmHg"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_bp), 0, 5, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_bp_high), 5, 9, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_bp_div), 9, 11, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_bp_low), 11, 15, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_mmhg), 15, 20, 1, 2);

  gtk_misc_set_alignment(GTK_MISC(label_bp_div), 0.5, 0.5);

  return GTK_WIDGET(table);
}

GtkWidget* ViewNewPat::create_note_uro() {
  GtkTable* table = Utils::create_table(5, 6);

  GtkLabel* label_uro_psa = Utils::create_label(_("PSA:"));
  m_entry_uro_psa = Utils::create_entry(8226);
  GtkLabel* label_uro_psa_unit = Utils::create_label(_("ng / ml"));

  gtk_table_attach_defaults(table, GTK_WIDGET(label_uro_psa), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_uro_psa), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_uro_psa_unit), 3, 4, 0, 1);

  return GTK_WIDGET(table);
}

GtkWidget* ViewNewPat::create_note_other() {
  GtkTable* table = Utils::create_table(5, 6);

  // Telephone

  GtkLabel* label_other_tel = Utils::create_label(_("Telephone:"));
  m_entry_other_tel = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_other_tel), 0, 1, 0, 1);
  gtk_table_attach(table, GTK_WIDGET(m_entry_other_tel), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_other_tel, 20);

  // Address

  GtkLabel* label_other_address = Utils::create_label(_("Address:"));
  m_entry_other_address = Utils::create_entry(9679);

  gtk_table_attach_defaults(table, GTK_WIDGET(label_other_address), 0, 1, 1, 2);
  gtk_table_attach(table, GTK_WIDGET(m_entry_other_address), 1, 5, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_other_address, 100);

  return GTK_WIDGET(table);
}

void ViewNewPat::CalcBSA() {
  if (!gtk_entry_get_text_length(m_entry_stature) || !gtk_entry_get_text_length(m_entry_weight)) {
    return;
  }

  double stature = atof(gtk_entry_get_text(m_entry_stature));
  double weight = atof(gtk_entry_get_text(m_entry_weight));

  SysCalculateSetting sysCalc;

  double bsa;

  if (sysCalc.GetBSAMethod()) {
    bsa = pow(weight, 0.425) * pow(stature, 0.725) * 71.84/10000; // Occidental
  } else {
    bsa = pow(weight, 0.425) * pow(stature, 0.725) * 73.58/10000; // Oriental
  }

  char bsa_str[10] = {0};
  sprintf(bsa_str, "%.2f", bsa);
  gtk_entry_set_text(m_entry_bsa, bsa_str);
}

void ViewNewPat::Calc_GW_EDD(const string year, const string month, const string day) {
  if (g_date_valid_dmy((GDateDay)atoi(day.c_str()), (GDateMonth)atoi(month.c_str()), (GDateYear)atoi(year.c_str()))) {
    int cur_year = 0;
    int cur_month = 0;
    int cur_day = 0;

    Utils::GetCurrentDate(cur_year, cur_month, cur_day);

    GDate* cur_date = g_date_new_dmy((GDateDay)cur_day, (GDateMonth)cur_month, (GDateYear)cur_year);
    GDate* date = g_date_new_dmy((GDateDay)atoi(day.c_str()), (GDateMonth)atoi(month.c_str()), (GDateYear)atoi(year.c_str()));

    if (g_date_compare(cur_date, date) > 0) {
      char tmp[20] = {0};

      if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
        int gw = MeaCalcFun::CalcGW_LMP(atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str()));
        sprintf(tmp, "%dw%dd", gw/7, gw%7);
        gtk_entry_set_text(m_entry_ob_gw, tmp);
      } else {
        int gw = MeaCalcFun::CalcGW_Ovul(atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str()));
        sprintf(tmp, "%dw%dd", gw/7, gw%7);
        gtk_entry_set_text(m_entry_ob_gw, tmp);
      }

      char edcb[20] = {0};

      if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
        MeaCalcFun::CalcEDCB_LMP(atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str()), edcb);
        gtk_entry_set_text(m_entry_ob_edd, edcb);
      } else {
        MeaCalcFun::CalcEDCB_Ovul(atoi(year.c_str()), atoi(month.c_str()), atoi(day.c_str()), edcb);
        gtk_entry_set_text(m_entry_ob_edd, edcb);
      }
    } else {
      gtk_entry_set_text(m_entry_ob_gw, "");
      gtk_entry_set_text(m_entry_ob_edd, "");
    }

    g_date_free(cur_date);
    g_date_free(date);
  } else {
    gtk_entry_set_text(m_entry_ob_gw, "");
    gtk_entry_set_text(m_entry_ob_edd, "");
  }
}

void ViewNewPat::CheckPatientId(const string pat_id) {
  Database db;

  if (!db.GetPatIDExist(pat_id.c_str()).empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_QUESTION,
      _("Patient ID exist, load data?"), signal_callback_load_patdata, signal_callback_auto_patid);
  }
}

void ViewNewPat::LoadPatData() {
  string pat_id = gtk_entry_get_text(m_entry_patient_id);

  Database db;
  PatientInfo::Patient pat_info;
  db.GetPatInfo(pat_id.c_str(), pat_info);
  FillPatInfo(pat_info);

  gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_patient_id), FALSE);
}

void ViewNewPat::AutoPatID() {
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_patient_id), TRUE);
  gtk_entry_set_text(m_entry_patient_id, GenPatID().c_str());
}

// DESCRIPTION: 自动生成ID, ID的生成规则, 获取当前系统时间和日期, 按规则排序生成标准ID号,
//              如果标准ID号与数据库中的ID发生冲突, 在标准ID号后加上校验位, 校验位从0开始累加。
// RETURN: 生成的ID号
string ViewNewPat::GenPatID() {
  const int id_len = 14;
  char id_std[id_len + 1] = {0};

  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;

  Utils::GetCurrentDateTime(year, month, day, hour, minute, second);
  sprintf(id_std, "%02d%02d%4d%02d%02d%02d", month, day, year, hour, minute, second);
  string id = id_std;

  Database db;
  while (!(db.GetPatIDExist(id.c_str())).empty()) {
    if (id.size() == id_len) {
      id = id + "0";
    } else if (id.size() > id_len) {
      string IDStd = id.substr(0, 14);
      string IDChk = id.substr(14);
      int ChkNum = atoi(IDChk.c_str()) + 1;
      ostringstream strm;
      strm << ChkNum;
      id = IDStd + strm.str();
    }
  }

  return id;
}

void ViewNewPat::FillPatInfo(const PatientInfo::Patient& pat_info) {
  gtk_entry_set_text(m_entry_patient_id, pat_info.id.c_str());
  gtk_entry_set_text(m_entry_last_name, pat_info.name.last.c_str());
  gtk_entry_set_text(m_entry_first_name, pat_info.name.first.c_str());
  gtk_entry_set_text(m_entry_mid_name, pat_info.name.mid.c_str());

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_gender), pat_info.sex);

  stringstream ss;

  if (pat_info.birthDate.year) {
    ss.str("");
    ss << pat_info.birthDate.year;
    gtk_entry_set_text(m_entry_birth_year, ss.str().c_str());
  }

  if (pat_info.birthDate.month) {
    ss.str("");
    ss << pat_info.birthDate.month;
    gtk_entry_set_text(m_entry_birth_month, ss.str().c_str());
  }

  if (pat_info.birthDate.day) {
    ss.str("");
    ss << pat_info.birthDate.day;
    gtk_entry_set_text(m_entry_birth_day, ss.str().c_str());
  }

  if (pat_info.age) {
    ss.str("");
    ss << pat_info.age;
    gtk_entry_set_text(m_entry_age, ss.str().c_str());
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), pat_info.ageUnit);
  }

  // Other
  gtk_entry_set_text(m_entry_other_address, pat_info.address.c_str());
  gtk_entry_set_text(m_entry_other_tel, pat_info.telephone.c_str());
}

void ViewNewPat::FillExamInfo(const PatientInfo::ExamGeneral& exam_info) {
  GtkTextBuffer* buffer = gtk_text_view_get_buffer(m_textview_comment);
  gtk_text_buffer_set_text(buffer, exam_info.comment.c_str(), -1);

  stringstream ss;

  // general info
  if (exam_info.height > 0) {
    ss.str("");
    ss << exam_info.height;
    gtk_entry_set_text(GTK_ENTRY(m_entry_stature), ss.str().c_str());
  }

  if (exam_info.weight > 0) {
    ss.str("");
    ss << exam_info.weight;
    gtk_entry_set_text(GTK_ENTRY(m_entry_weight), ss.str().c_str());
  }

  if (exam_info.BSA > 0) {
    ss.str("");
    ss << exam_info.BSA;
    gtk_entry_set_text(m_entry_bsa, ss.str().c_str());
  }

  if (!exam_info.examDoctor.empty()) {
    int index = Utils::GetComboBoxIndex(GTK_COMBO_BOX(m_comboboxentry_diagnostician), exam_info.examDoctor);

    if (index < 0) {
      gtk_combo_box_append_text(GTK_COMBO_BOX(m_comboboxentry_diagnostician), exam_info.examDoctor.c_str());
      index = Utils::GetComboBoxIndex(GTK_COMBO_BOX(m_comboboxentry_diagnostician), exam_info.examDoctor);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_diagnostician), index);
  }

  if (!exam_info.reportDoctor.empty()) {
    int index = Utils::GetComboBoxIndex(GTK_COMBO_BOX(m_comboboxentry_physician), exam_info.reportDoctor);

    if (index < 0) {
      gtk_combo_box_append_text (GTK_COMBO_BOX(m_comboboxentry_physician), exam_info.reportDoctor.c_str());
      index = Utils::GetComboBoxIndex(GTK_COMBO_BOX(m_comboboxentry_physician), exam_info.reportDoctor);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_physician), index);
  }
}

void ViewNewPat::FillObInfo(const PatientInfo::ObExam& ob) {
  stringstream ss;

  string year;
  string month;
  string day;

  if (ob.LMPDate.year != 0 && ob.LMPDate.month != 0 && ob.LMPDate.day != 0) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);

    ss.str("");
    ss << ob.LMPDate.year;
    year = ss.str();

    ss.str("");
    ss << ob.LMPDate.month;
    month = ss.str();

    ss.str("");
    ss << ob.LMPDate.day;
    day = ss.str();

    if(m_date_format == 0) {
      // 年/月/日
      gtk_entry_set_text(m_entry_ob_year, year.c_str());
      gtk_entry_set_text(m_entry_ob_month, month.c_str());
      gtk_entry_set_text(m_entry_ob_day, day.c_str());
    } else if(m_date_format == 1) {
      // 月/日/年
      gtk_entry_set_text(m_entry_ob_year, month.c_str());
      gtk_entry_set_text(m_entry_ob_month, day.c_str());
      gtk_entry_set_text(m_entry_ob_day, year.c_str());
    } else {
      // 日/月/年
      gtk_entry_set_text(m_entry_ob_year, day.c_str());
      gtk_entry_set_text(m_entry_ob_month, month.c_str());
      gtk_entry_set_text(m_entry_ob_day, year.c_str());
    }

    Calc_GW_EDD(year, month, day);
  } else if (ob.OvulateDate.year != 0 && ob.OvulateDate.month != 0 && ob.OvulateDate.day != 0) {
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 1);

    ss.str("");
    ss << ob.OvulateDate.year;
    year = ss.str();

    ss.str("");
    ss << ob.OvulateDate.month;
    month = ss.str();

    ss.str("");
    ss << ob.OvulateDate.day;
    day = ss.str();

    if(m_date_format == 0) {
      // 年/月/日
      gtk_entry_set_text(m_entry_ob_year, year.c_str());
      gtk_entry_set_text(m_entry_ob_month, month.c_str());
      gtk_entry_set_text(m_entry_ob_day, day.c_str());
    } else if(m_date_format == 1) {
      // 月/日/年
      gtk_entry_set_text(m_entry_ob_year, month.c_str());
      gtk_entry_set_text(m_entry_ob_month, day.c_str());
      gtk_entry_set_text(m_entry_ob_day, year.c_str());
    } else {
      // 日/月/年
      gtk_entry_set_text(m_entry_ob_year, day.c_str());
      gtk_entry_set_text(m_entry_ob_month, month.c_str());
      gtk_entry_set_text(m_entry_ob_day, year.c_str());
    }

    Calc_GW_EDD(year, month, day);
  }

  if (ob.pregCount != -1) {
    ss.str("");
    ss << ob.pregCount;
    gtk_entry_set_text(m_entry_ob_gravida, ss.str().c_str());
  }

  if (ob.abnormalPregCount != -1) {
    ss.str("");
    ss << ob.abnormalPregCount;
    gtk_entry_set_text(m_entry_ob_ectopic, ss.str().c_str());
  }

  if (ob.fetusCount != -1) {
    ss.str("");
    ss << ob.fetusCount;
    gtk_entry_set_text(m_entry_ob_gestations, ss.str().c_str());
  }

  if (ob.deliveryCount != -1) {
    ss.str("");
    ss << ob.deliveryCount;
    gtk_entry_set_text(m_entry_ob_para, ss.str().c_str());
  }

  if (ob.miscarryCount != -1) {
    ss.str("");
    ss << ob.miscarryCount;
    gtk_entry_set_text(m_entry_ob_aborta, ss.str().c_str());
  }
}

void ViewNewPat::FillCarInfo(const PatientInfo::CarExam& car) {
  stringstream ss;

  // card
  if (car.HR != -1) {
    ss.str("");
    ss << car.HR;
    gtk_entry_set_text(m_entry_hr, ss.str().c_str());
  }
  if (car.pressureHigh != -1) {
    ss.str("");
    ss << car.pressureHigh;
    gtk_entry_set_text(m_entry_bp_high, ss.str().c_str());
  }
  if (car.pressureHigh != -1) {
    ss.str("");
    ss << car.pressureLow;
    gtk_entry_set_text(m_entry_bp_low, ss.str().c_str());
  }
}

void ViewNewPat::FillUroInfo(const PatientInfo::UroExam& uro) {
  stringstream ss;

  // URO
  if (uro.PSA != -1) {
    ss.str("");
    ss << uro.PSA;
    gtk_entry_set_text(m_entry_uro_psa, ss.str().c_str());
  }
}

void ViewNewPat::GetPatInfo(PatientInfo::Info &info) {
  string pat_id = gtk_entry_get_text(m_entry_patient_id);
  string name_last = gtk_entry_get_text(m_entry_last_name);
  string name_first = gtk_entry_get_text(m_entry_first_name);
  string name_mid;

  if (!m_langCN) {
    name_mid = gtk_entry_get_text(m_entry_mid_name);
  } else {
    name_mid = "";
  }

  int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_age));
  string age_text = gtk_entry_get_text(m_entry_age);
  string birth_year = gtk_entry_get_text(m_entry_birth_year);
  string birth_month = gtk_entry_get_text(m_entry_birth_month);
  string birth_day = gtk_entry_get_text(m_entry_birth_day);

  int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_gender));

  GtkTextBuffer* buffer = gtk_text_view_get_buffer(m_textview_comment);

  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(buffer, &start);
  gtk_text_buffer_get_end_iter(buffer, &end);

  string comment = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
  string telephone = gtk_entry_get_text(m_entry_other_tel);
  string address = gtk_entry_get_text(m_entry_other_address);

  string diagnostician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_diagnostician));
  string physician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_physician));

  info.p.id = pat_id;

  info.p.name.last = name_last;
  info.p.name.first = name_first;
  info.p.name.mid = name_mid;

  info.p.sex = gender_index;
  info.p.age = atoi(age_text.c_str());
  info.p.ageUnit = age_unit_index;
  info.p.birthDate.year = atoi(birth_year.c_str());
  info.p.birthDate.month = atoi(birth_month.c_str());
  info.p.birthDate.day = atoi(birth_day.c_str());
  info.e.comment = comment;
  info.e.examDoctor = diagnostician;
  info.e.reportDoctor = physician;

  int examYear, examMonth, examDay;
  int examHour, examMin, examSec;
  Utils::GetCurrentDateTime(examYear, examMonth, examDay, examHour, examMin, examSec);
  info.e.examDate.year = examYear;
  info.e.examDate.month = examMonth;
  info.e.examDate.day = examDay;
  info.e.examTime.hour = examHour;
  info.e.examTime.minute = examMin;
  info.e.examTime.second = examSec;

  info.e.examType = TopArea::GetInstance()->GetCheckPart();

  // general
  if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_stature))) {
    string stature = gtk_entry_get_text(GTK_ENTRY(m_entry_stature));
    info.e.height = atoi(stature.c_str());
  } else {
    info.e.height = 0;
  }

  if (gtk_entry_get_text_length(m_entry_weight)) {
    string weight = gtk_entry_get_text(m_entry_weight);
    info.e.weight = atof(weight.c_str());
  } else {
    info.e.weight = 0.0;
  }

  if (gtk_entry_get_text_length(m_entry_bsa)) {
    string BSA = gtk_entry_get_text(m_entry_bsa);
    info.e.BSA = atof(BSA.c_str());
  } else {
    info.e.BSA = 0.0;
  }

  // ob
  int iYear = 0;
  int iMonth = 0;
  int iDay = 0;

  if(m_date_format == 0) {
    // 年/月/日
    iYear = atoi(gtk_entry_get_text(m_entry_ob_year));
    iMonth = atoi(gtk_entry_get_text(m_entry_ob_month));
    iDay = atoi(gtk_entry_get_text(m_entry_ob_day));
  } else if(m_date_format == 1) {
    // 月/日/年
    iMonth = atoi(gtk_entry_get_text(m_entry_ob_year));
    iDay = atoi(gtk_entry_get_text(m_entry_ob_month));
    iYear = atoi(gtk_entry_get_text(m_entry_ob_day));
  } else {
    // 日/月/年
    iDay = atoi(gtk_entry_get_text(m_entry_ob_year));
    iMonth = atoi(gtk_entry_get_text(m_entry_ob_month));
    iYear = atoi(gtk_entry_get_text(m_entry_ob_day));
  }

  if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date))) {
    info.ob.OvulateDate.year = iYear;
    info.ob.OvulateDate.month = iMonth;
    info.ob.OvulateDate.day = iDay;
    info.ob.LMPDate.year = 0;
    info.ob.LMPDate.month = 0;
    info.ob.LMPDate.day = 0;
  } else {
    info.ob.LMPDate.year = iYear;
    info.ob.LMPDate.month = iMonth;
    info.ob.LMPDate.day = iDay;
    info.ob.OvulateDate.year = 0;
    info.ob.OvulateDate.month = 0;
    info.ob.OvulateDate.day = 0;
  }

  if (gtk_entry_get_text_length(m_entry_ob_gravida)) {
    info.ob.pregCount= atoi(gtk_entry_get_text(m_entry_ob_gravida));
  } else {
    info.ob.pregCount = -1;
  }

  if (gtk_entry_get_text_length(m_entry_ob_ectopic)) {
    info.ob.abnormalPregCount = atoi(gtk_entry_get_text(m_entry_ob_ectopic));
  } else {
    info.ob.abnormalPregCount = -1;
  }

  if (gtk_entry_get_text_length(m_entry_ob_gestations)) {
    info.ob.fetusCount= atoi(gtk_entry_get_text(m_entry_ob_gestations));
  } else {
    info.ob.fetusCount = -1;
  }

  if (gtk_entry_get_text_length(m_entry_ob_para)) {
    info.ob.deliveryCount= atoi(gtk_entry_get_text(m_entry_ob_para));
  } else {
    info.ob.deliveryCount = -1;
  }

  if (gtk_entry_get_text_length(m_entry_ob_aborta)) {
    info.ob.miscarryCount= atoi(gtk_entry_get_text(m_entry_ob_aborta));
  } else {
    info.ob.miscarryCount = -1;
  }

  // card
  if (gtk_entry_get_text_length(m_entry_hr)) {
    info.car.HR = atoi(gtk_entry_get_text(m_entry_hr));
  } else {
    info.car.HR = -1;
  }

  if (gtk_entry_get_text_length(m_entry_bp_high)) {
    info.car.pressureHigh = atoi(gtk_entry_get_text(m_entry_bp_high));
  } else {
    info.car.pressureHigh = -1;
  }

  if (gtk_entry_get_text_length(m_entry_bp_low)) {
    info.car.pressureLow = atoi(gtk_entry_get_text(m_entry_bp_low));
  } else {
    info.car.pressureLow = -1;
  }

  // URO
  if (gtk_entry_get_text_length(m_entry_uro_psa)) {
    info.uro.PSA = atoi(gtk_entry_get_text(m_entry_uro_psa));
  } else {
    info.uro.PSA = -1;
  }

  // Other
  info.p.telephone = telephone;
  info.p.address = address;
}

const string ViewNewPat::GetLMP() {
  return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_ob_date));
}

DCMMPPSELEMENT ViewNewPat::GetMPPSElement(PatientInfo::Info& info) {
  DCMMPPSELEMENT ms;

  if (ViewWorkList::GetInstance()->GetWorkListQuery() == 0) {
    ms.msSpecificCharacterSet = "";
    ms.msPatientName = info.p.name.first + info.p.name.mid +info.p.name.last;
    ms.msPatientID = info.p.id;

    string year, mon, day, birthDate,studyDate;
    ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
    birthDate = year+mon+day;
    ms.msPatientBirthDate = birthDate;

    if(info.p.sex == 0) {
      ms.msPatientSex = "F";
    } else if(info.p.sex ==1 ) {
      ms.msPatientSex = "M";
    } else if(info.p.sex ==0 ) {
      ms.msPatientSex = "O";
    }

    ms.msRequestedProcedureID = "";
    ms.msStudyInstanceUID = "";
    ms.msAccessionNumber = "";
    ms.msStudyDescription = "";
    ms.msRequestedProcedureDescription ="";
    ms.msScheduledProcedureStepID = "";

    ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day);
    studyDate = year+mon+day;
    ms.msStudyStartDate = studyDate;

    string hour, min, sec, studyTime;
    ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec);
    studyTime = hour + min + sec;
    ms.msStudyStartTime = studyTime;
  } else {
    ms = ViewWorkList::GetInstance()->GetMPPSElement(info);
  }

  return ms;
}

bool ViewNewPat::AutoCalcAge(const int year, const int month, const int day) {
  if (g_date_valid_dmy((GDateDay)day, (GDateMonth)month, (GDateYear)year) == FALSE) {
    return false;
  }

  int age = 0;
  int age_unit = 0;

  if (CalcAge (year, month, day, age, age_unit)) {
    stringstream ss;
    ss << age;

    gtk_entry_set_text(m_entry_age, ss.str().c_str());
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), age_unit);

    return true;
  } else {
    gtk_entry_set_text(m_entry_age, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);

    return false;
  }
}

void ViewNewPat::EditStudyInfo(PatientInfo::Info& info) {
  SysGeneralSetting sgs;
  int lang = sgs.GetLanguage();

  if(!lang) {
    m_studyInfo.stPatientName = info.p.name.last + '^' + info.p.name.first + '^' + info.p.name.mid;
  } else {
    m_studyInfo.stPatientName = info.p.name.last + info.p.name.first;
  }

  m_studyInfo.stPatientID = info.p.id;

  string year, mon, day;

  ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
  m_studyInfo.stPatientBirthDate = year + mon + day;

  if(info.p.sex == 0) {
    m_studyInfo.stPatientSex = "F";
  } else if(info.p.sex ==1) {
    m_studyInfo.stPatientSex = "M";
  } else if(info.p.sex ==2) {
    m_studyInfo.stPatientSex = "O";
  } else {
    m_studyInfo.stPatientSex = "";
  }

  string age = "";
  string ageUnit = "";

  if (info.p.ageUnit == 0) {
    ageUnit = "Y";
  } else if (info.p.ageUnit == 1) {
    ageUnit = "M";
  } else if (info.p.ageUnit == 2) {
    ageUnit = "D";
  }

  if (info.p.age > 999 || info.p.age <= 0) {
    age = "";
  } else {
    char ageTemp[4] = {0};
    sprintf(ageTemp, "%03d", info.p.age);
    age = ageTemp + ageUnit;
  }

  m_studyInfo.stPatientAge = age;

  char size[20]= {0};
  sprintf(size, "%.2f", info.e.height*0.01);
  m_studyInfo.stPatientSize = Utils::CommaToDotLocaleNumeric(size);

  char weight[30]= {0};
  sprintf(weight, "%.1f", info.e.weight);
  m_studyInfo.stPatientWeight = Utils::CommaToDotLocaleNumeric(weight);

  m_studyInfo.stStudyDescription =info.e.comment;
  m_studyInfo.stStudyDoctor = info.e.examDoctor;

  string hospital = TopArea::GetInstance()->GetHospitalName();
  string machine = UpgradeMan::GetInstance()->GetMachineType();

  ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day);
  m_studyInfo.stStudyDate = year + mon + day;

  string hour;
  string min;
  string sec;
  ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec);
  m_studyInfo.stStudyTime = hour + min + sec;

  m_studyInfo.stFactoryName = "EMP";
  m_studyInfo.stHospital = hospital;
  m_studyInfo.stMechineType = machine;
  m_studyInfo.stMechineSN ="";
  m_studyInfo.stStudyNo = atoi(info.e.examNum.c_str());

  CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
}
