#ifndef __MENU_M_H__
#define __MENU_M_H__

#include "display/CusSpin.h"
#include "utils/Utils.h"

class MenuM {
public:
  MenuM();
  ~MenuM();

public:
  GtkWidget* Create();

  void Show();
  void Hide();

  void UpdateLabel();

  void UpdatePolarity(bool on);
  void UpdateTimeMark(bool on);
  void UpdateLine(const string str, EKnobReturn status);
  void UpdateEnhanceM(const string str, EKnobReturn status);
  void UpdateSmooth(const string str, EKnobReturn status);

  void Sensitive(bool on);

private:
  // signal

  static void signal_checkbutton_clicked_polarity(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->CheckButtonClickedPolarity(button);
    }
  }

  static void signal_checkbutton_clicked_timemark(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->CheckButtonClickedTimemark(button);
    }
  }

  static void signal_button_clicked_anatomic_m(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedAnatomic(button);
    }
  }

  static void signal_button_clicked_display_1(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedDisplay1(button);
    }
  }

  static void signal_button_clicked_display_2(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedDisplay2(button);
    }
  }

  static void signal_button_clicked_display_3(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedDisplay3(button);
    }
  }

  static void signal_button_clicked_display_4(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedDisplay4(button);
    }
  }

  static void signal_button_clicked_display_5(GtkButton* button, MenuM* data) {
    if (data != NULL) {
      data->ButtonClickedDisplay5(button);
    }
  }

  static void signal_callback_foreach_widget(gpointer element, MenuM* data) {
    if (data != NULL) {
      data->CallbackForeachWidget(GTK_WIDGET(element));
    }
  }

  // signal

  void CheckButtonClickedPolarity(GtkButton* button);
  void CheckButtonClickedTimemark(GtkButton* button);
  void ButtonClickedAnatomic(GtkButton* button);
  void ButtonClickedDisplay1(GtkButton* button);
  void ButtonClickedDisplay2(GtkButton* button);
  void ButtonClickedDisplay3(GtkButton* button);
  void ButtonClickedDisplay4(GtkButton* button);
  void ButtonClickedDisplay5(GtkButton* button);

  void CallbackForeachWidget(GtkWidget* widget);

private:
  void WriteFormatM(int format);

private:
  static CusSpin::CusSpinItem m_item_line;
  static CusSpin::CusSpinItem m_item_smooth;
  static CusSpin::CusSpinItem m_item_enhance;

  static void ChgLine(EKnobOper oper);
  static void ChgEnhance(EKnobOper oper);
  static void ChgSmooth(EKnobOper oper);

private:
  GtkTable* m_table;

  GtkCheckButton* m_checkbutton_polarity;
  GtkCheckButton* m_checkbutton_timemark;

  GtkButton* m_button_anatomic_m;
  GtkFrame* m_frame_display;

  CusSpin m_spin_line;
  CusSpin m_spin_enhance;
  CusSpin m_spin_smooth;

  bool m_sensitive;
  int m_format;
};

extern MenuM g_menuM;

#endif
