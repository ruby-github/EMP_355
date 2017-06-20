#include "keyboard/MultiFuncValue.h"

#include "utils/FakeXUtils.h"

#include "comment/NoteArea.h"
#include "display/HintArea.h"
#include "imageControl/Img2D.h"
#include "imageControl/Knob2D.h"
#include "imageProc/GlobalClassMan.h"
#include "imageProc/ImgProc2D.h"
#include "imageProc/ModeStatus.h"
#include "imageProc/PartFuncMan.h"
#include "imageProc/Zoom.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/MultiFuncMode.h"
#include "keyboard/MultiFuncFactory.h"
#include "measure/MeasureFactory.h"
#include "probe/MenuBiopsy.h"

////////////////////// MultiFuncFocus //////////////////////

MultiFuncFocus::MultiFuncFocus() {
  Img2D* ptrImg = Img2D::GetInstance();

  if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || Zoom::GetInstance()->GetLocalZoomStatus()|| ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus())
    HintArea::GetInstance()->UpdateHint(_("[Focus]: Mouse to move focus. When zoom, compound imaging, Tp-View or EFVI is on, it only has one focus."));
  else
    g_ptrKeyFunc = ChgFocusNum;
}

MultiFuncFocus::~MultiFuncFocus() {
  HintArea::GetInstance()->ClearHint();
  g_ptrKeyFunc = NULL;
}

void MultiFuncFocus::Value(EKnobOper opr) {
  Img2D* ptrImg = Img2D::GetInstance();

  if (ptrImg->IsCompoundSpaceOn() || ptrImg->IsCompoundFreqOn() || Zoom::GetInstance()->GetLocalZoomStatus() || ptrImg->GetTpViewStatus() || ptrImg->GetEFVIStatus()) {
    return;
  }

  Img2D::GetInstance()->ChangeFocSum(opr);

  // change tis
  ChangeTis();
}

void MultiFuncFocus::Mouse(int offsetX, int offsetY) {
  OverturnMouseOpr(offsetX, offsetY);

  if (offsetY > 0)
    Img2D::GetInstance()->ChangeFocPos(SUB);
  else if (offsetY < 0)
    Img2D::GetInstance()->ChangeFocPos(ADD);

  // change tis
  ChangeTis();
}

void MultiFuncFocus::Undo() {
  MultiFuncUndo();
}

void MultiFuncFocus::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncFocus::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

////////////////////// MultiFuncDepth //////////////////////

MultiFuncDepth::MultiFuncDepth() {
  g_ptrKeyFunc = D2PressDepth;
}

MultiFuncDepth::~MultiFuncDepth() {
  HintArea::GetInstance()->ClearHint();
  g_ptrKeyFunc = NULL;
}

void MultiFuncDepth::Value(EKnobOper opr) {
  ModeStatus s;
  int flag = 0;
  ScanMode::EScanMode mode = ScanMode::GetInstance()->GetScanMode();

  if (s.IsColorMode())
    flag = 1;
  else if (s.IsSpectrumMode())
    flag = 2;
  else if (s.IsSpectrumColorMode())
    flag = 3;
  else if (s.IsMMode())
    flag = 4;

  // clear biopsy line
  g_menuBiopsy.ClearBiopsyLine();

  if ((flag == 1) || (flag == 3))
    ImgCfm::GetInstance()->ClearBox();
  if ((flag == 2) || (flag == 3))
    ImgPw::GetInstance()->ClearSv();
  else if (flag == 4)
    Img2D::GetInstance()->ClearMLine();

  //prepare
  ImgCfm::GetInstance()->ChangeDepthBefore();

  //apply
  if (Img2D::GetInstance()->ChangeDepth(opr) != ERROR) {
    ImgCfm::GetInstance()->ChangeDepth();
    ImgPw::GetInstance()->ChangeDepth();
  }

  if ((flag == 1) || (flag == 3))
    ImgCfm::GetInstance()->ReDrawBox();
  if ((flag == 2) || (flag == 3))
    ImgPw::GetInstance()->ReDrawSv();
  else if (flag == 4)
    Img2D::GetInstance()->ReDrawMLine();

  // redraw biopsy line
  g_menuBiopsy.UpdateBiopsyLine();

  // change Tis
  ChangeTis();
}

void MultiFuncDepth::Undo() {
  MultiFuncUndo();
}

