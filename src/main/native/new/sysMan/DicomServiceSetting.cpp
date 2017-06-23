#include "sysMan/DicomServiceSetting.h"

#include <gdk/gdkkeysyms.h>
#include "utils/MessageDialog.h"

#include "thirdparty/MyDCMMan.h"
#include "probe/ExamItem.h"
#include "sysMan/SysDicomSetting.h"
#include "sysMan/ViewSystem.h"

enum {
  COL_STORAGE_DEVICE,
  COL_SERVICE_NAME,
  COL_AE_TITLE,
  COL_PORT,
  COL_DEFAULT,
  NUM_COLS
};

string frames[]= {
  "10","20","50","100","200","300"
};

DicomServiceSetting* DicomServiceSetting::m_instance = NULL;

// ---------------------------------------------------------

DicomServiceSetting* DicomServiceSetting::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new DicomServiceSetting();
  }

  return m_instance;
}

DicomServiceSetting::DicomServiceSetting() {
  m_parent = NULL;
  m_notebook = NULL;

  // storage
  m_combobox_storage_device = NULL;
  m_entry_storage_service = NULL;
  m_entry_storage_ae = NULL;
  m_entry_storage_port = NULL;
  m_treeview_storage = NULL;

  m_checkbutton_storage_report = NULL;
  m_checkbutton_storage_frame = NULL;
  m_combobox_storage_frames = NULL;

  // worklist
  m_combobox_worklist_device = NULL;
  m_entry_worklist_service = NULL;
  m_entry_worklist_ae = NULL;
  m_entry_worklist_port = NULL;
  m_treeview_worklist = NULL;

  m_checkbutton_worklist_auto = NULL;

  // mpps
  m_combobox_mpps_device = NULL;
  m_entry_mpps_service = NULL;
  m_entry_mpps_ae = NULL;
  m_entry_mpps_port = NULL;
  m_treeview_mpps = NULL;

  m_checkbutton_mpps_send = NULL;

  // storage commitment
  m_combobox_storage_commitment_device = NULL;
  m_entry_storage_commitment_service = NULL;
  m_entry_storage_commitment_ae = NULL;
  m_entry_storage_commitment_port = NULL;
  m_treeview_storage_commitment = NULL;

  m_checkbutton_storage_commitment_send = NULL;

  // query/retrieve
  m_combobox_query_retrieve_device = NULL;
  m_entry_query_retrieve_service = NULL;
  m_entry_query_retrieve_ae = NULL;
  m_entry_query_retrieve_port = NULL;
  m_treeview_query_retrieve = NULL;
}

DicomServiceSetting::~DicomServiceSetting() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* DicomServiceSetting::CreateDicomWindow(GtkWidget* parent) {
  m_parent = parent;
  m_notebook = Utils::create_notebook();

  gtk_notebook_append_page(m_notebook, CreateNoteStorage(), GTK_WIDGET(Utils::create_label(_("Storage"))));
  gtk_notebook_append_page(m_notebook, CreateNoteWorklist(), GTK_WIDGET(Utils::create_label(_("Worklist"))));
  gtk_notebook_append_page(m_notebook, CreateNoteMpps(), GTK_WIDGET(Utils::create_label(_("MPPS"))));
  gtk_notebook_append_page(m_notebook, CreateNoteStorageCommitment(), GTK_WIDGET(Utils::create_label(_("Storage Commitment"))));
  gtk_notebook_append_page(m_notebook, CreateNoteQueryRetrieve(), GTK_WIDGET(Utils::create_label(_("Query/Retrieve"))));

  g_signal_connect(G_OBJECT(m_notebook), "switch-page", G_CALLBACK(signal_notebook_switch_page), this);

  gtk_notebook_set_current_page(m_notebook, 0);

  return GTK_WIDGET(m_notebook);
}

void DicomServiceSetting::InitServiceSetting() {
  if (gtk_notebook_get_current_page(m_notebook) == 1) {
    InitWorklistSetting();
  }

  gtk_notebook_set_current_page(m_notebook, 1);
}

// ---------------------------------------------------------

void DicomServiceSetting::NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num) {
  switch(page_num) {
  case 0:
    {
      InitStorageSetting();

      break;
    }
  case 1:
    {
      InitWorklistSetting();

      break;
    }
  case 2:
    {
      InitMppsSetting();

      break;
    }
  case 3:
    {
      InitStorageCommitmentSetting();

      break;
    }
  case 4:
    {
      InitQueryRetrieveSetting();

      break;
    }
  default:
      break;
  }
}

bool DicomServiceSetting::EntryKeyFilter(GtkWidget* entry, GdkEventKey* event) {
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

// Storage

void DicomServiceSetting::ComboBoxChangedDeviceStorage(GtkComboBox* combobox) {
  string device = Utils::combobox_active_text(m_combobox_storage_device);

  if (!device.empty()) {
    string serviceName = gtk_entry_get_text(m_entry_storage_service);

    if (serviceName.empty()) {
      serviceName = device + "-" + _("Storage");
    }

    gtk_entry_set_text(m_entry_storage_service, serviceName.c_str());
  }
}

void DicomServiceSetting::ButtonClickedAddStorage(GtkButton* button) {
  string device = Utils::combobox_active_text(m_combobox_storage_device);
  string ae = gtk_entry_get_text(m_entry_storage_ae);
  string port = gtk_entry_get_text(m_entry_storage_port);

  if (device.empty() || ae.empty() || port.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device, AE or Port is empty, please input!"), NULL);

      return;
  }

  string serviceName = gtk_entry_get_text(m_entry_storage_service);

  if (serviceName.empty()) {
    serviceName = device + "-" + _("Storage");
  }

  GtkTreeIter iter;
  char* device_tmp = NULL;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage);
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while (exist) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device_tmp, -1);

    if(device_tmp == NULL) {
      return;
    }

    if (strcmp(device_tmp, device.c_str()) == 0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

      return;
    }

    exist = gtk_tree_model_iter_next(model, &iter);
  }

  if(!MyDCMMan::AddStorageService(device, serviceName, ae, atoi(port.c_str()))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

    return;
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_STORAGE_DEVICE, device.c_str(),
    COL_SERVICE_NAME, serviceName.c_str(),
    COL_AE_TITLE, ae.c_str(),
    COL_PORT, port.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServiceSetting::ButtonClickedDeleteStorage(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);
    MyDCMMan::DeleteStorageService(device);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedClearStorage(GtkButton* button) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_device), -1);
  gtk_entry_set_text(m_entry_storage_ae, "");
  gtk_entry_set_text(m_entry_storage_service, "");
  gtk_entry_set_text(m_entry_storage_port, "");
}

