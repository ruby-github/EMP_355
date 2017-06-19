#ifndef __HINT_AREA_H__
#define __HINT_AREA_H__

#include "utils/Utils.h"

class HintArea {
public:
  static HintArea* GetInstance();

public:
  ~HintArea();

  GtkWidget* Create();

  void UpdateHint(const string text, int timeout = 0);
  void ClearHint();

public:

private:
  // signal

  static gboolean signal_callback_clearhint(gpointer data) {
    HintArea* hintArea = (HintArea*)data;

    if (hintArea != NULL) {
      hintArea->ClearHint();
    }

    return FALSE;
  }

private:
  HintArea();

private:
  static HintArea* m_instance;

private:
  GtkLabel* m_label_hint;
};

#endif
