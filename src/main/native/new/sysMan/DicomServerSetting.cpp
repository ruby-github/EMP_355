#include "sysMan/DicomServerSetting.h"

#include "utils/MessageDialog.h"

#include "Def.h"
#include "periDevice/DCMMan.h"

enum {
  COL_DEVICE,
  COL_IP,
  NUM_COLS
};

DicomServerSetting* DicomServerSetting::m_instance = NULL;

// ---------------------------------------------------------

DicomServerSetting* DicomServerSetting::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new DicomServerSetting();
  }

  return m_instance;
}

DicomServerSetting::DicomServerSetting() {
  m_parent = NULL;

  m_entry_device = NULL;
  m_entry_ip = NULL;
  m_treeview = NULL;
}

DicomServerSetting::~DicomServerSetting() {
}

GtkWidget* DicomServerSetting::CreateDicomWindow(GtkWidget* parent) {
  m_parent = parent;

  GtkTable* table = GTK_TABLE(gtk_table_new(11, 1, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table), 20);

  gtk_table_set_row_spacings(table, 10);
  gtk_table_set_col_spacings(table, 10);

  GtkFrame* frame_device = Utils::create_frame(_("Device"));
  GtkFrame* frame_device_list = Utils::create_frame(_("Device List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 1, 0, 5);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device_list), 0, 1, 5, 11);

  // Device
  GtkTable* table_device = GTK_TABLE(gtk_table_new(3, 6, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 10);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  gtk_table_set_row_spacings(table_device, 10);
  gtk_table_set_col_spacings(table_device, 10);

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_entry_device = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_device, 15);

  // IP Address
  GtkLabel* label_ip = Utils::create_label(_("IP Address:"));
  m_entry_ip = Utils::create_entry(9679);
  GtkLabel* label_ip_style = Utils::create_label(_("(xxx.xxx.xxx.xxx)"));

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ip), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_ip), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ip_style), 3, 5, 1, 2);

  gtk_entry_set_max_length(m_entry_ip, 15);

  // button
  GtkButton* button_ping = Utils::create_button(_("Ping"));
  GtkButton* button_add = Utils::create_button(_("Add"));

  g_signal_connect(button_ping, "clicked", G_CALLBACK(signal_button_clicked_ping), this);
  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_add), this);

  gtk_table_attach(table_device, GTK_WIDGET(button_ping), 0, 1, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_add), 1, 2, 2, 3, GTK_FILL, GTK_SHRINK, 0, 0);

  // Device List
  GtkTable* table_device_list = GTK_TABLE(gtk_table_new(4, 6, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table_device_list), 10);

  gtk_container_add(GTK_CONTAINER(frame_device_list), GTK_WIDGET(table_device_list));

  gtk_table_set_row_spacings(table_device_list, 10);
  gtk_table_set_col_spacings(table_device_list, 10);

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  gtk_table_attach_defaults(table_device_list, GTK_WIDGET(scrolled_window), 0, 6, 0, 3);

  m_treeview = CreateServerTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview));

  // button delete
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  gtk_table_attach(table_device_list, GTK_WIDGET(button_delete), 0, 1, 3, 4, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_delete), this);

  return GTK_WIDGET(table);
}

void DicomServerSetting::InitServerSetting() {
  m_group_device_server.clear();

  GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
  gtk_list_store_clear(GTK_LIST_STORE(model));
  CDCMMan::GetMe()->GetAllServer(signal_callback_attribute, this);
}

// ---------------------------------------------------------

void DicomServerSetting::ButtonClickedPing(GtkButton* button) {
  const char * ip = gtk_entry_get_text(GTK_ENTRY(m_entry_ip));
  const char * device = gtk_entry_get_text(GTK_ENTRY(m_entry_device));

  if(ip[0] == '\0' || device[0] == '\0') {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device or IP is empty, please input!"), NULL);

    return;
  }

  MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Ping..."));

  g_timeout_add(2000, signal_callback_pingtimeout, this);
}

void DicomServerSetting::ButtonClickedAdd(GtkButton* button) {
  const char* device = gtk_entry_get_text(GTK_ENTRY(m_entry_device));
  const char* ip = gtk_entry_get_text(GTK_ENTRY(m_entry_ip));

  if(strcmp(device, "localhost") ==0) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("'localhost' has been used, please input again!"), NULL);

    return;

  }

  if(ip[0] == '\0' || device[0] == '\0') {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device or IP is empty, please input!"), NULL);

    return;
  }

  char device_tmp[256];
  char ip_tmp[256];

  for(int i=0; i<m_group_device_server.size(); i++) {
    sprintf(device_tmp,"%s",m_group_device_server[i].c_str());
    sprintf(ip_tmp,"%s",m_group_ip_server[i].c_str());

    if((strcmp(device,device_tmp)==0)||(strcmp(ip,ip_tmp)==0)) {
      PRINTF("------add failed\n");
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed,device or IP has been existed\n"), NULL);

      return;
    }
  }

  if(!CDCMMan::GetMe()->AddServer(device,ip)) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed,device or IP has been existed\n"), NULL);

    return;
  }

  GtkTreeModel* model;
  GtkTreeIter iter;

  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));
  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEVICE, device, COL_IP,ip, -1);

  m_group_device_server.push_back(device);
  m_group_ip_server.push_back(ip);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServerSetting::ButtonClickedDelete(GtkButton* button) {
  GtkTreeModel *model;
  GtkTreeIter iter;
  GtkTreeSelection *selection;
  char *device;
  char *ip;

  vector<string>::iterator Iter;

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_treeview));
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(m_treeview));

  if(gtk_tree_selection_get_selected(selection,NULL,&iter)) {
    gtk_tree_model_get(model, &iter, COL_DEVICE,&device, COL_IP, &ip, -1);

    for(Iter = m_group_device_server.begin(); Iter < m_group_device_server.end(); Iter++ ) {
      if (strcmp(device,(*Iter).c_str())==0) {
        m_group_device_server.erase(Iter);
      }
    }

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

    printf("-----------Delete:%d %s\n",CDCMMan::GetMe()->DeleteServer(device),device);
  }
}

void DicomServerSetting::PingTimeout() {
  const char * ip = gtk_entry_get_text(GTK_ENTRY(m_entry_ip));
  char strHint[256] = {0};

  if (CDCMMan::GetMe()->Ping((char *)ip)) {
    sprintf(strHint, "%s %s\n", _("Success to ping ip"), ip);
  } else {
    sprintf(strHint, "%s %s\n", _("Failed to ping ip"), ip);
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, strHint, NULL);
}

void DicomServerSetting::GetSingleServerAttribute(string device, string ip) {
  m_group_device_server.push_back(device);
  m_group_ip_server.push_back(ip);

  GtkTreeIter iter;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview);
  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_DEVICE, device.c_str(),
    COL_IP,ip.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

GtkTreeView* DicomServerSetting::CreateServerTreeview() {
  GtkTreeView* treeview = Utils::create_tree_view();

  gtk_tree_view_set_enable_search(treeview, FALSE);
  gtk_tree_view_set_rules_hint(treeview, TRUE);

  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
    _("Device"), renderer, "text", COL_DEVICE, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column),
    "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 180, NULL);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(
    _("IP Address"), renderer, "text", COL_IP, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column),
    "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

  GtkTreeModel* model = CreateDeviceModel();

  if (model != NULL) {
      gtk_tree_view_set_model(treeview, model);
  }

  g_object_unref (model);

  return treeview;
}

GtkTreeModel* DicomServerSetting::CreateDeviceModel() {
  GtkListStore* store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING);

  return GTK_TREE_MODEL(store);
}
