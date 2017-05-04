#include "utils/MessageDialog.h"

#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"
#include "utils/Utils.h"

#include "keyboard/KeyDef.h"
#include "ViewMain.h"

#include <sstream>

using namespace std;

MessageDialog* MessageDialog::m_ptrInstance = NULL;

// ---------------------------------------------------------

MessageDialog* MessageDialog::GetInstance() {
  if (m_ptrInstance == NULL) {
    m_ptrInstance = new MessageDialog();
  }

  return m_ptrInstance;
}

MessageDialog::MessageDialog() {
  m_ptrFunc = NULL;
  m_ptrFuncCancel = NULL;
  m_type = DLG_INFO;
  m_preCursor = true;

  m_dialog = NULL;
  m_label = NULL;
  m_entry = NULL;
  m_progress_bar = NULL;
}

MessageDialog::~MessageDialog() {
  if (m_ptrInstance != NULL) {
    delete m_ptrInstance;

    m_ptrInstance = NULL;
  }
}

void MessageDialog::Create(GtkWindow* parent, DialogType type, const string text, pFuncDialog ptrFunc, pFuncDialog ptrFuncCancel) {
  m_ptrFunc = ptrFunc;
  m_ptrFuncCancel = ptrFuncCancel;
  m_type = type;
  m_preCursor = ViewMain::GetInstance()->GetCursorVisible();
  InvisibleCursor(false, false);

  if (MessageHintDialog::GetInstance()->Exist()) {
    MessageHintDialog::GetInstance()->Destroy();
  }

  m_label = NULL;
  m_entry = NULL;
  m_progress_bar = NULL;

  GtkTable* table = GTK_TABLE(gtk_table_new(2, 5, TRUE));
  gtk_container_set_border_width(GTK_CONTAINER(table), 10);
  gtk_table_set_col_spacings(table, 10);

  GtkImage* image = Utils::create_image();

  switch (m_type) {
  case DLG_INFO:
    {
      m_dialog = Utils::create_dialog(parent, _("Information"));

      GtkButton* button_close = Utils::add_dialog_button(m_dialog, _("Close"), GTK_RESPONSE_CLOSE);
      g_signal_connect(button_close, "clicked", G_CALLBACK(signal_button_clicked_close), this);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);

      break;
    }
  case DLG_WARNING:
    {
      m_dialog = Utils::create_dialog(parent, _("Warning"));

      GtkButton* button_close = Utils::add_dialog_button(m_dialog, _("Close"), GTK_RESPONSE_CLOSE);
      g_signal_connect(button_close, "clicked", G_CALLBACK(signal_button_clicked_close), this);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_WARNING, GTK_ICON_SIZE_DIALOG);

      break;
    }
  case DLG_ERROR:
    {
      m_dialog = Utils::create_dialog(parent, _("Error"));

      GtkButton* button_close = Utils::add_dialog_button(m_dialog, _("Close"), GTK_RESPONSE_CLOSE);
      g_signal_connect(button_close, "clicked", G_CALLBACK(signal_button_clicked_close), this);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_ERROR, GTK_ICON_SIZE_DIALOG);

      break;
    }
  case DLG_QUESTION:
    {
      m_dialog = Utils::create_dialog(parent, "");

      GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK);
      g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);

      GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL);
      g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);

      break;
    }
  case DLG_PROGRESS:
    {
      m_dialog = Utils::create_dialog(parent, "");
      gtk_window_set_decorated(GTK_WINDOW(m_dialog), FALSE);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);

      m_progress_bar = Utils::create_progress_bar();
      gtk_table_attach(table, GTK_WIDGET(m_progress_bar), 1, 5, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

      break;
    }
  case DLG_PROGRESS_CANCEL:
    {
      m_dialog = Utils::create_dialog(parent, "");
      gtk_window_set_decorated(GTK_WINDOW(m_dialog), FALSE);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);

      m_progress_bar = Utils::create_progress_bar();

      GtkButton* button_cancel = Utils::create_button();
      gtk_widget_set_size_request(GTK_WIDGET(button_cancel), 38, 38);

      Utils::set_button_image(button_cancel, GTK_IMAGE(gtk_image_new_from_stock(GTK_STOCK_STOP, GTK_ICON_SIZE_DND)));

      gtk_table_attach(table, GTK_WIDGET(m_progress_bar), 1, 4, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);
      gtk_table_attach(table, GTK_WIDGET(button_cancel), 4, 5, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

      break;
    }
  case DLG_FILENAME:
    {
      m_dialog = Utils::create_dialog(parent, "");
      gtk_window_set_decorated(GTK_WINDOW(m_dialog), FALSE);

      GtkButton* button_ok = Utils::add_dialog_button(m_dialog, _("OK"), GTK_RESPONSE_OK);
      g_signal_connect(button_ok, "clicked", G_CALLBACK(signal_button_clicked_ok), this);

      GtkButton* button_cancel = Utils::add_dialog_button(m_dialog, _("Cancel"), GTK_RESPONSE_CANCEL);
      g_signal_connect(button_cancel, "clicked", G_CALLBACK(signal_button_clicked_cancel), this);

      gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);

      m_entry = Utils::create_entry();
      gtk_entry_set_max_length(m_entry, 64);
      gtk_table_attach(table, GTK_WIDGET(m_entry), 1, 5, 1, 2, GTK_FILL, GTK_SHRINK, 0, 0);

      g_signal_connect((gpointer)m_entry, "insert_text", G_CALLBACK(signal_entry_insert_text), this);

      break;
    }
  default:
    {
      m_dialog = NULL;

      break;
    }
  }

  if (m_dialog != NULL) {
    gtk_widget_set_size_request(GTK_WIDGET(m_dialog), 400, 160);
    gtk_dialog_set_has_separator(m_dialog, FALSE);

    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

    m_label = Utils::create_label(text);

    gtk_table_attach_defaults(table, GTK_WIDGET(image), 0, 1, 0, 1);
    gtk_table_attach_defaults(table, GTK_WIDGET(m_label), 1, 5, 0, 1);

    gtk_widget_show_all(GTK_WIDGET(m_dialog));

    g_keyInterface.Push(this);
    SetSystemCursorToCenter();
  }
}

