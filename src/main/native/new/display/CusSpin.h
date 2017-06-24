#ifndef __CUS_SPIN__
#define __CUS_SPIN__

#include "utils/Utils.h"

class CusSpin {
public:
  CusSpin();
  ~CusSpin();

public:
  GtkWidget* Create();

  typedef void (*CusSpinFunc)(EKnobOper opr);

  struct CusSpinItem {
    string name;
    string val;
    EKnobReturn status;
    CusSpinFunc func;
  };

  void SetItem(CusSpinItem* item);
  void SetValue(const string str, EKnobReturn flag);

  void UpdateLabel();

private:
  // signal

  static void signal_button_clicked_sub(GtkButton* button, CusSpin* data) {
    if (data != NULL) {
      data->m_item->func(SUB);
    }
  }

  static void signal_button_clicked_add(GtkButton* button, CusSpin* data) {
    if (data != NULL) {
      data->m_item->func(ADD);
    }
  }

private:
  GtkTable* m_cusspin;
  GtkButton* m_button_label;
  GtkButton* m_button_sub;
  GtkEntry* m_entry;
  GtkButton* m_button_add;

  CusSpinItem* m_item;
};

#endif
