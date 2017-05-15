#include "sysMan/ViewDicomSupervise.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include "keyboard/KeyDef.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/MultiFuncFactory.h"
#include "periDevice/DCMRegister.h"
#include "periDevice/PeripheralMan.h"

ViewDicomSupervise* ViewDicomSupervise::m_instance = NULL;

// ---------------------------------------------------------

ViewDicomSupervise* ViewDicomSupervise::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewDicomSupervise();
  }

  return m_instance;
}

ViewDicomSupervise::ViewDicomSupervise() {
  m_dialog = NULL;
  m_label_export = NULL;
  m_label_register = NULL;
  m_entry_key = NULL;
}

ViewDicomSupervise::~ViewDicomSupervise() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewDicomSupervise::CreateWindow() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  m_dialog = Utils::create_dialog(NULL, _("DICOM Register"), 560, 420);

  GtkButton* button_exit = Utils::add_dialog_button(m_dialog, _("Exit"), GTK_RESPONSE_CLOSE, GTK_STOCK_QUIT);
  g_signal_connect(button_exit, "clicked", G_CALLBACK(signal_button_clicked_exit), this);

  GtkTable* table = Utils::create_table(10, 1);
  gtk_container_set_border_width(GTK_CONTAINER(table), 0);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  GtkTable* table_export = Utils::create_table(6, 3);
  GtkHSeparator* hseparator = Utils::create_hseparator();
  GtkTable* table_register = Utils::create_table(2, 3);

  gtk_table_attach_defaults(table, GTK_WIDGET(table_export), 0, 1, 0, 6);
  gtk_table_attach(table, GTK_WIDGET(hseparator), 0, 1, 6, 7, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(table_register), 0, 1, 7, 10);

  gtk_container_set_border_width(GTK_CONTAINER(table_export), 10);
  gtk_container_set_border_width(GTK_CONTAINER(table_register), 10);

  // export
  GtkLabel* label_note = Utils::create_label(_("Note: According to the license file,the manufacturer will generate a unique\nregister key for every machine, you can register DICOM function by inputting\nthist register key. If register is successful, you can use the DICOM function.\n\nPlease insert Udisk, and click button 'Export License File'. You will find this\nfile named 'license' in Udisk. Please send this file to manufacturer."));
  GtkButton* button_export = Utils::create_button(_("Export License File"));
  m_label_export = Utils::create_label("");

  gtk_label_set_line_wrap(label_note, FALSE);
  gtk_misc_set_alignment(GTK_MISC(label_note), 0, 0);

  gtk_table_attach_defaults(table_export, GTK_WIDGET(label_note), 0, 3, 0, 5);
  gtk_table_attach(table_export, GTK_WIDGET(button_export), 0, 1, 5, 6, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_export, GTK_WIDGET(m_label_export), 1, 3, 5, 6);

  g_signal_connect(button_export, "clicked", G_CALLBACK(signal_button_clicked_export), this);

  // register
  GtkLabel* label_key = Utils::create_label(_("Register Key:"));
  m_entry_key = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_register, GTK_WIDGET(label_key), 0, 1, 0, 1);
  gtk_table_attach(table_register, GTK_WIDGET(m_entry_key), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  GtkButton* button_register = Utils::create_button(_("Register"));
  m_label_register = Utils::create_label("");

  gtk_table_attach(table_register, GTK_WIDGET(button_register), 0, 1, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_register, GTK_WIDGET(m_label_register), 1, 3, 1, 2);

  g_signal_connect(button_register, "clicked", G_CALLBACK(signal_button_clicked_register), this);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ViewDicomSupervise::Authenticate() {
  if (m_timer > 0) {
    g_source_remove(m_timer);
    m_timer = 0;
    g_keyInterface.Pop();
  }

  g_keyInterface.Push(this);
  m_vecAuthenInfo.clear();
  m_timer = g_timeout_add(10000, signal_callback_authen, this);
}

// ---------------------------------------------------------

void ViewDicomSupervise::ButtonClickedExport(GtkButton* button) {
  if(!PeripheralMan::GetInstance()->CheckUsbStorageState()) {
    gtk_label_set_text(m_label_export, _("No USB storage found!"));

    return;
  } else {
    if(!PeripheralMan::GetInstance()->MountUsbStorage()) {
      gtk_label_set_text(m_label_export, _("Failed to mount USB storage!"));

      return;
    }
  }

  string destFileDir = UDISK_PATH;
  if(CDCMRegister::GetMe() == NULL) {
    return;
  }

  bool value = CDCMRegister::GetMe()->GenerateLicenseFile(destFileDir);
  string info;

  if(value) {
    info = _("Succeed to export file!");
  } else {
    info = _("Fail to export file!");
  }

  gtk_label_set_text(m_label_export, info.c_str());

  PeripheralMan::GetInstance()->UmountUsbStorage();
}

void ViewDicomSupervise::ButtonClickedRegister(GtkButton* button) {
  string registerKey = gtk_entry_get_text(m_entry_key);

  if(CDCMRegister::GetMe() == NULL) {
    return;
  }

  bool value = CDCMRegister::GetMe()->CheckAuthorize(registerKey);
  string info;

  if(value) {
    info = _("Succeed to register!");
  } else {
    info = _("Fail to register!");
  }

  gtk_label_set_text(m_label_register, info.c_str());
}

void ViewDicomSupervise::ButtonClickedExit(GtkButton* button) {
  DestroyWindow();
}

bool ViewDicomSupervise::IsAuthenValid() {
  m_timer = 0;

  if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'I')) {
    PRINTF("not authen\n");
    Exit();
  } else {
    PRINTF("authen\n");
  }

  return false;
}

void ViewDicomSupervise::KeyEvent(unsigned char keyValue) {
  switch(keyValue) {
  case KEY_ESC:
    {
      g_timeout_add(100, signal_callback_exitwindow, this);

      FakeEscKey();

      break;
    }
  default:
    {
      FakeXEvent::KeyEvent(keyValue);

      break;
    }
  }
}

void ViewDicomSupervise::DestroyWindow() {
  if (GTK_IS_WIDGET(m_dialog)) {
    Exit();

    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }

  m_dialog = NULL;
  m_label_export = NULL;
  m_label_register = NULL;
  m_entry_key = NULL;
}

void ViewDicomSupervise::Exit() {
  g_keyInterface.Pop();
}
