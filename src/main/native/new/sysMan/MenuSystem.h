#ifndef __MENU_SYSTEM_H__
#define __MENU_SYSTEM_H__

#include "utils/Utils.h"

class MenuSystem {
public:
  static MenuSystem* GetInstance();

public:
  ~MenuSystem();

  GtkWidget* Create();

  void Show();
  void Hide();

private:
  // signal

  static void signal_button_clicked_general(GtkButton* button, MenuSystem* data) {
    if (data != NULL) {
      data->ButtonClickedGeneral(button);
    }
  }

  static void signal_button_clicked_biopsy(GtkButton* button, MenuSystem* data) {
    if (data != NULL) {
      data->ButtonClickedBiopsy(button);
    }
  }

  // signal

  void ButtonClickedGeneral(GtkButton* button);
  void ButtonClickedBiopsy(GtkButton* button);

public:
  MenuSystem();

private:
  static MenuSystem* m_instance;

private:
  GtkTable* m_table;
};

#endif
