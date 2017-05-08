#ifndef __CONFIG_TO_USB_H__
#define __CONFIG_TO_USB_H__

#include "utils/FakeXEvent.h"

#include <string>
#include <vector>

using std::string;
using std::vector;

class ConfigToUSB: public FakeXEvent {
public:
  static ConfigToUSB* GetInstance();

  void CreateWindow(GtkWindow* parent);

public:
  ~ConfigToUSB();

private:
  // signal

  static gboolean on_window_delete_event(GtkWidget* widget, GdkEvent* event, ConfigToUSB* data) {
    return data->WindowDeleteEvent(widget, event);
  }

  static void on_data_toggled(GtkCellRendererToggle* cell, gchar* path_str, ConfigToUSB* data) {
    data->ToggleData(cell, path_str);
  }

  static void on_root_selection_changed(GtkTreeSelection* selction, ConfigToUSB* data) {
    data->RootSelectionChanged(selction);
  }

  static void on_button_ok_clicked(GtkButton* button, ConfigToUSB* data) {
    data->BtnOKClicked(button);
  }

  static void on_button_cancel_clicked(GtkButton* button, ConfigToUSB* data) {
    data->BtnCancelClicked(button);
  }

  // signal
  gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
  void BtnOKClicked(GtkButton *button);
  void BtnCancelClicked(GtkButton *button);
  void ToggleData(GtkCellRendererToggle *cell, gchar *path_str);
  void RootSelectionChanged(GtkTreeSelection *selection);

private:
  ConfigToUSB();

  void KeyEvent(unsigned char KeyValue);

private:
  static ConfigToUSB* m_ptrInstance;

private:
  GtkWidget* m_window;

////////////////////////////////////////////////////////////

public:
    GtkWidget* GetWindow() {
        return m_window;
    }
    vector<string> GetSelectedVec() {
        return m_vecPath;
    }
    void ClearData() {
        m_vecPath.clear();
    }
    void DestroyWindow(void);

private:
    enum {
        COL_CHECKED,
        COL_NAME,
        NUM_COLS
    };






    GtkWidget *m_treeRoot;
    GtkWidget *m_treeBranch;
    GtkWidget *m_chkbtnDel;
    GList *m_listBranch;

    vector<string> m_vecPath;

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
