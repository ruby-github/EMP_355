#ifndef __MENU_PW_H__
#define __MENU_PW_H__

#include "display/CusSpin.h"
#include "utils/Utils.h"

class MenuPW {
public:
  MenuPW();
  ~MenuPW();

public:
  GtkWidget* Create();

  void Show();
  void Hide();

  void UpdateLabel();

  void UpdateTimeMark(bool on);
  void UpdateAutoCalc(bool on);
  void UpdateAutoTrace(bool on);
  void UpdateTimeRes(const string str, EKnobReturn status);

  bool GetAutoTraceStatus();
  void Sensitive(bool on);

private:
  // signal

  static void signal_checkbutton_clicked_timemark(GtkToggleButton* togglebutton, MenuPW* data) {
    data->CheckButtonClickedTimeMark(togglebutton);
  }

  static void signal_checkbutton_clicked_autocalc(GtkToggleButton* togglebutton, MenuPW* data) {
    data->CheckButtonClickedAutoCalc(togglebutton);
  }

  static void signal_checkbutton_clicked_autotrace(GtkToggleButton* togglebutton, MenuPW* data) {
    data->CheckButtonClickedAutoTrace(togglebutton);
  }

  static void signal_combobox_changed_scale(GtkComboBox* combobox, MenuPW* data) {
    data->ComboBoxChangedScale(combobox);
  }

  static void signal_combobox_changed_tracetype(GtkComboBox* combobox, MenuPW* data) {
    data->ComboBoxChangedTraceType(combobox);
  }

  static void signal_combobox_changed_direction(GtkComboBox* combobox, MenuPW* data) {
    data->ComboBoxChangedDirection(combobox);
  }

  static void signal_combobox_changed_threshold(GtkComboBox* combobox, MenuPW* data) {
    data->ComboBoxChangedThreshold(combobox);
  }

  static void signal_combobox_changed_smooth(GtkComboBox* combobox, MenuPW* data) {
    data->ComboBoxChangedSmooth(combobox);
  }

  static void signal_button_clicked_display_1(GtkButton* button, MenuPW* data) {
    data->ButtonClickedDisplay1(button);
  }

  static void signal_button_clicked_display_2(GtkButton* button, MenuPW* data) {
    data->ButtonClickedDisplay2(button);
  }

  static void signal_button_clicked_display_3(GtkButton* button, MenuPW* data) {
    data->ButtonClickedDisplay3(button);
  }

  static void signal_button_clicked_display_4(GtkButton* button, MenuPW* data) {
    data->ButtonClickedDisplay4(button);
  }

  static void signal_button_clicked_display_5(GtkButton* button, MenuPW* data) {
    data->ButtonClickedDisplay5(button);
  }

  static void signal_callback_foreach_widget(gpointer element, MenuPW* data) {
    data->CallbackForeachWidget(GTK_WIDGET(element));
  }

  // signal

  void CheckButtonClickedAutoTrace(GtkToggleButton* togglebutton);
  void CheckButtonClickedTimeMark(GtkToggleButton* togglebutton);
  void CheckButtonClickedAutoCalc(GtkToggleButton* togglebutton);

  void ComboBoxChangedScale(GtkComboBox* combobox);
  void ComboBoxChangedTraceType(GtkComboBox* combobox);
  void ComboBoxChangedDirection(GtkComboBox* combobox);
  void ComboBoxChangedThreshold(GtkComboBox* combobox);
  void ComboBoxChangedSmooth(GtkComboBox* combobox);

  void ButtonClickedDisplay1(GtkButton* button);
  void ButtonClickedDisplay2(GtkButton* button);
  void ButtonClickedDisplay3(GtkButton* button);
  void ButtonClickedDisplay4(GtkButton* button);
  void ButtonClickedDisplay5(GtkButton* button);

  void CallbackForeachWidget(GtkWidget* widget);

private:
  void UpdateScale(GtkComboBox* combobox);
  void UpdateTraceType(GtkComboBox* combobox);
  void UpdateDirection(GtkComboBox* combobox);
  void UpdateThreshold(GtkComboBox* combobox);
  void UpdateSmooth(GtkComboBox* combobox);

  bool StatusChangeFormat();
  void WriteFormatPw(int format);

private:
  static CusSpin::CusSpinItem m_item_fastangle;
  static CusSpin::CusSpinItem m_item_timeres;

  static void ChgFastAngle(EKnobOper oper);
  static void ChgTimeRes(EKnobOper oper);

private:
  GtkTable* m_table;

  GtkCheckButton* m_checkbutton_timemark;
  GtkCheckButton* m_checkbutton_autocalc;
  GtkCheckButton* m_checkbutton_autotrace;
  GtkButton* m_button_scale;
  GtkComboBoxText* m_combobox_scale;
  GtkButton* m_button_tracetype;
  GtkComboBoxText* m_combobox_tracetype;
  GtkButton* m_button_direction;
  GtkComboBoxText* m_combobox_direction;
  GtkButton* m_button_threshold;
  GtkComboBoxText* m_combobox_threshold;
  GtkButton* m_button_smooth;
  GtkComboBoxText* m_combobox_smooth;
  GtkFrame* m_frame_display;

  CusSpin m_spin_fastangle;
  CusSpin m_spin_timeres;

  GtkWidget* m_fastangle;
  GtkWidget* m_timeres;

  bool m_sensitive;
};

extern MenuPW g_menuPW;
extern MenuPW g_menuCW;

#endif
