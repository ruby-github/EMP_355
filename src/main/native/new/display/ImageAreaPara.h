#ifndef __IMAGE_AREA_PARA_H__
#define __IMAGE_AREA_PARA_H__

#include "display/ImageArea.h"

class ImageAreaPara {
public:
  static ImageAreaPara* GetInstance();

public:
  ~ImageAreaPara();

  // General
  void UpdateGenPwr(int data, bool draw = true);
  void UpdateGenFps(int receiveFps, int processFps, bool draw = true);
  void UpdateVolume(int data, bool draw = true);
  void UpdateGenZoomScale(double data, bool draw = true);
  void UpdateAutoOptimize(bool data, bool draw = true);

  // 2D
  void Update2DGain(int data, bool draw = true);
  void Update2DDynamicRange(int data, bool draw = true);
  void Update2DMBP(int data, bool draw = true);
  void UpdateDepth(int depth, bool draw = true);
  void Update2DTHI(bool data, bool draw = true);
  void Update2DTSI(const string data, bool draw = true);
  void Update2DLineDensity(const string data, bool draw = true);
  void UpdateFreq(const string freq, bool draw = true);

  // M
  void UpdateMGain(int data, bool draw = true);
  void UpdateMDynamicRange(int data, bool draw = true);

  // Pw
  void UpdateCwGain(int data, bool draw = true);
  void UpdatePwGain(int data, bool draw = true);
  void UpdatePwWF(int data, bool draw = true);
  void UpdatePwAngle(int data, bool draw = true);
  void UpdatePwPRF(double data, bool draw = true);
  void UpdatePwSVPos(double data, bool draw = true);
  void UpdatePwSV(double data, bool draw = true);
  void UpdatePwHPRFStatus(bool on, bool draw = true);

  // Cw
  void UpdateCwWF(int data, bool draw = true);
  void UpdateCwAngle(int data, bool draw = true);
  void UpdateCwPRF(double data, bool draw = true);
  void UpdateCwSVPos(double data, bool draw = true);

  // Cfm
  void UpdateCfmGain(int data, bool draw = true);
  void UpdateCfmWF(int data, bool draw = true);
  void UpdateCfmPRF(int data, bool draw = true);
  void UpdateCfmPersist(int data, bool draw = true);
  void UpdateCfmSensitive(int data, bool draw = true);
  void UpdateCfmDopplerFreq(const string freq, bool draw = true);

  // Pdi
  void UpdatePdiGain(int data, bool draw = true);
  void UpdatePdiWF(int data, bool draw = true);
  void UpdatePdiPersist(int data, bool draw = true);
  void UpdatePdiSensitive(int data, bool draw = true);
  void UpdatePdiDopplerFreq(const string freq, bool draw = true);

  int GetGenFpsReplay();
  int GetGenFps();

  void DrawGeneralPara();
  void Draw2DPara();
  void DrawMPara();
  void DrawCfmPara();
  void DrawPdiPara();
  void DrawPwPara();
  void DrawCwPara();

  void FillRectangle(int x, int y, int w, int h);

private:
  ImageAreaPara();

  void DrawPwGenPara();
  void DrawPwGen2Para();

private:
  static ImageAreaPara* m_instance;

private:
  ImageArea* m_ptrImg;
  ImageArea::DrawAttr m_draw_attr;

  char m_bufText[100];

  // General
  int m_pwr;
  int m_fps;
  int m_fpsBak;
  int m_fpsForReplay;
  int m_volumePw;
  double m_zoomScale;
  bool m_optimize;

  // 2D
  int m_gain2D;
  int m_dynamicRange;
  int m_mbp;
  int m_depth;
  bool m_thi;
  string m_tsi;
  string m_lineDensity;
  string m_freq;

  // M
  int m_gainM;

  // Pw
  int m_gainCw;
  int m_gainPw;
  int m_wfPw;
  int m_anglePw;
  double m_PRFPw;
  double m_svPosPw;
  double m_svPw;
  bool m_HPRFStatus;

  // Cw
  int m_wfCw;
  int m_angleCw;
  double m_PRFCw;
  double m_svPosCw;

  // Cfm
  int m_gainCfm;
  int m_wfCfm;
  int m_prfCfm;
  int m_persistCfm;
  int m_sensitiveCfm;

  // Pdi
  int m_gainPdi;
  string m_freqDoppler;
};

#endif
