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
      data->StorageDeviceChanged(combobox);
    }
  }

  static void signal_button_clicked_storage_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageAddClicked(button);
    }
  }

  static void signal_button_clicked_storage_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageClearClicked(button);
    }
  }

  static void signal_button_clicked_storage_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageDeleteClicked(button);
    }
  }

  static void signal_button_clicked_storage_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageDefaultClicked(button);
    }
  }

  static void signal_button_clicked_storage_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageConnectClicked(button);
    }
  }

  static void signal_checkbutton_toggled_report(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ChkBtnSendReportToggled(togglebutton);
    }
  }

  static void signal_checkbutton_toggled_frame(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ChkBtnSendVideoToggled(togglebutton);
    }
  }

  static void signal_combobox_changed_storage_frames(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->BtnComboboxVideoFrames(combobox);
    }
  }

  // worklist

  static void signal_combobox_changed_worklist_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->WorklistDeviceChanged(combobox);
    }
  }

  static void signal_button_clicked_worklist_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonWorkListAddClicked(button);
    }
  }

  static void signal_button_clicked_worklist_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonWorkListClearClicked(button);
    }
  }

  static void signal_button_clicked_worklist_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL)
        data->ButtonWorkListDeleteClicked(button);
  }

  static void signal_button_clicked_worklist_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonWorkListDefaultClicked(button);
    }
  }

  static void signal_button_clicked_worklist_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonWorkListConnectClicked(button);
    }
  }

  static void signal_checkbutton_toggled_worklist_auto(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ChkBtnAutoQueryToggled(togglebutton);
    }
  }

  // mpps

  static void signal_combobox_changed_mpps_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->MPPSDeviceChanged(combobox);
    }
  }

  static void signal_button_clicked_mpps_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonMPPSAddClicked(button);
    }
  }

  static void signal_button_clicked_mpps_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonMPPSClearClicked(button);
    }
  }

  static void signal_button_clicked_mpps_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonMPPSDeleteClicked(button);
    }
  }

  static void signal_button_clicked_mpps_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonMPPSDefaultClicked(button);
    }
  }

  static void signal_button_clicked_mpps_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonMPPSConnectClicked(button);
    }
  }

  static void signal_checkbutton_toggled_mpps_send(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ChkBtnMPPSToggled(togglebutton);
    }
  }

  // storage commitment

  static void signal_combobox_changed_storage_commitment_device(GtkComboBox* combobox,DicomServiceSetting* data) {
    if (data != NULL) {
      data->StorageCommitmentDeviceChanged(combobox);
    }
  }

  static void signal_button_clicked_storage_commitment_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageCommitmentAddClicked(button);
    }
  }

  static void signal_button_clicked_storage_commitment_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageCommitmentClearClicked(button);
    }
  }

  static void signal_button_clicked_storage_commitment_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageCommitmentDeleteClicked(button);
    }
  }

  static void signal_button_clicked_storage_commitment_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageCommitmentDefaultClicked(button);
    }
  }

  static void signal_button_clicked_storage_commitment_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonStorageCommitmentConnectClicked(button);
    }
  }

  static void signal_checkbutton_toggled_storage_commitment_send(GtkToggleButton* togglebutton, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ChkBtnStorageCommitmentToggled(togglebutton);
    }
  }

  // query/retrieve

  static void signal_combobox_changed_query_retrieve_device(GtkComboBox* combobox, DicomServiceSetting* data) {
    if (data != NULL) {
      data->QueryRetrieveDeviceChanged(combobox);
      }
  }

  static void signal_button_clicked_query_retrieve_add(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonQueryRetrieveAddClicked(button);
    }
  }

  static void signal_button_clicked_query_retrieve_clear(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonQueryRetrieveClearClicked(button);
    }
  }

  static void signal_button_clicked_query_retrieve_delete(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonQueryRetrieveDeleteClicked(button);
    }
  }

  static void signal_button_clicked_query_retrieve_default(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonQueryRetrieveDefaultClicked(button);
    }
  }

  static void signal_button_clicked_query_retrieve_connect(GtkButton* button, DicomServiceSetting* data) {
    if (data != NULL) {
      data->ButtonQueryRetrieveConnectClicked(button);
    }
  }

  static void signal_callback_attribute(string device, string serviceName, string aeTitle, int port, bool isDefault, void* data) {
    DicomServiceSetting* setting = (DicomServiceSetting*)data;

    if (setting != NULL) {
      setting->GetSingleServiceAttribute(device, serviceName, aeTitle, port, isDefault);
    }
  }

  // signal

  void NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num);
  bool EntryKeyFilter(GtkWidget* entry, GdkEventKey* event);

  // storage

  void BtnComboboxVideoFrames(GtkComboBox* combobox);
  void ButtonStorageAddClicked(GtkButton* button);
  void ButtonStorageClearClicked(GtkButton* button);
  void ButtonStorageDeleteClicked(GtkButton* button);
  void ButtonStorageDefaultClicked(GtkButton* button);
  void ButtonStorageConnectClicked(GtkButton* button);

  void ChkBtnSendReportToggled(GtkToggleButton* togglebutton);
  void ChkBtnSendVideoToggled(GtkToggleButton* togglebutton);
  void StorageDeviceChanged(GtkComboBox* combobox);

  // worklist

  void WorklistDeviceChanged(GtkComboBox* combobox);
  void ButtonWorkListAddClicked(GtkButton* button);
  void ButtonWorkListClearClicked(GtkButton* button);
  void ButtonWorkListDeleteClicked(GtkButton* button);
  void ButtonWorkListDefaultClicked(GtkButton* button);
  void ButtonWorkListConnectClicked(GtkButton* button);

  void ChkBtnAutoQueryToggled(GtkToggleButton* togglebutton);

  // mpps

  void MPPSDeviceChanged(GtkComboBox* combobox);
  void ButtonMPPSAddClicked(GtkButton* button);
  void ButtonMPPSClearClicked(GtkButton* button);
  void ButtonMPPSDeleteClicked(GtkButton* button);
  void ButtonMPPSDefaultClicked(GtkButton* button);
  void ButtonMPPSConnectClicked(GtkButton* button);

  void ChkBtnMPPSToggled(GtkToggleButton* togglebutton);

  // storage commitment

  void StorageCommitmentDeviceChanged(GtkComboBox* combobox);
  void ButtonStorageCommitmentAddClicked(GtkButton* button);
  void ButtonStorageCommitmentClearClicked(GtkButton* button);
  void ButtonStorageCommitmentDeleteClicked(GtkButton* button);
  void ButtonStorageCommitmentDefaultClicked(GtkButton* button);
  void ButtonStorageCommitmentConnectClicked(GtkButton* button);

  void ChkBtnStorageCommitmentToggled(GtkToggleButton* togglebutton);

  // query/retrieve

  void QueryRetrieveDeviceChanged(GtkComboBox* combobox);
  void ButtonQueryRetrieveAddClicked(GtkButton* button);
  void ButtonQueryRetrieveClearClicked(GtkButton* button);
  void ButtonQueryRetrieveDeleteClicked(GtkButton* button);
  void ButtonQueryRetrieveDefaultClicked(GtkButton* button);
  void ButtonQueryRetrieveConnectClicked(GtkButton* button);

  void GetSingleServiceAttribute(string device, string serviceName, string aeTitle, int port, bool isDefault);

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

private:
  bool show_storage;
  bool show_worklist;
  string m_selectedDevice;

  static gboolean StorageConnectTimeout(gpointer data);
  static gboolean WorklistConnectTimeout(gpointer data);
  static gboolean MPPSConnectTimeout(gpointer data);
  static gboolean StorageCommitmentConnectTimeout(gpointer data);
  static gboolean QueryRetrieveConnectTimeout(gpointer data);
};

#endif