//////////////////// MultiFuncLocalZoom ////////////////////

MultiFuncLocalZoom::MultiFuncLocalZoom() {
  HintArea::GetInstance()->UpdateHint(_("[Local Zoom]: Mouse to move box, <Set> to change box size, <Update> to zoom."));
  m_changeSize = FALSE;
  Zoom::GetInstance()->LocalZoomInit();
  Zoom::GetInstance()->SetLocalZoomBoxStatus(m_changeSize);
  n1 = 0;
}

MultiFuncLocalZoom::~MultiFuncLocalZoom() {
  HintArea::GetInstance()->ClearHint();
  Zoom::GetInstance()->ClearLocalZoom();
  if((MultiFuncFactory::GetInstance()->GetMultiFuncType() == MultiFuncFactory::LOCAL_ZOOM)&&(!(Zoom::GetInstance()->GetLocalZoomStatus()))) {
  }
}

void MultiFuncLocalZoom::Do() {
  m_changeSize = !m_changeSize;
  Zoom::GetInstance()->SetLocalZoomBoxStatus(m_changeSize);
}

void MultiFuncLocalZoom::Undo() {
  MultiFuncUndo();
}

void MultiFuncLocalZoom::Update() {
  HintArea::GetInstance()->ClearHint();
  Zoom::GetInstance()->LocalZoomCtrl(TRUE);
}

void MultiFuncLocalZoom::Mouse(int offsetX, int offsetY) {
  OverturnMouseOpr(offsetX, offsetY);

  if (m_changeSize) { // change size
    bool lrStatus = ImgProc2D::GetInstance()->GetLRStatus();
    bool udStatus = ImgProc2D::GetInstance()->GetUDStatus();
    Zoom::GetInstance()->LocalZoomBoxSize(offsetX, offsetY, lrStatus, udStatus);
  } else { // move pos
    Zoom::GetInstance()->LocalZoomBoxPos(offsetX, offsetY);
  }
}

void MultiFuncLocalZoom::KeyLeft() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncLocalZoom::KeyRight() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncLocalZoom::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncLocalZoom::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

//////////////////// MultiFuncGlobalZoom ///////////////////

MultiFuncGlobalZoom::MultiFuncGlobalZoom() {
  HintArea::GetInstance()->UpdateHint(_("[Global Zoom]: <Auto> to change scale, Mouse to scroll."));
  Zoom::GetInstance()->GlobalZoomCtrl(TRUE);
  g_ptrKeyFunc = GlobalZoom;
}

MultiFuncGlobalZoom::~MultiFuncGlobalZoom() {
  HintArea::GetInstance()->ClearHint();
  Zoom::GetInstance()->GlobalZoomCtrl(FALSE);
  g_ptrKeyFunc = NULL;
}

void MultiFuncGlobalZoom::Value(EKnobOper opr) {
  Zoom::GetInstance()->GlobalZoomScale(opr);
}

void MultiFuncGlobalZoom::Mouse(int offsetX, int offsetY) {
  OverturnMouseOpr(offsetX, offsetY);
  Zoom::GetInstance()->GlobalZoomScroll(offsetX, offsetY);
}

void MultiFuncGlobalZoom::Undo() {
  MultiFuncUndo();
}

void MultiFuncGlobalZoom::KeyLeft() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncGlobalZoom::KeyRight() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncGlobalZoom::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncGlobalZoom::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

////////////////////// MultiFuncPIPZoom ////////////////////

MultiFuncPIPZoom::MultiFuncPIPZoom() {
  HintArea::GetInstance()->UpdateHint(_("[PIP]: <Auto> to change scale, Mouse to Move Magnifier."));
  Zoom::GetInstance()->PIPZoomCtrl(TRUE);
  g_ptrKeyFunc = PIPZoom;
}

MultiFuncPIPZoom::~MultiFuncPIPZoom() {
  HintArea::GetInstance()->ClearHint();
  Zoom::GetInstance()->PIPZoomCtrl(FALSE);
  g_ptrKeyFunc = NULL;
}

void MultiFuncPIPZoom::Value(EKnobOper opr) {
  Zoom::GetInstance()->PIPZoomScale(opr);
}

void MultiFuncPIPZoom::Mouse(int offsetX, int offsetY) {
  OverturnMouseOpr(offsetX, offsetY);
  Zoom::GetInstance()->PIPZoomPos(offsetX, offsetY);
}

