#ifndef __IMAGE_AREA_H__
#define __IMAGE_AREA_H__

#include "utils/MainWindowConfig.h"
#include "utils/Utils.h"

#include <opencv/cv.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define MEASURE_RES_LINES_MAX 13

class ImageArea {
public:
  static ImageArea* GetInstance();

public:
  ~ImageArea();

  enum DrawArea {
    SYMBOL, SPECTRA, PARA, PIXMAP, IMT
  };

  union DrawMode {
    bool cvDraw; 		      // OpenCV draw or eraser;
    GdkFunction gdkMode;	// GDK drawing function
  };

  struct DrawAttr {
    DrawArea area;
    DrawMode mode;
  };

  struct RectArea {
    int x;
    int y;
    int w;
    int h;
  };

  GtkWidget* Create(const int width, const int height);

public:
  static unsigned char m_bitsImg[CANVAS_AREA_W * CANVAS_AREA_H * CANVAS_BPP];
  static unsigned char m_TpbitsIMT[CANVAS_AREA_W * CANVAS_AREA_H * CANVAS_BPP];

  static unsigned char m_bitsEFOV[IMAGE_AREA_W * IMAGE_AREA_H * CANVAS_BPP];
  static unsigned char m_bitsIMT[IMAGE_AREA_W * IMAGE_AREA_H * CANVAS_BPP];
  static unsigned char m_bitsLastReplay[IMAGE_AREA_W * IMAGE_AREA_H * CANVAS_BPP];
  static unsigned char m_bitsMix[IMAGE_AREA_W * IMAGE_AREA_H * CANVAS_BPP];
  static unsigned char m_bitsReplayMix[IMAGE_AREA_W * IMAGE_AREA_H * CANVAS_BPP];

private:
  // signal

  static gboolean signal_configure_event_imagearea(GtkWidget* widget, GdkEventConfigure* event, ImageArea* data) {
    if (data != NULL) {
      data->ImageAreaConfigure(widget, event);
    }

    return FALSE;
  }

  static gboolean signal_expose_event_imagearea(GtkWidget* widget, GdkEventExpose* event, ImageArea* data) {
    if (data != NULL) {
      data->ImageAreaExpose(widget, event);
    }

    return FALSE;
  }

  void ImageAreaConfigure(GtkWidget* widget, GdkEventConfigure* event);
  void ImageAreaExpose(GtkWidget* widget, GdkEventExpose* event);

private:
  ImageArea();

private:
  static ImageArea* m_instance;
  static GtkWidget* m_image_da;

  static RectArea m_spectraArea;
  static RectArea m_symbolArea;

  static int m_cineRemoveImg;

private:
  IplImage* m_imageIMT;
  IplImage* m_imagePara;
  IplImage* m_imagePreBox;
  IplImage* m_imageSpectra;
  IplImage* m_imageSymbol;

  GdkPixbuf* m_IMTPixbuf;
  GdkPixbuf* m_lastReplayPixbuf;
  GdkPixbuf* m_mixPixbuf;
  GdkPixbuf* m_mixReplayPixbuf;
  GdkPixbuf* m_frmPixbuf;

  GdkPixmap* m_pixmapFps;
  GdkPixmap* m_pixmapHistogram;

  GdkPixmap* m_pixmapArea;
  GdkPixmap* m_pixmapReplayBar;
  GdkPixmap* m_pixmapPwTrace;
  GdkPixmap* m_pixmapMeaResult;

  GdkPixmap* m_pixmapBDMK[4];

  int m_meaResultFontSize;

public:





    static void DrawImgData(void* pBits, int nWidth, int nHeight);
    static void DrawImgDataFreeze(void* pBits, int nWidth, int nHeight);
    static void CopyImgDataToReplay();


    inline void UpdateImgArea();
    void ClearArea(bool update = false);
    void ClearScreen(bool update=true);
    void ClearScreenUnFreeze(bool update = false);
    static void UpdateSymbolArea(int x, int y, int width, int height);
    static void UpdateSpectraArea(int x, int y, int width, int height);
    void UpdateMeaResultArea(int fontsize);
    void DrawCFMPreBox(const RectArea& area);
    void ClearCFMPreBox(const RectArea& area);

