#ifndef __MENU_SYSTEM_H__
#define __MENU_SYSTEM_H__

#include "utils/Utils.h"

class MenuSystem {
public:
  MenuSystem();
  ~MenuSystem();

  GtkWidget* Create();

public:

private:
  // signal

  static void HandleBtnGeneral(GtkButton* button, MenuSystem* data) {
    if (data != NULL) {
      data->BtnGeneral(button);
    }
  }

  static void HandleBtnBiopsy(GtkButton* button, MenuSystem* data) {
    if (data != NULL) {
      data->BtnBiopsy(button);
    }
  }

  // signal

  void BtnGeneral(GtkButton* button);
  void BtnBiopsy(GtkButton* button);

private:

private:

private:

public:


    void UpdateLabel(void);
    void Show(void);
    void Hide(void);
private:
    GtkWidget *m_table;
    GtkWidget *m_labelGeneral;
    GtkWidget *m_labelBiopsy;

};

extern MenuSystem g_menuSystem;

#endif