void DicomServiceSetting::ButtonClickedDefaultStorage(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeIter iter0;

  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    bool exist = gtk_tree_model_get_iter_first(model, &iter0);

    while (exist) {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter0, COL_DEFAULT, "", -1);
      exist = gtk_tree_model_iter_next(model, &iter0);
    }

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEFAULT, _("Yes"), -1);
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    MyDCMMan::SetDefaultStorageService(device);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedConnectStorage(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    m_selected_device = device;

    MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Connect..."));

    g_timeout_add(2000, signal_callback_timeout_connect_storage, this);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::CheckButtonToggledReportStorage(GtkToggleButton* togglebutton) {
  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetSendReport(gtk_toggle_button_get_active(togglebutton));
  sysDicomSetting.SyncFile();
}

void DicomServiceSetting::CheckButtonToggledFrameStorage(GtkToggleButton* togglebutton) {
  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetSendVideo(gtk_toggle_button_get_active(togglebutton));
  sysDicomSetting.SyncFile();
}

void DicomServiceSetting::ComboBoxChangedFramesStorage(GtkComboBox* combobox) {
  int index = gtk_combo_box_get_active(combobox);

  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetVideoFrames(atoi(frames[index].c_str()));
  sysDicomSetting.SyncFile();
}

// Worklist

void DicomServiceSetting::ComboBoxChangedDeviceWorklist(GtkComboBox* combobox) {
  string device = Utils::combobox_active_text(m_combobox_worklist_device);

  if (!device.empty()) {
    string serviceName = gtk_entry_get_text(m_entry_worklist_service);

    if (serviceName.empty()) {
      serviceName = device + "-" + _("Worklist");
    }

    gtk_entry_set_text(m_entry_worklist_service, serviceName.c_str());
  }
}

void DicomServiceSetting::ButtonClickedAddWorklist(GtkButton* button) {
  string device = Utils::combobox_active_text(m_combobox_worklist_device);

  string ae = gtk_entry_get_text(m_entry_worklist_ae);
  string port = gtk_entry_get_text(m_entry_worklist_port);

  if (device.empty() || ae.empty() || port.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device, AE or Port is empty, please input!"), NULL);

      return;
  }

  string serviceName = gtk_entry_get_text(m_entry_worklist_service);

  if (serviceName.empty()) {
    serviceName = device + "-" + _("Worklist");
  }

  GtkTreeIter iter;
  char* device_tmp = NULL;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_worklist);
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while (exist) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device_tmp, -1);

    if(device_tmp == NULL) {
      return;
    }

    if (strcmp(device_tmp, device.c_str()) == 0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

      return;
    }

    exist = gtk_tree_model_iter_next(model, &iter);
  }

  if(!MyDCMMan::AddWorklistService(device, serviceName, ae, atoi(port.c_str()))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

    return;
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_STORAGE_DEVICE, device.c_str(),
    COL_SERVICE_NAME, serviceName.c_str(),
    COL_AE_TITLE, ae.c_str(),
    COL_PORT, port.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServiceSetting::ButtonClickedClearWorklist(GtkButton* button) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_worklist_device), -1);
  gtk_entry_set_text(m_entry_worklist_service, "");
  gtk_entry_set_text(m_entry_worklist_ae, "");
  gtk_entry_set_text(m_entry_worklist_port, "");
}

void DicomServiceSetting::ButtonClickedDeleteWorklist(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_worklist);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_worklist);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);
    MyDCMMan::DeleteWorklistService(device);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedDefaultWorklist(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeIter iter0;

  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_worklist);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_worklist);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    bool exist = gtk_tree_model_get_iter_first(model, &iter0);

    while (exist) {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter0, COL_DEFAULT, "", -1);
      exist = gtk_tree_model_iter_next(model, &iter0);
    }

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEFAULT, _("Yes"), -1);
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    MyDCMMan::SetDefaultWorklistService(device);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedConnectWorklist(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_worklist);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_worklist);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    m_selected_device = device;

    MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Connect..."));

    g_timeout_add(2000, signal_callback_timeout_connect_worklist, this);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::CheckButtonToggledAutoWorklist(GtkToggleButton* togglebutton) {
  string device = MyDCMMan::GetDefaultWorklistServiceDevice();

  if (device.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please Set the default Worklist server in system setting"), NULL);

    return ;
  }

  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetAutoQuery(gtk_toggle_button_get_active(togglebutton));
  sysDicomSetting.SyncFile();
}

// Mpps

void DicomServiceSetting::ComboBoxChangedDeviceMpps(GtkComboBox* combobox) {
  string device = Utils::combobox_active_text(m_combobox_mpps_device);

  if (!device.empty()) {
    string serviceName = gtk_entry_get_text(m_entry_mpps_service);

    if (serviceName.empty()) {
      serviceName = device + "-" + _("MPPS");
    }

    gtk_entry_set_text(m_entry_mpps_service, serviceName.c_str());
  }
}

void DicomServiceSetting::ButtonClickedAddMpps(GtkButton* button) {
  string device = Utils::combobox_active_text(m_combobox_mpps_device);
  string ae = gtk_entry_get_text(m_entry_mpps_ae);
  string port = gtk_entry_get_text(m_entry_mpps_port);

  if (device.empty() || ae.empty() || port.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device, AE or Port is empty, please input!"), NULL);

      return;
  }

  string serviceName = gtk_entry_get_text(m_entry_mpps_service);

  if (serviceName.empty()) {
    serviceName = device + "-" + _("MPPS");
  }

  GtkTreeIter iter;
  char* device_tmp = NULL;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_mpps);
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while (exist) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device_tmp, -1);

    if(device_tmp == NULL) {
      return;
    }

    if (strcmp(device_tmp, device.c_str()) == 0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

      return;
    }

    exist = gtk_tree_model_iter_next(model, &iter);
  }

  if(!MyDCMMan::AddMPPSService(device, serviceName, ae, atoi(port.c_str()))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

    return;
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_STORAGE_DEVICE, device.c_str(),
    COL_SERVICE_NAME, serviceName.c_str(),
    COL_AE_TITLE, ae.c_str(),
    COL_PORT, port.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServiceSetting::ButtonClickedClearMpps(GtkButton* button) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_mpps_device), -1);
  gtk_entry_set_text(m_entry_mpps_service, "");
  gtk_entry_set_text(m_entry_mpps_ae, "");
  gtk_entry_set_text(m_entry_mpps_port, "");
}

