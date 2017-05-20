#include "probe/MenuBiopsyBracket.h"

#include "utils/FakeXUtils.h"

#include "display/HintArea.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"
#include "probe/MenuBiopsy.h"

#define WIDTH_BIOPSYBRACKET_MENU      175
#define HEIGHT_BIOPSYBRACKET_MENU     635
#define HEIGHT_BIOPSYBRACKET_MENUITEM 25

MenuBiopsyBracket g_menuBiopsyBracket;

// ---------------------------------------------------------

MenuBiopsyBracket::MenuBiopsyBracket() {
}

MenuBiopsyBracket::~MenuBiopsyBracket() {
}

GtkWidget* MenuBiopsyBracket::Create() {
  GtkWidget* menuItemExit;

  m_vboxBioBracketType=gtk_vbox_new(false,0);

  m_menuBar=gtk_menu_bar_new();
  gtk_widget_modify_bg(m_menuBar,GTK_STATE_NORMAL, g_deep);//2016.10.10

  menuItemExit=gtk_menu_item_new_with_label("");

  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItemExit)), GTK_STATE_NORMAL, g_white);

  gtk_widget_set_usize(menuItemExit, WIDTH_BIOPSYBRACKET_MENU, HEIGHT_BIOPSYBRACKET_MENUITEM);

  gtk_menu_item_set_label(GTK_MENU_ITEM(menuItemExit), _("Exit"));

  gtk_menu_bar_append(GTK_MENU_BAR(m_menuBar),menuItemExit);
  g_signal_connect(G_OBJECT(menuItemExit),"activate",G_CALLBACK(signal_menuitem_activate_exit),this);

  gtk_widget_show(menuItemExit);

  gtk_box_pack_start(GTK_BOX(m_vboxBioBracketType),m_menuBar,false,false,0);
  gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(m_menuBar),GTK_PACK_DIRECTION_TTB);

  return m_vboxBioBracketType;
}

void MenuBiopsyBracket::Show() {
  g_menuBiopsy.SetDrawStatus(true);

  UpdateMenuBiopsyBracket();
  gtk_widget_show_all(m_vboxBioBracketType);

  SetSystemCursor(90,110);
  doBtnEvent(1, 1);
  doBtnEvent(1, 0);
  SetMenuBiopsyCursorYRange(110,160);

  ModeStatus ms;
  if (ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B) && ms.IsUnFreezeMode()) {
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Biopsy]: Only valid in B mode and UnFreeze status."), 1);
  }

  BiopsyLine::GetInstance()->Draw();//2016.11.01
}

void MenuBiopsyBracket::Hide() {
  gtk_widget_hide_all(m_vboxBioBracketType);
  BiopsyLine::GetInstance()->Clear();
  g_menuBiopsy.SetDrawStatus(false);
}

void MenuBiopsyBracket::UpdateMenuBiopsyBracket() {
  for(int i = 0; i < m_vecMenuItem.size(); i++) {
    gtk_widget_destroy(m_vecMenuItem[i]);
  }

  m_vecMenuItem.clear();

  vector<string> vecBracket = BiopsyMan::GetInstance()->GetBioBracketTypesOfCurProbe();

  for(int i = 0; i < vecBracket.size(); i++) {
    GtkWidget* menuItemType=gtk_menu_item_new_with_label(vecBracket[i].c_str());
    //gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItemType)), GTK_STATE_NORMAL, g_white);

    gtk_widget_set_usize(menuItemType, WIDTH_BIOPSYBRACKET_MENU, HEIGHT_BIOPSYBRACKET_MENUITEM);
    gtk_menu_bar_append(GTK_MENU_BAR(m_menuBar),menuItemType);
    g_signal_connect(G_OBJECT(menuItemType), "activate", G_CALLBACK(signal_menuitem_activate_type),this);

    m_vecMenuItem.push_back(menuItemType);
  }
}

// ---------------------------------------------------------

void MenuBiopsyBracket::MenuItemActivateType(GtkMenuItem* menuitem) {
  string bracketType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
  BiopsyMan::GetInstance()->SetCurBioBracketType(bracketType);
  g_menuBiopsy.UpdateBioBracketTypeLabel();
  MenuArea::GetInstance()->ShowBioVerifyMenu();
}

void MenuBiopsyBracket::MenuItemActivateExit(GtkMenuItem* menuitem) {
  MenuArea::GetInstance()->ShowBiopsyMenu();
}