    void SetReadImg(bool status);
    void ResetReadImg();
    void GetImgPtr(unsigned char ** image);//获取图像区(m_bits)的首地址
    GdkPixbuf* GetDrawingAreaImage();
    void PutDrawingArea(GdkPixbuf *pixbuf);
    void GetFrmData(guchar **pBits, int *nWidth, int *nHeight);
    void LoadFrm();
    void GetSnapData(guchar **pBits, int *nWidth, int *nHeight);
    void DrawSnap(GdkPixbuf *pixbuf, int src_x, int src_y, int width, int height);
    gboolean GetReadImg() {
        return m_inReadImg;
    }

    GdkGC* NewHistogramDC(GdkColor * color, GdkFunction mode);
    void DrawHistogram(GdkGC *gc, int x1, int y1, int x2, int y2);

    GdkGC* GetPixmapDC(GdkColor* color, GdkFunction mode);
    void DrawPixmapLine(GdkGC *gc, int x1, int y1, int x2, int y2);
    // void DrawLine(int x1, int y1, int x2, int y2, bool draw, const GdkColor* const color, DrawArea area);
    // void DrawLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    // void DrawMeasureLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    // void DrawLine(POINT p, int angle, int length, bool draw, const GdkColor* const color);
    // void DrawLineNoUpdate(const GdkColor* const color, int x1, int y1, int x2, int y2);
    void DrawLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true);

    // void DrawPoint(GdkFunction mode, const GdkColor* const color, int x, int y);
    // void DrawPoint(int x, int y, bool draw, const GdkColor* const color);
    // void DrawScalePoint(int x, int y, bool draw, const GdkColor* const color);
    // void DrawMeasurePoint(GdkFunction mode, const GdkColor* const color, int x, int y);
    void DrawPoint(DrawAttr &attr, const GdkColor* const color, int x, int y, bool update=true);

    // void DrawCircle(int x, int y, int radius, bool filled, bool draw, const GdkColor* const color);
    // void DrawCircle(GdkFunction mode, const GdkColor* const color, int x, int y, int radius, gboolean filled);
    void DrawCircle(DrawAttr &attr, const GdkColor* const color, int x, int y, int radius, bool filled, bool update=true);

    // void DrawArc(int x, int y, int width, int height, double angle1, double angle2, bool draw, const GdkColor* const color);
    void DrawArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update=true);
    void DrawDashArc(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, double angle1, double angle2, bool update=true);

    // void DrawRectangle(int x, int y, int width, int height, bool filled, const GdkColor* const color);
    // void DrawRectangle(GdkFunction mode, const GdkColor* const color, gboolean filled, int x, int y, int width, int height);
    void DrawRectangle(DrawAttr &attr, const GdkColor* const color, int x, int y, int width, int height, bool filled, bool update=true);

    // void DrawPolygon(GdkPoint *points, int nPoint, bool draw, const GdkColor* const color);
    void DrawPolygon(DrawAttr &attr, const GdkColor* const color, GdkPoint *points, int nPoint, bool update=true);

    // void DrawDashLine(int x1, int y1, int x2, int y2, bool draw, const GdkColor* const color);
    // void DrawMeasureDashLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2, DotSize size=MID);

    void DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true, int distance=10);
    void DrawDashLine(DrawAttr &attr, const GdkColor* const color, int x1, int y1, int x2, int y2, int size, bool update=true);

    void ClearReplayBar();
    void DrawReplayBar(int cur_img, int total_img, int left, int right);

    GdkGC* NewPwTraceGC(GdkColor *color, GdkFunction mode);
    void DrawTrace(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2);
    void DrawTracePt(GdkFunction mode, const GdkColor* const color, int x, int y);
    void DrawTraceTag(GdkFunction mode, const GdkColor* const color, int x, int y, bool update=false);
    void ClearTrace();
    void SetTraceDraw(const bool drawPwTrace) {
        m_drawPwTrace = drawPwTrace;
    }

    void DrawProbeMark(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex, guint index);

    // void DrawCursor(GdkFunction mode, int type, int size, const GdkColor* const color, int x, int y);
    void DrawMeaCursor(GdkFunction mode, int type, int size, const GdkColor* const color, int x, int y);
    // measure result area
    void DrawMeasureResult(const char *result, int pos, int lines, int cursorType, const GdkColor* const color, bool update=true);

    int GetMeasurePosSize();
    void ClearLastMeasureResult();
    void ClearMeasureAll();

    // Draw Measure Ellipse only
    GdkGC* NewMeasureGC(GdkColor *color, GdkFunction mode);
    void DrawPixmapPt(GdkGC *gc, int x, int y);

    void DrawNoteText(const char *str, int x, int y, const GdkColor* color, int font_size);
    void DrawString(GdkGC *gc, int x, int y, const char *str);
    void DrawString(const char *str, int x, int y, GdkColor* color = NULL);
    void DrawString(const char *str, int x, int y, const GdkColor* color, bool xorMode);

    void DrawStringFps(const char *str, int x, int y, GdkColor* color = NULL);

    void DrawBodyMark(int x, int y, GdkPixbuf* pixbuf, double scale, guint colorIndex, guint index);
    void HideBodyMark();
    void ClearBodyMark(guint index);
    void RedrawBodyMarkWhenModeChanged();
    void ChangeCurrentBDMK(guint index);

    void DrawMagnifier(int x, int y);

    void DrawArrowHollow(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex);
    void DrawArrowSimple(int x, int y, guint direction, guint directionMax, double scale, guint colorIndex);
    void DrawArrow(POINT head, POINT tail, bool draw, const GdkColor* const color, bool update=true);

    void AddTimeOutFps();
    void DelTimeOutFps();
    void ClearUpdateCounts();

    gboolean UpdateFps();
    static void SetCineRemoveImg(int count);

    void EFOVZoomMiniatureBox(int start_x, int start_y, int end_x, int end_y);
    void ClearEFOVSpeedbar();
    void DrawEFOVSpeedbar(int cur, int total, float speed);
    void ClearEFOVImg();

    void DrawBiopsyLine(GdkFunction mode, const GdkColor* const color, int x1, int y1, int x2, int y2, bool update=true, int distance=10);



