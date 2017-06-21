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
    _("Patient Information"), 800, 600);

  GtkButton* button_end_exam = Utils::add_dialog_button(m_dialog, _("End Exam"), GTK_RESPONSE_NONE, GTK_STOCK_MEDIA_STOP);
  GtkButton* button_new_patient = Utils::add_dialog_button(m_dialog, _("New Patient"), GTK_RESPONSE_NONE, GTK_STOCK_ORIENTATION_PORTRAIT);
  GtkButton* button_new_exam = Utils::add_dialog_button(m_dialog, _("New Exam"), GTK_RESPONSE_NONE, GTK_STOCK_NEW);

  g_signal_connect(button_end_exam, "clicked", G_CALLBACK(signal_button_clicked_end_exam), this);
  g_signal_connect(button_new_patient, "clicked", G_CALLBACK(signal_button_clicked_new_patient), this);
  g_signal_connect(button_new_exam, "clicked", G_CALLBACK(signal_button_clicked_new_exam), this);

  //if (CDCMRegister::GetMe()->IsAuthorize()) {
    GtkButton* button_worklist = Utils::add_dialog_button(m_dialog, _("Worklist"), GTK_RESPONSE_NONE, GTK_STOCK_INFO);
    g_signal_connect(button_worklist, "clicked", G_CALLBACK(signal_button_clicked_worklist), this);
  //}

  GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK, GTK_STOCK_OK);
  GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL, GTK_STOCK_CANCEL);

  g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);
  g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

  // table

  GtkTable* table = Utils::create_table(13, 9);
  gtk_container_set_border_width(GTK_CONTAINER(table), 10);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  // General Information

  GtkFrame* frame = Utils::create_frame(_("General Information"));
  gtk_table_attach_defaults(table, GTK_WIDGET(frame), 0, 9, 0, 4);

  GtkTable* table_frame = Utils::create_table(3, 45);
  gtk_container_set_border_width(GTK_CONTAINER(table_frame), 5);
  gtk_table_set_col_spacings(table_frame, 5);

  gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(table_frame));

  // Patient ID

  GtkLabel* label_patient_id = Utils::create_label(_("Patient ID:"));
  m_entry_patient_id = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_patient_id), 0, 6, 0, 1);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_patient_id), 6, 15, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_patient_id, 15);

  g_signal_connect(m_entry_patient_id, "insert_text", G_CALLBACK(signal_entry_insert_patient_id), this);
  g_signal_connect(m_entry_patient_id, "focus-out-event", G_CALLBACK(signal_entry_focusout_patient_id), this);

  // Auto Generated ID

  GtkCheckButton* m_checkbutton_patient_id = Utils::create_check_button(_("Auto Generated ID"));
  gtk_table_attach_defaults(table_frame, GTK_WIDGET(m_checkbutton_patient_id), 15, 30, 0, 1);

  g_signal_connect(m_checkbutton_patient_id, "clicked", G_CALLBACK(signal_checkbutton_clicked_patient_id), this);

  // Search

  GtkButton* button_search_patient_info = Utils::create_button(_("Search"));
  gtk_table_attach(table_frame, GTK_WIDGET(button_search_patient_info), 36, 45, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_search_patient_info, "clicked", G_CALLBACK (signal_button_clicked_search), this);

  // Last Name

  GtkLabel* label_last_name = Utils::create_label(_("Last Name:"));
  m_entry_last_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_last_name), 0, 6, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_last_name), 6, 15, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_last_name, m_patient_name_last);

  g_signal_connect(G_OBJECT(m_entry_last_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // First Name

  GtkLabel* label_first_name = Utils::create_label(_("First Name:"));
  m_entry_first_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_first_name), 15, 21, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_first_name), 21, 30, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_first_name, m_patient_name_first);

  g_signal_connect(G_OBJECT(m_entry_first_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // Middle Name

  GtkLabel* label_mid_name = Utils::create_label(_("Middle Name:"));
  m_entry_mid_name = Utils::create_entry();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_mid_name), 30, 26, 1, 2);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_mid_name), 36, 45, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_mid_name, m_patient_name_middle);

  g_signal_connect(G_OBJECT(m_entry_mid_name), "insert_text", G_CALLBACK(signal_entry_insert_name), this);

  // Date of Birth

  GtkLabel* label_birth_date = Utils::create_label(_("Date of Birth:"));

  m_entry_birth_year = Utils::create_entry();
  GtkLabel* label_birth_year = Utils::create_label(_("Y"));
  gtk_entry_set_max_length(m_entry_birth_year, 4);

  m_entry_birth_month = Utils::create_entry();
  GtkLabel* label_birth_month = Utils::create_label(_("M"));
  gtk_entry_set_max_length(m_entry_birth_month, 2);

  m_entry_birth_day = Utils::create_entry();
  GtkLabel* label_birth_day = Utils::create_label(_("D"));
  gtk_entry_set_max_length(m_entry_birth_day, 2);

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_date), 0, 6, 2, 3);

  m_date_format = setting.GetDateFormat();

  switch (m_date_format) {
  case 1:
    // M/D/Y
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 6, 8, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 8, 9, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 9, 11, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 11, 12, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 12, 16, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 16, 17, 2, 3);
    }

    break;
  case 2:
    // D/M/Y
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 6, 8, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 8, 9, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 9, 11, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 11, 12, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 12, 16, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 16, 17, 2, 3);
    }

    break;
  default:
    // Y/M/D
    {
      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_year), 6, 8, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_year), 8, 9, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_month), 9, 11, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_month), 11, 12, 2, 3);

      gtk_table_attach(table_frame, GTK_WIDGET(m_entry_birth_day), 12, 16, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_birth_day), 16, 17, 2, 3);
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
  gtk_table_attach(table_frame, GTK_WIDGET(button_birth_calendar), 17, 20, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  Utils::set_button_image(button_birth_calendar, Utils::create_image("./res/icon/Calendar.png"));
  g_signal_connect(button_birth_calendar, "clicked", G_CALLBACK(signal_button_clicked_calendar), this);

  // Age

  GtkLabel* label_age = Utils::create_label(_("Age:"));
  m_entry_age = Utils::create_entry();
  m_combobox_text_age = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_age), 21, 24, 2, 3);
  gtk_table_attach(table_frame, GTK_WIDGET(m_entry_age), 24, 27, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_frame, GTK_WIDGET(m_combobox_text_age), 27, 30, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_age, 3);
  g_signal_connect(m_entry_age, "insert_text", G_CALLBACK(signal_entry_age), this);

  gtk_combo_box_text_append_text(m_combobox_text_age, _("Y"));
  gtk_combo_box_text_append_text(m_combobox_text_age, _("M"));
  gtk_combo_box_text_append_text(m_combobox_text_age, _("D"));

  // Gender

  GtkLabel* label_gender = Utils::create_label(_("Gender:"));
  m_combobox_text_gender = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_frame, GTK_WIDGET(label_gender), 30, 36, 2, 3);
  gtk_table_attach(table_frame, GTK_WIDGET(m_combobox_text_gender), 36, 45, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

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

// ---------------------------------------------------------



#include "display/gui_global.h"






namespace {
int CallBackLoadPatData(gpointer data) {
    ViewNewPat::GetInstance()->LoadPatData();
    return 0;
}
int CallBackAutoPatID(gpointer data) {
    ViewNewPat::GetInstance()->AutoPatID();
    return 0;
}
}

void ViewNewPat::CheckPatID(const gchar *pat_id) {
    Database db;
    if ((db.GetPatIDExist(pat_id)).empty()) {
        return ;
    } else {

        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_QUESTION, _("Patient ID exist, load data?"), CallBackLoadPatData, CallBackAutoPatID);
    }
}

void ViewNewPat::LoadPatData() {
    const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(m_entry_patient_id));
    Database db;
    PatientInfo::Patient pat_info;
    db.GetPatInfo(pat_id, pat_info);
    FillPatInfo(pat_info);
    gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_checkbutton_patient_id), FALSE);
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

void ViewNewPat::AutoPatID() {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(m_checkbutton_patient_id), TRUE);
    string ID = GenPatID();
    gtk_entry_set_text(GTK_ENTRY(m_entry_patient_id), ID.c_str());
}

void ViewNewPat::PatIDFocusOut(GtkWidget *widget, GdkEventFocus *event) {
    const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(widget));
    CheckPatID(pat_id);
}

