#ifndef __ARROW_H_
#define __ARROW_H_

#include <deque>
#include "AbsUpdateArrow.h"
#include "Def.h"

using namespace std;

class CArrow {
public:
  static void DeleteAllForFreeze();

public:
  CArrow(CAbsUpdateArrow& updateArrow, POINT initPos);
  virtual ~CArrow();

public:
  enum COLOR {
    WHITE, GRAY, RED, GREEN, YELLOW, BLUE
  };

  enum SIZE {
    SMALL, MID, BIG
  };

  enum SHAPE {
    SIMPLEARROW, HOLLOWARROW
  };

  enum DIRECTION {
    DI0,  DI1,  DI2,  DI3,  DI4,  DI5,  DI6,  DI7,  DI8,  DI9,
    DI10, DI11, DI12, DI13, DI14, DI15, DI16, DI17, DI18, DI19,
    DI20, DI21, DI22, DI23
  };

  enum {
    DIRECTION_MAX = 24
  };

public:
  void SetArrowPos(POINT pos);
  void SetArrowDir(DIRECTION direction);
  void SetArrowShape(SHAPE shape);
  void SetArrowSize(SIZE size);
  void SetArrowColor(COLOR color);

  POINT GetArrowPos();
  DIRECTION GetArrowDir();
  SHAPE GetArrowShape();
  SIZE GetArrowSize();
  COLOR GetArrowColor();

  bool ArrowListEmpty();

  virtual bool Add();
  virtual bool Delete();
  virtual bool DeleteAll();
  virtual bool MoveBegin();
  virtual bool MoveEnd();
  virtual bool UndoMove();

protected:
    unsigned int GetDistance(POINT pos1, POINT pos2);

protected:
    typedef struct tagArrowElem {
      POINT pos;
      DIRECTION direction;
      SIZE size;
      COLOR color;
      SHAPE shape;
    } ArrowElem, *PArrowElem;

    static SIZE m_curSize;
    static COLOR m_curColor;
    static SHAPE m_curShape;

    static deque<ArrowElem> m_arrowList;

protected:
  POINT m_curPos;
  DIRECTION m_curDir;

  CAbsUpdateArrow& m_updateArrow;

private:
    static const double ARROW_SCALE[3];

private:
  enum {
    MAXDIS = 15
  };


  POINT m_initPos;
  SIZE m_sizeT;
  COLOR m_colorT;
  SHAPE m_shapeT;

  bool m_bDrawed;
  bool m_bMoving;
  deque<ArrowElem>::iterator m_pFind;
};

#endif
