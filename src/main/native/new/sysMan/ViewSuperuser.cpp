#include "sysMan/ViewSuperuser.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

#include "keyboard/KeyDef.h"
#include "keyboard/MultiFuncFactory.h"
#include "imageControl/Img2D.h"
#include "periDevice/MonitorControl.h"
#include "probe/ProbeMan.h"
#include "sysMan/ScreenSaver.h"
#include "sysMan/SysGeneralSetting.h"

#include "ViewMain.h"

string probeName[] = {
  "35C50L", "35C20I", "70L40J", "75L40J", "70L60J", "90L40J", "65C10L", "65C15E(APX)", "65C15E(JR)", "30P16B", "10L25K", "65C10I"
};

#define AUTHEN_NUM  11
#define DEMO_NUM    5

ViewSuperuser* ViewSuperuser::m_instance = NULL;

// ---------------------------------------------------------

ViewSuperuser* ViewSuperuser::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewSuperuser();
  }

  return m_instance;
}

ViewSuperuser::ViewSuperuser() {
  m_dialog = NULL;
  m_label_hint = NULL;
  m_treeview_probe = NULL;

  m_timer = 0;
  m_probeType = 0;

  m_statusAuthen = true;
  m_statusDemo = true;
}

ViewSuperuser::~ViewSuperuser() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

void ViewSuperuser::CreateWindow() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  m_dialog = Utils::create_dialog(NULL, _("超级用户工具"), 480, 360);

  GtkButton* button_exit = Utils::add_dialog_button(m_dialog, _("退出"), GTK_RESPONSE_CLOSE, GTK_STOCK_QUIT);
  g_signal_connect(button_exit, "clicked", G_CALLBACK(signal_button_clicked_exit), this);

  GtkTable* table = Utils::create_table(6, 2);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  GtkFrame* frame_probe = Utils::create_frame(_("探头工具"));
  GtkFrame* frame_aperutre = Utils::create_frame(_("暗道测试工具"));
  GtkButton* button_monitor = Utils::create_button(_("显示器测试"));
  GtkButton* button_monitor2 = Utils::create_button(_("显示器测试 2"));
  GtkLabel* m_label_hint = Utils::create_label("");

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_probe), 0, 2, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_aperutre), 0, 1, 3, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(button_monitor), 1, 2, 3, 4);
  gtk_table_attach_defaults(table, GTK_WIDGET(button_monitor2), 1, 2, 4, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_hint), 0, 2, 5, 6);

  g_signal_connect(G_OBJECT(button_monitor), "clicked", G_CALLBACK(signal_button_clicked_monitor), this);
  g_signal_connect(G_OBJECT(button_monitor2), "clicked", G_CALLBACK(signal_button_clicked_monitor2), this);

  // 探头工具
  GtkTable* table_probe = Utils::create_table(2, 4);
  gtk_container_set_border_width(GTK_CONTAINER(table_probe), 10);

  gtk_container_add(GTK_CONTAINER(frame_probe), GTK_WIDGET(table_probe));

  GtkComboBoxText* combobox_machine = Utils::create_combobox_text();
  GtkComboBoxText* combobox_probe = Utils::create_combobox_text();
  GtkButton* button_probe = Utils::create_button(_("写探头"));
  m_treeview_probe = CreateProbeTreeview();

  gtk_table_attach_defaults(table_probe, GTK_WIDGET(combobox_machine), 0, 2, 0, 1);
  gtk_table_attach_defaults(table_probe, GTK_WIDGET(combobox_probe), 0, 2, 1, 2);
  gtk_table_attach_defaults(table_probe, GTK_WIDGET(button_probe), 2, 3, 0, 2);
  gtk_table_attach_defaults(table_probe, GTK_WIDGET(m_treeview_probe), 3, 4, 0, 2);

  gtk_combo_box_text_append_text(combobox_machine, "G30Plus");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_machine), 0);
  g_signal_connect(G_OBJECT(combobox_machine), "changed", G_CALLBACK(signal_combobox_changed_machine), this);

  size_t size = sizeof(probeName) / sizeof(probeName[0]);

  for (int i = 0; i < size ; i++) {
    gtk_combo_box_text_append_text(combobox_probe, ProbeMan::GetInstance()->VerifyProbeName(probeName[i]).c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_probe), 0);
  g_signal_connect(G_OBJECT(combobox_probe), "changed", G_CALLBACK(signal_combobox_changed_probe), this);

  gtk_tree_view_set_grid_lines(m_treeview_probe, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);

  Utils::set_button_image(button_probe, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_BUTTON)));
  g_signal_connect(G_OBJECT(button_probe), "clicked", G_CALLBACK(signal_button_clicked_probe_write), this);

  // 暗道测试工具
  GtkTable* table_aperutre = Utils::create_table(1, 1);
  gtk_container_set_border_width(GTK_CONTAINER(table_aperutre), 10);

  gtk_container_add(GTK_CONTAINER(frame_aperutre), GTK_WIDGET(table_aperutre));

  GtkComboBoxText* combobox_aperutre = Utils::create_combobox_text();
  gtk_table_attach_defaults(table_aperutre, GTK_WIDGET(combobox_aperutre), 0, 1, 0, 1);

  gtk_combo_box_text_append_text(combobox_aperutre, "OFF");
  gtk_combo_box_text_append_text(combobox_aperutre, "ON");

  gtk_combo_box_set_active(GTK_COMBO_BOX(combobox_aperutre), 0);
  g_signal_connect(G_OBJECT(combobox_aperutre), "changed", G_CALLBACK(signal_combobox_changed_aperture), this);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  m_probeType = 0;

  m_statusAuthen = false;
  m_vecAuthenInfo.clear();

  g_keyInterface.Pop();
  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ViewSuperuser::CreateDemoWindow() {
  MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::NONE);

  m_dialog = Utils::create_dialog(NULL, _("进入演示"), 480, 360);

  GtkTable* table = Utils::create_table(3, 3);
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  GtkButton* button_demo = Utils::create_button(_("进入演示功能"));
  gtk_table_attach_defaults(table, GTK_WIDGET(button_demo), 1, 2, 1, 2);

  g_signal_connect(button_demo, "clicked", G_CALLBACK(signal_button_clicked_demo), this);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));
  g_signal_connect(G_OBJECT(m_dialog), "delete-event", G_CALLBACK(signal_window_delete_event), this);

  m_statusAuthen = false;
  m_vecAuthenInfo.clear();

  g_keyInterface.Pop();
  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

