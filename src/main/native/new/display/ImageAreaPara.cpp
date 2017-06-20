#include "display/ImageAreaPara.h"

#include "display/TopArea.h"
#include "imageProc/ModeStatus.h"

#define LINE_HEIGHT   20
#define RECT_ANGLE_W  120
#define RECT_ANGLE_H  LINE_HEIGHT

#define DRAW_STRING_X 5
#define DRAW_STRING_Y 5

extern bool g_printProcessFps;

ImageAreaPara* ImageAreaPara::m_instance = NULL;

// ---------------------------------------------------------

ImageAreaPara* ImageAreaPara::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ImageAreaPara();
  }

  return m_instance;
}

ImageAreaPara::ImageAreaPara() {
  m_ptrImg = ImageArea::GetInstance();
  memset(m_bufText, 0, sizeof (m_bufText));

  m_pwr = 100;
  m_zoomScale = 1.0;
  m_fps = 20;
  m_fpsForReplay = m_fps;
  m_fpsBak = m_fps;

  m_gain2D = 90;
  m_dynamicRange = 1;
  m_mbp = 1;
  m_thi = FALSE;
  m_tsi = "";
  m_gainM = 90;

  m_gainCw = 50;
  m_gainPw = 70;
  m_wfPw = 19;
  m_svPw = 2.0;
  m_PRFPw = 4.5;
  m_anglePw = 14;
  m_volumePw = 40;
  m_svPosPw = 10;
  m_HPRFStatus = FALSE;

  m_wfCw = 19;
  m_PRFCw = 400;
  m_angleCw = 29;

  m_gainCfm = 80;
  m_wfCfm = 25;
  m_prfCfm = 350;
  m_persistCfm = 1;
  m_sensitiveCfm = 2;

  m_gainPdi = 80;
  m_optimize = FALSE;

  m_draw_attr.area = ImageArea::PARA;
  m_draw_attr.mode.cvDraw = true;
}

ImageAreaPara::~ImageAreaPara() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

// General

void ImageAreaPara::UpdateGenPwr(int data, bool draw) {
  m_pwr = data;

  if (draw) {
    DrawGeneralPara();
  }
}

void ImageAreaPara::UpdateGenFps(int receiveFps, int processFps, bool draw) {
  m_fps = receiveFps;
  m_fpsForReplay = receiveFps;

  if (ModeStatus::IsUnFreezeMode()) {
    // 去掉1帧的波动。
    if ((m_fps < m_fpsBak - 2) || (m_fps > m_fpsBak + 2)) {
      m_fpsBak = m_fps;

      if (draw) {
        DrawGeneralPara();
      }
    }
  } else {
    m_fps = 0;

    // draw when freeze mode
    sprintf(m_bufText, " %3dfps", m_fps);
    m_ptrImg->DrawStringFps(m_bufText, 0, 0);
  }

  ModeStatus s;

  if (g_printProcessFps) {
    int fps = processFps;

    FormatM::EFormatM formatM = s.GetFormatM();
    FormatPw::EFormatPw formatPw = s.GetFormatPw();
    int curPwImg = s.GetPwCurImg();

    if (ModeStatus::IsSpectrumSimult() || ModeStatus::IsSpectrumColorSimult()) {
      float speed = (float)ImgPw::GetInstance()->GetRealSpectrumSpeed();
      int prf = ImgPw::GetInstance()->GetRealPRF();
      float n = (float)(DscMan::GetInstance()->GetDscPara()->dcaPWSpeed);
      int fpsPw = prf / (speed * n);
      fps = fps - fpsPw;
    } else if ((ModeStatus::IsSpectrumImgMode() || ModeStatus::IsSpectrumColorImgMode()) && (formatPw == FormatPw::P_TOTAL || curPwImg == 2)) {
      fps = 0;
    } else if (ModeStatus::IsMImgMode()) {
      float updateSpeed = (float)(DscMan::GetInstance()->GetDscPara()->dcaMSpeed);
      int fpsM = 1 / Img2D::GetInstance()->GetScaleMTime() / updateSpeed; // fps of M image
      fps = fps - fpsM;

      if (formatM == FormatM::M_TOTAL) {
        fps = 0;
      }
    }

    cout << "Receive fps is " << m_fps << ", Process fps is " << fps;
  }
}

