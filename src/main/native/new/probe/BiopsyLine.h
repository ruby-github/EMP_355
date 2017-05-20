#ifndef __BIOPSY_LINE_H__
#define __BIOPSY_LINE_H__

#include "display/ImageAreaDraw.h"

class BiopsyLine {
public:
  static BiopsyLine* GetInstance();

public:
  ~BiopsyLine();

  bool Create();

  bool InitPara();
  void WritePara();
  void SetDefault();
  void Clear();
  void Draw();
  void Redraw();
  void AngleSwitch();
  void AngleChange(EKnobOper oper);
  void PositionChange(int offsetX);
  void ClearSingleLine();
  void ClearDoubleLine();

private:
  BiopsyLine();

private:
  static BiopsyLine* m_instance;

private:
  double  m_biopX;
  double  m_biopY;
  double  m_biopX1;
  double  m_biopY1;
  double  m_biopX2;
  double  m_biopY2;
  double  m_biopAngle1;
  double  m_biopAngle2;

  double m_angle;
  int m_baseAngle;
  int m_adjustAngle;

  int m_baseVal_x;
  int m_baseVal_y;
  int m_adjustRange_x;
  int m_adjustRange_y;

  char m_probeType;
  int m_probeRadius;

  ImageAreaDraw* m_ptrImgDraw;
};
#endif