void MultiFuncPIPZoom::Undo() {
  MultiFuncUndo();
}

void MultiFuncPIPZoom::KeyLeft() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncPIPZoom::KeyRight() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncPIPZoom::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncPIPZoom::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

////////////////////// MultiFuncMeasure ////////////////////

MultiFuncMeasure::MultiFuncMeasure() {
  PRINTF("enter measure mode\n");
}

MultiFuncMeasure::~MultiFuncMeasure() {
  PRINTF("exit measure mode\n");
  if (g_ptrAbsMeasure != NULL) {
    delete g_ptrAbsMeasure;
    g_ptrAbsMeasure  = NULL;
  }
}

void MultiFuncMeasure::Do() {
  if (g_ptrAbsMeasure != NULL)
    g_ptrAbsMeasure->PressLeft(m_p);
}

void MultiFuncMeasure::Undo() {
  if (g_ptrAbsMeasure != NULL)
    g_ptrAbsMeasure->PressRight(m_p);
}

void MultiFuncMeasure::Update() {
  if (g_ptrAbsMeasure != NULL)
    g_ptrAbsMeasure->Change();
}

void MultiFuncMeasure::Value(EKnobOper opr) {
  if (g_ptrAbsMeasure != NULL)
    g_ptrAbsMeasure->Value(opr);
}

void MultiFuncMeasure::Mouse(int offsetX, int offsetY) {
  if (g_ptrAbsMeasure != NULL) {
    GetPoint(offsetX, offsetY);
    g_ptrAbsMeasure->MouseMove(m_p);
  }
}

void MultiFuncMeasure::KeyLeft() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncMeasure::KeyRight() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncMeasure::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncMeasure::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

// ---------------------------------------------------------

POINT MultiFuncMeasure::GetPoint(int offsetX, int offsetY) {
  m_p = m_draw.GetCursor();

  if (((m_p.x + offsetX) >= 0) &&((m_p.x + offsetX) <= CANVAS_AREA_W))
    m_p.x += offsetX;

  offsetY = -offsetY;
  if (((m_p.y + offsetY) >= 0) &&((m_p.y + offsetY) <= CANVAS_AREA_H))
    m_p.y += offsetY;

  m_draw.SetCursor(m_p);

  return m_p;
}

//////////////////////// MultiFuncText /////////////////////

MultiFuncText::MultiFuncText() {
  PRINTF("enter text mode\n");
  NoteArea::GetInstance()->Enter();
}

MultiFuncText::~MultiFuncText() {
  PRINTF("exit text mode\n");
  NoteArea::GetInstance()->Hide();
}

void MultiFuncText::Undo() {
  NoteArea::GetInstance()->Undo();
  MultiFuncUndo();
}

////////////////////// MultiFuncGrayTrans //////////////////

MultiFuncGrayTrans::MultiFuncGrayTrans() {
  HintArea::GetInstance()->UpdateHint(_("[Gray Transform]: Mouse to move dot, <Auto> to select dot, <Update> to select curve."));
  ImgProc2D::GetInstance()->DisplayGrayTrans(TRUE);
  g_ptrKeyFunc = GrayTransport;
}

MultiFuncGrayTrans::~MultiFuncGrayTrans() {
  HintArea::GetInstance()->ClearHint();
  ImgProc2D::GetInstance()->DisplayGrayTrans(FALSE);
  g_ptrKeyFunc = NULL;
}

void MultiFuncGrayTrans::Mouse(int offsetX, int offsetY) {
  if (offsetY > 0)
    ImgProc2D::GetInstance()->ChangeGrayTransPointY(ADD);
  else if (offsetY < 0)
    ImgProc2D::GetInstance()->ChangeGrayTransPointY(SUB);
}

void MultiFuncGrayTrans::Value(EKnobOper opr) {
  ImgProc2D::GetInstance()->ChangeGrayTransPointX(opr);
}

void MultiFuncGrayTrans::Do() {
}

void MultiFuncGrayTrans::Undo() {
  MultiFuncUndo();
}

void MultiFuncGrayTrans::Update() {
  ImgProc2D::GetInstance()->ResetGrayTrans();
}

void MultiFuncGrayTrans::KeyUp() {
  int offsetX = 0;
  int offsetY = 1;

  Mouse(offsetX, offsetY);
}