void DicomServiceSetting::ButtonClickedDeleteMpps(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_mpps);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_mpps);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);
    MyDCMMan::DeleteMPPSService(device);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedDefaultMpps(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeIter iter0;

  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_mpps);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_mpps);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    bool exist = gtk_tree_model_get_iter_first(model, &iter0);

    while (exist) {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter0, COL_DEFAULT, "", -1);
      exist = gtk_tree_model_iter_next(model, &iter0);
    }

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEFAULT, _("Yes"), -1);
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    MyDCMMan::SetDefaultMPPSService(device);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedConnectMpps(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_mpps);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_mpps);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    m_selected_device = device;

    MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Connect..."));

    g_timeout_add(2000, signal_callback_timeout_connect_mpps, this);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::CheckButtonToggledSendMpps(GtkToggleButton* togglebutton) {
  string device = MyDCMMan::GetDefaultMPPSServiceDevice();

  if (device.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please Set the default MPPS server in system setting"), NULL);

    return ;
  }

  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetMPPS(gtk_toggle_button_get_active(togglebutton));
  sysDicomSetting.SyncFile();
}

// StorageCommitment

void DicomServiceSetting::ComboBoxChangedDeviceStorageCommitment(GtkComboBox* combobox) {
  string device = Utils::combobox_active_text(m_combobox_storage_commitment_device);

  if (!device.empty()) {
    string serviceName = gtk_entry_get_text(m_entry_storage_commitment_service);

    if (serviceName.empty()) {
      serviceName = device + "-" + _("StorageCommitment");
    }

    gtk_entry_set_text(m_entry_storage_commitment_service, serviceName.c_str());
  }
}

void DicomServiceSetting::ButtonClickedAddStorageCommitment(GtkButton* button) {
  string device = Utils::combobox_active_text(m_combobox_storage_commitment_device);
  string ae = gtk_entry_get_text(m_entry_storage_commitment_ae);
  string port = gtk_entry_get_text(m_entry_storage_commitment_port);

  if (device.empty() || ae.empty() || port.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device, AE or Port is empty, please input!"), NULL);

      return;
  }

  string serviceName = gtk_entry_get_text(m_entry_storage_commitment_service);

  if (serviceName.empty()) {
    serviceName = device + "-" + _("StorageCommitment");
  }

  GtkTreeIter iter;
  char* device_tmp = NULL;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage_commitment);
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while (exist) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device_tmp, -1);

    if(device_tmp == NULL) {
      return;
    }

    if (strcmp(device_tmp, device.c_str()) == 0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

      return;
    }

    exist = gtk_tree_model_iter_next(model, &iter);
  }

  if(!MyDCMMan::AddStorageCommitService(device, serviceName, ae, atoi(port.c_str()))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

    return;
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_STORAGE_DEVICE, device.c_str(),
    COL_SERVICE_NAME, serviceName.c_str(),
    COL_AE_TITLE, ae.c_str(),
    COL_PORT, port.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServiceSetting::ButtonClickedClearStorageCommitment(GtkButton* button) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_commitment_device), -1);
  gtk_entry_set_text(m_entry_storage_commitment_service, "");
  gtk_entry_set_text(m_entry_storage_commitment_ae, "");
  gtk_entry_set_text(m_entry_storage_commitment_port, "");
}

void DicomServiceSetting::ButtonClickedDeleteStorageCommitment(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage_commitment);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage_commitment);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);
    MyDCMMan::DeleteStorageCommitService(device);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedDefaultStorageCommitment(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeIter iter0;

  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage_commitment);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage_commitment);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    bool exist = gtk_tree_model_get_iter_first(model, &iter0);

    while (exist) {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter0, COL_DEFAULT, "", -1);
      exist = gtk_tree_model_iter_next(model, &iter0);
    }

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEFAULT, _("Yes"), -1);
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    MyDCMMan::SetDefaultStorageCommitService(device);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedConnectStorageCommitment(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_storage_commitment);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_storage_commitment);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    m_selected_device = device;

    MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Connect..."));

    g_timeout_add(2000, signal_callback_timeout_connect_storage_commitment, this);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::CheckButtonToggledSendStorageCommitment(GtkToggleButton* togglebutton) {
  string device = MyDCMMan::GetDefaultStorageCommitServiceDevice();

  if (device.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_ERROR, _("Please Set the default Storage Commitment server in system setting"), NULL);

    return ;
  }

  SysDicomSetting sysDicomSetting;
  sysDicomSetting.SetStorageCommitment(gtk_toggle_button_get_active(togglebutton));
  sysDicomSetting.SyncFile();
}

// QueryRetrieve

void DicomServiceSetting::ComboBoxChangedDeviceQueryRetrieve(GtkComboBox* combobox) {
  string device;

  if (gtk_combo_box_get_active(GTK_COMBO_BOX(m_combobox_query_retrieve_device)) >= 0) {
    device = Utils::combobox_active_text(m_combobox_query_retrieve_device);
  }

  if (!device.empty()) {
    string serviceName = gtk_entry_get_text(m_entry_query_retrieve_service);

    if (serviceName.empty()) {
      serviceName = device + "-" + _("QueryRetrieve");
    }

    gtk_entry_set_text(m_entry_query_retrieve_service, serviceName.c_str());
  }
}

