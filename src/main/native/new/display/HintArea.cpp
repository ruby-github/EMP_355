#include "display/HintArea.h"

HintArea* HintArea::m_instance = NULL;

// ---------------------------------------------------------

HintArea* HintArea::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new HintArea();
  }

  return m_instance;
}

HintArea::HintArea() {
  m_label_hint = NULL;
}

HintArea::~HintArea() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* HintArea::Create() {
  m_label_hint = Utils::create_label("");

  return GTK_WIDGET(m_label_hint);
}

void HintArea::UpdateHint(const string text, int timeout) {
  gtk_label_set_label(m_label_hint, text.c_str());

  if (timeout > 0) {
    g_timeout_add_seconds(timeout, signal_callback_clearhint, this);
  }
}

void HintArea::ClearHint() {
  gtk_label_set_label(m_label_hint, "");
}
