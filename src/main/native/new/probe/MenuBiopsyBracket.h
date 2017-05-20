#ifndef __MENU_BIOPSY_BRACKET_H__
#define __MENU_BIOPSY_BRACKET_H__

#include "utils/Utils.h"

class MenuBiopsyBracket {
public:
  MenuBiopsyBracket();
  ~MenuBiopsyBracket();

public:
  GtkWidget* Create();

  void Show();
  void Hide();
  void UpdateMenuBiopsyBracket();

private:
  // signal

  static void signal_menuitem_activate_type(GtkMenuItem* menuitem, MenuBiopsyBracket* data) {
    if (data != NULL) {
      data->MenuItemActivateType(menuitem);
    }
  }
  static void signal_menuitem_activate_exit(GtkMenuItem* menuitem, MenuBiopsyBracket* data) {
    if (data != NULL) {
      data->MenuItemActivateExit(menuitem);
    }
  }

  // signal
  void MenuItemActivateType(GtkMenuItem* menuitem);
  void MenuItemActivateExit(GtkMenuItem* menuitem);

private:
  GtkWidget* m_vboxBioBracketType;
  GtkWidget* m_menuBar;

  vector<GtkWidget*> m_vecMenuItem;
};

extern MenuBiopsyBracket g_menuBiopsyBracket;

#endif
