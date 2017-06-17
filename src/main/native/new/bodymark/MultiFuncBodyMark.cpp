#include "bodymark/MultiFuncBodyMark.h"

#include "utils/FakeXUtils.h"

#include "display/KnobMenu.h"
#include "display/KnobNone.h"
#include "display/HintArea.h"
#include "imageProc/ModeStatus.h"
#include "keyboard/KeyFunc.h"
#include "keyboard/KeyValueOpr.h"
#include "keyboard/LightDef.h"
#include "sysMan/SysNoteSetting.h"
#include "ViewMain.h"

enum EKnobBodymark {
  BDMK_BODYSIZE, BDMK_PROBECOLOR
};

KnobMenu::KnobItem KnobBodymarkMenu[] = {
  {_("BodyMark Size"), "", MIN, MultiFuncBodyMark::SetBodyMarkSize, NULL},
  {_("ProbeMark Color"), "", MIN, MultiFuncBodyMark::SetProbeMarkColor, NULL},
  {_("Hide BodyMark"), "Press", PRESS,MultiFuncBodyMark::PressSetHideBodyMark, NULL},
  {"", "", ERROR, NULL, NULL},
  {"", "", ERROR, NULL, NULL},
  {"", "", ERROR, NULL, NULL}
};

const string bodyMarkSizeName[] = {
  _("Small"),
  _("Mid"),
  _("Big"),
};

const string bodyMarkColorName[] = {
  _("White"),
  _("Gray"),
  _("Red"),
  _("Green"),
  _("Yellow"),
  _("Blue"),
};

MultiFuncBodyMark* MultiFuncBodyMark::m_this = NULL;

// ---------------------------------------------------------

void MultiFuncBodyMark::SetNewBodyMark(GdkPixbuf* pixbuf) {
  if (m_this != NULL) {
    m_this->AddNewBodyMark(pixbuf);
    m_this->BeginMoveProbeMark();
  }
}

EKnobReturn MultiFuncBodyMark::SetBodyMarkSize(EKnobOper opr) {
  EKnobReturn ret = ERROR;

  if (m_this != NULL && m_this->m_ptrBodyMark != NULL) {
    if (!m_this->m_ptrBodyMark->GetBodyMarkStatus(m_this->m_active)) {
      HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);

      return ERROR;
    }

    BodyMark::SIZE size = m_this->m_ptrBodyMark->GetBodyMarkSize();
    int temp = size;
    double old_scale = m_this->m_ptrBodyMark->GetBodyMarkScale();

    switch(opr) {
    case ADD:
      if(size != BodyMark::BIG) {
        temp++;
        size = (BodyMark::SIZE)temp;
        m_this->m_bodyPos[m_this->m_active] = m_this->m_ptrBodyMark->SetBodyMarkSize(size, m_this->m_active);
        double new_scale = m_this->m_ptrBodyMark->GetBodyMarkScale();
        m_this->ReviseProbePos(new_scale/old_scale);
      }

      break;
    case SUB:
      if(size != BodyMark::SMALL) {
        temp--;
        size = (BodyMark::SIZE)temp;
        m_this->m_bodyPos[m_this->m_active] = m_this->m_ptrBodyMark->SetBodyMarkSize(size, m_this->m_active);
        double new_scale = m_this->m_ptrBodyMark->GetBodyMarkScale();
        m_this->ReviseProbePos(new_scale/old_scale);
      }

      break;
    default:
      return ERROR;
      break;
    }

    double scale = m_this->m_ptrBodyMark->GetBodyMarkScale();
    int x = IMG_AREA_X + IMAGE_X + m_this->m_bodyPos[m_this->m_active].x + BDMK_W * scale / 2;
    int y = IMG_AREA_Y + IMAGE_Y + m_this->m_bodyPos[m_this->m_active].y + BDMK_H * scale / 2;
    SetSystemCursor(x, y);

    SysNoteSetting sys;
    sys.SetBodyMarkSize(size);
    sys.SyncFile();

    ret = GetKnobRetBodyMarkSize(size);

    if (size < (int)sizeof(bodyMarkSizeName)) {
      KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, bodyMarkSizeName[size], ret);
    }
  }

  return ret;
}

