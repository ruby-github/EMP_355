#ifndef __DICOM_SERVER_SETTING_H__
#define __DICOM_SERVER_SETTING_H__

#include "utils/Utils.h"

class DicomServerSetting {
public:
  static DicomServerSetting* GetInstance();

public:
  ~DicomServerSetting();

  GtkWidget* CreateDicomWindow(GtkWidget* parent);

  void InitServerSetting();

private:
  // signal

  static void signal_button_clicked_ping(GtkButton* button, DicomServerSetting* data) {
    if (data != NULL) {
      data->ButtonClickedPing(button);
    }
  }

  static void signal_button_clicked_add(GtkButton* button, DicomServerSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAdd(button);
    }
  }

  static void signal_button_clicked_delete(GtkButton* button, DicomServerSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDelete(button);
    }
  }

  static gboolean signal_callback_pingtimeout(gpointer data) {
    DicomServerSetting* setting = (DicomServerSetting*)data;

    if (setting != NULL) {
      setting->PingTimeout();
    }

    return FALSE;
  }

  static void signal_callback_attribute(string device, string ip, void* data) {
    DicomServerSetting* setting = (DicomServerSetting*)data;

    if (setting != NULL) {
      setting->GetSingleServerAttribute(device, ip);
    }
  }

  // signal

  void ButtonClickedPing(GtkButton* button);
  void ButtonClickedAdd(GtkButton* button);
  void ButtonClickedDelete(GtkButton* button);

  void PingTimeout();
  void GetSingleServerAttribute(string device, string ip);

private:
  DicomServerSetting();

  GtkTreeView* CreateServerTreeview();
  GtkTreeModel* CreateDeviceModel();

private:
  static DicomServerSetting* m_instance;

private:
  GtkWidget* m_parent;

  GtkEntry* m_entry_device;
  GtkEntry* m_entry_ip;
  GtkTreeView* m_treeview;

  vector<string> m_group_device_server;
  vector<string> m_group_ip_server;
};

#endif