void ViewSuperuser::Authenticate() {
  PRINTF("--------------enter authenticate\n");

  if (m_timer > 0) {
    g_source_remove(m_timer);
    m_timer = 0;
    g_keyInterface.Pop();
  }

  g_keyInterface.Push(this);
  m_vecAuthenInfo.clear();
  m_statusAuthen = true;
  m_statusDemo = true;
  m_timer = g_timeout_add(1000, signal_callback_authen, NULL);
}

void ViewSuperuser::ExitDemo() {
  ViewMain::GetInstance()->MenuReviewCallBack();
  m_statusDemo = true;
}

bool ViewSuperuser::GetDemoStatus() {
  return !m_statusDemo;
}

// ---------------------------------------------------------

void ViewSuperuser::ComboBoxChangedMachine(GtkComboBox* combobox) {
}

void ViewSuperuser::ComboBoxChangedProbe(GtkComboBox* combobox) {
  m_probeType = gtk_combo_box_get_active(combobox);

  int i;
  char index[5];
  char name[20];
  for (i = 0; i < ProbeMan::MAX_SOCKET; i ++) {
    sprintf(index, "%d", i);
    sprintf(name, "%d: 无探头", i+1);
    UpdateProbeStatus(index, name);
  }
}

void ViewSuperuser::ComboBoxChangedAperture(GtkComboBox* combobox) {
  if (gtk_combo_box_get_active(combobox) == 0) {
    Img2D::GetInstance()->EnterSingleAperture(FALSE);
  } else {
    Img2D::GetInstance()->EnterSingleAperture(TRUE);
  }
}

