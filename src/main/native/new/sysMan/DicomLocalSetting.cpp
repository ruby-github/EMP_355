#include "sysMan/DicomLocalSetting.h"

#include <stdlib.h>
#include <iostream>
#include <gdk/gdkkeysyms.h>
#include "utils/MessageDialog.h"

#include "keyboard/KeyDef.h"
#include "thirdparty/MyDCMMan.h"
#include "periDevice/NetworkMan.h"
#include "sysMan/SysDicomSetting.h"

using namespace std;

DicomLocalSetting* DicomLocalSetting::m_instance = NULL;

// ---------------------------------------------------------

DicomLocalSetting* DicomLocalSetting::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new DicomLocalSetting();
  }

  return m_instance;
}

DicomLocalSetting::DicomLocalSetting() {
  m_parent = NULL;

  m_entry_network_ip = NULL;
  m_entry_network_mask = NULL;
  m_entry_network_gateway = NULL;

  m_entry_host_port = NULL;
  m_entry_host_ae = NULL;
}

DicomLocalSetting::~DicomLocalSetting() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* DicomLocalSetting::CreateDicomWindow(GtkWidget* parent) {
  m_parent = parent;

  GtkTable* table = Utils::create_table(8, 6);

  GtkFrame* frame_network = Utils::create_frame(_("NetWork Property"));
  GtkFrame* frame_host = Utils::create_frame(_("Host"));
  GtkButton* button_setting = Utils::create_button(_("Setting"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_network), 0, 3, 0, 8);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_host), 3, 6, 0, 4);
  gtk_table_attach(table, GTK_WIDGET(button_setting), 3, 4, 7, 8, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  g_signal_connect(button_setting, "clicked", G_CALLBACK(signal_button_clicked_setting), this);

  // NetWork Property
  GtkTable* table_network = Utils::create_table(6, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_network), 10);

  gtk_container_add(GTK_CONTAINER(frame_network), GTK_WIDGET(table_network));

  // ip
  GtkLabel* label_network_ip = Utils::create_label(_("IP Address:"));
  m_entry_network_ip = Utils::create_entry(9679);
  GtkLabel* label_network_ip_style = Utils::create_label(_("(xxx.xxx.xxx.xxx)"));

  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_ip), 0, 1, 0, 1);
  gtk_table_attach(table_network, GTK_WIDGET(m_entry_network_ip), 1, 3, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_ip_style), 1, 3, 1, 2);

  gtk_entry_set_max_length(m_entry_network_ip, 15);
  gtk_misc_set_alignment(GTK_MISC(label_network_ip_style), 0.5, 0);

  // netmask
  GtkLabel* label_network_mask = Utils::create_label(_("Netmask:"));
  m_entry_network_mask = Utils::create_entry(9679);
  GtkLabel* label_network_mask_style = Utils::create_label(_("(xxx.xxx.xxx.xxx)"));

  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_mask), 0, 1, 2, 3);
  gtk_table_attach(table_network, GTK_WIDGET(m_entry_network_mask), 1, 3, 2, 3, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_mask_style), 1, 3, 3, 4);

  gtk_entry_set_max_length(m_entry_network_mask, 15);
  gtk_misc_set_alignment(GTK_MISC(label_network_mask_style), 0.5, 0);

  // gateway
  GtkLabel* label_network_gateway = Utils::create_label(_("Gateway:"));
  m_entry_network_gateway = Utils::create_entry(9679);
  GtkLabel* label_network_gateway_style = Utils::create_label(_("(xxx.xxx.xxx.xxx)"));

  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_gateway), 0, 1, 4, 5);
  gtk_table_attach(table_network, GTK_WIDGET(m_entry_network_gateway), 1, 3, 4, 5, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table_network, GTK_WIDGET(label_network_gateway_style), 1, 3, 5, 6);

  gtk_entry_set_max_length(m_entry_network_gateway, 15);
  gtk_misc_set_alignment(GTK_MISC(label_network_gateway_style), 0.5, 0);

  // Host
  GtkTable* table_host = Utils::create_table(3, 3);
  gtk_container_set_border_width(GTK_CONTAINER(table_host), 10);

  gtk_container_add(GTK_CONTAINER(frame_host), GTK_WIDGET(table_host));

  // AE Title
  GtkLabel* label_host_ae = Utils::create_label(_("AE Title:"));
  m_entry_host_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_host, GTK_WIDGET(label_host_ae), 0, 1, 0, 1);
  gtk_table_attach(table_host, GTK_WIDGET(m_entry_host_ae), 1, 3, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_host_ae, 15);

  // Port
  GtkLabel* label_host_port = Utils::create_label(_("Port:"));
  m_entry_host_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_host, GTK_WIDGET(label_host_port), 0, 1, 1, 2);
  gtk_table_attach(table_host, GTK_WIDGET(m_entry_host_port), 1, 3, 1, 2, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_host_port, 15);
  g_signal_connect(m_entry_host_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  return GTK_WIDGET(table);
}