void ViewNewPat::BirthDateYearInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");

    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() == 4) {
        int year = atoi(old_str.c_str());
        if (year < 1900) {
            *position = 4;      // 移动光标之输入框末尾
            gtk_entry_set_text(m_entry_birth_year, "1900");
            year = 1900;
        } else if (year > m_curYear) {
            *position = 4;
            char curYear[5];
            sprintf(curYear, "%d", m_curYear);
            gtk_entry_set_text(m_entry_birth_year, curYear);
            year = m_curYear;
        }
        const char *month_text = gtk_entry_get_text(m_entry_birth_month);
        const char *day_text = gtk_entry_get_text(m_entry_birth_day);
        AutoCalcAge(year, atoi(month_text), atoi(day_text));
        gtk_widget_grab_focus(GTK_WIDGET(m_entry_birth_month));
    }
    return;
}

void ViewNewPat::BirthDateYearDelete(GtkEditable *editable, gint start_pos, gint end_pos) {
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);
    if (new_str.length() != 4) {
        gtk_entry_set_text(m_entry_age, "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
    }
}

void ViewNewPat::BirthDateMonthInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    if (old_str.length() != 0) {
        int month = atoi(old_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
            return ;
        } else if (month > 12) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(m_entry_birth_month, "12");
            month = 12;
        }
        const char *year_text = gtk_entry_get_text(m_entry_birth_year);
        if (strlen(year_text) != 4)
            return;
        const char *day_text = gtk_entry_get_text(m_entry_birth_day);
        AutoCalcAge(atoi(year_text), month, atoi(day_text));
        if (old_str.length() == 2)
            gtk_widget_grab_focus(GTK_WIDGET(m_entry_birth_day));
    }
    return;
}

void ViewNewPat::BirthDateMonthDelete(GtkEditable *editable, gint start_pos, gint end_pos) {
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);

    if (new_str.length() != 0) {
        int month = atoi(new_str.c_str());
        if (month < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
            return ;
        }
        const char *year_text = gtk_entry_get_text(m_entry_birth_year);
        if (strlen(year_text) != 4)
            return;
        const char *day_text = gtk_entry_get_text(m_entry_birth_day);
        AutoCalcAge(atoi(year_text), month, atoi(day_text));
    } else {
        gtk_entry_set_text(m_entry_age, "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
    }
}

void ViewNewPat::BirthDateDayInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    string old_str = gtk_entry_get_text(GTK_ENTRY(editable));
    string new_str = new_text;
    old_str.insert(*position, new_str);
    int day = atoi(old_str.c_str());

    if (day < 1) {
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
        return ;
    }
    const char *year_text = gtk_entry_get_text(m_entry_birth_year);
    const char *month_text = gtk_entry_get_text(m_entry_birth_month);
    if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
        if (day > 31) {
            *position = 2;      // 移动光标之输入框末尾
            gtk_entry_set_text(m_entry_birth_day, "31");
        }
        return ;
    } else {
        int year = atoi(year_text);
        int month = atoi(month_text);
        int last_day = g_date_get_days_in_month (GDateMonth(month), GDateYear(year));
        if (day > last_day) {
            *position = 2;      // 移动光标之输入框末尾
            char str_last_day[3];
            sprintf(str_last_day, "%d", last_day);
            gtk_entry_set_text(m_entry_birth_day, str_last_day);
            day = last_day;
        }
        AutoCalcAge(year, month, day);
    }
    return;
}

void ViewNewPat::BirthDateDayDelete(GtkEditable *editable, gint start_pos, gint end_pos) {
    string new_str = gtk_entry_get_text(GTK_ENTRY(editable));
    new_str.erase(start_pos, end_pos);

    if (new_str.length() != 0) {
        int day = atoi(new_str.c_str());
        if (day < 1) {
            g_signal_stop_emission_by_name((gpointer)editable, "delete_text");
            return ;
        }
        const char *year_text = gtk_entry_get_text(m_entry_birth_year);
        const char *month_text = gtk_entry_get_text(m_entry_birth_month);
        if (strlen(year_text) == 0 || strlen(year_text) < 4 || strlen(month_text) == 0) {
            return ;
        } else {
            int year = atoi(year_text);
            int month = atoi(month_text);
            AutoCalcAge(year, month, day);
        }
    } else {
        gtk_entry_set_text(m_entry_age, "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
    }
}



GtkWidget* ViewNewPat::create_note_general() {
    GtkWidget *fixed_tab_general;
    GtkWidget *label_stature;
    GtkWidget *label_weight;
    GtkWidget *label_BSA;
    GtkWidget *label_cm;
    GtkWidget *label_kg;
    GtkWidget *label_m2;

    fixed_tab_general = gtk_fixed_new ();
    gtk_widget_show (fixed_tab_general);

    m_entry_stature = gtk_entry_new ();
    gtk_widget_show (m_entry_stature);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_stature, 110, 10);
    gtk_widget_set_size_request (m_entry_stature, 100, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_stature), 3);
    g_signal_connect(G_OBJECT(m_entry_stature), "insert_text", G_CALLBACK(on_entry_stature), this);
    g_signal_connect(G_OBJECT(m_entry_stature), "focus-out-event", G_CALLBACK(HandleStatureFocusOut), this);

    m_entry_weight = gtk_entry_new ();
    gtk_widget_show (m_entry_weight);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_weight, 110, 45);
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_weight), 6);
    gtk_widget_set_size_request (m_entry_weight, 100, 30);
    g_signal_connect(G_OBJECT(m_entry_weight), "insert_text", G_CALLBACK(on_entry_weight), this);
    g_signal_connect(G_OBJECT(m_entry_weight), "focus-out-event", G_CALLBACK(HandleWeightFocusOut), this);

    m_entry_BSA = gtk_entry_new ();
    gtk_widget_show (m_entry_BSA);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), m_entry_BSA, 110, 80);
    gtk_widget_set_size_request (m_entry_BSA, 100, 30);
    gtk_editable_set_editable (GTK_EDITABLE(m_entry_BSA), FALSE);

    label_stature = gtk_label_new (_("Stature:"));
    gtk_widget_show (label_stature);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_stature, 10, 10);
    gtk_widget_set_size_request (label_stature, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_stature), 0.9, 0.5);

    label_weight = gtk_label_new (_("Weight:"));
    gtk_widget_show (label_weight);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_weight, 10, 45);
    gtk_widget_set_size_request (label_weight, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_weight), 0.9, 0.5);

    label_BSA = gtk_label_new (_("BSA:"));
    gtk_widget_show (label_BSA);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_BSA, 10, 80);
    gtk_widget_set_size_request (label_BSA, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_BSA), 0.9, 0.5);

    label_cm = gtk_label_new ("cm");
    gtk_widget_show (label_cm);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_cm, 210, 10);
    gtk_widget_set_size_request (label_cm, 30, 30);

    label_kg = gtk_label_new ("kg");
    gtk_widget_show (label_kg);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_kg, 210, 45);
    gtk_widget_set_size_request (label_kg, 30, 30);

    label_m2 = gtk_label_new ("m²");
    gtk_widget_show (label_m2);
    gtk_fixed_put (GTK_FIXED (fixed_tab_general), label_m2, 210, 80);
    gtk_widget_set_size_request (label_m2, 30, 30);

    return fixed_tab_general;
}

const gchar* ViewNewPat::GetLMP() {
    return gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_combobox_ob_date));
}

