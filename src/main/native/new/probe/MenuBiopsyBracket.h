#ifndef __MENU_BIOPSY_BRACKET_H__
#define __MENU_BIOPSY_BRACKET_H__

#include "utils/Utils.h"

class MenuBiopsyBracket {
public:
    MenuBiopsyBracket();
    ~MenuBiopsyBracket() {}
    GtkWidget * Create(void);
    void UpdateLabel(void);
    void Show(void);
    void Hide(void);
    void UpdateMenuBiopsyBracket(void);
private:

    GtkWidget *m_vboxBioBracketType;
    GtkWidget *m_menuBar;

    vector<GtkWidget*> m_vecMenuItem;

    //signal handle
    void MenuItemTypeActivate(GtkMenuItem *menuitem);
    void MenuItemExitActivate(GtkMenuItem *menuitem);
    //signal connect
    static void HandleMenuItemTypeActivate(GtkMenuItem *menuitem, MenuBiopsyBracket *data) {
        data->MenuItemTypeActivate(menuitem);
    }
    static void HandleMenuItemExitActivate(GtkMenuItem *menuitem, MenuBiopsyBracket *data) {
        data-> MenuItemExitActivate(menuitem);
    }

};

extern MenuBiopsyBracket g_menuBiopsyBracket;
#endif
