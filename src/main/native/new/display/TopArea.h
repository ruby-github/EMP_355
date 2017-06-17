#ifndef __TOPAREA_H__
#define __TOPAREA_H__

#include "utils/Utils.h"

class TopArea {
public:
  static TopArea* GetInstance();

public:
  ~TopArea();

  GtkWidget* Create(const int width, const int height);

  void UpdateCheckPart(const string part);
  void UpdateFreq(const string freq);
  void UpdateHospitalName(const string name);
  void UpdateImageParam(const string param);
  void UpdatePatInfo(const string name, const string sex, const string age, const string id);
  void UpdateProbeType(const string type);
  void UpdateTIS(double tis);

  string GetCheckPart();
  string GetHospitalName();
  string GetTIS();
  string GetProbeType();
  bool GetReadImg();

  void SetDateFormat(int dateFormat);
  void SetReadImg(bool status);
  void AddTimeOut();

public:

private:
  // signal

  static gboolean signal_callback_update_datetime(gpointer data) {
    TopArea* toparea = (TopArea*)data;

    if (toparea != NULL) {
      toparea->DrawDateTime();
    }

    return TRUE;
  }

  static gboolean signal_expose_event(GtkWidget* widget, GdkEventExpose* event, TopArea* data) {
    TopArea* toparea = (TopArea*)data;

    if (toparea != NULL) {
      toparea->DrawingExpose(widget, event);
    }

    return FALSE;
  }

  void DrawingExpose(GtkWidget* widget, GdkEventExpose* event);

private:
  TopArea();

  void UpdateSysInfo();
  void DrawDateTime();

private:
  static TopArea* m_instance;

private:
  GtkLabel* m_label_hospital;
  GtkLabel* m_label_patient_info;
  GtkLabel* m_label_sys_info;
  GtkLabel* m_label_tis;
  GtkLabel* m_label_time;

  int m_cell_width;
  int m_cell_height;

  string m_checkPart;
  string m_freq;
  string m_imageParam;
  string m_probeType;
  bool m_inReadImg;
  int m_dateFormat;
};

#endif