void ViewNewPat::OBFocusOut(GtkWidget *widget, GdkEventFocus *event) {
    time_t now;
    time(&now);

    struct tm *ct;
    ct = localtime(&now);

    int year_now = ct->tm_year + 1900;
    int month_now = ct->tm_mon + 1;
    int day_now = ct->tm_mday;

    const gchar *year, *month, *day;
    if(m_date_format == 0) {
        //年/月/日
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    } else if(m_date_format == 1) {
        //月/日/年
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    } else {
        //日/月/年
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }

    unsigned char monthdays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if(!(atoi(year) % 4)) monthdays[1] = 29; ///闰年

    char warming[512];
    if(atoi(month) > 12) {
        sprintf(warming, "%s", _("Month out of range"));
        gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
        gtk_widget_show(m_warming_label);
        return;
    } else {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    }

    if(atoi(day) > monthdays[atoi(month) - 1]) {
        sprintf(warming, "%s", _("Day out of range"));
        gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
        gtk_widget_show(m_warming_label);
        return;
    } else {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    }

    //year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
    if (strlen(year)) {
        if ((g_date_valid_year((GDateYear)atoi(year)) == FALSE) || (atoi(year) > year_now)
                || (abs(atoi(year) - year_now) > 1)
                || (((abs(atoi(year) - year_now)) > 0) && (atoi(month) != 0) && (atoi(month) < month_now))) {
            //gtk_entry_set_text(GTK_ENTRY(widget), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
            ///>The entered date of LMP is under the range:

            if(strcmp(_("LMP"), GetLMP())== 0) {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            } else {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }
            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
            gtk_widget_show(m_warming_label);
            PRINTF("Invalid Year!\n");
            return;
        } else {
            ///>clear
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
        }
    } else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        return;
    }
    //month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
    if (strlen(month)) {
        if ((g_date_valid_month((GDateMonth)atoi(month)) == FALSE)
                || ((atoi(year) == year_now) && (atoi(month) > month_now))
                || ((atoi(year) < year_now) && (atoi(month) < month_now))) {
            if(strcmp(_("LMP"), GetLMP())== 0) {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            } else {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }

            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
            //gtk_entry_set_text(GTK_ENTRY(widget), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
            PRINTF("Invalid Month!\n");
            return;
        } else
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    } else {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        return;
    }
    //day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    if (strlen(day)) {
        if ((g_date_valid_day((GDateDay)atoi(day)) == FALSE)
                || ((atoi(year) < year_now) && (atoi(month) == month_now) && (atoi(day) < day_now))
                || ((atoi(year) == year_now) && (atoi(month) == month_now) && (atoi(day) > day_now))) {

            if(strcmp(_("LMP"), GetLMP())== 0) {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of LMP is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            } else {
                if(m_date_format == 0) {
                    //年/月/日
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  year_now-1, month_now, day_now, year_now, month_now, day_now);

                } else if(m_date_format == 1) {
                    //月/日/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  month_now, day_now, year_now-1, month_now, day_now, year_now);
                } else {
                    //日/月/年
                    sprintf(warming, "%s%d/%d/%d-%d/%d/%d", _("The entered date of Ovul. Date is under the range: "),  day_now, month_now, year_now-1, day_now, month_now, year_now);
                }
            }

            gtk_label_set_text(GTK_LABEL(m_warming_label), warming);
            //gtk_entry_set_text(GTK_ENTRY(widget), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
            PRINTF("Invalid Day!\n");
            return;
        } else
            gtk_label_set_text(GTK_LABEL(m_warming_label), "");
    } else {
        gtk_label_set_text(GTK_LABEL(m_warming_label), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        return;
    }

    gtk_widget_show(m_warming_label);
    Calc_GA_EDD(year, month, day);
}

void ViewNewPat::Calc_GA_EDD(const gchar *year, const gchar *month, const gchar *day) {
    if(g_date_valid_dmy((GDateDay)atoi(day), (GDateMonth)atoi(month), (GDateYear)atoi(year))) {
        int cur_year, cur_month, cur_day;
        Utils::GetCurrentDate(cur_year, cur_month, cur_day);
        GDate* cur_date = g_date_new_dmy((GDateDay)cur_day, (GDateMonth)cur_month, (GDateYear)cur_year);
        GDate* date = g_date_new_dmy((GDateDay)atoi(day), (GDateMonth)atoi(month), (GDateYear)atoi(year));
        if(g_date_compare(cur_date, date) > 0) {
            char tmp[20];
            if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
                int gw = MeaCalcFun::CalcGW_LMP(atoi(year), atoi(month), atoi(day));
                sprintf(tmp, "%dw%dd", gw/7, gw%7);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), tmp);
            } else {
                int gw = MeaCalcFun::CalcGW_Ovul(atoi(year), atoi(month), atoi(day));
                sprintf(tmp, "%dw%dd", gw/7, gw%7);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), tmp);
            }

            char edcb[20];
            if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_ob_date)) == 0) {
                MeaCalcFun::CalcEDCB_LMP(atoi(year), atoi(month), atoi(day), edcb);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), edcb);
            } else {
                MeaCalcFun::CalcEDCB_Ovul(atoi(year), atoi(month), atoi(day), edcb);
                gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), edcb);
            }
        } else {
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        }
        g_date_free(cur_date);
        g_date_free(date);
    } else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
        printf("The Date is not exist!\n");
    }
}

void ViewNewPat::CalcBSA() {
    if (!gtk_entry_get_text_length(GTK_ENTRY(m_entry_stature)) ||
            !gtk_entry_get_text_length(GTK_ENTRY(m_entry_weight)))
        return ;
    double stature = atof(gtk_entry_get_text(GTK_ENTRY(m_entry_stature)));
    double weight = atof(gtk_entry_get_text(GTK_ENTRY(m_entry_weight)));

    double bsa;
    SysCalculateSetting sysCalc;
    if (sysCalc.GetBSAMethod())
        bsa = pow(weight, 0.425)*pow(stature, 0.725)*71.84/10000; //Occidental
    else
        bsa = pow(weight, 0.425)*pow(stature, 0.725)*73.58/10000; //Oriental
    char bsa_str[10];
    sprintf(bsa_str, "%.2f", bsa);
    gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), bsa_str);

}
void ViewNewPat::StatureFocusOut(GtkWidget *widget, GdkEventFocus *event) {
    CalcBSA();
}

void ViewNewPat::WeightFocusOut(GtkWidget *widget, GdkEventFocus *event) {
    CalcBSA();
}

