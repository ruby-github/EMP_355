#include "probe/MenuBiopsyBracket.h"

#include "utils/FakeXUtils.h"

#include "display/HintArea.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"
#include "probe/MenuBiopsy.h"

MenuBiopsyBracket g_menuBiopsyBracket;

// ---------------------------------------------------------

MenuBiopsyBracket::MenuBiopsyBracket() {
  m_box = NULL;
  m_menubar = NULL;
}

MenuBiopsyBracket::~MenuBiopsyBracket() {
}

GtkWidget* MenuBiopsyBracket::Create() {
  m_box = Utils::create_vbox();

  m_menubar = Utils::create_menu_bar();

  gtk_box_pack_start(m_box, GTK_WIDGET(m_menubar), FALSE, FALSE, 0);

  GtkMenuItem* menuitem_exit = Utils::create_menu_item(_("Exit"));
  gtk_menu_bar_append(m_menubar, GTK_WIDGET(menuitem_exit));
  g_signal_connect(G_OBJECT(menuitem_exit), "activate", G_CALLBACK(signal_menuitem_activate_exit), this);

  gtk_menu_bar_set_pack_direction(m_menubar, GTK_PACK_DIRECTION_TTB);

  return GTK_WIDGET(m_box);
}

void MenuBiopsyBracket::Show() {
  g_menuBiopsy.SetDrawStatus(true);

  UpdateMenuBiopsyBracket();
  gtk_widget_show_all(GTK_WIDGET(m_box));

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

  BiopsyLine::GetInstance()->Draw();
}

void MenuBiopsyBracket::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_box));
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
    GtkMenuItem* menuItemType=Utils::create_menu_item(vecBracket[i]);

    gtk_menu_bar_append(m_menubar, GTK_WIDGET(menuItemType));
    g_signal_connect(G_OBJECT(menuItemType), "activate", G_CALLBACK(signal_menuitem_activate_type), this);

    m_vecMenuItem.push_back(GTK_WIDGET(menuItemType));
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
