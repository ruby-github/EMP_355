#include "sysMan/MenuSystem.h"

#include "display/MenuArea.h"
#include "imageProc/FreezeMode.h"
#include "imageProc/ModeStatus.h"
#include "sysMan/ViewSystem.h"

MenuSystem* MenuSystem::m_instance = NULL;

// ---------------------------------------------------------

MenuSystem* MenuSystem::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new MenuSystem();
  }

  return m_instance;
}

MenuSystem::MenuSystem() {
  m_table = NULL;
}

MenuSystem::~MenuSystem() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* MenuSystem::Create() {
  m_table = Utils::create_table(2, 1);

  GtkButton* button_general = Utils::create_button(_("System Setting"));
  GtkButton* button_biopsy = Utils::create_button(_("Biopsy Setting"));

  g_signal_connect(button_general, "clicked", G_CALLBACK(signal_button_clicked_general), this);
  g_signal_connect(button_biopsy, "clicked", G_CALLBACK(signal_button_clicked_biopsy), this);

  gtk_table_attach(m_table, GTK_WIDGET(button_general), 0, 1, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_table, GTK_WIDGET(button_biopsy), 0, 1, 1, 2, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  return GTK_WIDGET(m_table);
}

void MenuSystem::Show() {
  if (m_table != NULL) {
    gtk_widget_show_all(GTK_WIDGET(m_table));
  }
}

void MenuSystem::Hide() {
  if (m_table != NULL) {
    gtk_widget_hide_all(GTK_WIDGET(m_table));
  }
}

// ---------------------------------------------------------

void MenuSystem::ButtonClickedGeneral(GtkButton* button) {
  if (ModeStatus::IsAutoReplayMode()) {
    FreezeMode::GetInstance()->ChangeAutoReplay();
  } else if (ModeStatus::IsUnFreezeMode()) {
    FreezeMode::GetInstance()->PressFreeze();
  } else {
  }

  ViewSystem::GetInstance()->CreateWindow();
  ViewSystem::GetInstance()->ChangeNoteBookPage(0);
}

void MenuSystem::ButtonClickedBiopsy(GtkButton* buuton) {
  MenuArea::GetInstance()->ShowBiopsyMenu();
}
