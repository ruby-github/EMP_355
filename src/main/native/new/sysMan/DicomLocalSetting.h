#ifndef __DICOM_LOCAL_SETTING_H__
#define __DICOM_LOCAL_SETTING_H__

#include "utils/Utils.h"

class DicomLocalSetting {
public:
  static DicomLocalSetting* GetInstance();

public:
  ~DicomLocalSetting();

  GtkWidget* CreateDicomWindow(GtkWidget* parent);

  void InitLocalSetting();

private:
  // signal

  static void signal_button_clicked_setting(GtkButton* button, DicomLocalSetting* data) {
    if (data != NULL) {
      data->ButtonClickedSetting(button);
    }
  }

  static gboolean signal_entry_keyfilter(GtkWidget* entry, GdkEventKey* event, gpointer data) {
    DicomLocalSetting* setting = (DicomLocalSetting*)data;

    if (setting != NULL) {
      if (setting->EntryKeyFilter(entry, event)) {
        return TRUE;
      } else {
        return FALSE;
      }
    }

    return FALSE;
  }

  // signal

  void ButtonClickedSetting(GtkButton* button);
  bool EntryKeyFilter(GtkWidget* entry, GdkEventKey* event);

private:
  DicomLocalSetting();

private:
  static DicomLocalSetting* m_instance;

private:
  GtkWidget* m_parent;

  GtkEntry* m_entry_network_ip;
  GtkEntry* m_entry_network_mask;
  GtkEntry* m_entry_network_gateway;

  GtkEntry* m_entry_host_port;
  GtkEntry* m_entry_host_ae;
};

#endif
