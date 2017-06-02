#ifndef __UPDATE_BODYMARK_H__
#define __UPDATE_BODYMARK_H__

#include "utils/Utils.h"

#include "Def.h"

class CUpdateBodyMark {
public:
  static void EnterBodyMark();
  static void ExitBodyMark();

public:
  void DrawBodyMark(POINT pos, GdkPixbuf* pixbuf, double scale, int colorIndex, guint index);
  void DrawProbeMark(POINT pos, int direction, int maxDir, double scale, int colorIndex, guint index);

  void HideBodyMark();
  void ClearBodyMark(guint index);
};

#endif