void DicomServiceSetting::ButtonClickedAddQueryRetrieve(GtkButton* button) {
  string device = Utils::combobox_active_text(m_combobox_query_retrieve_device);
  string ae = gtk_entry_get_text(m_entry_query_retrieve_ae);
  string port = gtk_entry_get_text(m_entry_query_retrieve_port);

  if (device.empty() || ae.empty() || port.empty()) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Device, AE or Port is empty, please input!"), NULL);

      return;
  }

  string serviceName = gtk_entry_get_text(m_entry_query_retrieve_service);

  if (serviceName.empty()) {
    serviceName = device + "-" + _("QueryRetrieve");
  }

  GtkTreeIter iter;
  char* device_tmp = NULL;

  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_query_retrieve);
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while (exist) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device_tmp, -1);

    if(device_tmp == NULL) {
      return;
    }

    if (strcmp(device_tmp, device.c_str()) == 0) {
      MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
        MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

      return;
    }

    exist = gtk_tree_model_iter_next(model, &iter);
  }

  if(!MyDCMMan::AddQueryRetrieveService(device, serviceName, ae, atoi(port.c_str()))) {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("Add failed, device has been existed"), NULL);

    return;
  }

  gtk_list_store_append(GTK_LIST_STORE(model), &iter);
  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
    COL_STORAGE_DEVICE, device.c_str(),
    COL_SERVICE_NAME, serviceName.c_str(),
    COL_AE_TITLE, ae.c_str(),
    COL_PORT, port.c_str(),
    -1);

  gtk_tree_model_iter_next(model, &iter);
}

void DicomServiceSetting::ButtonClickedClearQueryRetrieve(GtkButton* button) {
  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_query_retrieve_device), -1);
  gtk_entry_set_text(m_entry_query_retrieve_service, "");
  gtk_entry_set_text(m_entry_query_retrieve_ae, "");
  gtk_entry_set_text(m_entry_query_retrieve_port, "");
}

void DicomServiceSetting::ButtonClickedDeleteQueryRetrieve(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_query_retrieve);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_query_retrieve);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);
    MyDCMMan::DeleteQueryRetrieveService(device);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedDefaultQueryRetrieve(GtkButton* button) {
  GtkTreeIter iter;
  GtkTreeIter iter0;

  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_query_retrieve);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_query_retrieve);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    bool exist = gtk_tree_model_get_iter_first(model, &iter0);

    while (exist) {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter0, COL_DEFAULT, "", -1);
      exist = gtk_tree_model_iter_next(model, &iter0);
    }

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_DEFAULT, _("Yes"), -1);
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    MyDCMMan::SetDefaultQueryRetrieveService(device);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

void DicomServiceSetting::ButtonClickedConnectQueryRetrieve(GtkButton* button) {
  GtkTreeIter iter;
  char* device = NULL;

  GtkTreeSelection* selection = gtk_tree_view_get_selection(m_treeview_query_retrieve);
  GtkTreeModel* model = gtk_tree_view_get_model(m_treeview_query_retrieve);

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)) {
    gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &device, -1);

    m_selected_device = device;

    MessageHintDialog::GetInstance()->Create(GTK_WINDOW(m_parent), _("Connect..."));

    g_timeout_add(2000, signal_callback_timeout_connect_query_retrieve, this);
  } else {
    MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
      MessageDialog::DLG_INFO, _("No device is selected!"), NULL);
  }
}

// Timeout