private:

    void DrawMeasureResultCursor(GdkGC *gc, int type, const GdkColor* const color, int x, int y);
    void DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color);
    void DrawFTBitmap(FT_Bitmap* bitmap, int x, int y, const GdkColor* const color, bool xorMode);





    PangoFontDescription *m_baseFont;

    GdkPixbuf* m_imgPixbuf;

    GdkPixbuf* m_snapPixbufBak;

    //static unsigned char m_bitsImg[IMAGE_W*IMAGE_H*IMG_BPP];
    //used for mix when in EFOV

    static int m_counts;
    bool m_drawPwTrace;






    GdkGC* m_gcTrace;
    GdkGC* m_gcMeaResult;
    PangoLayout* m_layout;

    bool m_inReadImg;
    bool m_inBDMK;
    GdkPoint m_bdmkPos[4];
    int m_bdmkWidth[4];
    int m_bdmkHeight[4];
    guint m_bdmkLast;

    struct MeasurePos {
        int posY;
        int lines;
        int width;
    };
    std::vector<MeasurePos> m_meaResultPos;
    int m_meaResultLineH; //测量结果每行高度（像素），通过pango_layout_get_size()得到，注意：调用该函数时如果layout中无字符，得到的结果可能不对
    struct RectArea m_meaResultRect;
    int m_curCountLines;

    guint m_tmFps;


    int m_slipPosEFOV;
    bool m_speedbarDraw;



};

void ImageArea::UpdateImgArea() {
  gtk_widget_queue_draw(m_image_da);
}

#endif