void MultiFuncGrayTrans::KeyDown() {
  int offsetX = 0;
  int offsetY = -1;

  Mouse(offsetX, offsetY);
}

///////////////////// MultiFuncGrayReject //////////////////

MultiFuncGrayReject::MultiFuncGrayReject() {
  HintArea::GetInstance()->UpdateHint(_("[Gray Reject]: Mouse to move position."));
  ImgProc2D::GetInstance()->DisplayGrayReject(TRUE);
}

MultiFuncGrayReject::~MultiFuncGrayReject() {
  HintArea::GetInstance()->ClearHint();
  ImgProc2D::GetInstance()->DisplayGrayReject(FALSE);
}

void MultiFuncGrayReject::Mouse(int offsetX, int offsetY) {
  if (offsetX > 0)
    ImgProc2D::GetInstance()->ChangeGrayReject(ADD);
  else if (offsetX < 0)
    ImgProc2D::GetInstance()->ChangeGrayReject(SUB);
}

void MultiFuncGrayReject::Undo() {
  MultiFuncUndo();
}

void MultiFuncGrayReject::KeyLeft() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncGrayReject::KeyRight() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

//////////////////////// MultiFuncFreq /////////////////////

MultiFuncFreq::MultiFuncFreq() {
  HintArea::GetInstance()->UpdateHint(_("[Freq.]: <Value> change freq. size"));
  g_ptrKeyFunc = D2ChgFreq;
}

MultiFuncFreq::~MultiFuncFreq() {
  HintArea::GetInstance()->ClearHint();
  g_ptrKeyFunc = NULL;
}

void MultiFuncFreq::Undo() {
  MultiFuncUndo();
}

/////////////////////// MultiFuncChroma ////////////////////

MultiFuncChroma::MultiFuncChroma() {
  HintArea::GetInstance()->UpdateHint(_("[Chroma  Reject]: <Value> change chroma "));
  g_ptrKeyFunc = D2ChgChroma;
}

MultiFuncChroma::~MultiFuncChroma() {
  HintArea::GetInstance()->ClearHint();
  g_ptrKeyFunc = NULL;
}

void MultiFuncChroma::Undo() {
  MultiFuncUndo();
}

//////////////////// MultiFuncBiopsyVerify /////////////////

MultiFuncBiopsyVerify::MultiFuncBiopsyVerify() {
  HintArea::GetInstance()->UpdateHint(_("[Verify]: <Auto> to adjust angle, Mouse to adjust position."));
}

MultiFuncBiopsyVerify::~MultiFuncBiopsyVerify() {
  HintArea::GetInstance()->ClearHint();
}

void MultiFuncBiopsyVerify::Value(EKnobOper opr) {
  BiopsyLine::GetInstance()->AngleChange(opr);
}

void MultiFuncBiopsyVerify::Mouse(int offsetX, int offsetY) {
  OverturnMouseOpr(offsetX, offsetY);
  BiopsyLine::GetInstance()->PositionChange(offsetX);
}

void MultiFuncBiopsyVerify::MultiFuncBiopsyVerify::Undo() {
  BiopsyLine::GetInstance()->Clear();
  BiopsyLine::GetInstance()->InitPara();
  BiopsyLine::GetInstance()->Draw();
}

void MultiFuncBiopsyVerify::Do() {
  MultiFuncFactory* ptr = MultiFuncFactory::GetInstance();

  ptr->Create(MultiFuncFactory::BIOPSY);

  SetSystemCursor(90,130);
  doBtnEvent(1, 1);
  doBtnEvent(1, 0);
  SetMenuBiopsyCursorYRange(113,213);
}

void MultiFuncBiopsyVerify::KeyLeft() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncBiopsyVerify::KeyRight() {
  int offsetX =-1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncBiopsyVerify::KeyUp() {
  int offsetX = -1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

void MultiFuncBiopsyVerify::KeyDown() {
  int offsetX = 1;
  int offsetY = 0;

  Mouse(offsetX, offsetY);
}

/////////////////////// MultiFuncBiopsy ////////////////////

void MultiFuncBiopsy::Mouse(int offsetX, int offsetY) {
  FakeXMotionEventMenuBiopsy(offsetX,offsetY);
}

void MultiFuncBiopsy::Undo() {
  MultiFuncUndo();
}

void MultiFuncBiopsy::Do() {
  doBtnEvent(1, 1);
  doBtnEvent(1, 0);
}