void DicomLocalSetting::InitLocalSetting() {
  if (m_entry_network_ip != NULL) {
    SysDicomSetting* sysDicomSetting = new SysDicomSetting();

    string local_ip = sysDicomSetting->GetLocalIP();
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_ip), local_ip.c_str());

    string local_netmask = sysDicomSetting->GetLocalNetMask();
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_mask), local_netmask.c_str());

    string local_gateway = sysDicomSetting->GetLocalGateWay();
    gtk_entry_set_text(GTK_ENTRY(m_entry_network_gateway), local_gateway.c_str());

    string host_ae = MyDCMMan::GetLocalAE();
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_ae), host_ae.c_str());

    int host_port = MyDCMMan::GetLocalPort();
    char tmp[256] = {0};
    sprintf(tmp, "%d", host_port);
    gtk_entry_set_text(GTK_ENTRY(m_entry_host_port), tmp);

    delete sysDicomSetting;
  }
}

// ---------------------------------------------------------

void DicomLocalSetting::ButtonClickedSetting(GtkButton* button) {
  string local_ip = gtk_entry_get_text(m_entry_network_ip);
  string local_mask = gtk_entry_get_text(m_entry_network_mask);
  string local_gateway = gtk_entry_get_text(m_entry_network_gateway);
  string host_port = gtk_entry_get_text(m_entry_host_port);
  string host_ae = gtk_entry_get_text(m_entry_host_ae);

  cout << "Setting: "
    << local_ip << " "
    << local_mask << " "
    << local_gateway << " "
    << host_port << " "
    << host_ae << endl;

  if (local_ip.empty() || local_mask.empty() || local_gateway.empty() || host_port.empty() || host_ae.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Local information must be not empty!"), NULL);

    return ;
  }

  string info;

  CNetworkMan localIP;
  if(localIP.SetLocalIP(local_ip, local_gateway, local_mask)) {
    MyDCMMan::SetLocalAE(host_ae);
    MyDCMMan::SetLocalPort(atoi(host_port.c_str()));
    info = _("Sucess to set the local network!");

    SysDicomSetting sysDicomSetting;
    sysDicomSetting.SetLocalIP(local_ip);
    sysDicomSetting.SetLocalNetMask(local_mask);
    sysDicomSetting.SetLocalGateWay(local_gateway);

    sysDicomSetting.SyncFile();
  } else {
    info = _("Fail to set the local network!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

bool DicomLocalSetting::EntryKeyFilter(GtkWidget* entry, GdkEventKey* event) {
  switch(event->keyval) {
  case GDK_0:
  case GDK_1:
  case GDK_2:
  case GDK_3:
  case GDK_4:
  case GDK_5:
  case GDK_6:
  case GDK_7:
  case GDK_8:
  case GDK_9:
  case GDK_KP_0:
  case GDK_KP_1:
  case GDK_KP_2:
  case GDK_KP_3:
  case GDK_KP_4:
  case GDK_KP_5:
  case GDK_KP_6:
  case GDK_KP_7:
  case GDK_KP_8:
  case GDK_KP_9:
  case GDK_BackSpace:
  case GDK_Left:
  case GDK_Up:
  case GDK_Right:
  case GDK_Down:
  case GDK_Delete:
  case GDK_Home:
  case GDK_End:
  case GDK_Tab:
    return false;
  default:
    return true;
  }
}
