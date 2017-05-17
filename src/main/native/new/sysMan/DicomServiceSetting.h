#ifndef __DICOM_SERVICE_SETTING_H__
#define __DICOM_SERVICE_SETTING_H__

#include "utils/Utils.h"

class DicomServiceSetting {
public:
  static DicomServiceSetting* GetInstance();

public:
  ~DicomServiceSetting();

  GtkWidget* CreateDicomWindow(GtkWidget* parent);

  void InitServiceSetting();

private:
  // signal

  static void signal_notebook_switch_page(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num, DicomServiceSetting* data) {
    if (data != NULL) {
      data->NotebookChanged(notebook, page, page_num);
    }
  }

  static gboolean signal_entry_keyfilter(GtkWidget* entry, GdkEventKey* event, gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      if (setting->EntryKeyFilter(entry, event)) {
        return TRUE;
      } else {
        return FALSE;
      }
    }

    return TRUE;
  }

  // storage
  static void signal_combobox_changed_storage_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDeviceStorage(combobox);
    }
  }

  static void signal_button_clicked_storage_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAddStorage(button);
    }
  }

  static void signal_button_clicked_storage_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedClearStorage(button);
    }
  }

  static void signal_button_clicked_storage_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDeleteStorage(button);
    }
  }

  static void signal_button_clicked_storage_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefaultStorage(button);
    }
  }

  static void signal_button_clicked_storage_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedConnectStorage(button);
    }
  }

  static void signal_checkbutton_toggled_report(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->CheckButtonToggledReportStorage(togglebutton);
    }
  }

  static void signal_checkbutton_toggled_frame(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->CheckButtonToggledFrameStorage(togglebutton);
    }
  }

  static void signal_combobox_changed_storage_frames(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedFramesStorage(combobox);
    }
  }

  // worklist

  static void signal_combobox_changed_worklist_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDeviceWorklist(combobox);
    }
  }

  static void signal_button_clicked_worklist_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAddWorklist(button);
    }
  }

  static void signal_button_clicked_worklist_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedClearWorklist(button);
    }
  }

  static void signal_button_clicked_worklist_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL)
        data->ButtonClickedDeleteWorklist(button);
  }

  static void signal_button_clicked_worklist_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefaultWorklist(button);
    }
  }

  static void signal_button_clicked_worklist_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedConnectWorklist(button);
    }
  }

  static void signal_checkbutton_toggled_worklist_auto(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->CheckButtonToggledAutoWorklist(togglebutton);
    }
  }

  // mpps

  static void signal_combobox_changed_mpps_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDeviceMpps(combobox);
    }
  }

  static void signal_button_clicked_mpps_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAddMpps(button);
    }
  }

  static void signal_button_clicked_mpps_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedClearMpps(button);
    }
  }

  static void signal_button_clicked_mpps_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDeleteMpps(button);
    }
  }

  static void signal_button_clicked_mpps_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefaultMpps(button);
    }
  }

  static void signal_button_clicked_mpps_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedConnectMpps(button);
    }
  }

  static void signal_checkbutton_toggled_mpps_send(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->CheckButtonToggledSendMpps(togglebutton);
    }
  }

  // storage commitment

  static void signal_combobox_changed_storage_commitment_device(GtkComboBox* combobox,DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDeviceStorageCommitment(combobox);
    }
  }

  static void signal_button_clicked_storage_commitment_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAddStorageCommitment(button);
    }
  }

  static void signal_button_clicked_storage_commitment_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedClearStorageCommitment(button);
    }
  }

  static void signal_button_clicked_storage_commitment_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDeleteStorageCommitment(button);
    }
  }

  static void signal_button_clicked_storage_commitment_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefaultStorageCommitment(button);
    }
  }

  static void signal_button_clicked_storage_commitment_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedConnectStorageCommitment(button);
    }
  }

  static void signal_checkbutton_toggled_storage_commitment_send(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->CheckButtonToggledSendStorageCommitment(togglebutton);
    }
  }

  // query/retrieve

  static void signal_combobox_changed_query_retrieve_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ComboBoxChangedDeviceQueryRetrieve(combobox);
      }
  }

  static void signal_button_clicked_query_retrieve_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedAddQueryRetrieve(button);
    }
  }

  static void signal_button_clicked_query_retrieve_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedClearQueryRetrieve(button);
    }
  }

  static void signal_button_clicked_query_retrieve_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDeleteQueryRetrieve(button);
    }
  }

  static void signal_button_clicked_query_retrieve_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedDefaultQueryRetrieve(button);
    }
  }

  static void signal_button_clicked_query_retrieve_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonClickedConnectQueryRetrieve(button);
    }
  }

  // timeout

  static gboolean signal_callback_timeout_connect_storage(gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->TimeoutConnectStorage();
    }

    return FALSE;
  }

  static gboolean signal_callback_timeout_connect_worklist(gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->TimeoutConnectWorklist();
    }

    return FALSE;
  }

  static gboolean signal_callback_timeout_connect_mpps(gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->TimeoutConnectMpps();
    }

    return FALSE;
  }

  static gboolean signal_callback_timeout_connect_storage_commitment(gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->TimeoutConnectStorageCommitment();
    }

    return FALSE;
  }

  static gboolean signal_callback_timeout_connect_query_retrieve(gpointer data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->TimeoutConnectQueryRetrieve();
    }

    return FALSE;
  }

  static void signal_callback_get_service_attribute(string device, string serviceName, string aeTitle, int port, bool isDefault, void* data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->GetServiceAttribute(device, serviceName, aeTitle, port, isDefault);
    }
  }

  // signal

  void NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num);
  bool EntryKeyFilter(GtkWidget* entry, GdkEventKey* event);

  // storage

  void ComboBoxChangedDeviceStorage(GtkComboBox* combobox);
  void ButtonClickedAddStorage(GtkButton* button);
  void ButtonClickedClearStorage(GtkButton* button);
  void ButtonClickedDeleteStorage(GtkButton* button);
  void ButtonClickedDefaultStorage(GtkButton* button);
  void ButtonClickedConnectStorage(GtkButton* button);

  void CheckButtonToggledReportStorage(GtkToggleButton* togglebutton);
  void CheckButtonToggledFrameStorage(GtkToggleButton* togglebutton);
  void ComboBoxChangedFramesStorage(GtkComboBox* combobox);

  // worklist

  void ComboBoxChangedDeviceWorklist(GtkComboBox* combobox);
  void ButtonClickedAddWorklist(GtkButton* button);
  void ButtonClickedClearWorklist(GtkButton* button);
  void ButtonClickedDeleteWorklist(GtkButton* button);
  void ButtonClickedDefaultWorklist(GtkButton* button);
  void ButtonClickedConnectWorklist(GtkButton* button);

  void CheckButtonToggledAutoWorklist(GtkToggleButton* togglebutton);

  // mpps

  void ComboBoxChangedDeviceMpps(GtkComboBox* combobox);
  void ButtonClickedAddMpps(GtkButton* button);
  void ButtonClickedClearMpps(GtkButton* button);
  void ButtonClickedDeleteMpps(GtkButton* button);
  void ButtonClickedDefaultMpps(GtkButton* button);
  void ButtonClickedConnectMpps(GtkButton* button);

  void CheckButtonToggledSendMpps(GtkToggleButton* togglebutton);

  // storage commitment

  void ComboBoxChangedDeviceStorageCommitment(GtkComboBox* combobox);
  void ButtonClickedAddStorageCommitment(GtkButton* button);
  void ButtonClickedClearStorageCommitment(GtkButton* button);
  void ButtonClickedDeleteStorageCommitment(GtkButton* button);
  void ButtonClickedDefaultStorageCommitment(GtkButton* button);
  void ButtonClickedConnectStorageCommitment(GtkButton* button);

  void CheckButtonToggledSendStorageCommitment(GtkToggleButton* togglebutton);

  // query/retrieve

  void ComboBoxChangedDeviceQueryRetrieve(GtkComboBox* combobox);
  void ButtonClickedAddQueryRetrieve(GtkButton* button);
  void ButtonClickedClearQueryRetrieve(GtkButton* button);
  void ButtonClickedDeleteQueryRetrieve(GtkButton* button);
  void ButtonClickedDefaultQueryRetrieve(GtkButton* button);
  void ButtonClickedConnectQueryRetrieve(GtkButton* button);

  // timeout

  void TimeoutConnectStorage();
  void TimeoutConnectWorklist();
  void TimeoutConnectMpps();
  void TimeoutConnectStorageCommitment();
  void TimeoutConnectQueryRetrieve();

  void GetServiceAttribute(string device, string serviceName, string aeTitle, int port, bool isDefault);

