// -*- c++ -*-
#ifndef _HINT_AREA_H_
#define _HINT_AREA_H_

#define HINT_AREA_W 844 //ModLayout 844 PreLayout 804
#define HINT_AREA_H 40+6

class HintArea
{
public:
    ~HintArea();
    static HintArea *GetInstance();
    GtkWidget * Create(void);
    void UpdateHint(const char *text, int timeout=0); 
    void ClearHint(void);
    
    guint m_timeout;
private:
    HintArea();
    void UpdateHintArea(void);
    void DrawString(const char *str, int x, int y, GdkColor *color, PangoFontDescription *font);

    static HintArea* m_ptrInstance;
    GtkWidget *m_hintArea;
    GdkPixmap *m_pixmapHint;
    int m_width;
    int m_height;

//signal handle
    void HintAreaConfigure(GtkWidget *widget, GdkEventConfigure *event);
    void HintAreaExpose(GtkWidget *widget, GdkEventExpose *event);
//signal connect
    static gboolean HandleHintAreaConfigure(GtkWidget *widget, GdkEventConfigure *event, HintArea *data)
	{ data->HintAreaConfigure(widget, event); return FALSE; }
    static gboolean HandleHintAreaExpose(GtkWidget *widget, GdkEventExpose *event, HintArea *data)
	{ data->HintAreaExpose(widget, event); return FALSE; }
};

inline void HintArea::UpdateHintArea(void)
{
    gtk_widget_queue_draw(m_hintArea);
}

#endif
