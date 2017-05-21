#ifndef __MENU_BIOPSY_VERIFY_H__
#define __MENU_BIOPSY_VERIFY_H__

#include "utils/Utils.h"

class MenuBiopsyVerify {
public:
  MenuBiopsyVerify();
  ~MenuBiopsyVerify();

public:
  GtkWidget* Create();

  void Show();
  void Hide();
  void UpdateSubMenuAngle();
  void UpdateBioBracketType();
  void UpdateAngleMenuItem();

private:
  struct StructWidgetIndex {
    MenuBiopsyVerify* pData;
    int index;
  };

private:
  // signal

  static void signal_menuitem_activate_exit(GtkMenuItem* menuitem, MenuBiopsyVerify* data) {
    if (data != NULL) {
      data->MenuItemActivateExit(menuitem);
    }
  }

  static gboolean signal_menuitem_release_save(GtkWidget* widget, GdkEvent* event, gpointer data) {
    MenuBiopsyVerify* menu = (MenuBiopsyVerify*)data;

    if (data != NULL) {
      menu->MenuItemActivateSave(GTK_MENU_ITEM(widget));
    }

    return TRUE;
  }

  static gboolean signal_menuitem_release_loadfactory(GtkWidget* widget, GdkEvent* event, gpointer data) {
    MenuBiopsyVerify* menu = (MenuBiopsyVerify*)data;

    if (data != NULL) {
      menu->MenuItemActivateLoadFactory(GTK_MENU_ITEM(widget));
    }

    return TRUE;
  }

  static gboolean signal_menuitem_release_angle(GtkWidget* widget, GdkEvent* event, gpointer data) {
    return TRUE;
  }

  static gboolean signal_submenuitem_release_angle(GtkWidget* widget, GdkEvent* event, gpointer data) {
    StructWidgetIndex* index = (StructWidgetIndex*)data;

    if (index != NULL) {
      index->pData->SubMenuItemButtonReleaseAngle(GTK_MENU_ITEM(widget), index->index);
    }

    return TRUE;
  }

  // signal

  void MenuItemActivateAngle(GtkMenuItem* menuitem);
  void MenuItemActivateExit(GtkMenuItem* menuitem);
  void MenuItemActivateSave(GtkMenuItem* menuitem);
  void MenuItemActivateLoadFactory(GtkMenuItem* menuitem);
  void SubMenuItemButtonReleaseAngle(GtkMenuItem* menuitem, int index);

private:
  GtkBox* m_box;
  GtkLabel* m_label;
  GtkMenuItem* m_menuitem_angle;
  GtkWidget* m_subMenuAngle;

  vector<GtkWidget*> m_vecMenuItem;
  vector<string> m_vecAngleType;
  vector<StructWidgetIndex*> m_vecWidgetIndex;
};

extern MenuBiopsyVerify g_menuBiopsyVerify;

#endif
