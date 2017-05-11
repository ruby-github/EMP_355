#ifndef __CONFIG_TO_HOST_H__
#define __CONFIG_TO_HOST_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

class ConfigToHost: public FakeXEvent {
public:
  static ConfigToHost* GetInstance();

public:
  ~ConfigToHost();

  void CreateWindow(GtkWindow* parent);
  void CreateCalcImportWindow(GtkWindow* parent);

  void DestroyWindow();

  void ClickedOKAndCancel();
  void ExportRightInfoNotice(string result);

private:
  // signal

  static void signal_button_clicked_ok(GtkButton* button, ConfigToHost* data) {
    if (data != NULL) {
      data->ButtonClickedOK(button);
    }
  }

  static void signal_button_clicked_import(GtkButton* button, ConfigToHost* data) {
    data->ButtonClickedImport(button);
  }

  static void signal_button_clicked_cancel(GtkButton* button, ConfigToHost* data) {
    if (data != NULL) {
      data->ButtonClickedCancel(button);
    }
  }

  static void signal_treeselection_changed_root(GtkTreeSelection* selction, ConfigToHost* data) {
    if (data != NULL) {
      data->RootSelectionChanged(selction);
    }
  }

  static void signal_treeselection_changed_root_calc(GtkTreeSelection* selction, ConfigToHost* data) {
    if (data != NULL) {
      data->RootSelectionChangedCalc(selction);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ConfigToHost* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_callback_load_selected_data(gpointer data) {
    ConfigToHost* config = (ConfigToHost*)data;

    if (config != NULL) {
      config->LoadSelectedData();
    }

    return FALSE;
  }

  static gboolean signal_callback_load_selected_data_calc(gpointer data) {
    ConfigToHost* config = (ConfigToHost*)data;

    if (config != NULL) {
      config->LoadSelectedDataCalc();
    }

    return FALSE;
  }

  static int signal_callback_cancelloadhost(gpointer data) {
    g_cancellable_cancel(m_cancellable);

    return 0;
  }

  static void signal_callback_progress(goffset current, goffset total, gpointer data) {
    ConfigToHost::GetInstance()->CallbackProgress(current, total);
  }

  static void signal_callback_toggled(GtkCellRendererToggle* cell, gchar* path_str, ConfigToHost* data) {
    if (data != NULL) {
      data->ToggleData(cell, path_str);
    }
  }

  // signal

  void ButtonClickedOK(GtkButton* button);
  void ButtonClickedImport(GtkButton *button);
  void ButtonClickedCancel(GtkButton* button);
  void RootSelectionChanged(GtkTreeSelection* selection);
  void RootSelectionChangedCalc(GtkTreeSelection* selection);

  void LoadSelectedData();
  void LoadSelectedDataCalc();
  void CallbackProgress(goffset current, goffset total);
  void ToggleData(GtkCellRendererToggle* cell, gchar* path_str);

private:
  ConfigToHost();

  void KeyEvent(unsigned char KeyValue);

  GtkTreeView* CreateTreeview(gint type);
  GtkTreeModel* CreateRootModel();

  GtkTreeView* CreateTreeviewCalc();
  GtkTreeModel* CreateRootModelCalc();

  void HideClickedOKAndCancel();

  void ExportErrorInfoNotice(string result);
  void ExportLoadInfoNotice(string result);

  void DeleteUdiskFile();

  void UpdateRootModel();
  void UpdateRootModelCalc();
  void UpdateBranchModel(gint rows);

  GtkTreeModel* LoadBranchModel(gchar* branch);

  bool GetAllSelectPath();
  bool GetAllSelectPathCalc();

  void SetAllToggleValue(GtkTreeModel* model, gboolean value);

  void SetProgressBar(double fraction);

  vector<string> GetSelectedVec() {
    return m_vecPath;
  }

private:
  static ConfigToHost* m_instance;
  static GCancellable* m_cancellable;

private:
  GtkWindow* m_parent;
  GtkDialog* m_dialog;

  GtkTreeView* m_treeview_root;
  GtkTreeView* m_treeview_branch;

  GtkCheckButton* m_check_button_del;
  GtkProgressBar* m_progress_bar;
  GtkImage* m_image;
  GtkLabel* m_label_notice;

  GList* m_listBranch;

  vector<string> m_vecPath;
};

#endif