void MessageDialog::Destroy() {
  if (GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();

    gtk_widget_hide_all(GTK_WIDGET(m_dialog));
    gtk_widget_destroy (GTK_WIDGET(m_dialog));

    m_dialog = NULL;

    InvisibleCursor(!m_preCursor);

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }
}

void MessageDialog::SetProgressBar(double fraction) {
  if (m_progress_bar != NULL) {
    gtk_progress_bar_set_fraction(m_progress_bar, fraction);

    stringstream ss;
    ss << fraction * 100 << "%";

    gtk_progress_bar_set_text(m_progress_bar, ss.str().c_str());
  }
}

bool MessageDialog::Exist() {
  if (m_dialog != NULL && GTK_IS_WINDOW(m_dialog)) {
    return true;
  } else {
    return false;
  }
}

void MessageDialog::SetText(const string text) {
  if (m_label != NULL) {
    gtk_label_set_text(m_label, text.c_str());
  }
}

// ---------------------------------------------------------

void MessageDialog::ButtonClickedClose() {
  Destroy();

  if (m_ptrFunc != NULL) {
    (*m_ptrFunc)(NULL);

    m_ptrFunc = NULL;
  }
}

void MessageDialog::ButtonClickedOk() {
  if (m_type == DLG_FILENAME) {
    string name = "";

    if (m_entry != NULL) {
      name = gtk_entry_get_text(m_entry);
    }

    Destroy();

    if (!name.empty()) {
      if (m_ptrFunc != NULL) {
        (*m_ptrFunc)((void*)name.c_str());

        m_ptrFunc = NULL;
      }
    }

  } else {
    Destroy();

    g_timeout_add(50, signal_handle, this);
  }
}

void MessageDialog::ButtonClickedCancel() {
  Destroy();

  if (m_ptrFuncCancel != NULL) {
    (*m_ptrFuncCancel)(NULL);

    m_ptrFuncCancel = NULL;
  }
}