GtkWidget* ViewNewPat::create_note_ob() {
    GtkWidget *fixed_tab_ob;
    GtkWidget *label_ob_year_spacing;
    GtkWidget *label_ob_month_spacing;
    GtkWidget *label_ob_gravida;
    GtkWidget *label_ob_ectopic;
    GtkWidget *label_ob_gestations;
    GtkWidget *label_ob_para;
    GtkWidget *label_ob_aborta;
    GtkWidget *label_ob_GA;
    GtkWidget *label_ob_edd;

    fixed_tab_ob = gtk_fixed_new ();
    gtk_widget_show (fixed_tab_ob);

    m_entry_ob_year = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_year);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_year, 110, 10);
    gtk_widget_set_size_request (m_entry_ob_year, 48, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_year), 9679);
    g_signal_connect(G_OBJECT(m_entry_ob_year), "insert_text", G_CALLBACK(on_entry_ob_year), this);
    g_signal_connect(G_OBJECT(m_entry_ob_year), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);

    m_entry_ob_month = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_month);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_month, 170+5, 10);
    gtk_widget_set_size_request (m_entry_ob_month, /*24*/48, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_month), 9679);
    g_signal_connect(G_OBJECT(m_entry_ob_month), "insert_text", G_CALLBACK(on_entry_ob_month), this);
    g_signal_connect(G_OBJECT(m_entry_ob_month), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);

    m_entry_ob_day = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_day);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_day, /*206*/230+13, 10);
    gtk_widget_set_size_request (m_entry_ob_day, /*24*/48, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_day), 9679);
    g_signal_connect(G_OBJECT(m_entry_ob_day), "insert_text", G_CALLBACK(on_entry_ob_day), this);
    g_signal_connect(G_OBJECT(m_entry_ob_day), "focus-out-event", G_CALLBACK(HandleOBFocusOut), this);
    GtkWidget *label_ob_day_spacing;
    if(m_date_format == 0) {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 4);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 2);
        label_ob_year_spacing = gtk_label_new (_("Y  "));
        label_ob_month_spacing = gtk_label_new (_("M  "));
        label_ob_day_spacing = gtk_label_new(_("D  "));
    } else if(m_date_format == 1) {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 4);
        label_ob_year_spacing = gtk_label_new (_("M  "));
        label_ob_month_spacing = gtk_label_new (_("D  "));
        label_ob_day_spacing = gtk_label_new(_("Y  "));
    } else {
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_year), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_month), 2);
        gtk_entry_set_max_length(GTK_ENTRY(m_entry_ob_day), 4);
        label_ob_year_spacing = gtk_label_new (_("D  "));
        label_ob_month_spacing = gtk_label_new (_("M  "));
        label_ob_day_spacing = gtk_label_new(_("Y  "));
    }
    gtk_widget_show (label_ob_year_spacing);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_year_spacing, 158, 10);
    gtk_widget_set_size_request (label_ob_year_spacing, 12+5, 30);

    gtk_widget_show (label_ob_month_spacing);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_month_spacing, 194+24+5, 10);
    gtk_widget_set_size_request (label_ob_month_spacing, 12+8, 30);

    gtk_widget_show (label_ob_day_spacing);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_day_spacing, 230+48+13, 10);
    gtk_widget_set_size_request (label_ob_day_spacing, 12+5, 30);

    m_entry_ob_GA = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_GA);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_GA, 110, 45);
    gtk_widget_set_size_request (m_entry_ob_GA, 100, 30);
    gtk_editable_set_editable (GTK_EDITABLE (m_entry_ob_GA), FALSE);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_GA), 9679);

    m_entry_ob_EDD = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_EDD);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_EDD, 110, 80);
    gtk_widget_set_size_request (m_entry_ob_EDD, 100, 30);
    gtk_editable_set_editable (GTK_EDITABLE (m_entry_ob_EDD), FALSE);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_EDD), 9679);

    ///>提示语
    m_warming_label = gtk_label_new ("");
    gtk_widget_hide (m_warming_label);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_warming_label, 10, 180);
    gtk_widget_set_size_request (m_warming_label, 450, 30);
    gtk_misc_set_alignment (GTK_MISC (m_warming_label), 0, 0.5); ///左对齐
    gtk_widget_modify_fg(m_warming_label, GTK_STATE_NORMAL, g_red);

    m_entry_ob_gravida = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_gravida);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_gravida, 450, 10);
    gtk_widget_set_size_request (m_entry_ob_gravida, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_gravida), 8226);

    m_entry_ob_ectopic = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_ectopic);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_ectopic, 450, 45);
    gtk_widget_set_size_request (m_entry_ob_ectopic, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_ectopic), 8226);

    m_entry_ob_gestations = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_gestations);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_gestations, 450, 80);
    gtk_widget_set_size_request (m_entry_ob_gestations, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_gestations), 8226);

    m_entry_ob_para = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_para);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_para, 450, 115);
    gtk_widget_set_size_request (m_entry_ob_para, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_para), 8226);

    m_entry_ob_aborta = gtk_entry_new ();
    gtk_widget_show (m_entry_ob_aborta);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_entry_ob_aborta, 450, 150);
    gtk_widget_set_size_request (m_entry_ob_aborta, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_ob_aborta), 8226);

    label_ob_gravida = gtk_label_new (_("Gravida:"));
    gtk_widget_show (label_ob_gravida);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_gravida, 350, 10);
    gtk_widget_set_size_request (label_ob_gravida, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_gravida), 0.9, 0.5);

    label_ob_ectopic = gtk_label_new (_("Ectopic:"));
    gtk_widget_show (label_ob_ectopic);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_ectopic, 350, 45);
    gtk_widget_set_size_request (label_ob_ectopic, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_ectopic), 0.9, 0.5);

    label_ob_gestations = gtk_label_new (_("Gestations:"));
    gtk_widget_show (label_ob_gestations);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_gestations, 350, 80);
    gtk_widget_set_size_request (label_ob_gestations, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_gestations), 0.9, 0.5);

    label_ob_para = gtk_label_new (_("Para:"));
    gtk_widget_show (label_ob_para);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_para, 350, 115);
    gtk_widget_set_size_request (label_ob_para, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_para), 0.9, 0.5);

    label_ob_aborta = gtk_label_new (_("Aborta:"));
    gtk_widget_show (label_ob_aborta);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_aborta, 350, 150);
    gtk_widget_set_size_request (label_ob_aborta, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_aborta), 0.9, 0.5);

    label_ob_GA = gtk_label_new (_("GW:"));
    gtk_widget_show (label_ob_GA);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_GA, 10, 45);
    gtk_widget_set_size_request (label_ob_GA, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_GA), 0.9, 0.5);

    label_ob_edd = gtk_label_new (_("EDD:"));
    gtk_widget_show (label_ob_edd);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), label_ob_edd, 10, 80);
    gtk_widget_set_size_request (label_ob_edd, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_ob_edd), 0.9, 0.5);

    m_combobox_ob_date = gtk_combo_box_new_text ();
    gtk_widget_show (m_combobox_ob_date);
    gtk_fixed_put (GTK_FIXED (fixed_tab_ob), m_combobox_ob_date, 10, 10);
    gtk_widget_set_size_request (m_combobox_ob_date, 100, 30);
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_date), _("LMP"));
    gtk_combo_box_append_text (GTK_COMBO_BOX (m_combobox_ob_date), _("Ovul. Date"));
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);
    g_signal_connect(G_OBJECT(m_combobox_ob_date), "changed", G_CALLBACK(on_combobox_ob_date_changed), this);

    return fixed_tab_ob;
}

void ViewNewPat::ComboboxOBDateChanged(GtkComboBox *widget) {
    const gchar *year, *month, *day;
    if(m_date_format == 0) {
        //年/月/日
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    } else if(m_date_format == 1) {
        //月/日/年
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    } else {
        //日/月/年
        day = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year));
        month = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month));
        year = gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day));
    }

    Calc_GA_EDD(year, month, day);
}

GtkWidget* ViewNewPat::create_note_card() {
    GtkWidget *fixed_tab_card;
    GtkWidget *label_bp_entry;
    GtkWidget *label_bp;
    GtkWidget *label_hr;
    GtkWidget *label_bpm;
    GtkWidget *label_mmhg;

    fixed_tab_card = gtk_fixed_new ();
    gtk_widget_show (fixed_tab_card);

    m_entry_hr = gtk_entry_new ();
    gtk_widget_show (m_entry_hr);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_hr, 115+28, 15);
    gtk_widget_set_size_request (m_entry_hr, 100, 30);
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_hr), 4);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_hr), 8226);
    g_signal_connect(G_OBJECT(m_entry_hr), "insert_text", G_CALLBACK(on_entry_hr), this);

    m_entry_bp_high = gtk_entry_new ();
    gtk_widget_show (m_entry_bp_high);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_bp_high, 115+28, 55);
    gtk_widget_set_size_request (m_entry_bp_high, 50, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_bp_high), 8226);

    label_bp_entry = gtk_label_new ("/");
    gtk_widget_show (label_bp_entry);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bp_entry, 165+28, 55);
    gtk_widget_set_size_request (label_bp_entry, 15, 30);

    m_entry_bp_low = gtk_entry_new ();
    gtk_widget_show (m_entry_bp_low);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), m_entry_bp_low, 180+28, 55);
    gtk_widget_set_size_request (m_entry_bp_low, 50, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_bp_low), 8226);

    label_hr = gtk_label_new (_("HR:"));
    gtk_widget_show (label_hr);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_hr, 5, 15);
    gtk_widget_set_size_request (label_hr, 110, 30);
    gtk_misc_set_alignment (GTK_MISC (label_hr), 0.9, 0.5);

    label_bp = gtk_label_new (_("BP(High/Low):"));
    gtk_widget_show (label_bp);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bp, 5, 55);
    gtk_widget_set_size_request (label_bp, 110+28, 30);
    gtk_misc_set_alignment (GTK_MISC (label_bp), 0.9, 0.5);

    label_bpm = gtk_label_new ("bpm");
    gtk_widget_show (label_bpm);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_bpm, 215+28, 15);
    gtk_widget_set_size_request (label_bpm, 40, 30);

    label_mmhg = gtk_label_new ("mmHg");
    gtk_widget_show (label_mmhg);
    gtk_fixed_put (GTK_FIXED (fixed_tab_card), label_mmhg, 230+28, 55);
    gtk_widget_set_size_request (label_mmhg, 50, 30);

    return fixed_tab_card;
}

GtkWidget* ViewNewPat::create_note_uro() {
    GtkWidget *fixed_tab_uro;
    GtkWidget *label_uro_psa;
    GtkWidget *label_uro_psa_unit;

    fixed_tab_uro = gtk_fixed_new ();
    gtk_widget_show (fixed_tab_uro);

    m_entry_uro_psa = gtk_entry_new ();
    gtk_widget_show (m_entry_uro_psa);
    gtk_fixed_put (GTK_FIXED (fixed_tab_uro), m_entry_uro_psa, 115, 16);
    gtk_widget_set_size_request (m_entry_uro_psa, 100, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_uro_psa), 8226);

    label_uro_psa = gtk_label_new (_("PSA:"));
    gtk_widget_show (label_uro_psa);
    gtk_fixed_put (GTK_FIXED (fixed_tab_uro), label_uro_psa, 15, 15);
    gtk_widget_set_size_request (label_uro_psa, 100, 30);
    gtk_misc_set_alignment (GTK_MISC (label_uro_psa), 0.9, 0.5);

    label_uro_psa_unit = gtk_label_new ("ng / ml");
    gtk_widget_show (label_uro_psa_unit);
    gtk_fixed_put (GTK_FIXED (fixed_tab_uro), label_uro_psa_unit, 215, 15);
    gtk_widget_set_size_request (label_uro_psa_unit, 60, 30);

    return fixed_tab_uro;
}