EKnobReturn MultiFuncBodyMark::SetProbeMarkColor(EKnobOper opr) {
  EKnobReturn ret = ERROR;

  if (m_this != NULL && m_this->m_ptrBodyMark != NULL) {
    if (!m_this->m_ptrBodyMark->GetBodyMarkStatus(m_this->m_active)) {
      HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);

      return ERROR;
    }

    BodyMark::COLOR color = m_this->m_ptrBodyMark->GetProbeMarkColor();

    int temp = color;

    switch(opr) {
    case ::ADD:
      if(color != BodyMark::BLUE) {
        temp++;
        color = (BodyMark::COLOR)temp;
        m_this->m_ptrBodyMark->SetProbeMarkColor(color, m_this->m_active);
      }

      break;
    case ::SUB:
      if(color != BodyMark::WHITE) {
        temp--;
        color = (BodyMark::COLOR)temp;
        m_this->m_ptrBodyMark->SetProbeMarkColor(color, m_this->m_active);
      }

      break;
    default:
      return ERROR;
      break;
    }

    ret = GetKnobRetProbeMarkColor(color);

    SysNoteSetting sys;
    sys.SetBodyMarkColor(color);
    sys.SyncFile();

    if (color < (int)sizeof(bodyMarkColorName)) {
      KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, bodyMarkColorName[color], ret);
    }
  }

  return ret;
}

EKnobReturn MultiFuncBodyMark::PressSetBodyMarkSize() {
  EKnobReturn ret = ERROR;

  if (m_this != NULL && m_this->m_ptrBodyMark != NULL) {
    if (!m_this->m_ptrBodyMark->GetBodyMarkStatus(m_this->m_active)) {
      HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);

      return ERROR;
    }

    BodyMark::SIZE size = m_this->m_ptrBodyMark->GetBodyMarkSize();
    double old_scale = m_this->m_ptrBodyMark->GetBodyMarkScale();

    int temp = size;

    if(temp < BodyMark::BIG) {
      temp++;
      size = (BodyMark::SIZE)temp;
    } else {
      size = BodyMark::SMALL;
    }

    m_this->m_bodyPos[m_this->m_active] = m_this->m_ptrBodyMark->SetBodyMarkSize(size, m_this->m_active);
    double new_scale = m_this->m_ptrBodyMark->GetBodyMarkScale();
    m_this->ReviseProbePos(new_scale/old_scale);

    double scale = m_this->m_ptrBodyMark->GetBodyMarkScale();
    int x = IMG_AREA_X + IMAGE_X + m_this->m_bodyPos[m_this->m_active].x + BDMK_W * scale / 2;
    int y = IMG_AREA_Y + IMAGE_Y + m_this->m_bodyPos[m_this->m_active].y + BDMK_H * scale / 2;
    SetSystemCursor(x, y);

    SysNoteSetting sys;
    sys.SetBodyMarkSize(size);
    sys.SyncFile();

    ret = GetKnobRetBodyMarkSize(size);
    KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, bodyMarkSizeName[size], ret);
  }

  return ret;
}

EKnobReturn MultiFuncBodyMark::PressSetProbeMarkColor() {
  EKnobReturn ret = ERROR;

  if (m_this != NULL && m_this->m_ptrBodyMark != NULL) {
    if (!m_this->m_ptrBodyMark->GetBodyMarkStatus(m_this->m_active)) {
      HintArea::GetInstance()->UpdateHint(_("[BodyMark]: Invalid when bodymark is off."), 1);

      return ERROR;
    }

    BodyMark::COLOR color = m_this->m_ptrBodyMark->GetProbeMarkColor();
    int temp = color;

    if(temp < BodyMark::BLUE) {
      temp++;
      color = (BodyMark::COLOR)temp;
      m_this->m_ptrBodyMark->SetProbeMarkColor(color, m_this->m_active);
    } else {
      color = BodyMark::WHITE;
      m_this->m_ptrBodyMark->SetProbeMarkColor(color, m_this->m_active);
    }

    SysNoteSetting sys;
    sys.SetBodyMarkColor(color);
    sys.SyncFile();

    ret = GetKnobRetProbeMarkColor(color);
    KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, bodyMarkColorName[color], ret);
  }

  return ret;
}

EKnobReturn MultiFuncBodyMark::PressSetHideBodyMark() {
  BodyMark::HideBodyMark();

  return OK;
}

EKnobReturn MultiFuncBodyMark::PressSetHideBodyMark(EKnobOper oper) {
  BodyMark::HideBodyMark();

  return OK;
}

MultiFuncBodyMark::MultiFuncBodyMark() {
  g_keyInterface.Push(this);

  m_probePosOffset.x = BDMK_W / 2;
  m_probePosOffset.y = BDMK_H / 2;
  m_ptrBodyMark  = NULL;

  for (int i=0; i<4; i++) {
    m_bodyPos[i].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
    m_bodyPos[i].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
    m_probePos[i].x = m_probePosOffset.x;
    m_probePos[i].y = m_probePosOffset.y;
  }

  ModeStatus ms;
  if (ms.IsD2Mode() || ms.IsCFMMode() || ms.IsPDIMode()) {
    m_active = ms.Get2DCurrentB();
  } else {
    m_active = 0;
  }

  CUpdateBodyMark::EnterBodyMark();

  m_bodyOpr = IDEL;
  m_this = this;
  KnobBodyMarkCreate();
  DarkFucusLight();
}

