#include "bodymark/BodyMark.h"

#include "sysMan/SysNoteSetting.h"
#include "utils/FakeXUtils.h"
#include "utils/MainWindowConfig.h"

BodyMark* BodyMark::m_this = NULL;

const double BodyMark::BODYMARK_SCALE[] = {0.8, 1.2, BDMK_MAX_SCALE};
const double BodyMark::PROBEMARK_SCALE[] = {0.8, 1.0, BDMK_PROBE_MAX_SCALE};

BodyMark::SIZE BodyMark::m_bodySize = MID;
BodyMark::SIZE BodyMark::m_probeSize = MID;
BodyMark::COLOR BodyMark::m_bodyColor = WHITE;
BodyMark::COLOR BodyMark::m_probeColor = WHITE;

POINT BodyMark::m_bodyPos[4];           // left-up corner of bodymark
POINT BodyMark::m_probePos[4];          // offset to bodyPos(left=up corner)
GdkRectangle BodyMark::m_bodyPosRange;  // range of bodymark's pos
GdkPixbuf* BodyMark::m_bodyPixbuf = NULL;
int BodyMark::m_probeDir = 0;

CUpdateBodyMark BodyMark::m_update;
bool BodyMark::m_bodyMarkOn[4] = {FALSE, FALSE, FALSE, FALSE};
bool BodyMark::m_bodyMarkMove = FALSE;
POINT BodyMark::m_bodyPosBak;
POINT BodyMark::m_probePosBak;

// ---------------------------------------------------------

void BodyMark::CheckBodyMarkPos(guint index) {
  int w = gdk_pixbuf_get_width(m_bodyPixbuf) * BODYMARK_SCALE[m_bodySize];
  int h = gdk_pixbuf_get_height(m_bodyPixbuf) * BODYMARK_SCALE[m_bodySize];

  if (m_bodyPos[index].x > m_bodyPosRange.x + m_bodyPosRange.width - w) {
    m_bodyPos[index].x = m_bodyPosRange.x + m_bodyPosRange.width - w;
  }

  if(m_bodyPos[index].y > m_bodyPosRange.y + m_bodyPosRange.height - h) {
    m_bodyPos[index].y = m_bodyPosRange.y + m_bodyPosRange.height - h;
  }
}

void BodyMark::SetProbeMarkColor(COLOR color, guint index) {
  if (m_bodyMarkOn[index]) {
    // clear
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);

    // draw on orginal pos
    m_bodyColor = color;
    m_probeColor = color;
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }
}

POINT BodyMark::SetBodyMarkSize(SIZE size, guint index) {
  if (m_bodyMarkOn[index]) {
    // clear
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);

    m_probePos[index].x = m_probePos[index].x*(BODYMARK_SCALE[size]/BODYMARK_SCALE[m_bodySize]);
    m_probePos[index].y = m_probePos[index].y*(BODYMARK_SCALE[size]/BODYMARK_SCALE[m_bodySize]);

    int w = BDMK_W * BODYMARK_SCALE[size];
    int h = BDMK_H * BODYMARK_SCALE[size];
    if (m_probePos[index].x > w)m_probePos[index].x = w / 2;
    if (m_probePos[index].y > h)m_probePos[index].y = h / 2;
    // draw on orginal pos
    m_bodySize = size;
    m_probeSize = size;

    CheckBodyMarkPos(index);

    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }

  return m_bodyPos[index];
}

void BodyMark::HideBodyMark() {
  for(int i=0; i<4; i++) {
    if (m_bodyMarkOn[i]) {
      m_update.DrawBodyMark(m_bodyPos[i], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, i);
      m_update.DrawProbeMark(m_probePos[i], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, i);
      m_bodyMarkOn[i] = FALSE;
    }
  }

  m_update.HideBodyMark();

  SetSystemCursor(550, 550);
}

bool BodyMark::GetBodyMarkStatus(guint index) {
  return m_bodyMarkOn[index];
}

