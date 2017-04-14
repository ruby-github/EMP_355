// -*- c++ -*-
#ifndef _VIEW_DIALOG_H_
#define _VIEW_DIALOG_H_

#include <gtk/gtk.h>
#include "utils/FakeXEvent.h"

typedef int (*pFuncDialog)(gpointer data);

class MessageDialog: public FakeXEvent {
public:
    pFuncDialog m_ptrFunc;

    ~MessageDialog();

    enum DialogType {
      DLG_INFO,
      DLG_WARNING,
      DLG_QUESTION,
      DLG_ERROR,
      DLG_PROGRESS,
      DLG_PROGRESS_CANCEL,
      DLG_FILENAME
    };

    static MessageDialog* GetInstance();

    void Create(GtkWindow *parent, DialogType type, const char *info, pFuncDialog ptrFunc, pFuncDialog ptrFuncCancel=NULL);
void Destroy(void);
void SetProgressBar(double fraction);
bool Exist(void);
    void SetText(const char *info);


private:
    MessageDialog();
    static MessageDialog* m_ptrInstance;

    void KeyEvent(unsigned char keyValue);

    GtkWidget *m_window;
    pFuncDialog m_ptrFuncCancel;
    GtkWidget *m_label_text;
    GtkWidget *m_progress_bar;
    GtkWidget *m_entry;
    DialogType m_type;
    bool m_preCursor;

    // signal handle
    gboolean WindowDeleteEvent(GtkWidget *widget, GdkEvent *event);
    void BtnOkClicked(GtkButton *button);
    void BtnCancelClicked(GtkButton *button);
    void BtnCloseClicked(GtkButton *button);
    void BtnPrgCancelClicked(GtkButton *button);
    void EntryInsertText(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position);

    // signal connect
    static gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, MessageDialog *data) {
        return data->WindowDeleteEvent(widget, event);
    }
    static void on_button_ok_clicked(GtkButton *button, MessageDialog *data) {
        data->BtnOkClicked(button);
    }
    static void on_button_cancel_clicked(GtkButton *button, MessageDialog *data) {
        data->BtnCancelClicked(button);
    }
    static void on_button_close_clicked(GtkButton *button, MessageDialog *data) {
        data->BtnCloseClicked(button);
    }
    static void on_button_prg_cancel_clicked(GtkButton *button, MessageDialog *data) {
        data->BtnPrgCancelClicked(button);
    }
    static void on_entry_insert_text(GtkEditable *editable, gchar *new_text, gint new_text_length, gint *position, MessageDialog *data) {
        data->EntryInsertText(editable, new_text, new_text_length, position);
    }

};

/*
 *  Used for show info when something is processing, e.g. copying, sending. It is different from MessageDialog. In the normal case, using MessageDialog
 *  to show info.
 */
class MessageHintDialog : public FakeXEvent {
public:
    ~MessageHintDialog();
    static MessageHintDialog* GetInstance();

    void Create(GtkWindow *parent, const char *info);


    void Destroy(void);
    bool Exist(void);
private:
    MessageHintDialog();
    static MessageHintDialog* m_ptrInstance;

    GtkWidget *m_window;
};

#endif