MultiFuncBodyMark::~MultiFuncBodyMark() {
  if (m_ptrBodyMark != NULL) {
    delete m_ptrBodyMark;
    m_ptrBodyMark = NULL;
  }

  g_keyInterface.Pop();
  KnobUndo();
}

void MultiFuncBodyMark::KeyEvent(unsigned char keyValue) {
  if (m_ptrBodyMark == NULL) {
    m_ptrBodyMark = new BodyMark(m_bodyPos[m_active], m_probePos[m_active]);
  }

  switch(keyValue) {
  case KEY_UPDATE:
    if (m_bodyOpr == MOVE_PROBE) {
      EndMoveProbeMark();
    } else if(m_bodyOpr == ADD) {
      BeginMoveProbeMark();
    }

    break;
  case KEY_SET:
    if (m_bodyOpr == MOVE_PROBE) {
      EndMoveProbeMark();
      DClicked();
    } else if (m_bodyOpr == MOVE_BODY) {
      EndMoveBodyMark();
    } else if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD)) {
      FakeXEvent::KeyEvent(keyValue);
    }

    break;
  case KEY_CLEAR:
    if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD)) {
      KeyClearScreen kcs;
      kcs.Execute();
    }

    break;
  case KEY_BODYMARK:
    Esc();
    break;
  case KEY_CURSOR:
    if(m_bodyOpr == MOVE_BODY || m_bodyOpr == MOVE_PROBE) {
      EndMoveBodyMark();
    }

    break;
  case KEY_F1:
  case KEY_F2:
  case KEY_F3:
  case KEY_F4:
  case KEY_F5:
    ViewMain::GetInstance()->KnobKeyEvent(keyValue);
    break;
  case KEY_F1ADD:
  case KEY_F1SUB:
  case KEY_F2ADD:
  case KEY_F2SUB:
  case KEY_F3ADD:
  case KEY_F3SUB:
  case KEY_F4ADD:
  case KEY_F4SUB:
  case KEY_F5ADD:
  case KEY_F5SUB:
    CKnobEvent::FEvent(keyValue);
    break;
  default:
    Esc();
    ViewMain::GetInstance()->KeyEvent(keyValue);
    break;
  }
}

void MultiFuncBodyMark::KnobEvent(unsigned char keyValue, unsigned char offset) {
  if (m_ptrBodyMark == NULL) {
    m_ptrBodyMark = new BodyMark(m_bodyPos[m_active], m_probePos[m_active]);
  }

  CKnobEvent ke;

  switch(keyValue) {
  case KNOB_VALUE:
    Value((EKnobOper)offset);
    break;
  default:
    ke.KnobEvent(keyValue, offset);
    break;
  }
}

void MultiFuncBodyMark::MouseEvent(char offsetX, char offsetY) {
  if (m_bodyOpr == MOVE_PROBE) {
    if (m_ptrBodyMark == NULL) {
      return;
    }

    m_probePos[m_active] = GetPointProbe(offsetX, offsetY);
    m_ptrBodyMark->SetProbeMarkPos(m_probePos[m_active], m_active);
  } else if (m_bodyOpr == MOVE_BODY) {
    if (m_ptrBodyMark == NULL) {
      return;
    }

    m_bodyPos[m_active] = GetPointBody(offsetX, offsetY);
    m_probePos[m_active].x = m_probePosOffset.x;
    m_probePos[m_active].y = m_probePosOffset.y;

    m_ptrBodyMark->MoveBody(m_bodyPos[m_active], m_probePos[m_active], m_active);

    double scale = m_ptrBodyMark->GetBodyMarkScale();
    int x = IMG_AREA_X + IMAGE_X + m_bodyPos[m_active].x + BDMK_W * scale / 2;
    int y = IMG_AREA_Y + IMAGE_Y + m_bodyPos[m_active].y + BDMK_H * scale / 2;

    SetSystemCursor(x, y);
  } else if ((m_bodyOpr == IDEL) || (m_bodyOpr == ADD)) {
    FakeXEvent::MouseEvent(offsetX, offsetY);
  }
}