void MessageDialog::ButtonProgressClickedCancel() {
  SetText(_("Cancelling..."));

  if (m_ptrFunc != NULL) {
    (*m_ptrFunc)(NULL);

    m_ptrFunc = NULL;
  }
}

void MessageDialog::EntryInsertText(GtkEditable* editable, gchar* new_text, gint new_text_length, gint* position) {
  string text = gtk_entry_get_text(GTK_ENTRY(editable));

  if (g_ascii_ispunct(*new_text) || ((text.size() + new_text_length) > gtk_entry_get_max_length(GTK_ENTRY(editable)))) {
    gtk_signal_emit_stop(GTK_OBJECT(editable), g_signal_lookup("insert-text", GTK_TYPE_EDITABLE));
  }
}

void MessageDialog::WindowDeleteEvent() {
  Destroy();

  if (m_ptrFuncCancel != NULL) {
    (*m_ptrFuncCancel)(NULL);

    m_ptrFuncCancel = NULL;
  }
}

void MessageDialog::Handle() {
  if (m_ptrFunc != NULL) {
    (*m_ptrFunc)(NULL);

    m_ptrFunc = NULL;
  }
}

void MessageDialog::KeyEvent(unsigned char key) {
  FakeXEvent::KeyEvent(key);

  switch (key) {
  case KEY_ESC:
    {
      if (m_type!=DLG_PROGRESS_CANCEL && m_type!=DLG_PROGRESS) {
        signal_exit_window(this);

        if (m_ptrFuncCancel != NULL) {
          (*m_ptrFuncCancel)(NULL);

          m_ptrFuncCancel = NULL;
        }
      }

      break;
    }
  default:
    break;
  }
}

////////////////////////////////////////////////////////////

MessageHintDialog* MessageHintDialog::m_ptrInstance = NULL;

MessageHintDialog* MessageHintDialog::GetInstance() {
  if (m_ptrInstance == NULL) {
    m_ptrInstance = new MessageHintDialog();
  }

  return m_ptrInstance;
}

MessageHintDialog::MessageHintDialog() {
  m_dialog = NULL;
}

MessageHintDialog::~MessageHintDialog() {
  if (m_ptrInstance != NULL) {
    delete m_ptrInstance;
  }

  m_ptrInstance = NULL;
}

void MessageHintDialog::Create(GtkWindow* parent, const string text) {
  m_dialog = Utils::create_dialog(parent, "", 400, 160);

  gtk_dialog_set_has_separator(m_dialog, FALSE);
  gtk_window_set_decorated(GTK_WINDOW(m_dialog), FALSE);

  GtkTable* table = GTK_TABLE(gtk_table_new(1, 1, TRUE));
  gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(m_dialog)), GTK_WIDGET(table));

  gtk_container_set_border_width(GTK_CONTAINER(table), 10);

  GtkBox* box = GTK_BOX(gtk_hbox_new(FALSE, 10));
  gtk_table_attach_defaults(table, GTK_WIDGET(box), 0, 1, 0, 1);

  GtkImage* image = Utils::create_image();
  gtk_image_set_from_stock(image, GTK_STOCK_DIALOG_INFO, GTK_ICON_SIZE_DIALOG);

  GtkLabel* label = Utils::create_label(text);

  gtk_box_pack_start(box, GTK_WIDGET(image), FALSE, FALSE, 0);
  gtk_box_pack_start(box, GTK_WIDGET(label), TRUE, TRUE, 0);

  gtk_widget_show_all(GTK_WIDGET(m_dialog));

  g_keyInterface.Push(this);
  SetSystemCursorToCenter();
}

bool MessageHintDialog::Exist() {
  if (m_dialog != NULL && GTK_IS_WINDOW(m_dialog)) {
    return true;
  }
  else {
    return false;
  }
}

void MessageHintDialog::Destroy() {
  if (GTK_IS_WIDGET(m_dialog)) {
    g_keyInterface.Pop();

    gtk_widget_destroy(GTK_WIDGET(m_dialog));
    m_dialog = NULL;

    if (g_keyInterface.Size() == 1) {
      SetSystemCursor(SYSCURSOR_X, SYSCUROSR_Y);
    }
  }
}