void ViewSuperuser::ButtonClickedProbeWrite(GtkButton* button) {
  gtk_label_set_text(m_label_hint, "探头参数写入中...");
  g_timeout_add(5, signal_callback_probe_write, NULL);
}

void ViewSuperuser::ButtonClickedExit(GtkButton* button) {
  DestroyWindow();
}

void ViewSuperuser::ButtonClickedMonitor(GtkButton* button) {
  MonitorControl mc;
  mc.AutoAdjust();
  ScreenSaver::GetInstance()->EnterScreenSaver2();
}

void ViewSuperuser::ButtonClickedMonitor2(GtkButton* button) {
  MonitorControl mc;
  mc.AutoBalance();
  ScreenSaver::GetInstance()->EnterScreenSaver3();
}

void ViewSuperuser::ButtonClickedDemo(GtkButton* button) {
  m_statusAuthen = false;
  m_statusDemo = false;
  ViewMain::GetInstance()->MenuReviewCallBack();
  DestroyWindow();
}

bool ViewSuperuser::IsAuthenValid() {
  m_timer = 0;

  if (m_vecAuthenInfo.empty() || ((!m_vecAuthenInfo.empty()) && m_vecAuthenInfo[0] != 'e')) {
    PRINTF("not authen\n");
    Exit();
  } else {
    PRINTF("authen\n");
  }

  return false;
}

void ViewSuperuser::WriteProbe() {
  ProbeMan::GetInstance()->WriteProbe(m_probeType);

  sleep(1);
  ProbeSocket::ProbePara para[ProbeMan::MAX_SOCKET];
  ProbeMan::GetInstance()->GetAllProbe(para);

  char index[5];
  char name[20];

  for (int i = 0; i < ProbeMan::MAX_SOCKET; i ++) {
    sprintf(index, "%d", i);
    if (para[i].exist) {
      sprintf(name, "%d: %s", i+1, para[i].model);
    } else {
      sprintf(name, "%d: 无探头", i+1);
    }

    UpdateProbeStatus(index, name);
  }

  gtk_label_set_text(m_label_hint, "");
}

void ViewSuperuser::KeyEvent(unsigned char keyValue) {
  switch(keyValue) {
  case KEY_ESC:
    {
      g_timeout_add(100, signal_callback_exit_window, this);

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

void ViewSuperuser::DestroyWindow() {
  if(GTK_IS_WIDGET(m_dialog)) {
    Exit();

    gtk_widget_destroy(GTK_WIDGET(m_dialog));

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }

  m_dialog = NULL;
}

void ViewSuperuser::Exit() {
  g_keyInterface.Pop();
}

void ViewSuperuser::UpdateProbeStatus(string socket, string status) {
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_probe);

  GtkTreeIter iter;
  if(gtk_tree_model_get_iter_from_string(model, &iter, socket.c_str())) {
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, status.c_str(), -1);
  }
}

GtkTreeView* ViewSuperuser::CreateProbeTreeview() {
  GtkTreeView* treeview = Utils::create_tree_view();
  g_object_set(G_OBJECT(treeview),
    "enable-search", FALSE,
    "headers-visible", FALSE,
    "sensitive", FALSE,
    NULL);

  GtkCellRenderer* render = gtk_cell_renderer_text_new();

  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes("Porbe", render, "text", 0, NULL);
  gtk_tree_view_append_column(treeview, column);

  GtkTreeModel* model = CreateProbeModel();
  if(model != NULL) {
    gtk_tree_view_set_model(treeview, model);
  }

  g_object_unref(model);

  return treeview;
}

GtkTreeModel* ViewSuperuser::CreateProbeModel() {
  GtkTreeIter iter;

  GtkListStore* store = gtk_list_store_new(1, G_TYPE_STRING);
  for (unsigned int i = 0; i < ProbeMan::MAX_SOCKET; i++) {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set(store, &iter, 0, "No Probe", -1);
  }

  return GTK_TREE_MODEL(store);
}