GtkWidget* ViewNewPat::create_note_other() {
    GtkWidget *fixed_tab_other;

    GtkWidget *label_other_tel;
    //    GtkWidget *entry_other_tel;
    GtkWidget *label_other_address;
    //    GtkWidget *entry_other_address;

    fixed_tab_other = gtk_fixed_new ();
    gtk_widget_show (fixed_tab_other);

    label_other_tel = gtk_label_new (_("Telephone:"));

    gtk_widget_show (label_other_tel);
    gtk_fixed_put (GTK_FIXED (fixed_tab_other), label_other_tel, 10, 10);
    gtk_widget_set_size_request (label_other_tel, 100, 30);

    m_entry_other_tel = gtk_entry_new ();
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_other_tel), 20);
    gtk_widget_show (m_entry_other_tel);
    gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_tel, 110, 10);
    gtk_widget_set_size_request (m_entry_other_tel, 150, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_other_tel), 9679);
    label_other_address = gtk_label_new (_("Address:"));
    gtk_widget_show (label_other_address);
    gtk_fixed_put (GTK_FIXED (fixed_tab_other), label_other_address, 10, 45);
    gtk_widget_set_size_request (label_other_address, 100, 30);

    m_entry_other_address = gtk_entry_new ();
    gtk_entry_set_max_length(GTK_ENTRY(m_entry_other_address), 100);
    gtk_widget_show (m_entry_other_address);
    gtk_fixed_put (GTK_FIXED (fixed_tab_other), m_entry_other_address, 110, 45);

    gtk_widget_set_size_request (m_entry_other_address, 350, 30);
    gtk_entry_set_invisible_char (GTK_ENTRY (m_entry_other_address), 9679);

    return fixed_tab_other;
}

void ViewNewPat::FillPatInfo(const PatientInfo::Patient &pat_info) {
    ostringstream strm;
    gtk_entry_set_text(m_entry_patient_id, pat_info.id.c_str());
    gtk_entry_set_text(m_entry_last_name, pat_info.name.last.c_str());
    gtk_entry_set_text(m_entry_first_name, pat_info.name.first.c_str());
    gtk_entry_set_text(m_entry_mid_name, pat_info.name.mid.c_str());

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_gender), pat_info.sex);

    if (pat_info.birthDate.year) {
        strm.str("");
        strm << pat_info.birthDate.year;
        gtk_entry_set_text(m_entry_birth_year, strm.str().c_str());
    }
    if (pat_info.birthDate.month) {
        strm.str("");
        strm << pat_info.birthDate.month;
        gtk_entry_set_text(m_entry_birth_month, strm.str().c_str());
    }
    if (pat_info.birthDate.day) {
        strm.str("");
        strm << pat_info.birthDate.day;
        gtk_entry_set_text(m_entry_birth_day, strm.str().c_str());
    }

    if (pat_info.age) {
        strm.str("");
        strm << pat_info.age;
        gtk_entry_set_text(m_entry_age, strm.str().c_str());
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), pat_info.ageUnit);
    }
    // Other
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_address), pat_info.address.c_str());
    gtk_entry_set_text(GTK_ENTRY(m_entry_other_tel), pat_info.telephone.c_str());
}

void ViewNewPat::FillExamInfo(const PatientInfo::ExamGeneral &exam_info) {
    ostringstream strm;
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, exam_info.comment.c_str(), -1);

    // general info
    if (exam_info.height > 0) {
        strm.str("");
        strm << exam_info.height;
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature), strm.str().c_str());
    }
    if (exam_info.weight > 0) {
        strm.str("");
        strm << exam_info.weight;
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight), strm.str().c_str());
    }
    if (exam_info.BSA > 0) {
        strm.str("");
        strm << exam_info.BSA;
        gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), strm.str().c_str());
    }

    if (!exam_info.examDoctor.empty()) {
        int index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor);
        if (index < 0) {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor.c_str());
            index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_diagnostician), exam_info.examDoctor);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxentry_diagnostician), index);
    }
    if (!exam_info.reportDoctor.empty()) {
        int index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor);
        if (index < 0) {
            gtk_combo_box_append_text (GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor.c_str());
            index = GetComboBoxIndex(GTK_COMBO_BOX (m_comboboxentry_physician), exam_info.reportDoctor);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX (m_comboboxentry_physician), index);
    }
}

void ViewNewPat::FillObInfo(const PatientInfo::ObExam &ob) {
    ostringstream strm;
    string year, month, day;
    if (ob.LMPDate.year != 0 && ob.LMPDate.month != 0 && ob.LMPDate.day != 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 0);
        strm.str("");
        strm << ob.LMPDate.year;
        year = strm.str();
        strm.str("");
        strm << ob.LMPDate.month;
        month = strm.str();
        strm.str("");
        strm << ob.LMPDate.day;
        day = strm.str();
        if(m_date_format == 0) {
            //年/月/日
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), year.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), day.c_str());
        } else if(m_date_format == 1) {
            //月/日/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        } else {
            //日/月/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }

        Calc_GA_EDD(year.c_str(), month.c_str(), day.c_str());
    } else if (ob.OvulateDate.year != 0 && ob.OvulateDate.month != 0 && ob.OvulateDate.day != 0) {
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_ob_date), 1);
        strm.str("");
        strm << ob.OvulateDate.year;
        year = strm.str();
        strm.str("");
        strm << ob.OvulateDate.month;
        month = strm.str();
        strm.str("");
        strm << ob.OvulateDate.day;
        day = strm.str();
        if(m_date_format == 0) {
            //年/月/日
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), year.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), day.c_str());
        } else if(m_date_format == 1) {
            //月/日/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        } else {
            //日/月/年
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), day.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), month.c_str());
            gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), year.c_str());
        }

        Calc_GA_EDD(year.c_str(), month.c_str(), day.c_str());
    }

    if (ob.pregCount != -1) {
        strm.str("");
        strm << ob.pregCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gravida), strm.str().c_str());
    }
    if (ob.abnormalPregCount != -1) {
        strm.str("");
        strm << ob.abnormalPregCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_ectopic), strm.str().c_str());
    }
    if (ob.fetusCount != -1) {
        strm.str("");
        strm << ob.fetusCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gestations), strm.str().c_str());
    }
    if (ob.deliveryCount != -1) {
        strm.str("");
        strm << ob.deliveryCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_para), strm.str().c_str());
    }
    if (ob.miscarryCount != -1) {
        strm.str("");
        strm << ob.miscarryCount;
        gtk_entry_set_text(GTK_ENTRY(m_entry_ob_aborta), strm.str().c_str());
    }
}

void ViewNewPat::FillCarInfo(const PatientInfo::CarExam &car) {
    ostringstream strm;
    //card
    if (car.HR != -1) {
        strm.str("");
        strm << car.HR;
        gtk_entry_set_text(GTK_ENTRY(m_entry_hr), strm.str().c_str());
    }
    if (car.pressureHigh != -1) {
        strm.str("");
        strm << car.pressureHigh;
        gtk_entry_set_text(GTK_ENTRY(m_entry_bp_high), strm.str().c_str());
    }
    if (car.pressureHigh != -1) {
        strm.str("");
        strm << car.pressureLow;
        gtk_entry_set_text(GTK_ENTRY(m_entry_bp_low), strm.str().c_str());
    }
}

void ViewNewPat::FillUroInfo(const PatientInfo::UroExam &uro) {
    ostringstream strm;
    // URO
    if (uro.PSA != -1) {
        strm.str("");
        strm << uro.PSA;
        gtk_entry_set_text(GTK_ENTRY(m_entry_uro_psa), strm.str().c_str());
    }
}