void DicomServiceSetting::TimeoutConnectStorage() {
  string info;

  if(MyDCMMan::TestLinkStorage(m_selected_device)) {
    PRINTF("Dicom test link OK!!\n");

    info = _("Connection test successfully!");
  } else {
    PRINTF("Dicom test link FAIL!!\n");

    info = _("Connection test fails!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

void DicomServiceSetting::TimeoutConnectWorklist() {
  string info;

  if(MyDCMMan::TestLinkWorklist(m_selected_device)) {
    PRINTF("Dicom test link OK!!\n");

    info = _("Connection test successfully!");
  } else {
    PRINTF("Dicom test link FAIL!!\n");

    info = _("Connection test fails!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

void DicomServiceSetting::TimeoutConnectMpps() {
  string info;

  if(MyDCMMan::TestLinkMPPS(m_selected_device)) {
    PRINTF("Dicom test link OK!!\n");

    info = _("Connection test successfully!");
  } else {
    PRINTF("Dicom test link FAIL!!\n");

    info = _("Connection test fails!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

void DicomServiceSetting::TimeoutConnectStorageCommitment() {
  string info;

  if(MyDCMMan::TestLinkStorageCommit(m_selected_device)) {
    PRINTF("Dicom test link OK!!\n");

    info = _("Connection test successfully!");
  } else {
    PRINTF("Dicom test link FAIL!!\n");

    info = _("Connection test fails!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

void DicomServiceSetting::TimeoutConnectQueryRetrieve() {
  string info;

  if(MyDCMMan::TestLinkQueryRetrieve(m_selected_device)) {
    PRINTF("Dicom test link OK!!\n");

    info = _("Connection test successfully!");
  } else {
    PRINTF("Dicom test link FAIL!!\n");

    info = _("Connection test fails!");
  }

  MessageDialog::GetInstance()->Create(GTK_WINDOW(m_parent),
    MessageDialog::DLG_INFO, info, NULL);
}

void DicomServiceSetting::GetServiceAttribute(string device, string serviceName, string aeTitle, int port, bool isDefault) {
  char tmp_port[256];
  char tmp_isDefault[20];
  char tmp_device[256];
  sprintf(tmp_port,"%d", port);
  sprintf(tmp_device,"%s", device.c_str());

  if (isDefault) {
    sprintf(tmp_isDefault,"%s", _("Yes"));
  } else {
    sprintf(tmp_isDefault,"%s", " ");
  }

  GtkTreeModel *model;
  GtkTreeIter iter;
  gboolean exist = FALSE;
  char *tmp_device1;

  switch(gtk_notebook_get_current_page(m_notebook)) {
  case 0:
    {
      model = gtk_tree_view_get_model(m_treeview_storage);
      exist = gtk_tree_model_get_iter_first(model, &iter);

      while(exist) {
        gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &tmp_device1, -1);

        if (strcmp(tmp_device, tmp_device1)==0) {
          return;
        }

        exist = gtk_tree_model_iter_next(model, &iter);
      }

      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        COL_STORAGE_DEVICE, device.c_str(),
        COL_SERVICE_NAME,serviceName.c_str(),
        COL_AE_TITLE,aeTitle.c_str(),
        COL_PORT,tmp_port,
        COL_DEFAULT,tmp_isDefault,
        -1);

      gtk_tree_model_iter_next(model, &iter);

      break;
    }
  case 1:
    {
      model = gtk_tree_view_get_model(m_treeview_worklist);
      exist = gtk_tree_model_get_iter_first(model, &iter);

      while(exist) {
        gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &tmp_device1, -1);

        if (strcmp(tmp_device,tmp_device1) == 0) {
          return;
        }

        exist = gtk_tree_model_iter_next(model, &iter);
      }

      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        COL_STORAGE_DEVICE, device.c_str(),
        COL_SERVICE_NAME,serviceName.c_str(),
        COL_AE_TITLE,aeTitle.c_str(),
        COL_PORT,tmp_port,
        COL_DEFAULT,tmp_isDefault,
        -1);

      gtk_tree_model_iter_next(model, &iter);

      break;
    }
  case 2:
    {
      model = gtk_tree_view_get_model(m_treeview_mpps);
      exist = gtk_tree_model_get_iter_first(model, &iter);

      while(exist) {
        gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &tmp_device1, -1);

        if (strcmp(tmp_device,tmp_device1)==0) {
          return;
        }

        exist = gtk_tree_model_iter_next(model, &iter);
      }

      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        COL_STORAGE_DEVICE, device.c_str(),
        COL_SERVICE_NAME,serviceName.c_str(),
        COL_AE_TITLE,aeTitle.c_str(),
        COL_PORT,tmp_port,
        COL_DEFAULT,tmp_isDefault,
        -1);

      gtk_tree_model_iter_next(model, &iter);

      break;
    }
  case 3:
    {
      model = gtk_tree_view_get_model(m_treeview_storage_commitment);
      exist = gtk_tree_model_get_iter_first(model, &iter);

      while(exist) {
        gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &tmp_device1, -1);

        if (strcmp(tmp_device,tmp_device1) == 0) {
          return;
        }

        exist = gtk_tree_model_iter_next(model, &iter);
      }

      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        COL_STORAGE_DEVICE, device.c_str(),
        COL_SERVICE_NAME,serviceName.c_str(),
        COL_AE_TITLE,aeTitle.c_str(),
        COL_PORT,tmp_port,
        COL_DEFAULT,tmp_isDefault,
        -1);

      gtk_tree_model_iter_next(model, &iter);

      break;
    }
  case 4:
    {
      model = gtk_tree_view_get_model(m_treeview_query_retrieve);
      exist = gtk_tree_model_get_iter_first(model, &iter);

      while(exist) {
        gtk_tree_model_get(model, &iter, COL_STORAGE_DEVICE, &tmp_device1, -1);

        if (strcmp(tmp_device, tmp_device1) == 0) {
          return;
        }

        exist = gtk_tree_model_iter_next(model, &iter);
      }

      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
        COL_STORAGE_DEVICE, device.c_str(),
        COL_SERVICE_NAME,serviceName.c_str(),
        COL_AE_TITLE,aeTitle.c_str(),
        COL_PORT,tmp_port,
        COL_DEFAULT,tmp_isDefault,
        -1);

      gtk_tree_model_iter_next(model, &iter);

      break;
    }
  default:
    break;
  }
}

GtkWidget* DicomServiceSetting::CreateNoteStorage() {
  GtkTable* table = Utils::create_table(9, 7);

  GtkFrame* frame_device = Utils::create_frame(_("Device Property"));
  GtkFrame* frame_service = Utils::create_frame(_("Service List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 7, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_service), 0, 7, 3, 8);

  // Device Property
  GtkTable* table_device = Utils::create_table(2, 7);
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 5);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_combobox_storage_device = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_combobox_storage_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_combobox_storage_device), "changed", G_CALLBACK(signal_combobox_changed_storage_device), this);

  // Service Name
  GtkLabel* label_service = Utils::create_label(_("Service Name:"));
  m_entry_storage_service = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_service), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_service), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_service, 45);

  // AE Title
  GtkLabel* label_ae = Utils::create_label(_("AE Title:"));
  m_entry_storage_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ae), 3, 4, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_ae), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_ae, 15);

  // Port
  GtkLabel* label_port = Utils::create_label(_("Port:"));
  m_entry_storage_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_port), 3, 4, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_port), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_port, 15);
  g_signal_connect(m_entry_storage_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  // Button
  GtkButton* button_add = Utils::create_button(_("Add"));
  GtkButton* button_clear = Utils::create_button(_("Clear"));

  gtk_table_attach(table_device, GTK_WIDGET(button_add), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_clear), 6, 7, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_storage_add), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_storage_clear), this);

  // Service List
  GtkTable* table_service = Utils::create_table(5, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_service), 5);

  gtk_container_add(GTK_CONTAINER(frame_service), GTK_WIDGET(table_service));

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  m_treeview_storage = CreateServiceTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview_storage));

  gtk_table_attach_defaults(table_service, GTK_WIDGET(scrolled_window), 0, 5, 0, 4);

  // Button
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  GtkButton* button_default = Utils::create_button(_("Default"));
  GtkButton* button_connect = Utils::create_button(_("Connect Test"));

  gtk_table_attach(table_service, GTK_WIDGET(button_delete), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_default), 1, 2, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_connect), 2, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_storage_delete), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_storage_default), this);
  g_signal_connect(button_connect, "clicked", G_CALLBACK(signal_button_clicked_storage_connect), this);

  // Check Button
  m_checkbutton_storage_report = Utils::create_check_button(_("Attach Structured Report"));
  m_checkbutton_storage_frame = Utils::create_check_button(_("Allow Multiframe"));
  GtkLabel* label_frames = Utils::create_label(_("Max number of frames:"));
  m_combobox_storage_frames = Utils::create_combobox_text();

  gtk_table_attach(table, GTK_WIDGET(m_checkbutton_storage_report), 0, 2, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table, GTK_WIDGET(m_checkbutton_storage_frame), 2, 4, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach_defaults(table, GTK_WIDGET(label_frames), 4, 6, 8, 9);
  gtk_table_attach(table, GTK_WIDGET(m_combobox_storage_frames), 6, 7, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  size_t size = sizeof(frames) / sizeof(frames[0]);

  for (int i = 0; i < size; i++) {
    gtk_combo_box_text_append_text(m_combobox_storage_frames, frames[i].c_str());
  }

  gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 0);

  g_signal_connect(G_OBJECT(m_checkbutton_storage_report), "toggled", G_CALLBACK(signal_checkbutton_toggled_report), this);
  g_signal_connect(G_OBJECT(m_checkbutton_storage_frame), "toggled", G_CALLBACK(signal_checkbutton_toggled_frame), this);
  g_signal_connect(G_OBJECT(m_combobox_storage_frames), "changed", G_CALLBACK(signal_combobox_changed_storage_frames), this);

  gtk_widget_show_all(GTK_WIDGET(table));

  return GTK_WIDGET(table);
}

