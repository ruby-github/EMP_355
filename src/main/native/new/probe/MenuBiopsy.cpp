#include "probe/MenuBiopsy.h"

#include "utils/FakeXUtils.h"

#include "display/HintArea.h"
#include "display/MenuArea.h"
#include "probe/BiopsyMan.h"

MenuBiopsy g_menuBiopsy;
bool MenuBiopsy::m_isDraw = false;
bool MenuBiopsy::m_isDoubleLine = false;

// ---------------------------------------------------------

bool MenuBiopsy::GetDrawStatus() {
  return m_isDraw;
}

bool MenuBiopsy::GetDoubleLineStatus() {
  return m_isDoubleLine;
}

MenuBiopsy::MenuBiopsy() {
  m_box = NULL;
  m_label = NULL;
  m_menuItemAngle = NULL;
  m_menuItemLineType = NULL;
  m_subMenuAngle = NULL;

  m_ptrImgDraw = ImageAreaDraw::GetInstance();
  m_ptrBiopsy = BiopsyLine::GetInstance();
}

MenuBiopsy::~MenuBiopsy() {
}

GtkWidget* MenuBiopsy::Create() {
  m_box = Utils::create_vbox();

  m_label = Utils::create_label();
  GtkMenuBar* menubar = Utils::create_menu_bar();

  gtk_box_pack_start(m_box, GTK_WIDGET(m_label), FALSE, FALSE, 0);
  gtk_box_pack_start(m_box, GTK_WIDGET(menubar), FALSE, FALSE, 0);

  m_menuItemAngle = Utils::create_menu_item("");
  gtk_menu_bar_append(menubar, GTK_WIDGET(m_menuItemAngle));
  g_signal_connect(G_OBJECT(m_menuItemAngle), "activate", G_CALLBACK(signal_menuitem_activate_angle), this);

  if (m_isDoubleLine) {
    m_menuItemLineType = Utils::create_menu_item(_("Line Type     Double"));
  } else {
    m_menuItemLineType = Utils::create_menu_item(_("Line Type     Single"));
  }

  gtk_menu_bar_append(menubar, GTK_WIDGET(m_menuItemLineType));
  g_signal_connect(G_OBJECT(m_menuItemLineType), "button-release-event", G_CALLBACK(signal_menuitem_release_linetype), this);

  GtkMenuItem* menuItemSetup = Utils::create_menu_item(_("Setting"));
  gtk_menu_bar_append(menubar, GTK_WIDGET(menuItemSetup));
  g_signal_connect(G_OBJECT(menuItemSetup), "activate", G_CALLBACK(signal_menuitem_activate_setup), this);

  gtk_menu_bar_set_pack_direction(menubar, GTK_PACK_DIRECTION_TTB);

  return GTK_WIDGET(m_box);
}

void MenuBiopsy::Show() {
  int bioBracketTypeNum=BiopsyMan::GetInstance()->GetBioBracketTypeNumOfCurProbe();

  if(bioBracketTypeNum <= 0) {
    HintArea::GetInstance()->UpdateHint(_("No biopsy bracket found!"), 2);
    MenuShowUndo();
  } else {
    SetDrawStatus(true);

    UpdateBioBracketTypeLabel();

    UpdateSubMenuAngle();

    gtk_widget_show_all(GTK_WIDGET(m_box));

    m_ptrBiopsy->Create();

    SetSystemCursor(90,130);
    doBtnEvent(1, 1);
    doBtnEvent(1, 0);
    SetMenuBiopsyCursorYRange(128,213);
  }
}

void MenuBiopsy::Hide() {
  gtk_widget_hide_all(GTK_WIDGET(m_box));

  m_ptrBiopsy->Clear();
  SetDrawStatus(false);
}

void MenuBiopsy::SetBiopsyBracketTypeLabel(string bioBracketType) {
  gtk_label_set_text(m_label, _(bioBracketType.c_str()));
}

void MenuBiopsy::SetAngleMenuItem(string strText) {
  gtk_menu_item_set_label(m_menuItemAngle, _(strText.c_str()));
}