void ViewNewPat::FillNewPat(const PatientInfo::Info &info) {
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

void ViewNewPat::BtnSearchClicked(GtkButton *button) {
    string id, name_last, name_first, name_mid, birth_year, birth_month, birth_day, age;
    char gender[2], age_unit[2];
    GetEntryTextForDB(GTK_WIDGET(m_entry_patient_id), id);

    GetEntryTextForDB(GTK_WIDGET(m_entry_last_name), name_last);
    GetEntryTextForDB(GTK_WIDGET(m_entry_first_name), name_first);
    GetEntryTextForDB(GTK_WIDGET(m_entry_mid_name), name_mid);
    GetEntryTextForDB(GTK_WIDGET(m_entry_birth_year), birth_year);
    GetEntryTextForDB(GTK_WIDGET(m_entry_birth_month), birth_month);
    GetEntryTextForDB(GTK_WIDGET(m_entry_birth_day), birth_day);
    GetEntryTextForDB(GTK_WIDGET(m_entry_age), age);

    int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_gender));
    if (gender_index == -1)
        sprintf(gender, "%%");// %
    else
        sprintf(gender, "%d", gender_index);

    Database::NewPatSearchTerm term;
    term.id = id;

    term.name.last = name_last;
    term.name.first = name_first;
    term.name.mid = name_mid;

    term.gender = gender;
    term.birthYear = birth_year;

    // format month and day
    char buf[3];
    if (birth_month != "%") {
        int iMon = atoi(birth_month.c_str());
        sprintf(buf, "%d%d", iMon/10, iMon%10);
        birth_month = buf;
    }
    if (birth_day != "%") {
        int iDay = atoi(birth_day.c_str());
        sprintf(buf, "%d%d", iDay/10, iDay%10);
        birth_day = buf;
    }
    term.birthMonth = birth_month;
    term.birthDay = birth_day;
    if (age != "%") {
        int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_age));
        sprintf(age_unit, "%d", age_unit_index);
    } else {
        sprintf(age_unit, "%%");
    }
    term.age = age;
    term.ageUnit = age_unit;

    Database db;
    vector<Database::NewPatSearchResult> vecResult;
    db.NewPatSearch(term, vecResult);
    if (vecResult.empty()) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_INFO, _("No result found!"), NULL);
        return;
    }
    ViewPatSearch::GetInstance()->CreateWindow(GTK_WINDOW(m_dialog), vecResult);
}

static gboolean ExitWindow(gpointer data) {
    ViewNewPat *tmp;
    tmp = (ViewNewPat *)data;
    tmp->DestroyWindow();

    return FALSE;
}

void ViewNewPat::KeyEvent(unsigned char keyValue) {
    FakeXEvent::KeyEvent(keyValue);

    switch(keyValue) {
    case KEY_ESC:
    case KEY_PATIENT:
        g_timeout_add(100, ExitWindow, this);
        FakeEscKey();
        break;

    default:
        break;
    }
}
void ViewNewPat::BtnCancelClicked(GtkButton *button) {
    DestroyWindow();

}

void ViewNewPat::BtnOkClicked(GtkButton *button) {
    PatientInfo::Info info;
    GetPatInfo(info);

    Database db;
    if (!(db.GetPatIDExist(info.p.id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id)) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                                          MessageDialog::DLG_ERROR,
                                          _("Please enter patient ID again!"),
                                          NULL);
        return ;
    }

    db.GetExamIDNext(info.e.examNum);
    if(CDCMRegister::GetMe()->IsAuthorize())
    {
        EditStudyInfo(info);
        SysDicomSetting sysDicomSetting;
        if(!g_patientInfo.GetExist()) {
            if(sysDicomSetting.GetMPPS()) {
                if(!m_flagMPPS) {
                    if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="") {
                        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Please Set the default MPPS service in system setting"), NULL);
                        return ;
                    }
                    CDCMMan::GetMe()->StartMPPS(GetMPPSElement(info));
                    m_flagMPPS = true;
                }
            }
        }
    }

    g_patientInfo.SetInfo(info);
    TopArea* ptrTopArea = TopArea::GetInstance();
    if(ptrTopArea->GetReadImg())
        ptrTopArea->SetReadImg(TRUE);

    DestroyWindow();
}