void MultiFuncBodyMark::KnobBodyMarkCreate() {
  KnobMenu::GetInstance()->SetItem(KnobBodymarkMenu, sizeof(KnobBodymarkMenu)/sizeof(KnobMenu::KnobItem), KnobMenu::BDMK);

  m_ptrBodyMark->GetBodyMarkKnobValue();

  //size
  BodyMark::SIZE size = m_ptrBodyMark->GetBodyMarkSize();
  KnobMenu::GetInstance()->SetValue(BDMK_BODYSIZE, bodyMarkSizeName[size], GetKnobRetBodyMarkSize(size));

  //color
  BodyMark::COLOR color = m_ptrBodyMark->GetProbeMarkColor();
  KnobMenu::GetInstance()->SetValue(BDMK_PROBECOLOR, bodyMarkColorName[color], GetKnobRetProbeMarkColor(color));
}

// ---------------------------------------------------------

EKnobReturn MultiFuncBodyMark::GetKnobRetBodyMarkSize(BodyMark::SIZE size) {
  EKnobReturn ret = ERROR;

  if(size == BodyMark::SMALL) {
    ret = MIN;
  } else if(size == BodyMark::BIG) {
    ret= MAX;
  } else {
    ret = OK;
  }

  return ret;
}

EKnobReturn MultiFuncBodyMark::GetKnobRetProbeMarkColor(BodyMark::COLOR color) {
  EKnobReturn ret = ERROR;

  if(color == BodyMark::WHITE) {
    ret = MIN;
  } else if(color == BodyMark::BLUE) {
    ret = MAX;
  } else {
    ret = OK;
  }

  return ret;
}

void MultiFuncBodyMark::BeginMoveProbeMark() {
  if(m_this != NULL) {
    m_this->m_bodyOpr = MOVE_PROBE;

    InvisibleCursor(TRUE);
  }
}

void MultiFuncBodyMark::EndMoveProbeMark() {
  m_this->m_bodyOpr = ADD;

  InvisibleCursor(FALSE);
}

void MultiFuncBodyMark::EndMoveBodyMark() {
  m_this->m_bodyOpr = ADD;

  InvisibleCursor(FALSE);

  if (m_ptrBodyMark != NULL) {
    m_ptrBodyMark->MoveBodyEnd(m_active);
  }
}

void MultiFuncBodyMark::UndoMoveBodyMark() {
  m_this->m_bodyOpr = ADD;

  InvisibleCursor(FALSE);

  if (m_ptrBodyMark != NULL) {
    m_ptrBodyMark->UndoMoveBody(m_active);
  }
}

void MultiFuncBodyMark::AddNewBodyMark(GdkPixbuf* pixbuf) {
  if (m_ptrBodyMark != NULL) {
    ModeStatus ms;
    Format2D::EFormat2D format2D = ms.GetFormat2D();
    FormatCfm::EFormatCfm formatCfm = ms.GetFormatCfm();

    if ((ms.IsD2Mode() && format2D == Format2D::BB)
      || ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::BB)) {
      // BB Mode

      if(m_active == 0) {
        m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = 0;
        m_bodyPosRange.y = 0;
      } else {
        m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = IMG_W / 2;
        m_bodyPosRange.y = 0;
      }

      m_bodyPosRange.width = IMG_W / 2;
      m_bodyPosRange.height = IMG_H;
    } else if ((ms.IsD2Mode() && format2D == Format2D::B4)
      || ((ms.IsCFMMode() || ms.IsPDIMode()) && formatCfm == FormatCfm::B4)) {
      // 4B Mode

      if(m_active == 0) {
        m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = 0;
        m_bodyPosRange.y = 0;
      } else if(m_active == 1) {
        m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H / 2 - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = IMG_W / 2;
        m_bodyPosRange.y = 0;
      } else if(m_active == 2) {
        m_bodyPos[m_active].x = IMG_W / 2 - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = 0;
        m_bodyPosRange.y = IMG_H / 2;
      } else {
        m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
        m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
        m_bodyPosRange.x = IMG_W / 2;
        m_bodyPosRange.y = IMG_H / 2;
      }

      m_bodyPosRange.width = IMG_W / 2;
      m_bodyPosRange.height = IMG_H / 2;
    } else {
      // Single B Mode

      m_active = 0;
      m_bodyPos[m_active].x = IMG_W - BDMK_W * BDMK_MAX_SCALE;
      m_bodyPos[m_active].y = IMG_H - BDMK_H * BDMK_MAX_SCALE;
      m_bodyPosRange.x = 0;
      m_bodyPosRange.y = 0;
      m_bodyPosRange.width = IMG_W;
      m_bodyPosRange.height = IMG_H;
    }

    m_ptrBodyMark->DisplayNewBodyMark(pixbuf, m_bodyPos[m_active], m_probePos[m_active], m_bodyPosRange, m_active, TRUE);
  }
}

