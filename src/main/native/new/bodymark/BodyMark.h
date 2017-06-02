#ifndef __BODY_MARK_H__
#define __BODY_MARK_H__

#include "utils/Utils.h"

#include "bodymark/UpdateBodyMark.h"

class BodyMark {
public:
  enum SIZE {
    SMALL, MID, BIG
  };

  enum COLOR {
    WHITE, GRAY, RED, GREEN, YELLOW, BLUE
  };

  enum {
    MAX_DIRECTION = 24
  };

public:
  static void CheckBodyMarkPos(guint index);
  static void SetProbeMarkColor(COLOR color, guint index);
  static POINT SetBodyMarkSize(SIZE size, guint index);

  static void HideBodyMark();
  static bool GetBodyMarkStatus(guint index);

public:
  BodyMark(POINT bodyMarkPos, POINT probeMarkPos);
  ~BodyMark();

public:
  void DisplayNewBodyMark(GdkPixbuf* pixbuf, POINT bodyPos, POINT probePos, GdkRectangle range, guint index, bool firstDraw = FALSE);

  double GetBodyMarkScale();
  double GetProbeMarkScale();
  SIZE GetBodyMarkSize();
  COLOR GetProbeMarkColor();

  void SetProbeMarkPos(POINT pos, guint index);
  bool ChangeProbeMarkDir(EKnobOper opr, guint index);

  void MoveBodyBegin(guint index);
  void MoveBodyEnd(guint index);
  void MoveBody(POINT posBody, POINT posProbe, guint index);
  void UndoMoveBody(guint index);
  void GetBodyMarkKnobValue();

private:
    static BodyMark* m_this;

    static const double BODYMARK_SCALE[3];
    static const double PROBEMARK_SCALE[3];

    static SIZE m_bodySize;
    static SIZE m_probeSize;
    static COLOR m_bodyColor;
    static COLOR m_probeColor;

    static POINT m_bodyPos[4];          // left-up corner of bodymark
    static POINT m_probePos[4];         // offset to bodyPos(left=up corner)
    static GdkRectangle m_bodyPosRange; // range of bodymark's pos
    static GdkPixbuf* m_bodyPixbuf;
    static int m_probeDir;

    static CUpdateBodyMark m_update;
    static bool m_bodyMarkOn[4];
    static bool m_bodyMarkMove;
    static POINT m_bodyPosBak;
    static POINT m_probePosBak;
};

#endif