private:
  DicomServiceSetting();

  GtkWidget* CreateNoteStorage();
  GtkWidget* CreateNoteWorklist();
  GtkWidget* CreateNoteMpps();
  GtkWidget* CreateNoteStorageCommitment();
  GtkWidget* CreateNoteQueryRetrieve();

  void InitStorageSetting();
  void InitWorklistSetting();
  void InitMppsSetting();
  void InitStorageCommitmentSetting();
  void InitQueryRetrieveSetting();

  GtkTreeView* CreateServiceTreeview();
  GtkTreeModel* CreateServiceModel();

private:
  static DicomServiceSetting* m_instance;

private:
  GtkWidget* m_parent;
  GtkNotebook* m_notebook;

  // storage
  GtkComboBoxText* m_combobox_storage_device;
  GtkEntry* m_entry_storage_service;
  GtkEntry* m_entry_storage_ae;
  GtkEntry* m_entry_storage_port;
  GtkTreeView* m_treeview_storage;

  GtkCheckButton* m_checkbutton_storage_report;
  GtkCheckButton* m_checkbutton_storage_frame;
  GtkComboBoxText* m_combobox_storage_frames;

  // worklist
  GtkComboBoxText* m_combobox_worklist_device;
  GtkEntry* m_entry_worklist_service;
  GtkEntry* m_entry_worklist_ae;
  GtkEntry* m_entry_worklist_port;
  GtkTreeView* m_treeview_worklist;

  GtkCheckButton* m_checkbutton_worklist_auto;

  // mpps
  GtkComboBoxText* m_combobox_mpps_device;
  GtkEntry* m_entry_mpps_service;
  GtkEntry* m_entry_mpps_ae;
  GtkEntry* m_entry_mpps_port;
  GtkTreeView* m_treeview_mpps;

  GtkCheckButton* m_checkbutton_mpps_send;

  // storage commitment
  GtkComboBoxText* m_combobox_storage_commitment_device;
  GtkEntry* m_entry_storage_commitment_service;
  GtkEntry* m_entry_storage_commitment_ae;
  GtkEntry* m_entry_storage_commitment_port;
  GtkTreeView* m_treeview_storage_commitment;

  GtkCheckButton* m_checkbutton_storage_commitment_send;

  // query/retrieve
  GtkComboBoxText* m_combobox_query_retrieve_device;
  GtkEntry* m_entry_query_retrieve_service;
  GtkEntry* m_entry_query_retrieve_ae;
  GtkEntry* m_entry_query_retrieve_port;
  GtkTreeView* m_treeview_query_retrieve;

  string m_selected_device;
};

#endif