void MultiFuncBodyMark::GetBodyMarkSize(int& w, int& h) {
  double scale = m_ptrBodyMark->GetBodyMarkScale();

  w = BDMK_W * scale;
  h = BDMK_H * scale;
}

void MultiFuncBodyMark::ReviseProbePos(double scale) {
  int w = 0;
  int h = 0;

  m_probePosOffset.x = m_probePosOffset.x * scale;
  m_probePosOffset.y = m_probePosOffset.y * scale;

  GetBodyMarkSize(w, h);

  if (m_probePosOffset.x > w) {
    m_probePosOffset.x = w / 2;
  }

  if (m_probePosOffset.y > h) {
    m_probePosOffset.y = h / 2;
  }

  m_probePos[m_active].x = m_probePosOffset.x;
  m_probePos[m_active].y = m_probePosOffset.y;

  if (m_ptrBodyMark != NULL) {
    m_ptrBodyMark->SetProbeMarkPos(m_probePos[m_active], m_active);
  }
}

void MultiFuncBodyMark::Esc() {
  if (m_bodyOpr == MOVE_BODY) {
    EndMoveBodyMark();
  }

  g_keyInterface.Pop();
  MultiFuncUndo();
  CUpdateBodyMark::ExitBodyMark();
}

void MultiFuncBodyMark::Hide() {
  m_bodyOpr = IDEL;

  InvisibleCursor(FALSE);
  m_ptrBodyMark->HideBodyMark();
}

bool MultiFuncBodyMark::Clicked() {
  EndMoveProbeMark();
  m_timer = 0;

  return FALSE;
}

bool MultiFuncBodyMark::DClicked() {
  if (m_ptrBodyMark != NULL) {
    m_bodyOpr = MOVE_BODY;
    m_ptrBodyMark->MoveBodyBegin(m_active);
    InvisibleCursor(FALSE);

    double scale = m_ptrBodyMark->GetBodyMarkScale();
    int x = IMG_AREA_X + IMAGE_X + m_bodyPos[m_active].x + BDMK_W * scale / 2;
    int y = IMG_AREA_Y + IMAGE_Y + m_bodyPos[m_active].y + BDMK_H * scale / 2;

    SetSystemCursor(x, y);
    ChangeSystemCursor(GDK_HAND1);
  }

  return TRUE;
}

POINT MultiFuncBodyMark::GetPointProbe(int offsetX, int offsetY) {
  double scaleProbe = m_ptrBodyMark->GetProbeMarkScale();
  int probeW = BDMK_PROBE_LEN * scaleProbe / 2;
  int w = 0;
  int h = 0;

  GetBodyMarkSize(w, h);
  PRINTF("-------w= %d, h = %d, offsetx = %d, offsety = %d\n", w, h, offsetX, offsetY);

  if ((m_probePosOffset.x + offsetX) >= probeW && (m_probePosOffset.x + offsetX) <= (w - probeW)) {
    m_probePosOffset.x += offsetX;
  }

  offsetY = - offsetY;
  if ((m_probePosOffset.y + offsetY) >= probeW && (m_probePosOffset.y + offsetY) <= (h - probeW)) {
    m_probePosOffset.y += offsetY;
  }

  m_probePos[m_active].x = m_probePosOffset.x;
  m_probePos[m_active].y = m_probePosOffset.y;

  return m_probePos[m_active];
}

POINT MultiFuncBodyMark::GetPointBody(int offsetX, int offsetY) {
  int w = 0;
  int h = 0;
  GetBodyMarkSize(w, h);

  if ((m_bodyPos[m_active].x + offsetX) > m_bodyPosRange.x &&
    (m_bodyPos[m_active].x + offsetX) <= m_bodyPosRange.x + m_bodyPosRange.width - w) {
    m_bodyPos[m_active].x += offsetX;
  }

  offsetY = -offsetY;
  if ((m_bodyPos[m_active].y + offsetY) > m_bodyPosRange.y &&
    (m_bodyPos[m_active].y + offsetY) <= (m_bodyPosRange.y + m_bodyPosRange.height - h)) {
    m_bodyPos[m_active].y += offsetY;
  }

  return m_bodyPos[m_active];
}

void MultiFuncBodyMark::Value(EKnobOper opr) {
  if (m_ptrBodyMark != NULL) {
    m_ptrBodyMark->ChangeProbeMarkDir(opr, m_active);
  }
}
