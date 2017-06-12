// -*- c++ -*-
#ifndef TOPAREA_H
#define TOPAREA_H

#include <gtk/gtk.h>
#include <string>

using std::string;

/*
 *      Logo |    Hospital        |  Probe     |  TIS |  Time
 *           |                    |            |      |
 *    --------------------------------------------------------------
 *	  0      P1                   P2           P3     P4          844
 */
#define TOP_AREA_P1 130
#define TOP_AREA_P2 260
#define TOP_AREA_P3 614
#define TOP_AREA_P4 694

class TopArea {
public:
    ~TopArea();
    static TopArea * GetInstance();

    GtkWidget* Create(void);
    void UpdateHospitalName(const string name);

    void UpdatePatInfo(const char *name, const char *sex, const char *age, const char *id);

    // void UpdatePatInfo(const char *name, const char *sex, const char *age, const char *id);
    void UpdateProbeType(const char *type);
    void UpdateFreq(const char *freq);
    void UpdateCheckPart(const char *part);
    void UpdateDepth(int depth);
    void UpdateMI(double MI);
    void UpdateTIS(double TIS);
    void DrawDateTime(void);
    void SetReadImg(bool status);
    bool GetReadImg() {
        return m_inReadImg;
    }
    void DrawSnap(GdkPixbuf *pixbuf, int src_x, int src_y, int width, int height);
    void AddTimeOut();
    void DelTimeOut();
    void GetTIS(std::string& TIS);
    string GetCheckPart();
    void GetDepth(int& Depth);
    string GetHospitalName();
    void SetDateFormat(int DateFormat) {
        m_dateFormat = DateFormat;
    }
    std::string GetProbeType(void) {
        return m_probeType_old;
    }

    void UpdateImageParam(std::string param);

private:
    TopArea();
    void UpdateSysInfo(void);
    void DrawString(const char *str, int x, int y, GdkColor *color, PangoFontDescription *font);
    void DrawLogo(void);
    void ClearArea(int x, int y, int width, int height);
    void UpdateTopArea(void);

    static TopArea* m_ptrInstance;
    GtkWidget *m_topArea;
    GdkPixmap *m_pixmapTop;
    GdkPixmap *m_pixmapTopBak;
    bool m_inReadImg;
    guint m_timeout;

    string m_probeType_old;
    string m_probeType;
    char m_freq[30];
    char m_checkPart[30];
    // std::string m_hospital;
    char m_hospital[256];

    int m_depth;
    double m_MI;
    double m_TIS;
    // float tis;
    int m_dateFormat;

    std::string m_image_param;

//signal handle
    void TopAreaConfigure(GtkWidget *widget, GdkEventConfigure *event);
    void TopAreaExpose(GtkWidget *widget, GdkEventExpose *event);
//signal connect
    static gboolean HandleTopAreaConfigure(GtkWidget *widget, GdkEventConfigure *event, TopArea *data) {
        data->TopAreaConfigure(widget, event);
        return FALSE;
    }
    static gboolean HandleTopAreaExpose(GtkWidget *widget, GdkEventExpose *event, TopArea *data) {
        data->TopAreaExpose(widget, event);
        return FALSE;
    }
};

inline void TopArea::UpdateTopArea(void) {
    gtk_widget_queue_draw(m_topArea);
}
#endif
