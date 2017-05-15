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

  static gboolean HandleWindowDeleteEvent(GtkWidget* widget, GdkEvent* event, ViewSuperuser* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static void HandleMachineChanged(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->MachineChanged(combobox);
    }
  }

  static void HandleProbeChanged(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ProbeChanged(combobox);
    }
  }

  static void HandleChgAperture(GtkComboBox* combobox, ViewSuperuser* data) {
    if (data != NULL) {
      return data->ChgAperture(combobox);
    }
  }

  static void HandleBtnWriteClicked(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->BtnExitClicked(button);
    }
  }

  static void HandleBtnExitClicked(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->BtnExitClicked(button);
    }
  }

  static void HandleBtnChgMonitorClicked(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->BtnChgMonitorClicked(button);
    }
  }

  static void HandleBtnChgMonitorClicked2(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->BtnChgMonitorClicked2(button);
    }
  }

  static void HandleBtnEnterDemoClicked(GtkButton* button, ViewSuperuser* data) {
    if (data != NULL) {
      return data->BtnEnterDemoClicked(button);
    }
  }

  static gboolean HandleAuthen(gpointer data) {
    if (ViewSuperuser::GetInstance()->IsAuthenValid()) {
      return TRUE;
    } else {
      return FALSE;
    }
  }

  static gboolean HandleWriteProbe(gpointer data) {
    ViewSuperuser::GetInstance()->WriteProbe();

    return FALSE;
  }

  static gboolean ExitWindow(gpointer data) {
    ViewSuperuser* superuser = (ViewSuperuser*)data;

    if (superuser != NULL) {
      superuser->DestroyWindow();
    }

    return FALSE;
  }

  #ifdef TRANSDUCER
    static void on_spinbutton_press_adjust(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, ViewSuperuser* data) {
      data->SpinbuttonPressAdjust(editable, new_text, new_text_length, position);
    }
  #endif

  // signal

  void MachineChanged(GtkComboBox* combobox);
  void ProbeChanged(GtkComboBox* combobox);
  void ChgAperture(GtkComboBox* combobox);
  void BtnWriteClicked(GtkButton* button);
  void BtnExitClicked(GtkButton* button);
  void BtnChgMonitorClicked(GtkButton* button);
  void BtnChgMonitorClicked2(GtkButton* button);
  void BtnEnterDemoClicked(GtkButton* button);

  bool IsAuthenValid();
  void WriteProbe();

  #ifdef TRANSDUCER
    void SpinbuttonPressAdjust(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  #endif

private:
  ViewSuperuser();

  void KeyEvent(unsigned char keyValue);

  void DestroyWindow();
  void Exit();

  void UpdateProbeStatus(string socket, string status);

  GtkWidget* CreateProbeTreeview();
  GtkTreeModel* CreateProbeModel();

private:
  static ViewSuperuser* m_instance;

private:

private:
    GtkWidget *m_window;
    GtkWidget *m_labelHint;
    GtkWidget *m_treeviewProbe;

    // anthen
    std::vector<unsigned char> m_vecAuthenInfo;
    bool m_statusAuthen;
    bool m_statusDemo;
    int m_timer;

    //probe
    int m_probeType;
    //transducer press value correct
    int m_tranPressCorrect;
};

#endif
