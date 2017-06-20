#include "probe/MenuBiopsyVerify.h"

#include "display/HintArea.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"
#include "probe/BiopsyLine.h"
#include "probe/MenuBiopsy.h"

#define WIDTH_BIOPSYVERIFY_MENU       175
#define HEIGHT_BIOPSYVERIFY_MENU      635
#define HEIGHT_BIOPSYVERIFY_MENUITEM  25

MenuBiopsyVerify g_menuBiopsyVerify;

// ---------------------------------------------------------

MenuBiopsyVerify::MenuBiopsyVerify() {
  m_box = NULL;
  m_label = NULL;
  m_menuitem_angle = NULL;
  m_subMenuAngle = NULL;
}

MenuBiopsyVerify::~MenuBiopsyVerify() {
}

GtkWidget* MenuBiopsyVerify::Create() {
  m_box = Utils::create_vbox();

  m_label = Utils::create_label();
  GtkMenuBar* menubar = Utils::create_menu_bar();

  gtk_box_pack_start(m_box, GTK_WIDGET(m_label), FALSE, FALSE, 0);
  gtk_box_pack_start(m_box, GTK_WIDGET(menubar), FALSE, FALSE, 0);

  GtkMenuItem* menuitem_exit = Utils::create_menu_item(_("Exit"));
  gtk_menu_bar_append(menubar, GTK_WIDGET(menuitem_exit));
  g_signal_connect(G_OBJECT(menuitem_exit), "activate", G_CALLBACK(signal_menuitem_activate_exit), this);

  m_menuitem_angle = Utils::create_menu_item("");
  gtk_menu_bar_append(menubar, GTK_WIDGET(m_menuitem_angle));
  g_signal_connect(G_OBJECT(m_menuitem_angle), "button-release-event", G_CALLBACK(signal_menuitem_release_angle), this);

  GtkMenuItem* menuitem_save = Utils::create_menu_item(_("Save"));
  gtk_menu_bar_append(menubar, GTK_WIDGET(menuitem_save));
  g_signal_connect(G_OBJECT(menuitem_save), "button-release-event", G_CALLBACK(signal_menuitem_release_save), this);

  GtkMenuItem* menuitem_loadfactory = Utils::create_menu_item(_("Default Factory"));
  gtk_menu_bar_append(menubar, GTK_WIDGET(menuitem_loadfactory));
  g_signal_connect(G_OBJECT(menuitem_loadfactory), "button-release-event", G_CALLBACK(signal_menuitem_release_loadfactory), this);

  gtk_menu_bar_set_pack_direction(menubar, GTK_PACK_DIRECTION_TTB);

  return GTK_WIDGET(m_box);
}

void MenuBiopsyVerify::Show() {
  g_menuBiopsy.SetDrawStatus(true);
  UpdateBioBracketType();
  UpdateAngleMenuItem();

  UpdateSubMenuAngle();

  gtk_widget_show_all(GTK_WIDGET(m_box));

  g_menuBiopsy.SetDrawStatus(true);

  BiopsyLine::GetInstance()->Create();
  ModeStatus ms;
  if (((ms.IsD2Mode() && (ms.GetFormat2D() == Format2D::B))||(ms.IsCFMMode()&&(ms.GetFormatCfm()==FormatCfm::B))||(ms.IsPDIMode()&&(ms.GetFormatCfm()==FormatCfm::B)))&& ms.IsUnFreezeMode()) {
    MultiFuncFactory::GetInstance()->Create(MultiFuncFactory::BIOPSY_VERIFY);
  } else {
    HintArea::GetInstance()->UpdateHint(_("[Verify]: Only valid in B mode,CFM,PDI and UnFreeze status."), 1);
  }
}

void MenuBiopsyVerify::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_box));
  BiopsyLine::GetInstance()->Clear();
  g_menuBiopsy.SetDrawStatus(false);
}

// ---------------------------------------------------------

void MenuBiopsyVerify::MenuItemActivateAngle(GtkMenuItem* menuitem) {
  string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuitem_angle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

  BiopsyMan::GetInstance()->SetCurBioAngleType(angleType);
  g_menuBiopsy.UpdateAngleMenuItem();
  BiopsyLine::GetInstance()->AngleSwitch();
}

void MenuBiopsyVerify::MenuItemActivateExit(GtkMenuItem* menuitem) {
  MenuArea::GetInstance()->ShowBiopsyMenu();
}

void  MenuBiopsyVerify::MenuItemActivateSave(GtkMenuItem* menuitem) {
  BiopsyLine::GetInstance()->WritePara();
}

void  MenuBiopsyVerify::MenuItemActivateLoadFactory(GtkMenuItem* menuitem) {
  BiopsyLine::GetInstance()->SetDefault();
}

void MenuBiopsyVerify::UpdateSubMenuAngle() {
  if(m_menuitem_angle != NULL) {
    gtk_widget_destroy(GTK_WIDGET(m_menuitem_angle));
  }

  m_vecMenuItem.clear();

  m_menuitem_angle = Utils::create_menu_item("");

  for (int i = 0; i < m_vecWidgetIndex.size(); i++) {
    delete m_vecWidgetIndex[i];
  }

  m_vecWidgetIndex.clear();
  m_vecAngleType.clear();

  vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();
  m_vecAngleType=vecAngleType;
  int size=vecAngleType.size();

  if (size == 1) {
    gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuitem_angle), _(vecAngleType[0].c_str()));
  } else {
    if (size > 1) {
      for(int i = 0; i < size; i++) {
        GtkWidget* menuItem=gtk_check_menu_item_new_with_label(_(vecAngleType[i].c_str()));

        gtk_widget_modify_base(menuItem,GTK_STATE_NORMAL, Utils::get_color("gray"));
        gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItem)), GTK_STATE_NORMAL, Utils::get_color("white"));

        gtk_widget_set_usize(menuItem, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

        StructWidgetIndex* pWidgetIndex0=new StructWidgetIndex;
        pWidgetIndex0->pData=this;
        pWidgetIndex0->index=i;
        m_vecWidgetIndex.push_back(pWidgetIndex0);

        g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(signal_submenuitem_release_angle), m_vecWidgetIndex[i]);
        gtk_menu_shell_append(GTK_MENU_SHELL(m_subMenuAngle), menuItem);
        m_vecMenuItem.push_back(menuItem);
      }

      gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menuitem_angle), m_subMenuAngle);

      string curAngleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
      for(int i = 0; i < size; i++) {
        if(vecAngleType[i].compare(curAngleType)==0) {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),true);
          break;
        }
      }
    }
  }
}

void MenuBiopsyVerify::UpdateBioBracketType() {
  string bracketType=BiopsyMan::GetInstance()->GetCurBioBracketType();
  gtk_label_set_text(GTK_LABEL(m_label), _(bracketType.c_str()));
}

void MenuBiopsyVerify::UpdateAngleMenuItem() {
  string angleType;
  if(BiopsyMan::GetInstance()->IsBioBracketChanged()) {
    angleType=BiopsyMan::GetInstance()->GetDefaultAngleTypeOfCurBioBracket();
  } else {
    angleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
  }

  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuitem_angle), _(angleType.c_str()));

  BiopsyMan::GetInstance()->SetCurBioAngleType(angleType);
}

void MenuBiopsyVerify::SubMenuItemButtonReleaseAngle(GtkMenuItem* menuitem, int index) {
  for(int i=0; i<m_vecMenuItem.size(); i++) {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),false);
  }

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem),true);

  string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuitem_angle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

  BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index]);
  BiopsyLine::GetInstance()->AngleSwitch();
}
