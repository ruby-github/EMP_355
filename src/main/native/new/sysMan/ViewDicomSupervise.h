#ifndef __VIEW_DICOM_SUPERVISE_H__
#define __VIEW_DICOM_SUPERVISE_H__

#include "utils/FakeXEvent.h"
#include "utils/Utils.h"

class ViewDicomSupervise: public FakeXEvent {
public:
  static ViewDicomSupervise* GetInstance();

public:
  ~ViewDicomSupervise();

  void CreateWindow();

  void Authenticate();

private:
  // signal

  static void signal_button_clicked_export(GtkButton* button, ViewDicomSupervise* data) {
    if (data != NULL) {
      data->ButtonClickedExport(button);
    }
  }

  static void signal_button_clicked_register(GtkButton* button, ViewDicomSupervise* data) {
    if (data != NULL) {
      data->ButtonClickedRegister(button);
    }
  }

  static void signal_button_clicked_exit(GtkButton* button, ViewDicomSupervise* data) {
    if (data != NULL) {
      data->ButtonClickedExit(button);
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, ViewDicomSupervise* data) {
    if (data != NULL) {
      data->DestroyWindow();
    }

    return FALSE;
  }

  static gboolean signal_callback_authen(gpointer data) {
    ViewDicomSupervise* dicom = (ViewDicomSupervise*)data;

    if (dicom != NULL) {
      if (dicom->IsAuthenValid()) {
        return TRUE;
      } else {
        return FALSE;
      }
    }

    return FALSE;
  }

  static gboolean signal_callback_exitwindow(gpointer data) {
    ViewDicomSupervise* dicom = (ViewDicomSupervise*)data;

    if (data != NULL) {
      dicom->DestroyWindow();
    }

    return FALSE;
  }

  // signal

  void ButtonClickedExport(GtkButton* button);
  void ButtonClickedRegister(GtkButton* button);
  void ButtonClickedExit(GtkButton* button);

  bool IsAuthenValid();

private:
  ViewDicomSupervise();

  void KeyEvent(unsigned char keyValue);

  void DestroyWindow();
  void Exit();

private:
  static ViewDicomSupervise* m_instance;

private:
  GtkDialog* m_dialog;
  GtkLabel* m_label_export;
  GtkLabel* m_label_register;
  GtkEntry* m_entry_key;

  int m_timer;
  std::vector<unsigned char> m_vecAuthenInfo;
};

#endif
