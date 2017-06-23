#ifndef __MENU_2D_H__
#define __MENU_2D_H__

#include "display/CusSpin.h"
#include "utils/Utils.h"

class Menu2D {
public:
  Menu2D();
  ~Menu2D();

  GtkWidget* Create();

  void Show();
  void Hide();

  void UpdateLabel();

  void UpdatePolarity(bool on);
  void UpdateNoiseReject(bool on);
  void UpdateRotate(const string str, EKnobReturn status);
  void UpdateFrame(const string str, EKnobReturn status);
  void UpdateLine(const string str, EKnobReturn status);
  void UpdateSmooth(const string str, EKnobReturn status);
  void UpdateGamma(const string str, EKnobReturn status);
  void UpdateIPurity(const string str, EKnobReturn status);

  void Sensitive(bool on);

  void ButtonClickedPIP(GtkButton* button);
  void ButtonClicked4B(GtkButton* button);

private:
  // signal

  static void signal_checkbutton_clicked_polarity(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->CheckButtonClickedPolarity(button);
    }
  }

  static void signal_checkbutton_clicked_restric(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->CheckButtonClickedNoiseReject(button);
    }
  }

  static void signal_button_clicked_trans(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClickedTrans(button);
    }
  }

  static void signal_button_clicked_reject(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClickedReject(button);
    }
  }

  static void signal_button_clicked_globalzoom(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClickedGlobalZoom(button);
    }
  }

  static void signal_button_clicked_pip(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClickedPIP(button);
    }
  }

  static void signal_button_clicked_4b(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClicked4B(button);
    }
  }

  static void signal_button_clicked_efov(GtkButton* button, Menu2D* data) {
    if (data != NULL) {
      data->ButtonClickedEnterEFOV(button);
    }
  }

  static void signal_callback_foreach_widget(gpointer element, Menu2D* data) {
    if (data != NULL) {
      data->CallbackForeachWidget(GTK_WIDGET(element));
    }
  }

  void CheckButtonClickedPolarity(GtkButton* button);
  void CheckButtonClickedNoiseReject(GtkButton* button);
  void ButtonClickedTrans(GtkButton* button);
  void ButtonClickedReject(GtkButton* button);
  void ButtonClickedGlobalZoom(GtkButton* button);
  void ButtonClickedEnterEFOV(GtkButton* button);

  void CallbackForeachWidget(GtkWidget* widget);

private:
  static CusSpin::CusSpinItem m_item_rotate;
  static CusSpin::CusSpinItem m_item_frame;
  static CusSpin::CusSpinItem m_item_line;
  static CusSpin::CusSpinItem m_item_smooth;
  static CusSpin::CusSpinItem m_item_gamma;
  static CusSpin::CusSpinItem m_item_ipurity;

  static void ChgRotate(EKnobOper oper);
  static void ChgFrame(EKnobOper oper);
  static void ChgLine(EKnobOper oper);
  static void ChgSmooth(EKnobOper oper);
  static void ChgGamma(EKnobOper oper);
  static void ChgIPurity(EKnobOper oper);

private:
  GtkTable* m_table;

  GtkCheckButton* m_checkbutton_polarity;
  GtkCheckButton* m_checkbutton_restric;

  GtkButton* m_button_gray_trans;
  GtkButton* m_button_gray_rejection;
  GtkButton* m_button_global_zoom;
  GtkButton* m_button_pip;
  GtkButton* m_button_4b;
  GtkButton* m_button_efov;

  CusSpin m_spin_rotate;
  CusSpin m_spin_frame;
  CusSpin m_spin_line;
  CusSpin m_spin_smooth;
  CusSpin m_spin_gamma;
  CusSpin m_spin_ipurity;

  bool m_sensitive;
};

extern Menu2D g_menu2D;

#endif
