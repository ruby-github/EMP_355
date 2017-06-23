#ifndef __MENU_AREA_H__
#define __MENU_AREA_H__

#include "utils/Utils.h"

class MenuArea {
public:
  static MenuArea* GetInstance();

public:
  ~MenuArea();

  enum EMenuType {
    MAIN, D2, M, PW, CW, CFM, PWCFM, CWCFM,
    MEASURE, BDMK, REVIEW, CALC, NOTE,
    BIOPSY, BIOPSYBRACKET, BIOPSYVERIFY, EFOV,
    SYSTEM
  };

  GtkWidget* Create(const int width, const int height);

  EMenuType GetMenuType();
  int GetMenuIndex();
  bool GetMenuVisible();

  void ShowMenu();
  void HideMenu();

  void ShowMainMenu();
  void Show2DMenu();
  void ShowMMenu();
  void ShowPWMenu();
  void ShowCWMenu();
  void ShowCFMMenu();
  void ShowPWCFMMenu(bool currentPw);
  void ShowCWCFMMenu(bool currentCw);

  void ShowMeasureMenu();
  void ShowBDMKMenu();
  void ShowReviewMenu();
  void ShowCalcMenu();
  void ShowNoteMenu();
  void ShowBiopsyMenu();
  void ShowBioBracketMenu();
  void ShowBioVerifyMenu();
  void ShowEFOVPrepare();
  void ShowEFOVCapture();
  void ShowEFOVView();
  void ShowEFOVReview();
  void ShowSystemMenu();

  void SwitchMenu(EMenuType type);
  void UpdateLabel();

private:
  // signal

  static void signal_notebook_changed(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num, MenuArea* data) {
    if (data != NULL) {
      data->NotebookChanged(notebook, page, page_num);
    }
  }

  void NotebookChanged(GtkNotebook* notebook, GtkNotebookPage* page, guint page_num);

private:
  MenuArea();

  void HideAllMenu();

private:
  static MenuArea* m_instance;

private:
  GtkBox* m_vbox;
  GtkLabel* m_label_sub;
  GtkNotebook* m_notebook;

  EMenuType m_menuType;
  bool m_2d_only;
  bool m_menuVisbible;
};

extern void MenuShowUndo();

#endif
