#ifndef __VIEW_SUPER_USER_H__
#define __VIEW_SUPER_USER_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

class ViewSuperuser: public FakeXEvent {
public:
  static ViewSuperuser* GetInstance();

public:
  ~ViewSuperuser();

  void CreateWindow();
  void CreateDemoWindow();

  void Authenticate();
  void ExitDemo();

  bool GetDemoStatus();

private:
  // signal

  static void signal_combobox_changed_machine(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ComboBoxChangedMachine(combobox);
    }
  }

  static void signal_combobox_changed_probe(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ComboBoxChangedProbe(combobox);
    }
  }

  static void signal_combobox_changed_aperture(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ComboBoxChangedAperture(combobox);
    }
  }

  static void signal_button_clicked_probe_write(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ButtonClickedProbeWrite(button);
    }
  }

  static void signal_button_clicked_exit(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ButtonClickedExit(button);
    }
  }

  static void signal_button_clicked_monitor(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ButtonClickedMonitor(button);
    }
  }

  static void signal_button_clicked_monitor2(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ButtonClickedMonitor2(button);
    }
  }

  static void signal_button_clicked_demo(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ButtonClickedDemo(button);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewSuperuser* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_callback_authen(gpointer data) {
    if (ViewSuperuser::GetInstance()->IsAuthenValid()) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  static gboolean signal_callback_probe_write(gpointer data) {
    ViewSuperuser::GetInstance()->WriteProbe();

    return FALSE;
  }

  static gboolean signal_callback_exit_window(gpointer data) {
    ViewSuperuser* superuser = (ViewSuperuser*)data;

    if (superuser != NULL) {
      superuser->DestroyWindow();
    }

    return FALSE;
  }

  // signal

  void ComboBoxChangedMachine(GtkComboBox* combobox);
  void ComboBoxChangedProbe(GtkComboBox* combobox);
  void ComboBoxChangedAperture(GtkComboBox* combobox);
  void ButtonClickedProbeWrite(GtkButton* button);
  void ButtonClickedExit(GtkButton* button);
  void ButtonClickedMonitor(GtkButton* button);
  void ButtonClickedMonitor2(GtkButton* button);
  void ButtonClickedDemo(GtkButton* button);

private:
  ViewSuperuser();

  void KeyEvent(unsigned char keyValue);

  void DestroyWindow();
  void Exit();

  void UpdateProbeStatus(string socket, string status);

  GtkTreeView* CreateProbeTreeview();
  GtkTreeModel* CreateProbeModel();

  bool IsAuthenValid();
  void WriteProbe();

private:
  static ViewSuperuser* m_instance;

private:
  GtkDialog* m_dialog;
  GtkLabel* m_label_hint;
  GtkTreeView* m_treeview_probe;

  int m_timer;
  int m_probeType;

  bool m_statusAuthen;
  bool m_statusDemo;

  vector<unsigned char> m_vecAuthenInfo;
};

#endif