void ImageAreaPara::UpdateVolume(int data, bool draw) {
  m_volumePw = data;

  if (draw) {
    DrawGeneralPara();
  }
}

void ImageAreaPara::UpdateGenZoomScale(double data, bool draw) {
  m_zoomScale = data;

  if (draw) {
    DrawGeneralPara();
  }
}

void ImageAreaPara::UpdateAutoOptimize(bool data, bool draw) {
  m_optimize = data;

  if (draw) {
    DrawGeneralPara();
  }
}

// 2D

void ImageAreaPara::Update2DGain(int data, bool draw) {
  m_gain2D = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::Update2DDynamicRange(int data, bool draw) {
  m_dynamicRange = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::Update2DMBP(int data, bool draw) {
  m_mbp = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::UpdateDepth(int depth, bool draw) {
  m_depth = depth;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::Update2DTHI(bool data, bool draw) {
  m_thi = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::Update2DTSI(const string data, bool draw) {
  m_tsi = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::Update2DLineDensity(const string data, bool draw) {
  m_lineDensity = data;

  if (draw) {
    Draw2DPara();
  }
}

void ImageAreaPara::UpdateFreq(const string freq, bool draw) {
  m_freq = freq;

  if (draw) {
    Draw2DPara();
  }
}

// M

void ImageAreaPara::UpdateMGain(int data, bool draw) {
  m_gainM = data;

  if (draw) {
    DrawMPara();
  }
}

void ImageAreaPara::UpdateMDynamicRange(int data, bool draw) {
  m_dynamicRange = data;

  if (draw) {
    DrawMPara();
  }
}

// Pw

void ImageAreaPara::UpdateCwGain(int data, bool draw) {
  m_gainCw = data;

  if (draw) {
    DrawPwGenPara();
  }
}

void ImageAreaPara::UpdatePwGain(int data, bool draw) {
  m_gainPw = data;

  if (draw) {
    DrawPwGenPara();
  }
}

void ImageAreaPara::UpdatePwWF(int data, bool draw) {
  m_wfPw = data;

  if (draw) {
    DrawPwGenPara();
  }
}

void ImageAreaPara::UpdatePwAngle(int data, bool draw) {
  m_anglePw = data;

  if (draw) {
    DrawPwGen2Para();
  }
}

void ImageAreaPara::UpdatePwPRF(double data, bool draw) {
  m_PRFPw = data;

  if (draw) {
    DrawPwGenPara();
  }
}

void ImageAreaPara::UpdatePwSVPos(double data, bool draw) {
  m_svPosPw = data;

  if (draw) {
    DrawPwGenPara();
  }
}

void ImageAreaPara::UpdatePwSV(double data, bool draw) {
  m_svPw = data;

  if (draw) {
    DrawPwGen2Para();
  }
}

void ImageAreaPara::UpdatePwHPRFStatus(bool on, bool draw) {
  m_HPRFStatus = on;

  if (draw) {
    DrawPwGen2Para();
  }
}

// Cw

void ImageAreaPara::UpdateCwWF(int data, bool draw) {
  m_wfCw = data;

  if (draw) {
    DrawCwPara();
  }
}

void ImageAreaPara::UpdateCwAngle(int data, bool draw) {
  m_angleCw = data;

  if (draw) {
    DrawCwPara();
  }
}

void ImageAreaPara::UpdateCwPRF(double data, bool draw) {
  m_PRFCw = data;

  if (draw) {
    DrawCwPara();
  }
}

void ImageAreaPara::UpdateCwSVPos(double data, bool draw) {
  m_svPosCw = data;

  if (draw) {
    DrawCwPara();
  }
}

// Cfm

void ImageAreaPara::UpdateCfmGain(int data, bool draw) {
  m_gainCfm = data;

  if (draw) {
    DrawCfmPara();
  }
}

void ImageAreaPara::UpdateCfmWF(int data, bool draw) {
  m_wfCfm = data;

  if (draw) {
    DrawCfmPara();
  }
}

void ImageAreaPara::UpdateCfmPRF(int data, bool draw) {
  m_prfCfm = data;

  if (draw) {
    DrawCfmPara();
  }
}

void ImageAreaPara::UpdateCfmPersist(int data, bool draw) {
  m_persistCfm = data;

  if (draw) {
    DrawCfmPara();
  }
}

void ImageAreaPara::UpdateCfmSensitive(int data, bool draw) {
  m_sensitiveCfm = data;

  if (draw) {
    DrawCfmPara();
  }
}

void ImageAreaPara::UpdateCfmDopplerFreq(const string freq, bool draw) {
  m_freqDoppler = freq;

  if (draw) {
    DrawCfmPara();
  }
}

// Pdi

void ImageAreaPara::UpdatePdiGain(int data, bool draw) {
  m_gainPdi = data;

  if (draw) {
    DrawPdiPara();
  }
}

void ImageAreaPara::UpdatePdiWF(int data, bool draw) {
  m_wfCw = data;

  if (draw) {
    DrawPdiPara();
  }
}

void ImageAreaPara::UpdatePdiPersist(int data, bool draw) {
  m_persistCfm = data;

  if (draw) {
    DrawPdiPara();
  }
}

void ImageAreaPara::UpdatePdiSensitive(int data, bool draw) {
  m_sensitiveCfm = data;

  if (draw) {
    DrawPdiPara();
  }
}

void ImageAreaPara::UpdatePdiDopplerFreq(const string freq, bool draw) {
  m_freqDoppler = freq;

  if (draw) {
    DrawPdiPara();
  }
}

int ImageAreaPara::GetGenFpsReplay() {
  return m_fpsForReplay;
}

int ImageAreaPara::GetGenFps() {
  return m_fps;
}

void ImageAreaPara::DrawGeneralPara() {
  if (m_optimize) {
    sprintf(m_bufText, "%3dP %3.1fX %2dV %3dfps eSpeed", m_pwr, m_zoomScale, m_volumePw, m_fps);
  } else {
    sprintf(m_bufText, "%3dP %3.1fX %2dV %3dfps", m_pwr, m_zoomScale, m_volumePw, m_fps);
  }

  TopArea::GetInstance()->UpdateImageParam(m_bufText);
}

void ImageAreaPara::Draw2DPara() {
  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>B</span></b>", x, y, Utils::get_color("yellow"));

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dG %ddB", m_gain2D, m_dynamicRange);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);

  FillRectangle(x, y + 2 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dMBP %s", m_mbp, m_tsi.c_str());
  m_ptrImg->DrawString(m_bufText, x, y + 2 * LINE_HEIGHT);

  FillRectangle(x, y + 3 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%s", m_freq.c_str());
  m_ptrImg->DrawString(m_bufText, x, y + 3 * LINE_HEIGHT);

  FillRectangle(x, y + 4 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dmm", m_depth);
  m_ptrImg->DrawString(m_bufText, x, y + 4 * LINE_HEIGHT);
}

void ImageAreaPara::DrawMPara() {
  Draw2DPara();

  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 5 * LINE_HEIGHT + 5;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>C</span></b>", x, y, Utils::get_color("yellow"));

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%3dG", m_gainM);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);
}

void ImageAreaPara::DrawCfmPara() {
  Draw2DPara();

  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 5 * LINE_HEIGHT + 5;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>Color</span></b>", x, y, Utils::get_color("yellow"));

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dG %.2fKHz", m_gainCfm, m_prfCfm/(float)1000);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);

  FillRectangle(x, y + 2 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "WF%d %dP %dS", m_wfCfm, m_persistCfm, m_sensitiveCfm);
  m_ptrImg->DrawString(m_bufText, x, y + 2 * LINE_HEIGHT);

  FillRectangle(x, y + 3 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%s", m_freqDoppler.c_str());
  m_ptrImg->DrawString(m_bufText, x, y + 3 * LINE_HEIGHT);
}

void ImageAreaPara::DrawPdiPara() {
  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 5 * LINE_HEIGHT + 5;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>PDI</span></b>", x, y, Utils::get_color("yellow"));

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dG %.2fKHz", m_gainPdi, m_prfCfm/(float)1000);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);

  FillRectangle(x, y + 2 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "WF%d %dP %dS", m_wfCfm, m_persistCfm, m_sensitiveCfm);
  m_ptrImg->DrawString(m_bufText, x, y + 2 * LINE_HEIGHT);

  FillRectangle(x, y + 3 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%s",m_freqDoppler.c_str());
  m_ptrImg->DrawString(m_bufText, x, y + 3 * LINE_HEIGHT);
}

void ImageAreaPara::DrawPwPara() {
  Draw2DPara();

  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 9 * LINE_HEIGHT + 5;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>Spectrum</span></b>", x, y, Utils::get_color("yellow"));

  DrawPwGenPara();
  DrawPwGen2Para();
}

void ImageAreaPara::DrawCwPara() {
  Draw2DPara();

  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 9 * LINE_HEIGHT + 5;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  m_ptrImg->DrawString("<b><span underline='low'>CW</span></b>", x, y, Utils::get_color("yellow"));

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "GC%d %.1fcm", m_gainCw, m_svPosCw/10);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);

  FillRectangle(x, y + 2 * LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dKHz %dHz %d°", (int)m_PRFCw, m_wfCw, m_angleCw);
  m_ptrImg->DrawString(m_bufText, x, y + 2 * LINE_HEIGHT);
}

void ImageAreaPara::DrawPwGenPara() {
  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 10 * LINE_HEIGHT;

  ModeStatus s1;
  ScanMode::EScanMode mode = s1.GetScanMode();

  int gain = 0;

  if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode()) {
    gain = m_gainCw;
  } else {
    gain = m_gainPw;
  }

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "%dG %.2fKHz", gain, m_PRFPw);
  m_ptrImg->DrawString(m_bufText, x, y);

  FillRectangle(x, y + LINE_HEIGHT, RECT_ANGLE_W, RECT_ANGLE_H);
  sprintf(m_bufText, "WF%d %.1fcm", m_wfPw, m_svPosPw/10);
  m_ptrImg->DrawString(m_bufText, x, y + LINE_HEIGHT);
}

void ImageAreaPara::DrawPwGen2Para() {
  int x = DRAW_STRING_X;
  int y = DRAW_STRING_Y + 12 * LINE_HEIGHT;

  FillRectangle(x, y, RECT_ANGLE_W, RECT_ANGLE_H);
  ModeStatus s1;

  ScanMode::EScanMode mode = s1.GetScanMode();

  if (ModeStatus::IsPWMode() || ModeStatus::IsPWColorMode()) {
    sprintf(m_bufText, "%.1fmm %dH", m_svPw, (int)m_HPRFStatus);
    m_ptrImg->DrawString(m_bufText, x, y);
    x += 80;
  } else if (ModeStatus::IsCWMode() || ModeStatus::IsCWColorMode()) {
    sprintf(m_bufText, "%dH", (int)m_HPRFStatus);
    m_ptrImg->DrawString(m_bufText, x, y);
    x += 30;
  }

  if (abs(m_anglePw) > 60) {
    sprintf(m_bufText, "%d°", m_anglePw);
    m_ptrImg->DrawString(m_bufText, x, y, Utils::get_color("red"));
  } else {
    sprintf(m_bufText, "%d°", m_anglePw);
    m_ptrImg->DrawString(m_bufText, x, y, Utils::get_color("white"));
  }
}

void ImageAreaPara::FillRectangle(int x, int y, int w, int h) {
  m_ptrImg->DrawRectangle(m_draw_attr, Utils::get_color("black"), x, y, w, h, true, false);
}
