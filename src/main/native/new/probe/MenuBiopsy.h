#ifndef __MENU_BIOPSY_H__
#define __MENU_BIOPSY_H__

#include "utils/Utils.h"
#include "display/ImageAreaDraw.h"
#include "probe/BiopsyLine.h"

class MenuBiopsy {
public:
    struct StructWidgetIndex {
        MenuBiopsy* pData;
        int index;
    };
    MenuBiopsy();
    ~MenuBiopsy() {}
    void Hide();
    void Show();
    GtkWidget * Create();

    void UpdateLabel();

    void SetBiopsyBracketTypeLabel(string bioBracketType);
    void SetAngleMenuItem(string strText);

    void UpdateSubMenuAngle();
    void UpdateBioBracketTypeLabel();
    void UpdateAngleMenuItem();
    void SetDrawStatus(bool status);

    void ClearBiopsyLine();
    void UpdateBiopsyLine();
    static bool GetDrawStatus() {
        return m_isDraw;
    }
    static bool GetDoubleLineStatus() {
        return m_isDoubleLine;
    }

    void CloseBiopsyLine();

public:
    static  bool m_isDraw;
    static bool m_isDoubleLine;
private:
    BiopsyLine * m_ptrBiopsy;
    ImageAreaDraw *m_ptrImgDraw;

    GtkWidget *m_vboxBiopsy;
    GtkWidget *m_labelBioBracketType;

    GtkWidget *m_menuItemAngle;
    GtkWidget *m_menuItemLineType;
    GtkWidget *m_menuItemSetup;
    GtkWidget *m_menuItemExit;

    GtkWidget*m_subMenuAngle;
    vector<GtkWidget*> m_vecMenuItem;

    vector<string> m_vecAngleType;
    vector<StructWidgetIndex*> m_vecWidgetIndex;

    //signal handle
    void SubMenuItemAngleActivate(GtkMenuItem *menuitem);
    void MenuItemLineTypeActivate(GtkMenuItem *menuitem);
    void MenuItemSetupActivate(GtkMenuItem *menuitem);
    void MenuItemAngleSelect(GtkMenuItem *menuitem);
    void MenuItemAngleDeSelect(GtkMenuItem *menuitem);

    void MenuItemAngleActivate(GtkMenuItem *menuitem);
    void MenuItemAngleButtonRelease(GtkMenuItem *menuitem);
    void SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem,int index);

    //signal connect
    static void HandleSubMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->SubMenuItemAngleActivate(menuitem);
    }
    static void HandleMenuItemLineTypeActivate(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->MenuItemLineTypeActivate(menuitem);
    }
    static void HandleMenuItemSetupActivate(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->MenuItemSetupActivate(menuitem);
    }
    static gboolean HandleMenuItemLineTypeButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        ((MenuBiopsy*)user_data)->MenuItemLineTypeActivate(GTK_MENU_ITEM(widget));
        return TRUE;
    }
    static void HandleMenuItemAngleSelect(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->MenuItemAngleSelect(menuitem);
    }
    static void HandleMenuItemAngleDeSelect(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->MenuItemAngleDeSelect(menuitem);
    }
    static void HandleMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsy *data) {
        data->MenuItemAngleActivate(menuitem);
    }
    static gboolean HandleMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        ((MenuBiopsy*)user_data)->MenuItemAngleButtonRelease(GTK_MENU_ITEM(widget));
        return TRUE;
    }
    static gboolean HandleMenuItemAngleButtonPress(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        return TRUE;
    }

    static gboolean HandleSubMenuItemAngleButtonRelease(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
        (((StructWidgetIndex*)user_data)->pData)->SubMenuItemAngleButtonRelease(GTK_MENU_ITEM(widget), ((StructWidgetIndex*)user_data)->index);
        return TRUE;
    }

};

extern MenuBiopsy g_menuBiopsy;

#endif