GtkWidget* DicomServiceSetting::CreateNoteWorklist() {
  GtkTable* table = Utils::create_table(9, 7);

  GtkFrame* frame_device = Utils::create_frame(_("Device Property"));
  GtkFrame* frame_service = Utils::create_frame(_("Service List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 7, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_service), 0, 7, 3, 8);

  // Device Property
  GtkTable* table_device = Utils::create_table(2, 7);
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 5);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_combobox_worklist_device = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_combobox_worklist_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_combobox_worklist_device), "changed", G_CALLBACK(signal_combobox_changed_worklist_device), this);

  // Service Name
  GtkLabel* label_service = Utils::create_label(_("Service Name:"));
  m_entry_worklist_service = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_service), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_worklist_service), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_worklist_service, 45);

  // AE Title
  GtkLabel* label_ae = Utils::create_label(_("AE Title:"));
  m_entry_worklist_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ae), 3, 4, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_worklist_ae), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_worklist_ae, 15);

  // Port
  GtkLabel* label_port = Utils::create_label(_("Port:"));
  m_entry_worklist_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_port), 3, 4, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_worklist_port), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_worklist_port, 15);
  g_signal_connect(m_entry_worklist_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  // Button
  GtkButton* button_add = Utils::create_button(_("Add"));
  GtkButton* button_clear = Utils::create_button(_("Clear"));

  gtk_table_attach(table_device, GTK_WIDGET(button_add), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_clear), 6, 7, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_worklist_add), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_worklist_clear), this);

  // Service List
  GtkTable* table_service = Utils::create_table(5, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_service), 5);

  gtk_container_add(GTK_CONTAINER(frame_service), GTK_WIDGET(table_service));

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  m_treeview_worklist = CreateServiceTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview_worklist));

  gtk_table_attach_defaults(table_service, GTK_WIDGET(scrolled_window), 0, 5, 0, 4);

  // Button
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  GtkButton* button_default = Utils::create_button(_("Default"));
  GtkButton* button_connect = Utils::create_button(_("Connect Test"));

  gtk_table_attach(table_service, GTK_WIDGET(button_delete), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_default), 1, 2, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_connect), 2, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_worklist_delete), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_worklist_default), this);
  g_signal_connect(button_connect, "clicked", G_CALLBACK(signal_button_clicked_worklist_connect), this);

  // Check Button
  m_checkbutton_worklist_auto = Utils::create_check_button(_("Auto Query"));

  gtk_table_attach(table, GTK_WIDGET(m_checkbutton_worklist_auto), 0, 2, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_checkbutton_worklist_auto), "toggled", G_CALLBACK(signal_checkbutton_toggled_worklist_auto), this);

  gtk_widget_show_all(GTK_WIDGET(table));

  return GTK_WIDGET(table);
}

GtkWidget* DicomServiceSetting::CreateNoteMpps() {
  GtkTable* table = Utils::create_table(9, 7);

  GtkFrame* frame_device = Utils::create_frame(_("Device Property"));
  GtkFrame* frame_service = Utils::create_frame(_("Service List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 7, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_service), 0, 7, 3, 8);

  // Device Property
  GtkTable* table_device = Utils::create_table(2, 7);
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 5);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_combobox_mpps_device = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_combobox_mpps_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_combobox_mpps_device), "changed", G_CALLBACK(signal_combobox_changed_mpps_device), this);

  // Service Name
  GtkLabel* label_service = Utils::create_label(_("Service Name:"));
  m_entry_mpps_service = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_service), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_mpps_service), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_mpps_service, 45);

  // AE Title
  GtkLabel* label_ae = Utils::create_label(_("AE Title:"));
  m_entry_mpps_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ae), 3, 4, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_mpps_ae), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_mpps_ae, 15);

  // Port
  GtkLabel* label_port = Utils::create_label(_("Port:"));
  m_entry_mpps_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_port), 3, 4, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_mpps_port), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_mpps_port, 15);
  g_signal_connect(m_entry_mpps_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  // Button
  GtkButton* button_add = Utils::create_button(_("Add"));
  GtkButton* button_clear = Utils::create_button(_("Clear"));

  gtk_table_attach(table_device, GTK_WIDGET(button_add), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_clear), 6, 7, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_mpps_add), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_mpps_clear), this);

  // Service List
  GtkTable* table_service = Utils::create_table(5, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_service), 5);

  gtk_container_add(GTK_CONTAINER(frame_service), GTK_WIDGET(table_service));

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  m_treeview_mpps = CreateServiceTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview_mpps));

  gtk_table_attach_defaults(table_service, GTK_WIDGET(scrolled_window), 0, 5, 0, 4);

  // Button
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  GtkButton* button_default = Utils::create_button(_("Default"));
  GtkButton* button_connect = Utils::create_button(_("Connect Test"));

  gtk_table_attach(table_service, GTK_WIDGET(button_delete), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_default), 1, 2, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_connect), 2, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_mpps_delete), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_mpps_default), this);
  g_signal_connect(button_connect, "clicked", G_CALLBACK(signal_button_clicked_mpps_connect), this);

  // Check Button
  m_checkbutton_mpps_send = Utils::create_check_button(_("Send MPPS after start exam and end exam"));

  gtk_table_attach(table, GTK_WIDGET(m_checkbutton_mpps_send), 0, 3, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_checkbutton_mpps_send), "toggled", G_CALLBACK(signal_checkbutton_toggled_mpps_send), this);

  gtk_widget_show_all(GTK_WIDGET(table));

  return GTK_WIDGET(table);
}

