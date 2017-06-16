#ifndef __VIEW_ICON_H__
#define __VIEW_ICON_H__

#include "utils/Utils.h"

class ViewIcon {
public:
  static ViewIcon* GetInstance();

public:
  ~ViewIcon();

  GtkWidget* Create();

  void Network(bool on);
  void Sound(bool on);
  void Replay(bool on);
  void Udisk(bool on);
  void Cdrom(bool on);
  void Printer(bool on);

private:
  // signal

  static gboolean signal_callback_scroll_scanicon(gpointer data) {
    ViewIcon* viewicon = (ViewIcon*)data;

    if (viewicon != NULL) {
      viewicon->ScrollScanIcon();
    }

    return TRUE;
  }

  static gboolean signal_callback_updatecharge(gpointer data) {
    ViewIcon* viewicon = (ViewIcon*)data;

    if (viewicon != NULL) {
      viewicon->UpdateCharge();
    }

    return TRUE;
  }

  void ScrollScanIcon();
  void UpdateCharge();

private:
  ViewIcon();

  void ScanIcon(const string iconName);
  int GetCountScanIcon();
  void SetCountScanIcon(int count);

  void InitCharge();
  void Charge(int data);

private:
  static ViewIcon* m_instance;

private:
  GtkImage* m_network;
  GtkImage* m_sound;
  GtkImage* m_replay;
  GtkImage* m_udisk;
  GtkImage* m_cdrom;
  GtkImage* m_charge;

  int m_count_scanicon;
};
#endif
