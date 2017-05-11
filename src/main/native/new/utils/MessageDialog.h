#ifndef __MESSAGE_DIALOG_H__
#define __MESSAGE_DIALOG_H__

#include <gtk/gtk.h>
#include "utils/FakeXEvent.h"

#include <string>

using std::string;

typedef int (*pFuncDialog)(gpointer data);

class MessageDialog: public FakeXEvent {
public:
  static MessageDialog* GetInstance();

  enum DialogType {
    DLG_INFO,
    DLG_WARNING,
    DLG_ERROR,
    DLG_QUESTION,
    DLG_PROGRESS,
    DLG_PROGRESS_CANCEL,
    DLG_FILENAME
  };

public:
  ~MessageDialog();

public:
  void Create(GtkWindow* parent, DialogType type, const string text, pFuncDialog ptrFunc = NULL, pFuncDialog ptrFuncCancel = NULL);
  void Destroy();
  void SetProgressBar(double fraction);
  bool Exist();

  void SetText(const string text);

private:
  MessageDialog();

private:
  // signal

  static void signal_button_clicked_close(GtkButton* button, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->ButtonClickedClose();
    }
  }

  static void signal_button_clicked_ok(GtkButton* button, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->ButtonClickedOk();
    }
  }

  static void signal_button_clicked_cancel(GtkButton* button, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->ButtonClickedCancel();
    }
  }

  static void signal_entry_insert_text(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->EntryInsertText(editable, new_text, new_text_length, position);
    }
  }

  static void signal_button_progress_clicked_cancel(GtkButton* button, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->ButtonProgressClickedCancel();
    }
  }

  static gboolean signal_window_delete_event(GtkWidget* widget, GdkEvent* event, MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->WindowDeleteEvent();
    }

    return FALSE;
  }

  static gboolean signal_exit_window(MessageDialog* dialog) {
    if (dialog != NULL) {
      dialog->Destroy();
    }

    return FALSE;
  }

  static gboolean signal_handle(gpointer data) {
    MessageDialog* dialog = (MessageDialog*)data;

    if (dialog != NULL) {
      dialog->Handle();
    }

    return FALSE;
  }

private:
  void ButtonClickedClose();
  void ButtonClickedOk();
  void ButtonClickedCancel();
  void ButtonProgressClickedCancel();
  void EntryInsertText(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position);
  void WindowDeleteEvent();
  void Handle();

  void KeyEvent(unsigned char key);

private:
  static MessageDialog* m_instance;

  pFuncDialog m_ptrFunc;
  pFuncDialog m_ptrFuncCancel;
  DialogType m_type;
  bool m_preCursor;

  GtkDialog* m_dialog;
  GtkLabel* m_label;
  GtkEntry* m_entry;
  GtkProgressBar* m_progress_bar;
};

/*
 *  Used for show info when something is processing, e.g. copying, sending.
 *  It is different from MessageDialog. In the normal case, using MessageDialog
 *  to show info.
 */
class MessageHintDialog: public FakeXEvent {
public:
  static MessageHintDialog* GetInstance();

public:
  ~MessageHintDialog();

public:
  void Create(GtkWindow* parent, const string text);

  void Destroy();
  bool Exist();

private:
  MessageHintDialog();

private:
  static MessageHintDialog* m_instance;

  GtkDialog* m_dialog;
};

#endif
