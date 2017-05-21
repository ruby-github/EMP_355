#ifndef __MENU_BIOPSY_H__
#define __MENU_BIOPSY_H__

#include "utils/Utils.h"
#include "display/ImageAreaDraw.h"
#include "probe/BiopsyLine.h"

class MenuBiopsy {
public:
  static bool GetDrawStatus();
  static bool GetDoubleLineStatus();

public:
  MenuBiopsy();
  ~MenuBiopsy();

public:
  GtkWidget* Create();

  void Show();
  void Hide();

  void SetBiopsyBracketTypeLabel(string bioBracketType);
  void SetAngleMenuItem(string strText);

  void UpdateSubMenuAngle();
  void UpdateBioBracketTypeLabel();
  void UpdateAngleMenuItem();
  void SetDrawStatus(bool status);

  void ClearBiopsyLine();
  void UpdateBiopsyLine();
  void CloseBiopsyLine();

private:
  struct StructWidgetIndex {
    MenuBiopsy* pData;
    int index;
  };

private:
  // signal

  static void signal_menuitem_activate_angle(GtkMenuItem* menuitem, MenuBiopsy* data) {
    if (data != NULL) {
      data->MenuItemActivateAngle(menuitem);
    }
  }

  static void signal_menuitem_activate_setup(GtkMenuItem* menuitem, MenuBiopsy* data) {
    if (data != NULL) {
      data->MenuItemActivateSetup(menuitem);
    }
  }

  static gboolean signal_menuitem_release_linetype(GtkWidget* widget, GdkEvent* event, gpointer data) {
    MenuBiopsy* menu = (MenuBiopsy*)data;

    if (menu != NULL) {
      menu->MenuItemActivateLineType(GTK_MENU_ITEM(widget));
    }

    return TRUE;
  }

  static gboolean signal_submenuitem_release_angle(GtkWidget* widget, GdkEvent* event, gpointer data) {
    StructWidgetIndex* menu = (StructWidgetIndex*)data;

    if (menu != NULL) {
      menu->pData->SubMenuItemButtonReleaseAngle(GTK_MENU_ITEM(widget), menu->index);
    }

    return TRUE;
  }

  // signal

  void MenuItemActivateAngle(GtkMenuItem* menuitem);
  void MenuItemActivateSetup(GtkMenuItem* menuitem);
  void MenuItemActivateLineType(GtkMenuItem* menuitem);
  void SubMenuItemButtonReleaseAngle(GtkMenuItem* menuitem, int index);

private:
  static bool m_isDraw;
  static bool m_isDoubleLine;

private:
  GtkBox* m_box;
  GtkLabel* m_label;
  GtkMenuItem* m_menuItemAngle;
  GtkMenuItem* m_menuItemLineType;
  GtkMenuItem* m_subMenuAngle;

  BiopsyLine* m_ptrBiopsy;
  ImageAreaDraw* m_ptrImgDraw;

  vector<GtkWidget*> m_vecMenuItem;
  vector<string> m_vecAngleType;
  vector<StructWidgetIndex*> m_vecWidgetIndex;
};

extern MenuBiopsy g_menuBiopsy;

#endif
