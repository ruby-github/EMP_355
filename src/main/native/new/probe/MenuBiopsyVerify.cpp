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
  m_subMenuAngle = NULL;
}

MenuBiopsyVerify::~MenuBiopsyVerify() {
}

GtkWidget* MenuBiopsyVerify::Create() {
  GtkWidget* menuBar0;

  m_vboxBioVerify=gtk_vbox_new(false,0);

  m_labelBioBracketType= create_label("", 0, 0, g_lightGray, NULL);
  gtk_box_pack_start(GTK_BOX(m_vboxBioVerify),m_labelBioBracketType,false,false,0);

  menuBar0=gtk_menu_bar_new();
  gtk_widget_modify_bg(menuBar0,GTK_STATE_NORMAL, g_deep);//2016.10.10

  m_menuItemExit=gtk_menu_item_new_with_label("");
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemExit)), GTK_STATE_NORMAL, g_white);

  gtk_widget_set_usize(m_menuItemExit, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

  gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemExit);
  g_signal_connect(G_OBJECT(m_menuItemExit),"activate",G_CALLBACK(HandleMenuItemExitActivate),this);
  gtk_widget_show(m_menuItemExit);

  m_menuItemAngle=gtk_menu_item_new_with_label("");
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemAngle)), GTK_STATE_NORMAL, g_white);

  gtk_widget_set_usize(m_menuItemAngle, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

  gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemAngle);
  g_signal_connect(G_OBJECT(m_menuItemAngle),"button-release-event",G_CALLBACK(HandleMenuItemAngleButtonRelease),this);
  gtk_widget_show(m_menuItemAngle);

  m_menuItemSave=gtk_menu_item_new_with_label("");
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemSave)), GTK_STATE_NORMAL, g_white);
  gtk_widget_set_usize(m_menuItemSave, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

  gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemSave);
  g_signal_connect(G_OBJECT(m_menuItemSave),"button-release-event",G_CALLBACK(HandleMenuItemSaveButtonRelease),this);

  gtk_widget_show(m_menuItemSave);

  m_menuItemLoadFactory=gtk_menu_item_new_with_label("");
  gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(m_menuItemLoadFactory)), GTK_STATE_NORMAL, g_white);

  gtk_widget_set_usize(m_menuItemLoadFactory, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);
  gtk_menu_bar_append(GTK_MENU_BAR(menuBar0),m_menuItemLoadFactory);
  g_signal_connect(G_OBJECT(m_menuItemLoadFactory),"button-release-event",G_CALLBACK(HandleMenuItemLoadFactoryButtonRelease),this);
  gtk_widget_show(m_menuItemLoadFactory);

  gtk_box_pack_start(GTK_BOX(m_vboxBioVerify),menuBar0,false,false,0);
  gtk_menu_bar_set_pack_direction(GTK_MENU_BAR(menuBar0),GTK_PACK_DIRECTION_TTB);

  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemSave), _("Save"));
  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemLoadFactory), _("Default Factory"));
  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemExit), _("Exit"));

  return m_vboxBioVerify;
}

void MenuBiopsyVerify::Show() {
  g_menuBiopsy.SetDrawStatus(true);
  UpdateBioBracketType();
  UpdateAngleMenuItem();

  UpdateSubMenuAngle();

  gtk_widget_show_all(m_vboxBioVerify);

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
  gtk_widget_hide_all(m_vboxBioVerify);
  BiopsyLine::GetInstance()->Clear();
  g_menuBiopsy.SetDrawStatus(false);
}

// ---------------------------------------------------------

void MenuBiopsyVerify::MenuItemActivateAngle(GtkMenuItem* menuitem) {
  string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));

  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

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
  if(m_subMenuAngle!=NULL) {
    gtk_widget_destroy(m_subMenuAngle);
  }

  m_vecMenuItem.clear();

  m_subMenuAngle=gtk_menu_new();
  gtk_widget_modify_bg(m_subMenuAngle,GTK_STATE_NORMAL, g_deep);
  gtk_widget_set_usize(m_subMenuAngle, WIDTH_BIOPSYVERIFY_MENU, -1);

  m_vecWidgetIndex.clear();
  m_vecAngleType.clear();

  vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();
  m_vecAngleType=vecAngleType;
  int size=vecAngleType.size();

  if (size == 1) {
    gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _(vecAngleType[0].c_str()));
  } else {
    if (size > 1) {
      for(int i = 0; i < size; i++) {
        GtkWidget* menuItem=gtk_check_menu_item_new_with_label(_(vecAngleType[i].c_str()));

        gtk_widget_modify_base(menuItem,GTK_STATE_NORMAL, g_deepGray);
        gtk_widget_modify_fg(gtk_bin_get_child(GTK_BIN(menuItem)), GTK_STATE_NORMAL, g_white);

        gtk_widget_set_usize(menuItem, WIDTH_BIOPSYVERIFY_MENU, HEIGHT_BIOPSYVERIFY_MENUITEM);

        StructWidgetIndex* pWidgetIndex0=new StructWidgetIndex;
        pWidgetIndex0->pData=this;
        pWidgetIndex0->index=i;
        m_vecWidgetIndex.push_back(pWidgetIndex0);

        g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(HandleSubMenuItemAngleButtonRelease),m_vecWidgetIndex[i]);
        gtk_menu_shell_append(GTK_MENU_SHELL(m_subMenuAngle),menuItem);
        m_vecMenuItem.push_back(menuItem);
      }

      gtk_menu_item_set_submenu(GTK_MENU_ITEM(m_menuItemAngle),m_subMenuAngle);

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
  gtk_label_set_text(GTK_LABEL(m_labelBioBracketType), _(bracketType.c_str()));
}

void MenuBiopsyVerify::UpdateAngleMenuItem() {
  string angleType;
  if(BiopsyMan::GetInstance()->IsBioBracketChanged()) {
    angleType=BiopsyMan::GetInstance()->GetDefaultAngleTypeOfCurBioBracket();
  } else {
    angleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
  }

  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle), _(angleType.c_str()));

  BiopsyMan::GetInstance()->SetCurBioAngleType(angleType);
}

void MenuBiopsyVerify::SubMenuItemAngleButtonRelease(GtkMenuItem* menuitem, int index) {
  for(int i=0; i<m_vecMenuItem.size(); i++) {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),false);
  }

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem),true);

  string angleType=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
  gtk_menu_item_set_label(GTK_MENU_ITEM(m_menuItemAngle),gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem)));

  BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index]);
  BiopsyLine::GetInstance()->AngleSwitch();
}
