#ifndef __MENU_BDMK_H__
#define __MENU_BDMK_H__

#include "utils/Utils.h"

class MenuBDMK {
public:
  MenuBDMK();
  ~MenuBDMK();

public:
  GtkWidget* Create();

  void Hide();
  void Show();

private:
  // signal

  static void signal_combobox_changed(GtkComboBox* combobox, MenuBDMK* data) {
    if (data != NULL) {
      data->ComboBoxChanged(combobox);
    }
  }

  static void signal_button_clicked_bdmk(GtkButton* button, MenuBDMK* data) {
    if (data != NULL) {
      data->ButtonClickedBDMK(button);
    }
  }

  static void signal_button_clicked_left(GtkButton* button, MenuBDMK* data) {
    if (data != NULL) {
      data->ButtonClickedLeft(button);
    }
  }

  static void signal_button_clicked_right(GtkButton* button, MenuBDMK* data) {
    if (data != NULL) {
      data->ButtonClickedRight(button);
    }
  }

  // signal

  void ComboBoxChanged(GtkComboBox* combobox);
  void ButtonClickedBDMK(GtkButton* button);
  void ButtonClickedLeft(GtkButton* button);
  void ButtonClickedRight(GtkButton*button);

private:
  static bool Sort(const string s1, const string s2);

private:
  string GetBdmkPath(const string item);

  void ChangeBDMKStudy(guchar study);
  void SetPageValue(guint page, guint total_page);
  void HideAllButton();
  void UpdateImage(const string path, int page);

private:
  static const int MAX_IMAGE = 10;
  static const int MAX_ROWS = 5;

private:
  GtkTable* m_table;
  GtkComboBoxText* m_combobox_text;
  GtkButton* m_button_images[MAX_IMAGE];
  GtkButton* m_button_left;
  GtkButton* m_button_right;
  GtkLabel* m_label_page;

  gint m_numTotal;
  gint m_pageCur;
  gint m_study;
  GDir* m_dir;
  GdkPixbuf* m_pixbufSel;

  vector<string> m_vecName;
};

extern MenuBDMK g_menuBDMK;

#endif