GtkWidget* DicomServiceSetting::CreateNoteStorageCommitment() {
  GtkTable* table = Utils::create_table(9, 7);

  GtkFrame* frame_device = Utils::create_frame(_("Device Property"));
  GtkFrame* frame_service = Utils::create_frame(_("Service List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 7, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_service), 0, 7, 3, 8);

  // Device Property
  GtkTable* table_device = Utils::create_table(2, 7);
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 5);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_combobox_storage_commitment_device = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_combobox_storage_commitment_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_combobox_storage_commitment_device), "changed", G_CALLBACK(signal_combobox_changed_storage_commitment_device), this);

  // Service Name
  GtkLabel* label_service = Utils::create_label(_("Service Name:"));
  m_entry_storage_commitment_service = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_service), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_commitment_service), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_commitment_service, 45);

  // AE Title
  GtkLabel* label_ae = Utils::create_label(_("AE Title:"));
  m_entry_storage_commitment_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ae), 3, 4, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_commitment_ae), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_commitment_ae, 15);

  // Port
  GtkLabel* label_port = Utils::create_label(_("Port:"));
  m_entry_storage_commitment_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_port), 3, 4, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_storage_commitment_port), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_storage_commitment_port, 15);
  g_signal_connect(m_entry_storage_commitment_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  // Button
  GtkButton* button_add = Utils::create_button(_("Add"));
  GtkButton* button_clear = Utils::create_button(_("Clear"));

  gtk_table_attach(table_device, GTK_WIDGET(button_add), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_clear), 6, 7, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_storage_commitment_add), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_storage_commitment_clear), this);

  // Service List
  GtkTable* table_service = Utils::create_table(5, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_service), 5);

  gtk_container_add(GTK_CONTAINER(frame_service), GTK_WIDGET(table_service));

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  m_treeview_storage_commitment = CreateServiceTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview_storage_commitment));

  gtk_table_attach_defaults(table_service, GTK_WIDGET(scrolled_window), 0, 5, 0, 4);

  // Button
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  GtkButton* button_default = Utils::create_button(_("Default"));
  GtkButton* button_connect = Utils::create_button(_("Connect Test"));

  gtk_table_attach(table_service, GTK_WIDGET(button_delete), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_default), 1, 2, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_connect), 2, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_storage_commitment_delete), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_storage_commitment_default), this);
  g_signal_connect(button_connect, "clicked", G_CALLBACK(signal_button_clicked_storage_commitment_connect), this);

  // Check Button
  m_checkbutton_storage_commitment_send = Utils::create_check_button(_("Send storage commitment after storage"));

  gtk_table_attach(table, GTK_WIDGET(m_checkbutton_storage_commitment_send), 0, 3, 8, 9, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_checkbutton_storage_commitment_send), "toggled", G_CALLBACK(signal_checkbutton_toggled_storage_commitment_send), this);

  gtk_widget_show_all(GTK_WIDGET(table));

  return GTK_WIDGET(table);
}

GtkWidget* DicomServiceSetting::CreateNoteQueryRetrieve() {
  GtkTable* table = Utils::create_table(9, 7);

  GtkFrame* frame_device = Utils::create_frame(_("Device Property"));
  GtkFrame* frame_service = Utils::create_frame(_("Service List"));

  gtk_table_attach_defaults(table, GTK_WIDGET(frame_device), 0, 7, 0, 3);
  gtk_table_attach_defaults(table, GTK_WIDGET(frame_service), 0, 7, 3, 8);

  // Device Property
  GtkTable* table_device = Utils::create_table(2, 7);
  gtk_container_set_border_width(GTK_CONTAINER(table_device), 5);

  gtk_container_add(GTK_CONTAINER(frame_device), GTK_WIDGET(table_device));

  // Device
  GtkLabel* label_device = Utils::create_label(_("Device:"));
  m_combobox_query_retrieve_device = Utils::create_combobox_text();

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_device), 0, 1, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_combobox_query_retrieve_device), 1, 3, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(G_OBJECT(m_combobox_query_retrieve_device), "changed", G_CALLBACK(signal_combobox_changed_query_retrieve_device), this);

  // Service Name
  GtkLabel* label_service = Utils::create_label(_("Service Name:"));
  m_entry_query_retrieve_service = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_service), 0, 1, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_query_retrieve_service), 1, 3, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_query_retrieve_service, 45);

  // AE Title
  GtkLabel* label_ae = Utils::create_label(_("AE Title:"));
  m_entry_query_retrieve_ae = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_ae), 3, 4, 0, 1);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_query_retrieve_ae), 4, 6, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_query_retrieve_ae, 15);

  // Port
  GtkLabel* label_port = Utils::create_label(_("Port:"));
  m_entry_query_retrieve_port = Utils::create_entry(9679);

  gtk_table_attach_defaults(table_device, GTK_WIDGET(label_port), 3, 4, 1, 2);
  gtk_table_attach(table_device, GTK_WIDGET(m_entry_query_retrieve_port), 4, 6, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  gtk_entry_set_max_length(m_entry_query_retrieve_port, 15);
  g_signal_connect(m_entry_query_retrieve_port, "key-press-event", G_CALLBACK(signal_entry_keyfilter), this);

  // Button
  GtkButton* button_add = Utils::create_button(_("Add"));
  GtkButton* button_clear = Utils::create_button(_("Clear"));

  gtk_table_attach(table_device, GTK_WIDGET(button_add), 6, 7, 0, 1, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_device, GTK_WIDGET(button_clear), 6, 7, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_add, "clicked", G_CALLBACK(signal_button_clicked_query_retrieve_add), this);
  g_signal_connect(button_clear, "clicked", G_CALLBACK(signal_button_clicked_query_retrieve_clear), this);

  // Service List
  GtkTable* table_service = Utils::create_table(5, 5);
  gtk_container_set_border_width(GTK_CONTAINER(table_service), 5);

  gtk_container_add(GTK_CONTAINER(frame_service), GTK_WIDGET(table_service));

  // scrolled_window
  GtkScrolledWindow* scrolled_window = Utils::create_scrolled_window();
  gtk_scrolled_window_set_policy(scrolled_window, GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  m_treeview_query_retrieve = CreateServiceTreeview();
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(m_treeview_query_retrieve));

  gtk_table_attach_defaults(table_service, GTK_WIDGET(scrolled_window), 0, 5, 0, 4);

  // Button
  GtkButton* button_delete = Utils::create_button(_("Delete"));
  GtkButton* button_default = Utils::create_button(_("Default"));
  GtkButton* button_connect = Utils::create_button(_("Connect Test"));

  gtk_table_attach(table_service, GTK_WIDGET(button_delete), 0, 1, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_default), 1, 2, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);
  gtk_table_attach(table_service, GTK_WIDGET(button_connect), 2, 3, 4, 5, GTK_FILL, GTK_SHRINK, 0, 0);

  g_signal_connect(button_delete, "clicked", G_CALLBACK(signal_button_clicked_query_retrieve_delete), this);
  g_signal_connect(button_default, "clicked", G_CALLBACK(signal_button_clicked_query_retrieve_default), this);
  g_signal_connect(button_connect, "clicked", G_CALLBACK(signal_button_clicked_query_retrieve_connect), this);

  gtk_widget_show_all(GTK_WIDGET(table));

  return GTK_WIDGET(table);
}

