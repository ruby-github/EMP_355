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

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ConfigToUSB* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_load_selected_data(gpointer data) {
    ConfigToUSB* config = (ConfigToUSB*)data;

    if (config != NULL) {
      if (config->LoadSelectedData()) {
        return TRUE;
      } else {
        return FALSE;
      }
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

  // signal

  void ButtonClickedOK(GtkButton* button);
  void ButtonClickedCancel(GtkButton* button);
  bool LoadSelectedData();
  void CallbackProgress(goffset current, goffset total);

private:
  ConfigToUSB();

  void KeyEvent(unsigned char KeyValue);

  void DestroyWindow();

  vector<string> GetSelectedVec() {
    return m_vecPath;
  }

private:
  static ConfigToUSB* m_instance;
  static GCancellable* m_cancellable;

private:
  GtkDialog* m_dialog;

  vector<string> m_vecPath;

private:
  static void on_data_toggled(GtkCellRendererToggle* cell, gchar* path_str, ConfigToUSB* data) {
    data->ToggleData(cell, path_str);
  }

  static void on_root_selection_changed(GtkTreeSelection* selction, ConfigToUSB* data) {
    data->RootSelectionChanged(selction);
  }

  // signal

  void ToggleData(GtkCellRendererToggle *cell, gchar *path_str);
  void RootSelectionChanged(GtkTreeSelection *selection);

private:
    GtkWidget *m_treeRoot;
    GtkWidget *m_treeBranch;
    GtkWidget *m_chkbtnDel;
    GList *m_listBranch;


    GtkWidget* create_treeview(gint type);
    GtkTreeModel* create_root_model(void);
    void UpdateRootModel(void);
    void UpdateBranchModel(gint rows);
    GtkTreeModel* LoadBranchModel(gchar *branch);
    void SetAllToggleValue(GtkTreeModel *model, gboolean value);
    gboolean GetAllSelectPath(void);
    gboolean CheckBranchStauts(void);
};

#endif
