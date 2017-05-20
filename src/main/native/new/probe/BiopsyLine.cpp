#include "probe/BiopsyLine.h"

#include "Def.h"
#include "display/HintArea.h"
#include "probe/BiopsyMan.h"
#include "probe/MenuBiopsy.h"
#include "probe/ProbeSocket.h"
#include "probe/ProbeMan.h"

BiopsyLine* BiopsyLine::m_instance = NULL;

// ---------------------------------------------------------

BiopsyLine* BiopsyLine::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new BiopsyLine();
  }

  return m_instance;
}

BiopsyLine::BiopsyLine() {
  m_biopX = 0;
  m_biopY = 0;
  m_biopX1 = 0;
  m_biopY1 = 0;
  m_biopX2 = 0;
  m_biopY2 = 0;
  m_biopAngle1 = 0;
  m_biopAngle2 = 0;

  m_angle = 0;
  m_baseAngle = 0;
  m_adjustAngle = 0;

  m_baseVal_x = 0;
  m_baseVal_y = 0;
  m_adjustRange_x = 0;
  m_adjustRange_y = 0;

  m_ptrImgDraw = ImageAreaDraw::GetInstance();
}

BiopsyLine::~BiopsyLine() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
  m_ptrImgDraw = NULL;
}

bool BiopsyLine::Create() {
  ProbeSocket::ProbePara curPara;
  ProbeMan::GetInstance()->GetCurProbe(curPara);

  m_probeType = curPara.type;
  m_probeRadius = curPara.r;

  PRINTF("probe type = %c, radius = %d\n", m_probeType, m_probeRadius);

  if (!InitPara()) {
    return false;
  }

  Clear();

  if (g_menuBiopsy.GetDrawStatus()) {
    Draw();
  }

  return true;
}

bool BiopsyLine::InitPara() {
  m_angle = BiopsyMan::GetInstance()->GetCurBioAngleValue();
  m_biopX = BiopsyMan::GetInstance()->GetBioXOfCurBioBracketAngleType();

  m_biopX1=m_biopX;
  m_biopX2=m_biopX;
  m_biopAngle1=m_angle-5;
  m_biopAngle2=m_angle+5;

  float angle = m_angle * PI /180;
  float angle1=m_biopAngle1 * PI /180;
  float angle2=m_biopAngle2 * PI /180;

  if (tan(angle) < 0.001) {
    m_biopY = 250;  // MaxDepth
    m_biopY1=250;
    m_biopY2=250;
  } else {
    m_biopY= m_biopX / tan(angle);
    m_biopY1= m_biopX1 / tan(angle1);
    m_biopY2= m_biopX2 / tan(angle2);
  }

  return true;
}

void BiopsyLine::WritePara() {
  BiopsyMan::GetInstance()->WriteXAngleValue(m_biopX, m_angle);
  HintArea::GetInstance()->UpdateHint(_("saving parameter succeed!"), 1);
}

void BiopsyLine::SetDefault() {
  m_angle=BiopsyMan::GetInstance()->GetCurBioAngleValueDefault();
  m_biopX=BiopsyMan::GetInstance()->GetDefaultBioXOfCurBioBracketAngleType();

  m_biopX1=m_biopX;
  m_biopX2=m_biopX;
  m_biopAngle1=m_angle-5;
  m_biopAngle2=m_angle+5;

  float m_angle = m_angle * PI /180;
  float m_angle1=m_biopAngle1 *PI /180;
  float m_angle2=m_biopAngle2 *PI /180;
  if (tan(m_angle) < 0.001) {
    m_biopY = 250;  // MaxDepth
    m_biopY1=250;
    m_biopY2=250;
  } else {
    m_biopY = m_biopX / tan(m_angle);
    m_biopY1 = m_biopX1 / tan(m_angle1);
    m_biopY2 = m_biopX2 / tan(m_angle2);
  }

  Draw();
  BiopsyMan::GetInstance()->WriteXAngleValue(m_biopX,m_angle);
  HintArea::GetInstance()->UpdateHint(_("Restoring default factory succeed!"), 1);
}

void BiopsyLine::Clear() {
  if (g_menuBiopsy.GetDoubleLineStatus()) {
    m_ptrImgDraw->ClearBiopsyDoubleLine();
  } else {
    m_ptrImgDraw->ClearBiopsyLine();
  }
}

