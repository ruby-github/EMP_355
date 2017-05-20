#ifndef __MENU_BIOPSY_VERIFY_H__
#define __MENU_BIOPSY_VERIFY_H__

#include "utils/Utils.h"

class MenuBiopsyVerify {
public:
    struct StructWidgetIndex {
        MenuBiopsyVerify* pData;
        int index;
    };

    MenuBiopsyVerify();
    ~MenuBiopsyVerify() {}
    GtkWidget * Create(void);
    void UpdateLabel(void);
    void Show(void);
    void Hide(void);
    void UpdateSubMenuAngle(void);
    void UpdateBioBracketType(void);
    void UpdateAngleMenuItem(void);

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

    //signal handle
    void MenuItemAngleActivate(GtkMenuItem *menuitem);
    void MenuItemExitActivate(GtkMenuItem *menuitem);
    void MenuItemSaveActivate(GtkMenuItem *menuitem);
    void MenuItemLoadFactoryActivate(GtkMenuItem *menuitem);
    void SubMenuItemAngleButtonRelease(GtkMenuItem *menuitem,int index);

    //signal connect
    static void HandleMenuItemAngleActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
        data->MenuItemAngleActivate(menuitem);
    }
    static void HandleMenuItemExitActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
        data-> MenuItemExitActivate(menuitem);
    }
    static void HandleMenuItemSaveActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
        data->MenuItemSaveActivate(menuitem);
    }
    static void HandleMenuItemLoadFactoryActivate(GtkMenuItem *menuitem, MenuBiopsyVerify *data) {
        data-> MenuItemLoadFactoryActivate(menuitem);
    }
    static gboolean HandleMenuItemSaveButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        ((MenuBiopsyVerify*)user_data)->MenuItemSaveActivate(GTK_MENU_ITEM(widget));
        return TRUE;

    }
    static gboolean HandleMenuItemLoadFactoryButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        ((MenuBiopsyVerify*)user_data)->MenuItemLoadFactoryActivate(GTK_MENU_ITEM(widget));
        return TRUE;
    }
    static gboolean HandleMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        return TRUE;
    }

    static gboolean HandleSubMenuItemAngleButtonRelease(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
        (((StructWidgetIndex*)user_data)->pData)->SubMenuItemAngleButtonRelease(GTK_MENU_ITEM(widget), ((StructWidgetIndex*)user_data)->index);
        return TRUE;

    }

};

extern MenuBiopsyVerify g_menuBiopsyVerify;
#endif