void MenuBiopsy::UpdateSubMenuAngle() {
  if(m_subMenuAngle!=NULL) {
    gtk_widget_destroy(GTK_WIDGET(m_subMenuAngle));
  }

  m_vecMenuItem.clear();
  m_subMenuAngle=Utils::create_menu_item("");

  for (int i = 0; i < m_vecWidgetIndex.size(); i++) {
    delete m_vecWidgetIndex[i];
  }

  m_vecWidgetIndex.clear();
  m_vecAngleType.clear();

  vector<string> vecAngleType=BiopsyMan::GetInstance()->GetBioAngleTypesOfcurBioBracket();

  m_vecAngleType=vecAngleType;

  int size =vecAngleType.size();
  if(size == 1) {
    string angleOnOff;
    angleOnOff=vecAngleType[0];
    SetAngleMenuItem(angleOnOff);
  } else {
    if(size > 1) {
      for(int i = 0; i < size; i++) {
        GtkMenuItem* menuItem=Utils::create_menu_item(vecAngleType[i]);

        StructWidgetIndex* pWidgetIndex0=new StructWidgetIndex;
        pWidgetIndex0->pData=this;
        pWidgetIndex0->index=i;
        m_vecWidgetIndex.push_back(pWidgetIndex0);

        g_signal_connect(G_OBJECT(menuItem),"button-release-event",G_CALLBACK(signal_submenuitem_release_angle),m_vecWidgetIndex[i]);

        gtk_menu_shell_append(GTK_MENU_SHELL(m_subMenuAngle),GTK_WIDGET(menuItem));
        m_vecMenuItem.push_back(GTK_WIDGET(menuItem));
      }

      gtk_menu_item_set_submenu(m_menuItemAngle, GTK_WIDGET(m_subMenuAngle));

      if(m_isDraw) {
        string curAngleType=BiopsyMan::GetInstance()->GetCurBioAngleType();

        for(int i = 0; i < size; i++) {
          if(vecAngleType[i].compare(curAngleType)==0) {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]),true);
            break;
          }
        }

        SetAngleMenuItem(curAngleType);
      }
    }
  }
}

void MenuBiopsy::UpdateBioBracketTypeLabel() {
  string bracketType=BiopsyMan::GetInstance()->GetCurBioBracketType();
  SetBiopsyBracketTypeLabel(bracketType);
}

void MenuBiopsy::UpdateAngleMenuItem() {
  string angleType=BiopsyMan::GetInstance()->GetCurBioAngleType();
  SetAngleMenuItem(angleType);
  SetDrawStatus(true);
}

void MenuBiopsy::SetDrawStatus(bool status) {
  m_isDraw=status;
}

void MenuBiopsy::ClearBiopsyLine() {
  if(m_isDraw) {
    BiopsyLine::GetInstance()->Clear();
  }
}

void MenuBiopsy::UpdateBiopsyLine() {
  if(m_isDraw) {
    BiopsyLine::GetInstance()->Clear();
    bool bisInitOk=BiopsyLine::GetInstance()->InitPara();

    if(bisInitOk) {
      BiopsyLine::GetInstance()->Redraw();
    } else {
      CloseBiopsyLine();
    }
  }
}

void MenuBiopsy::CloseBiopsyLine() {
  m_isDraw=false;
  SetDrawStatus(false);
}

// ---------------------------------------------------------

void MenuBiopsy::MenuItemActivateAngle(GtkMenuItem* menuitem) {
  SetMenuBiopsyCursorYRange(128, 228);
}

void  MenuBiopsy::MenuItemActivateSetup(GtkMenuItem* menuitem ) {
  int bioBracketTypeNum=BiopsyMan::GetInstance()->GetBioBracketTypeNumOfCurProbe();

  if(bioBracketTypeNum <= 1) {
    MenuArea::GetInstance()->ShowBioVerifyMenu();
  } else {
    MenuArea::GetInstance()->ShowBioBracketMenu();
  }
}

void  MenuBiopsy::MenuItemActivateLineType(GtkMenuItem* menuitem) {
  if(m_isDoubleLine) {
    m_ptrBiopsy->ClearDoubleLine();
    m_isDoubleLine=false;
    gtk_menu_item_set_label(m_menuItemLineType, _("Line Type     Single"));
  } else {
    m_ptrBiopsy->ClearSingleLine();
    m_isDoubleLine=true;
    gtk_menu_item_set_label(m_menuItemLineType, _("Line Type     Double"));
  }

  if(m_isDraw) {
    m_ptrBiopsy->Draw();
  }
}

void MenuBiopsy::SubMenuItemButtonReleaseAngle(GtkMenuItem* menuitem, int index) {
  for(int i=0; i<m_vecMenuItem.size(); i++) {
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(m_vecMenuItem[i]), FALSE);
  }

  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(menuitem), TRUE);

  string text=gtk_menu_item_get_label(GTK_MENU_ITEM(menuitem));
  gtk_menu_item_set_label(m_menuItemAngle,text.c_str());

  BiopsyMan::GetInstance()->SetCurBioAngleType(m_vecAngleType[index]);
  m_ptrBiopsy->AngleSwitch();
}