DCMMPPSELEMENT ViewNewPat::GetMPPSElement(PatientInfo::Info &info) {
    DCMMPPSELEMENT ms;
    if(ViewWorkList::GetInstance()->GetWorkListQuery()==0) {
        ms.msSpecificCharacterSet = "";
        ms.msPatientName = info.p.name.first + info.p.name.mid +info.p.name.last;
        ms.msPatientID = info.p.id;

        string year, mon, day, birthDate,studyDate;
        ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
        birthDate = year+mon+day;
        ms.msPatientBirthDate = birthDate;

        if(info.p.sex == 0)
            ms.msPatientSex = "F";
        else if(info.p.sex ==1 )
            ms.msPatientSex = "M";
        else if(info.p.sex ==0 )
            ms.msPatientSex = "O";

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

void ViewNewPat::EditStudyInfo(PatientInfo::Info &info) {
    SysGeneralSetting sgs;
    int lang = sgs.GetLanguage();
    if(!lang)
        m_studyInfo.stPatientName = info.p.name.last + '^' + info.p.name.first + '^' + info.p.name.mid;
    else
        m_studyInfo.stPatientName = info.p.name.last + info.p.name.first;

    m_studyInfo.stPatientID = info.p.id;
    string year, mon, day, birthDate,studyDate;
    ChangeDateFormatToString(info.p.birthDate.year, info.p.birthDate.month, info.p.birthDate.day, year, mon, day);
    birthDate = year+mon+day;
    m_studyInfo.stPatientBirthDate = birthDate;
    //st.stPatientBirthDate = info.p.birthDate.year + info.p.birthDate.month + info.p.birthDate.day;
    if(info.p.sex == 0)
        m_studyInfo.stPatientSex = "F";
    else if(info.p.sex ==1 )
        m_studyInfo.stPatientSex = "M";
    else if(info.p.sex ==2 )
        m_studyInfo.stPatientSex = "O";
    else
        m_studyInfo.stPatientSex = "";

    string age = "";
    string ageUnit = "";
    if (info.p.ageUnit == 0)
        ageUnit = "Y";
    else if (info.p.ageUnit == 1)
        ageUnit = "M";
    else if (info.p.ageUnit == 2)
        ageUnit = "D";

    char ageTemp[4];
    if (info.p.age > 999 || info.p.age <= 0)
        age = "";
    else {
        sprintf(ageTemp, "%03d", info.p.age);
        age = ageTemp + ageUnit;
    }
    m_studyInfo.stPatientAge = age;

    char size[20]= "\0";
    sprintf(size,"%.2f",info.e.height*0.01);
    CommaToDotLocaleNumeric(size, sizeof(size));
    m_studyInfo.stPatientSize =(string)size;
    char weight[30]= "\0";
    sprintf(weight,"%.1f",info.e.weight);
    CommaToDotLocaleNumeric(weight, sizeof(weight));
    m_studyInfo.stPatientWeight =(string)weight;
    m_studyInfo.stStudyDescription =info.e.comment;
    m_studyInfo.stStudyDoctor = info.e.examDoctor;

    string hospital = TopArea::GetInstance()->GetHospitalName();
    string machine = UpgradeMan::GetInstance()->GetMachineType();

    ChangeDateFormatToString(info.e.examDate.year, info.e.examDate.month, info.e.examDate.day, year, mon, day);
    studyDate = year+mon+day;
    m_studyInfo.stStudyDate =studyDate;

    string hour, min, sec, studyTime;
    ChangeTimeFormatToString(info.e.examTime.hour, info.e.examTime.minute, info.e.examTime.second, hour, min, sec);
    studyTime = hour + min + sec;
    m_studyInfo.stStudyTime = studyTime;

    m_studyInfo.stFactoryName = "EMP";
    m_studyInfo.stHospital = hospital;
    m_studyInfo.stMechineType = machine;
    m_studyInfo.stMechineSN ="";
    m_studyInfo.stStudyNo = atoi(info.e.examNum.c_str());

    CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
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

void ViewNewPat::UpdateStudyInfo() {
    Database db;
    string examNum;
    db.GetExamIDCurrent(examNum);
    m_studyInfo.stStudyNo = atoi(examNum.c_str());
    CDCMMan::GetMe()->EditStudyInfo(m_studyInfo);
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
    m_studyInfo.stStudyDate ="";
    m_studyInfo.stStudyTime ="";
    m_studyInfo.stFactoryName ="";
    m_studyInfo.stHospital ="";
    m_studyInfo.stMechineType ="";
    m_studyInfo.stMechineSN ="";
    m_studyInfo.stStudyNo=0;

}

// DESCRIPTION: 自动生成ID, ID的生成规则, 获取当前系统时间和日期, 按规则排序生成标准ID号,
//              如果标准ID号与数据库中的ID发生冲突, 在标准ID号后加上校验位, 校验位从0开始累加。
// RETURN: 生成的ID号
string ViewNewPat::GenPatID() {
    const unsigned int id_len = 14;
    char id_std[id_len+1];
    int Year, Month, Day, Hour, Minute, Second;
    Utils::GetCurrentDateTime(Year, Month, Day, Hour, Minute, Second);
    sprintf(id_std, "%02d%02d%4d%02d%02d%02d", Month, Day, Year, Hour, Minute, Second);
    string ID = id_std;
    Database db;
    while (!(db.GetPatIDExist(ID.c_str())).empty()) {
        if (ID.size() == id_len) {
            ID = ID + "0";
        } else if (ID.size() > id_len) {
            string IDStd = ID.substr(0, 14);
            string IDChk = ID.substr(14);
            int ChkNum = atoi(IDChk.c_str()) + 1;
            ostringstream strm;
            strm << ChkNum;
            ID = IDStd + strm.str();
        }
    }
    return ID;
}

void ViewNewPat::ChkBtnPatIDClicked(GtkButton *button) {
    SysGeneralSetting sys;

    gboolean value = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    if (value) {
        string ID = GenPatID();
        gtk_entry_set_text(GTK_ENTRY(m_entry_patient_id), ID.c_str());
        sys.SetAutoGeneratedID(1);
        sys.SyncFile();
    } else {
        gtk_entry_set_text(GTK_ENTRY(m_entry_patient_id), "");
        gtk_widget_grab_focus(GTK_WIDGET(m_entry_patient_id));
        sys.SetAutoGeneratedID(0);
        sys.SyncFile();
    }
}

void ViewNewPat::BtnWorkListClicked(GtkButton *button) {
    string device = CDCMMan::GetMe()->GetDefaultWorklistServiceDevice();
    if (device == "") {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()), MessageDialog::DLG_ERROR, _("Please Set the default worklist service in system setting"), NULL);
        return ;
    }
    ViewWorkList::GetInstance()->CreateWorkListWin(GTK_WIDGET(m_dialog));
}

void ViewNewPat::BtnExamEndClicked(GtkButton *button) {
    PatientInfo::Info info;
    GetPatInfo(info);

    Database db;
    if (!(db.GetPatIDExist(info.p.id.c_str())).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id)) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog),
                                          MessageDialog::DLG_ERROR,
                                          _("Please enter patient ID again!"),
                                          NULL);
        return ;
    }

    db.GetExamIDNext(info.e.examNum);

    g_patientInfo.SetInfo(info);

    if(CDCMRegister::GetMe()->IsAuthorize())
    {
        SysDicomSetting sysDicomSetting;
        if(!g_patientInfo.GetExist()) {
            if(sysDicomSetting.GetMPPS()) {
                if(!m_flagMPPS) {
                    if(CDCMMan::GetMe()->GetDefaultMPPSServiceDevice()=="") {
                        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_ERROR, _("Please Set the default MPPS service in system setting"), NULL);
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

void ViewNewPat::BtnNewPatClicked(GtkButton *button) {
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

void ViewNewPat::BtnNewExamClicked(GtkButton *button) {
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


void ViewNewPat::CommentInsert(GtkTextBuffer *textbuffer, GtkTextIter *location, gchar *text, gint len) {
    if (g_ascii_strcasecmp(text, "'") == 0)
        g_signal_stop_emission_by_name((gpointer)textbuffer, "insert_text");
    return;
}

void ViewNewPat::EntryNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isdigit(*new_text) && g_ascii_strcasecmp(new_text, ".") != 0)
        g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    return;
}

void ViewNewPat::EntryAlNumInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    if (!g_ascii_isalnum(*new_text)) {
        if((g_ascii_strcasecmp(new_text, "-") !=0) && (g_ascii_strcasecmp(new_text, "_") !=0) &&  (g_ascii_strcasecmp(new_text, "/") !=0))
            g_signal_stop_emission_by_name((gpointer)editable, "insert_text");
    }
    return;
}

void ViewNewPat::EntryNameInsert(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position) {
    gint old_text_length = strlen(gtk_entry_get_text(GTK_ENTRY(editable)));

    if((old_text_length + new_text_length > gtk_entry_get_max_length(GTK_ENTRY(editable)))||(g_ascii_strcasecmp(new_text, "'")==0)) {
        gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
    }
}


bool ViewNewPat::AutoCalcAge(const int year, const int month, const int day) {
    if (g_date_valid_dmy((GDateDay)day, (GDateMonth)month, (GDateYear)year) == FALSE) {
        return false;
    }

    int age, age_unit;
    if (CalcAge (year, month, day, age, age_unit)) {
        ostringstream strm;
        strm << age;
        gtk_entry_set_text(m_entry_age, strm.str().c_str());
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), age_unit);
        return true;
    } else {
        gtk_entry_set_text(m_entry_age, "");
        gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_text_age), 0);
        return false;
    }
}

void ViewNewPat::ClearData() {
    if (!GTK_IS_WIDGET(m_dialog))
        return ;

    gtk_entry_set_text(GTK_ENTRY(m_entry_patient_id), "");
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
    if (!GTK_IS_WIDGET(m_dialog))
        return ;

    // general
    if(m_clearAll) {
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_BSA), "");
    }
    //ob
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_year), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_month), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_day), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_GA), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_EDD), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gravida), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_ectopic), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_gestations), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_para), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_ob_aborta), "");
    //card
    gtk_entry_set_text(GTK_ENTRY(m_entry_hr), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_bp_high), "");
    gtk_entry_set_text(GTK_ENTRY(m_entry_bp_low), "");
    // URO
    gtk_entry_set_text(GTK_ENTRY(m_entry_uro_psa), "");
    // Other
    if(m_clearAll) {
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_tel), "");
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_address), "");
    }

    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, "", -1);

    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_diagnostician), -1);
    gtk_combo_box_set_active(GTK_COMBO_BOX(m_comboboxentry_physician), -1);
}

void ViewNewPat::GetPatInfo(PatientInfo::Info &info) {
    const gchar *pat_id = gtk_entry_get_text(GTK_ENTRY(m_entry_patient_id));

    const gchar *name_last = gtk_entry_get_text(m_entry_last_name);
    const gchar *name_first = gtk_entry_get_text(m_entry_first_name);
    const gchar *name_mid = NULL;
    if (!m_langCN)
        name_mid = gtk_entry_get_text(m_entry_mid_name);
    else
        name_mid = "";

    const gchar *age_text = gtk_entry_get_text(m_entry_age);
    int age_unit_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_age));
    const gchar *birth_year = gtk_entry_get_text(m_entry_birth_year);
    const gchar *birth_month = gtk_entry_get_text(m_entry_birth_month);
    const gchar *birth_day = gtk_entry_get_text(m_entry_birth_day);

    int gender_index = gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_text_gender));
    // if (gender_index == -1)
    // 	gender_index = 0;

    GtkTextBuffer *buffer;
    GtkTextIter start, end;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    const gchar *comment = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    const gchar *telephone = gtk_entry_get_text(GTK_ENTRY(m_entry_other_tel));
    const gchar *address = gtk_entry_get_text(GTK_ENTRY(m_entry_other_address));

    gchar *diagnostician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_diagnostician));
    gchar *physician = gtk_combo_box_get_active_text(GTK_COMBO_BOX(m_comboboxentry_physician));

    info.p.id = pat_id;

    info.p.name.last = name_last;
    info.p.name.first = name_first;
    info.p.name.mid = name_mid;

    info.p.sex = gender_index;
    info.p.age = atoi(age_text);
    info.p.ageUnit = age_unit_index;
    info.p.birthDate.year = atoi(birth_year);
    info.p.birthDate.month = atoi(birth_month);
    info.p.birthDate.day = atoi(birth_day);
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
        const char *stature = gtk_entry_get_text(GTK_ENTRY(m_entry_stature));
        info.e.height = atoi(stature);
    } else {
        info.e.height = 0;
    }
    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_weight))) {
        const char *weight = gtk_entry_get_text(GTK_ENTRY(m_entry_weight));
        info.e.weight = atof(weight);
    } else {
        info.e.weight = 0.0;
    }
    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_BSA))) {
        const char* BSA = gtk_entry_get_text(GTK_ENTRY(m_entry_BSA));
        info.e.BSA = atof(BSA);
    } else {
        info.e.BSA = 0.0;
    }

    //ob
    int iYear, iMonth, iDay;
    if(m_date_format == 0) {
        //年/月/日
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
    } else if(m_date_format == 1) {
        //月/日/年
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
    } else {
        //日/月/年
        iDay = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_year)));
        iMonth = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_month)));
        iYear = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_day)));
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
    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_gravida)))
        info.ob.pregCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_gravida)));
    else
        info.ob.pregCount = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_ectopic)))
        info.ob.abnormalPregCount = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_ectopic)));
    else
        info.ob.abnormalPregCount = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_gestations)))
        info.ob.fetusCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_gestations)));
    else
        info.ob.fetusCount = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_para)))
        info.ob.deliveryCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_para)));
    else
        info.ob.deliveryCount = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_ob_aborta)))
        info.ob.miscarryCount= atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_ob_aborta)));
    else
        info.ob.miscarryCount = -1;

    // card
    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_hr)))
        info.car.HR = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_hr)));
    else
        info.car.HR = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_bp_high)))
        info.car.pressureHigh = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_bp_high)));
    else
        info.car.pressureHigh = -1;

    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_bp_low)))
        info.car.pressureLow = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_bp_low)));
    else
        info.car.pressureLow = -1;
    // URO
    if (gtk_entry_get_text_length(GTK_ENTRY(m_entry_uro_psa)))
        info.uro.PSA = atoi(gtk_entry_get_text(GTK_ENTRY(m_entry_uro_psa)));
    else
        info.uro.PSA = -1;
    // Other
    info.p.telephone = telephone;
    info.p.address = address;
}

