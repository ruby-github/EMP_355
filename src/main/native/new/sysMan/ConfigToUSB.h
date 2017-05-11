#ifndef __CONFIG_TO_USB_H__
#define __CONFIG_TO_USB_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

class ConfigToUSB: public FakeXEvent {
public:
  static ConfigToUSB* GetInstance();

public:
  ~ConfigToUSB();

  void CreateWindow(GtkWindow* parent);

private:
  // signal

  static void signal_button_clicked_ok(GtkButton* button, ConfigToUSB* data) {
    if (data != NULL) {
      data->ButtonClickedOK(button);
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, ConfigToUSB* data) {
    if (data != NULL) {
      data->ButtonClickedCancel(button);
    }
  }

  static void signal_treeselection_changed_root(GtkTreeSelection* selction, ConfigToUSB* data) {
    if (data != NULL) {
      data->RootSelectionChanged(selction);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ConfigToUSB* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_callback_load_selected_data(gpointer data) {
    ConfigToUSB* config = (ConfigToUSB*)data;

    if (config != NULL) {
      config->LoadSelectedData();
    }

    return FALSE;
  }

  static int signal_callback_cancelloadusb(gpointer data) {
    g_cancellable_cancel(m_cancellable);

    return 0;
  }

  static void signal_callback_progress(goffset current, goffset total, gpointer data) {
    ConfigToUSB::GetInstance()->CallbackProgress(current, total);
  }

  static void signal_callback_toggled(GtkCellRendererToggle* cell, gchar* path_str, ConfigToUSB* data) {
    if (data != NULL) {
      data->ToggleData(cell, path_str);
    }
  }

  // signal

  void ButtonClickedOK(GtkButton* button);
  void ButtonClickedCancel(GtkButton* button);
  void RootSelectionChanged(GtkTreeSelection* selection);

  void LoadSelectedData();
  void CallbackProgress(goffset current, goffset total);
  void ToggleData(GtkCellRendererToggle* cell, gchar* path_str);

private:
  ConfigToUSB();

  void KeyEvent(unsigned char KeyValue);

  void DestroyWindow();

  vector<string> GetSelectedVec() {
    return m_vecPath;
  }

  GtkTreeView* CreateTreeview(gint type);
  GtkTreeModel* CreateRootModel();

  void UpdateRootModel();
  void UpdateBranchModel(gint rows);

  GtkTreeModel* LoadBranchModel(gchar* branch);
  bool GetAllSelectPath();
  void SetAllToggleValue(GtkTreeModel* model, gboolean value);

private:
  static ConfigToUSB* m_instance;
  static GCancellable* m_cancellable;

private:
  GtkWindow* m_parent;
  GtkDialog* m_dialog;
  GtkTreeView* m_treeview_root;
  GtkTreeView* m_treeview_branch;

  GList* m_listBranch;

  vector<string> m_vecPath;
};

#endif