void DicomServiceSetting::InitStorageSetting() {
  GtkTreeIter iter;

  GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_combobox_storage_device));
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while(exist) {
    char* path_string = gtk_tree_model_get_string_from_iter(model, &iter);
    int path_num = atoi(path_string);

    exist = gtk_tree_model_iter_next(model, &iter);
    gtk_combo_box_text_remove(m_combobox_storage_device, path_num);
  }

  vector<string> vec = MyDCMMan::GetAllDevice();

  for(int i = 0; i < vec.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_storage_device, vec[i].c_str());
  }

  SysDicomSetting sysDicomSetting;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_storage_report), sysDicomSetting.GetSendReport());
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_storage_frame), sysDicomSetting.GetSendVideo());

  int frames = sysDicomSetting.GetVideoFrames();

  switch(frames) {
  case 10:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 0);
      break;
  case 20:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 1);
      break;
  case 50:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 2);
      break;
  case 100:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 3);
      break;
  case 200:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 4);
      break;
  case 300:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 6);
      break;
  default:
      gtk_combo_box_set_active(GTK_COMBO_BOX(m_combobox_storage_frames), 0);
      break;
  }

  MyDCMMan::GetAllStorageService(signal_callback_get_service_attribute, this);
}

void DicomServiceSetting::InitWorklistSetting() {
  GtkTreeIter iter;

  GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_combobox_worklist_device));
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while(exist) {
    char* path_string = gtk_tree_model_get_string_from_iter(model, &iter);
    int path_num = atoi(path_string);

    exist = gtk_tree_model_iter_next(model, &iter);
    gtk_combo_box_text_remove(m_combobox_worklist_device, path_num);
  }

  vector<string> vec = MyDCMMan::GetAllDevice();

  for(int i = 0; i< vec.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_worklist_device, vec[i].c_str());
  }

  SysDicomSetting sysDicomSetting;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_worklist_auto), sysDicomSetting.GetAutoQuery());

  MyDCMMan::GetAllWorklistService(signal_callback_get_service_attribute, this);
}

void DicomServiceSetting::InitMppsSetting() {
  GtkTreeIter iter;

  GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_combobox_mpps_device));
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while(exist) {
    char* path_string = gtk_tree_model_get_string_from_iter(model, &iter);
    int path_num = atoi(path_string);

    exist = gtk_tree_model_iter_next(model, &iter);
    gtk_combo_box_text_remove(m_combobox_mpps_device, path_num);
  }

  vector<string> vec = MyDCMMan::GetAllDevice();

  for(int i = 0; i< vec.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_mpps_device, vec[i].c_str());
  }

  SysDicomSetting sysDicomSetting;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_mpps_send), sysDicomSetting.GetMPPS());

  MyDCMMan::GetAllMPPSService(signal_callback_get_service_attribute, this);
}

void DicomServiceSetting::InitStorageCommitmentSetting() {
  GtkTreeIter iter;

  GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_combobox_storage_commitment_device));
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while(exist) {
    char* path_string = gtk_tree_model_get_string_from_iter(model,&iter);
    int path_num = atoi(path_string);

    exist = gtk_tree_model_iter_next(model, &iter);
    gtk_combo_box_text_remove(m_combobox_storage_commitment_device, path_num);
  }

  vector<string> vec = MyDCMMan::GetAllDevice();

  for(int i = 0; i< vec.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_mpps_device, vec[i].c_str());
  }

  SysDicomSetting sysDicomSetting;
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkbutton_storage_commitment_send), sysDicomSetting.GetStorageCommitment());

  MyDCMMan::GetAllStorageCommitService(signal_callback_get_service_attribute, this);
}

void DicomServiceSetting::InitQueryRetrieveSetting() {
  GtkTreeIter iter;

  GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX(m_combobox_query_retrieve_device));
  bool exist = gtk_tree_model_get_iter_first(model, &iter);

  while(exist) {
    char* path_string = gtk_tree_model_get_string_from_iter(model,&iter);
    int path_num = atoi(path_string);

    exist = gtk_tree_model_iter_next(model, &iter);
    gtk_combo_box_text_remove(m_combobox_query_retrieve_device, path_num);
  }

  vector<string> vec = MyDCMMan::GetAllDevice();

  for(int i = 0; i< vec.size(); i++) {
    gtk_combo_box_text_append_text(m_combobox_query_retrieve_device, vec[i].c_str());
  }

  MyDCMMan::GetAllQueryRetrieveService(signal_callback_get_service_attribute, this);
}

GtkTreeView* DicomServiceSetting::CreateServiceTreeview() {
  GtkTreeView* treeview = Utils::create_tree_view();
  gtk_tree_view_set_enable_search (treeview, FALSE);
  gtk_tree_view_set_rules_hint (treeview, TRUE);

  GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(_("Device"), renderer, "text", COL_STORAGE_DEVICE, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Service Name"), renderer, "text", COL_SERVICE_NAME, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 200, NULL);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("AE Title"), renderer, "text", COL_AE_TITLE, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Port"), renderer, "text", COL_PORT, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 100, NULL);

  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes(_("Default"), renderer, "text", COL_DEFAULT, NULL);
  gtk_tree_view_append_column(treeview, column);
  g_object_set(G_OBJECT(column), "sizing", GTK_TREE_VIEW_COLUMN_FIXED, "fixed-width", 120, NULL);

  GtkTreeModel* model = CreateServiceModel();

  if (model != NULL) {
    gtk_tree_view_set_model(treeview, model);
  }

  g_object_unref(model);

  return treeview;
}

GtkTreeModel* DicomServiceSetting::CreateServiceModel() {
  GtkListStore* store = gtk_list_store_new(NUM_COLS,
    G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  return GTK_TREE_MODEL(store);
}