void ViewNewPat::ButtonClickedCalendar(GtkButton* button) {
  ViewCalendar::GetInstance()->CreateWindow(GTK_WIDGET(m_dialog), ViewCalendar::START, this);
}

void ViewNewPat::SetStartDate(int year, int month, int day) {
    guint i;
    int pos, iCurrent;
    char tmp[10];
    char str[2];

    //add restirction
    time_t now;
    struct tm *tm_now;
    now = time(NULL);
    tm_now = localtime(&now);
    iCurrent =  (tm_now->tm_year + 1900) * 10000 + (tm_now->tm_mon + 1) * 100 + tm_now->tm_mday * 1;
    int iTime = year * 10000 + month * 100 + day * 1;
    if (iTime > iCurrent) {
        MessageDialog::GetInstance()->Create(GTK_WINDOW(ViewNewPat::GetInstance()->GetWindow()),
                                          MessageDialog::DLG_ERROR,
                                          _("The time input is not correct!"),
                                          NULL);
    } else {

        pos = 0;
        gtk_entry_set_text(m_entry_birth_year, "");
        sprintf(tmp, "%d", year);
        for(i=0; i<strlen(tmp); i++) {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_year), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(m_entry_birth_month, "");
        sprintf(tmp, "%d", month);
        for(i=0; i<strlen(tmp); i++) {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_month), str, 1, &pos);
        }

        pos = 0;
        gtk_entry_set_text(m_entry_birth_day, "");
        sprintf(tmp, "%d", day);
        for(i=0; i<strlen(tmp); i++) {
            sprintf(str, "%c", tmp[i]);
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_day), str, 1, &pos);
        }

        // auto fill age
        if (!AutoCalcAge(year, month, day)) {
            PRINTF("Calc Age Error!\n");
        }
    }
}

void ViewNewPat::InsertPatientInfo(const char *ID, PatientInfo::Name patientName, string birthDate,string age,const char *sex,const char *size,const char *weight,const char *address,const char *doctor,const char *description) {
    m_clearAll = true;
    ClearData();

    char size_tmp[256];
    char str[2];

    char birthDate_tmp[256];
    char birth_str[2];
    int birth_pos ;

    gtk_entry_set_text(GTK_ENTRY(m_entry_patient_id),ID);
    // CheckPatID(ID);
    Database db;
    if(!((db.GetPatIDExist(ID)).empty() && GTK_WIDGET_IS_SENSITIVE(m_entry_patient_id))) {
        gtk_widget_set_sensitive(GTK_WIDGET(m_entry_patient_id), false);
        MessageDialog::GetInstance()->Create(GTK_WINDOW(m_dialog), MessageDialog::DLG_QUESTION, _("Patient ID exist, load data?"), CallBackLoadPatData, CallBackAutoPatID);

    } else {
        char name_last[PATIENT_NAME_MAX_LEN+1] = {0};
        char name_first[PATIENT_NAME_MAX_LEN+1]= {0};
        char name_mid[PATIENT_NAME_MAX_LEN+1]= {0};
        strncpy(name_last, patientName.last.c_str(), PATIENT_NAME_MAX_LEN);
        strncpy(name_first, patientName.first.c_str(), PATIENT_NAME_MAX_LEN);
        strncpy(name_mid, patientName.mid.c_str(), PATIENT_NAME_MAX_LEN);
        gtk_entry_set_text(m_entry_last_name, name_last);
        gtk_entry_set_text(m_entry_first_name, name_first);
        gtk_entry_set_text(m_entry_mid_name, name_mid);

        //Age
        if(strlen(age.c_str())!=0) {
            int age_len = strlen(age.c_str());
            char age_tmp[256]="\0";
            char str[256]="\0";
            sprintf(str,"%c%c%c",age[0],age[1],age[2]);
            sprintf(age_tmp,"%d",atoi(str));
            gtk_entry_set_text(m_entry_age,age_tmp);

            char str_tmp[2];
            sprintf(str_tmp,"%c",age[age_len-1]);
            if(strcmp(str_tmp,"Y")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age),0);
            else if(strcmp(str_tmp,"M")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age),1);
            else if(strcmp(str_tmp,"D")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age),2);
        } else {
            gtk_entry_set_text(m_entry_age," ");
            gtk_combo_box_set_active (GTK_COMBO_BOX(m_combobox_text_age),-1);
        }
        //sex
        if (sex !=NULL) {
            if(strcmp(sex ,"F")==0)
                gtk_combo_box_set_active (GTK_COMBO_BOX (m_combobox_text_gender),0);
            else if(strcmp(sex , "M")==0)
                gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender), 1);
            else if(strcmp(sex , "O")==0)
                gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender), 2);

        } else
            gtk_combo_box_set_active(GTK_COMBO_BOX (m_combobox_text_gender),-1);

        //birthDate
        if(strlen(birthDate.c_str())!=0) {
            int pos ;
            char birth_year[100] = "\0";
            char birth_month[100] = "\0";
            char birth_day[100] = "\0";
            sprintf(birth_year,"%c%c%c%c",birthDate[0],birthDate[1],birthDate[2],birthDate[3]);
            sprintf(birth_month,"%c%c",birthDate[4],birthDate[5]);
            sprintf(birth_day,"%c%c",birthDate[6],birthDate[7]);
            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_year),birth_year,4,&pos);

            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_month),birth_month,2,&pos);

            pos = 0;
            gtk_editable_insert_text(GTK_EDITABLE(m_entry_birth_day),birth_day,2,&pos);
        } else {
            gtk_entry_set_text(m_entry_birth_year," ");
            gtk_entry_set_text(m_entry_birth_month," ");
            gtk_entry_set_text(m_entry_birth_day," ");
        }

        char sizeT[256];
        strcpy(sizeT,size);
        char weightT[256];
        strcpy(weightT,weight);
        DotToCommaLocaleNumeric(sizeT, sizeof(sizeT));

        float stature = atof(sizeT);
        int stature_real=stature*100;
        sprintf(size_tmp,"%d",stature_real);
        gtk_entry_set_text(GTK_ENTRY(m_entry_stature),size_tmp);

        char weight_buf[30]="\0";
        DotToCommaLocaleNumeric(weightT, sizeof(weightT));

        float weight_tmp = atof(weightT);
        sprintf(weight_buf,"%.1f",weight_tmp);
        gtk_entry_set_text(GTK_ENTRY(m_entry_weight),weight_buf);
        CalcBSA();
        gtk_entry_set_text(GTK_ENTRY(m_entry_other_address),address);

    }

    GtkWidget *entry = gtk_bin_get_child(GTK_BIN(m_comboboxentry_diagnostician));
    gtk_entry_set_text(GTK_ENTRY(entry),doctor);
    GtkTextBuffer *buffer;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(m_textview_comment));
    gtk_text_buffer_set_text(buffer, description, -1);

}
