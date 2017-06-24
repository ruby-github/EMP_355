#include "display/CusSpin.h"

// ---------------------------------------------------------

CusSpin::CusSpin() {
  m_cusspin = NULL;
  m_button_label = NULL;
  m_button_sub = NULL;
  m_entry = NULL;
  m_button_add = NULL;

  m_item = NULL;
}

CusSpin::~CusSpin() {
  if (m_cusspin != NULL) {
    gtk_widget_destroy(GTK_WIDGET(m_cusspin));
  }

  m_cusspin = NULL;
}

GtkWidget* CusSpin::Create() {
  m_cusspin = Utils::create_table(1, 8);
  gtk_container_set_border_width(GTK_CONTAINER(m_cusspin), 0);
  gtk_table_set_col_spacings(m_cusspin, 1);

  m_button_label = Utils::create_button();
  m_button_sub = Utils::create_button("◁");
  m_entry = Utils::create_entry();
  m_button_add = Utils::create_button("▷");

  gtk_table_attach(m_cusspin, GTK_WIDGET(m_button_label), 0, 4, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_cusspin, GTK_WIDGET(m_button_sub), 4, 5, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_cusspin, GTK_WIDGET(m_entry), 5, 7, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);
  gtk_table_attach(m_cusspin, GTK_WIDGET(m_button_add), 7, 8, 0, 1, (GtkAttachOptions)(GTK_FILL | GTK_EXPAND), GTK_SHRINK, 0, 0);

  gtk_widget_modify_bg(GTK_WIDGET(m_button_label), GTK_STATE_NORMAL, Utils::get_color("black"));
  GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(m_button_label), GTK_CAN_FOCUS);
  GTK_WIDGET_UNSET_FLAGS(GTK_WIDGET(m_entry), GTK_CAN_FOCUS);
  gtk_entry_set_editable(m_entry, FALSE);

  gtk_button_set_focus_on_click(m_button_sub, FALSE);
  gtk_button_set_focus_on_click(m_button_add, FALSE);

  g_signal_connect(m_button_sub, "clicked", G_CALLBACK(signal_button_clicked_sub), this);
  g_signal_connect(m_button_add, "clicked", G_CALLBACK(signal_button_clicked_add), this);

  if (m_item != NULL) {
    UpdateLabel();

    if (!m_item->val.empty()) {
      SetValue(m_item->val, m_item->status);
    }
  }

  return GTK_WIDGET(m_cusspin);
}

void CusSpin::SetItem(CusSpinItem* item) {
  m_item = item;
}

void CusSpin::SetValue(const string str, EKnobReturn flag) {
  if (!str.empty()) {
    gtk_entry_set_text(m_entry, str.c_str());
  }

  switch(flag) {
  case OK:
    gtk_button_set_label(m_button_sub, "◁");
    gtk_button_set_label(m_button_add, "▷");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_sub), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_add), TRUE);
    break;
  case MIN:
    gtk_button_set_label(m_button_sub, "◁");
    gtk_button_set_label(m_button_add, "▷");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_sub), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_add), TRUE);
    break;
  case MAX:
    gtk_button_set_label(m_button_sub, "◁");
    gtk_button_set_label(m_button_add, "▷");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_sub), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_add), FALSE);
    break;
  case ERROR:
    break;
  case PRESS:
    break;
  }

  if (m_item->name == "Threshold" ||  m_item->name == "Color Reject") {
    gtk_button_set_label(m_button_sub, "◁");
    gtk_button_set_label(m_button_add, "▷");
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_sub), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(m_button_add), FALSE);
  }
}

void CusSpin::UpdateLabel() {
  gtk_button_set_label(m_button_label, _(m_item->name.c_str()));
}
