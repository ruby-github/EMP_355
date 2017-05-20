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
  static void HandleMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
    data->MenuItemActivateAngle(menuitem);
  }

  static void HandleMenuItemExitActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
    data-> MenuItemActivateExit(menuitem);
  }

  static void HandleMenuItemSaveActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
    data->MenuItemActivateSave(menuitem);
  }

  static void HandleMenuItemLoadFactoryActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
    data-> MenuItemActivateLoadFactory(menuitem);
  }

  static gboolean HandleMenuItemSaveButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
    ((MenuBiopsyVerify*)user_data)->MenuItemActivateSave(GTK_MENU_ITEM(widget));
    return TRUE;
  }

  static gboolean HandleMenuItemLoadFactoryButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
    ((MenuBiopsyVerify*)user_data)->MenuItemActivateLoadFactory(GTK_MENU_ITEM(widget));
    return TRUE;
  }

  static gboolean HandleMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
    return TRUE;
  }

  static gboolean HandleSubMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
    (((StructWidgetIndex*)user_data)->pData)->SubMenuItemAngleButtonRelease(GTK_MENU_ITEM(widget), ((StructWidgetIndex*)user_data)->index);
    return TRUE;
  }

  // signal
  void MenuItemActivateAngle(GtkMenuItem* menuitem);
  void MenuItemActivateExit(GtkMenuItem* menuitem);
  void MenuItemActivateSave(GtkMenuItem* menuitem);
  void MenuItemActivateLoadFactory(GtkMenuItem* menuitem);
  void SubMenuItemAngleButtonRelease(GtkMenuItem* menuitem, int index);

private:
    GtkWidget *m_vboxBioVerify;
    GtkWidget *m_labelBioBracketType;

    GtkWidget *m_menuItemAngle;
    GtkWidget *m_menuItemSave;
    GtkWidget *m_menuItemLoadFactory;
    GtkWidget *m_menuItemExit;

    GtkWidget*m_subMenuAngle;
    vector<GtkWidget*> m_vecMenuItem;

    vector<string> m_vecAngleType;
    vector<StructWidgetIndex*> m_vecWidgetIndex;
};

extern MenuBiopsyVerify g_menuBiopsyVerify;

#endif