void BiopsyLine::Draw() {
  if (g_menuBiopsy.GetDoubleLineStatus()) {
    m_ptrImgDraw->DrawBiopsyDoubleLine(m_biopX1, m_biopY1, m_biopX2, m_biopY2);
  } else {
    m_ptrImgDraw->DrawBiopsyLine(m_biopX, m_biopY);
  }
}

void BiopsyLine::Redraw() {
  if (g_menuBiopsy.GetDoubleLineStatus()) {
    m_ptrImgDraw->ReDrawBiopsyDoubleLine();
  } else {
    m_ptrImgDraw->ReDrawBiopsyLine();
  }
}

void BiopsyLine::AngleSwitch() {
  m_angle=BiopsyMan::GetInstance()->GetCurBioAngleValue();
  m_biopX=BiopsyMan::GetInstance()->GetBioXOfCurBioBracketAngleType();

  m_biopX1=m_biopX;
  m_biopX2=m_biopX;

  m_biopAngle1=m_angle -5;
  m_biopAngle2=m_angle +5;

  float angle = m_angle * PI /180;
  float angle1 = m_biopAngle1 * PI /180;
  float angle2 = m_biopAngle2 * PI /180;

  if (tan(angle) < 0.001) {
    m_biopY = 250;  // MaxDepth
    m_biopY1 =250;
    m_biopY2 =250;
  } else {
    m_biopY = m_biopX / tan(angle);
    m_biopY1 = m_biopX1 / tan(angle1);
    m_biopY2 = m_biopX2 / tan(angle2);
  }

  Draw();
}

void BiopsyLine::AngleChange(EKnobOper oper) {
  int max=80; // max angle
  int min=1;  // min angle
  float angle=0;

  float angle1=0;
  float angle2=0;

  if (oper == ADD && m_angle < max) {
    ++m_angle;
    angle = m_angle * PI /180;

    m_biopAngle1=m_angle-5;
    m_biopAngle2=m_angle+5;
    angle1=m_biopAngle1 *PI/180;
    angle2=m_biopAngle2 *PI/180;

    if (ABS(tan(angle)) <= 0.001) {
      m_biopY = 250;  // MaxDepth;
      m_biopY1 =250;
      m_biopY2 =250;
    } else {
      m_biopY = m_biopX / tan(angle);
      m_biopY1 = m_biopX1 / tan(angle1);
      m_biopY2 = m_biopX2 / tan(angle2);
    }
  } else if (oper == SUB && m_angle > min) {
    --m_angle;
    angle = m_angle * PI/180;

    m_biopAngle1=m_angle - 5;
    m_biopAngle2=m_angle + 5;
    angle1=m_biopAngle1 *PI/180;
    angle2=m_biopAngle2 *PI/180;

    if ( ABS(tan(angle)) <= 0.001) {
      m_biopY = 250;  //  MaxDepth;
      m_biopY1 =250;
      m_biopY2 =250;
    } else {
      m_biopY = m_biopX / tan(angle);
      m_biopY1 = m_biopX1 / tan(angle1);
      m_biopY2 = m_biopX2 / tan(angle2);
    }
  }

  Draw();
}

void BiopsyLine::PositionChange(int offsetX) {
  m_baseVal_x = BiopsyMan::GetInstance()->GetDefaultBioXOfCurBioBracketAngleType();

  int max=m_baseVal_x+25;
  int min=m_baseVal_x-25;

  if (min < 0) { // 防止为m_biopx < 0
    min = 0;
  }

  if (m_biopX+offsetX > min && m_biopX+offsetX < max) {
    m_biopX=m_biopX+offsetX;
    m_biopX1=m_biopX;
    m_biopX2=m_biopX;
  }

  float angleTmp = m_angle * PI /180;
  float angleTmp1= m_biopAngle1 * PI /180;
  float angleTmp2= m_biopAngle2 * PI /180;

  if ( ABS(tan(angleTmp)) <= 0.001) {
    m_biopY = 250;  // MaxDepth;
    m_biopY1 =250;
    m_biopY2 =250;
  } else {
    if(m_biopX / tan(angleTmp)<2) {
      return;
    }

    m_biopY = m_biopX / tan(angleTmp);
    m_biopY1 = m_biopX1 / tan(angleTmp1);
    m_biopY2 = m_biopX2 / tan(angleTmp2);
  }

  Draw();
}

void BiopsyLine::ClearSingleLine() {
  m_ptrImgDraw->ClearBiopsyLine();
}

void BiopsyLine::ClearDoubleLine() {
  m_ptrImgDraw->ClearBiopsyDoubleLine();
}