BodyMark::BodyMark(POINT bodyMarkPos, POINT probeMarkPos) {
  m_this = this;
}

BodyMark::~BodyMark() {
}

void BodyMark::DisplayNewBodyMark(GdkPixbuf* pixbuf, POINT bodyPos, POINT probePos, GdkRectangle range, guint index, bool firstDraw) {
  if (m_bodyMarkOn[index]) {
    m_update.ClearBodyMark(index);  // must clear to earse, not XOR to erase
  }

  if (m_bodyPixbuf != NULL) {
    g_object_unref(m_bodyPixbuf);
  }

  if (pixbuf != NULL) {
    m_bodyPixbuf = gdk_pixbuf_copy(pixbuf);
    g_object_unref(pixbuf);
  }

  if (firstDraw) {
    m_bodyPos[index] = bodyPos;
    m_probePos[index] = probePos;
    m_bodyPosRange = range;
  }

  m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
  m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);

  m_bodyMarkOn[index] = TRUE;
}

double BodyMark::GetBodyMarkScale() {
  return BODYMARK_SCALE[m_bodySize];
}

double BodyMark::GetProbeMarkScale() {
  return PROBEMARK_SCALE[m_probeSize];
}

BodyMark::SIZE BodyMark::GetBodyMarkSize() {
  return m_bodySize;
}

BodyMark::COLOR BodyMark::GetProbeMarkColor() {
  return m_probeColor;
}

// ---------------------------------------------------------

void BodyMark::SetProbeMarkPos(POINT pos, guint index) {
  if (m_bodyMarkOn[index]) {
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
    m_probePos[index] = pos;
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }
}

bool BodyMark::ChangeProbeMarkDir(EKnobOper opr, guint index) {
  if (m_bodyMarkOn[index]) {
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);

    if (opr == ADD) {
      if (m_probeDir < MAX_DIRECTION-1) {
        m_probeDir ++;
      } else {
        m_probeDir = 0;
      }
    } else if (opr == SUB) {
      if (m_probeDir > 0) {
        m_probeDir --;
      } else {
        m_probeDir = MAX_DIRECTION - 1;
      }
    } else {
      return FALSE;
    }

    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }

  return TRUE;
}

void BodyMark::MoveBodyBegin(guint index) {
  if (m_bodyMarkOn[index]) {
    m_bodyMarkMove = TRUE;
    m_bodyPosBak = m_bodyPos[index];
    m_probePosBak = m_probePos[index];
  }
}

void BodyMark::MoveBodyEnd(guint index) {
  if (m_bodyMarkOn[index]) {
    m_bodyMarkMove = FALSE;
  }
}

void BodyMark::MoveBody(POINT posBody, POINT posProbe, guint index) {
  if ((m_bodyMarkOn[index]) && (m_bodyMarkMove)) {
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
    m_bodyPos[index] = posBody;
    m_probePos[index] = posProbe;
    CheckBodyMarkPos(index);
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }
}

void BodyMark::UndoMoveBody(guint index) {
  if (m_bodyMarkOn[index]) {
    m_bodyMarkMove = FALSE;

    // clear
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);

    // draw on orginal pos
    m_bodyPos[index] = m_bodyPosBak;
    m_probePos[index] = m_probePosBak;
    m_update.DrawBodyMark(m_bodyPos[index], m_bodyPixbuf, BODYMARK_SCALE[m_bodySize], m_bodyColor, index);
    m_update.DrawProbeMark(m_probePos[index], m_probeDir, MAX_DIRECTION, PROBEMARK_SCALE[m_probeSize], m_probeColor, index);
  }
}

void BodyMark::GetBodyMarkKnobValue() {
  SysNoteSetting sns;
  m_bodySize = (SIZE)sns.GetBodyMarkSize();
  m_bodyColor = (COLOR)sns.GetBodyMarkColor();
  m_probeSize = (SIZE)sns.GetBodyMarkSize();
  m_probeColor = (COLOR)sns.GetBodyMarkColor();
}
