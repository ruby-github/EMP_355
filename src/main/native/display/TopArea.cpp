#include <gtk/gtk.h>
#include <time.h>

#include "Def.h"
#include "display/TopArea.h"
#include "display/gui_func.h"
#include "display/gui_global.h"
#include "sysMan/SysGeneralSetting.h"
#include "probe/ProbeMan.h"

#include "utils/MainWindowConfig.h"

using std::string;

TopArea* TopArea::m_ptrInstance = NULL;

TopArea* TopArea::GetInstance() {
    if (m_ptrInstance == NULL)
        m_ptrInstance = new TopArea;
    return m_ptrInstance;
}

TopArea::TopArea(void) {
    m_pixmapTop = 0;
    sprintf(m_freq, " ");
    m_probeType = " ";
    m_probeType_old = " ";
    sprintf(m_checkPart, " ");
#ifdef VET
    sprintf(m_hospital, " ");
#endif

    m_depth = 0;
    m_MI = 0.0;
    m_TIS = 0.0;
    m_inReadImg = false;
    m_timeout = 0;

    m_image_param = "";

    SysGeneralSetting sys;
    m_dateFormat = sys.GetDateFormat();
}

TopArea::~TopArea() {
    if (m_ptrInstance != NULL)
        delete m_ptrInstance;
}

void TopArea::DrawLogo(void) {
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file((string(CFG_RES_PATH) + string("res/logo.png")).c_str(), NULL);
    GdkPixbuf* pixbuf_scale = gdk_pixbuf_scale_simple(pixbuf, 120, 50, GDK_INTERP_BILINEAR);

    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    gdk_draw_pixbuf(m_pixmapTop, gc, pixbuf_scale, 0, 0, 0, 5, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
    g_object_unref(gc);

    g_object_unref(pixbuf_scale);
    g_object_unref(pixbuf);
}

void TopArea::UpdateHospitalName(const char *name) {
    const int y = 10;
    // sprintf(m_hospital, "%s", name);

    PRINTF("hospital name====%s\n",name);
    //m_hospital = name;
    sprintf(m_hospital, "%s", name);
    ClearArea(TOP_AREA_P1 + 5, y, 110, 45);
    PangoLayout *layout = gtk_widget_create_pango_layout(m_topArea, name);
    int font_size = 24;
    PangoFontDescription* font = AdaptStringFont("WenQuanYi Zen Hei", "bold", font_size, layout, name, 110);
    g_object_unref(layout);

    DrawString(name, TOP_AREA_P1 + 5, y, g_white, font);
    pango_font_description_free(font);
}
#ifdef VET
void TopArea::UpdatePatInfo(const char *animal_name, const char *owner_name, const char *species, const char *sex, const char *age, const char *id) {
    const int x = TOP_AREA_P2 + 5;
    const int y = 5;

    char text_buf[256];
    char *text_animal_name = _("Animal:");
    char *text_owner_name = _("Owner:");
    char *text_species = _("Species:");
    char *text_sex = _("Gender:"); //_("Sex:");
    char *text_age = _("Age:");
    char *text_id = _("ID:");

    sprintf(text_buf, "%s%s  %s%s  %s%s  %s%s  %s%s  %s%s", text_animal_name, animal_name, text_owner_name, owner_name, text_species, species, text_sex, sex, text_age, age, text_id, id);
    ClearArea(x, y, 424, 20);

    int font_size = 12;
    PangoLayout *layout = gtk_widget_create_pango_layout(m_topArea, text_buf);
    PangoFontDescription* font = AdaptStringFont(FONT_STRING, "", font_size, layout, text_buf, 424);
    g_object_unref(layout);

    DrawString(text_buf, x, y, g_white, font);
    pango_font_description_free(font);
}
#else
void TopArea::UpdatePatInfo(const char *name, const char *sex, const char *age, const char *id) {
    const int x = TOP_AREA_P2 + 5;
    const int y = 5;

    char text_buf[256];
    char *text_name = _("Name:");
    //char *text_sex = _("Sex:");
    char *text_sex = _("Gender:");
    char *text_age = _("Age:");
    char *text_id = _("ID:");

    sprintf(text_buf, "%s %s    %s %s    %s %s    %s %s", text_name, name, text_sex, sex, text_age, age, text_id, id);
    ClearArea(x, y, 424, 20);

    PangoLayout *layout = gtk_widget_create_pango_layout(m_topArea, text_buf);
    int font_size = 12;
    PangoFontDescription* font = AdaptStringFont("DejaVu Sans", "Book", font_size, layout, text_buf, 424);
    g_object_unref(layout);

    DrawString(text_buf, x, y, g_white, font);
    pango_font_description_free(font);
}
#endif
void TopArea::UpdateProbeType(const char *type) {
    m_probeType_old = type;
    m_probeType = ProbeMan::GetInstance()->VerifyProbeName(type);
    UpdateSysInfo();
}

void TopArea::UpdateCheckPart(const char *part) {
    sprintf(m_checkPart, "%s", part);
    UpdateSysInfo();
}

void TopArea::UpdateFreq(const char *freq) {
    sprintf(m_freq, "%s", freq);
    UpdateSysInfo();
}

void TopArea::UpdateDepth(int depth) {
    m_depth = depth;
    UpdateSysInfo();
}

void TopArea::UpdateMI(double MI) {
    m_MI = MI;
    UpdateSysInfo();
}

void TopArea::UpdateTIS(double TIS) {
    m_TIS = TIS;
    UpdateSysInfo();
}

void TopArea::UpdateSysInfo(void) {
    char probe_info[100];
    char other_info[100];
    string probe_type;
    if (m_probeType.empty()) {
        probe_type = _("No Probe");
    } else {
        probe_type = m_probeType;
    }

    if (m_image_param.empty()) {
        sprintf(probe_info, "%s    %s", probe_type.c_str(), m_checkPart);
    } else {
        sprintf(probe_info, "%s    %s    %s", m_image_param.c_str(), probe_type.c_str(), m_checkPart);
    }

    //sprintf(probe_info, "%s /%3dmm\n%s", probe_type.c_str(), m_depth, m_checkPart);
    ClearArea(TOP_AREA_P2+5, 35, TOP_AREA_P3-TOP_AREA_P2-10, 20); //LL
    DrawString(probe_info, TOP_AREA_P2+5, 35, g_white, NULL);

    //tis = m_TIS;
    float tis = m_TIS;
#if (defined (EMP_322) || defined(EMP_313))
    if (tis < 0.0001) {
        sprintf(other_info, "          ");
    } else {
        if (m_probeType.empty()) {
            sprintf(m_freq, "   ");
        } else {
            sprintf(m_freq,"   ");
        }
        tis = (int)(tis / 0.2) * 0.2;

        if (tis < 0.4) {
            sprintf(other_info, "%s\nTIS < 0.4", m_freq);
        } else {
            sprintf(other_info, "%s\nTIS   %.1f", m_freq, tis);
        }
    }
#else
    if (tis < 0.0001) {
        sprintf(other_info, "          ");
    } else {
        tis = (int)(tis / 0.2) * 0.2;

        if (tis < 0.4)
            sprintf(other_info, "TIS < 0.4");
        else
            sprintf(other_info, "TIS   %.1f", tis);
    }
#endif
    ClearArea(TOP_AREA_P3+5, 35, TOP_AREA_P4-TOP_AREA_P3-10, 20); //LL
    DrawString(other_info, TOP_AREA_P3+5, 35, g_white, NULL);
}

void TopArea::GetTIS(string& TIS) {
    char TempTis[30];
    memset(TempTis,0,30);
    float tisTemp = m_TIS;
    if (tisTemp < 0.0001) {
        sprintf(TempTis, "          ");
    } else {
        tisTemp = (int)(tisTemp / 0.2) * 0.2;

        if (tisTemp < 0.4)
            sprintf(TempTis, "TIS < 0.4  ");
        else
            sprintf(TempTis, "TIS   %.1f  ", tisTemp);
    }

    // sprintf(TempTis,"%.1f",m_TIS);
    //sprintf(TempTis,"%.1f",);
    TIS = TempTis;
}

string TopArea::GetCheckPart() {
  return m_checkPart;
}

void TopArea::GetDepth(int& Depth) {
    Depth = m_depth;
}

string TopArea::GetHospitalName() {
    return m_hospital;
}

void TopArea::DrawDateTime(void) {
    time_t now;
    struct tm *now_tm;
    char buf[50];
    char buf_week[10];

// Get Time
    time(&now);
    now_tm = localtime(&now);
    switch (now_tm->tm_wday) {
    case 0:
        sprintf(buf_week, _("Sun."));
        break;
    case 1:
        sprintf(buf_week, _("Mon."));
        break;
    case 2:
        sprintf(buf_week, _("Tues."));
        break;
    case 3:
        sprintf(buf_week, _("Wed."));
        break;
    case 4:
        sprintf(buf_week, _("Thur."));
        break;
    case 5:
        sprintf(buf_week, _("Fri."));
        break;
    case 6:
        sprintf(buf_week, _("Sat."));
        break;
    default:
        break;
    }

    switch (m_dateFormat) {
    case 0:
        sprintf(buf, "%04d-%02d-%02d %s\n%02d:%02d:%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday, buf_week, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        break;
    case 1:
        sprintf(buf, "%02d-%02d-%04d %s\n%02d:%02d:%02d", now_tm->tm_mon+1, now_tm->tm_mday, now_tm->tm_year+1900, buf_week, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        break;
    case 2:
        sprintf(buf, "%02d-%02d-%04d %s\n%02d:%02d:%02d", now_tm->tm_mday, now_tm->tm_mon+1, now_tm->tm_year+1900, buf_week, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        break;
    default:
        sprintf(buf, "%04d-%02d-%02d %s\n%02d:%02d:%02d", now_tm->tm_year+1900, now_tm->tm_mon+1, now_tm->tm_mday, buf_week, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
        break;
    }

    ClearArea(TOP_AREA_P4+10, 10, 130, 40);
    DrawString(buf, TOP_AREA_P4+10, 10, g_white, NULL);
}

GtkWidget * TopArea::Create(void) {
    m_topArea = gtk_drawing_area_new();
    gtk_drawing_area_size(GTK_DRAWING_AREA(m_topArea), TOP_AREA_W, TOP_AREA_H);
    g_signal_connect(G_OBJECT(m_topArea), "configure_event", G_CALLBACK(HandleTopAreaConfigure), this);
    g_signal_connect(G_OBJECT(m_topArea), "expose_event", G_CALLBACK(HandleTopAreaExpose), this);

    return m_topArea;
}

gboolean UpdateDateTime(gpointer data) {
    TopArea *tmp;
    tmp = (TopArea *)data;
    tmp->DrawDateTime();
    return TRUE;
}

void TopArea::AddTimeOut() {
    if(m_timeout > 0) {
        g_source_remove(m_timeout);
        m_timeout = 0;
    }

//	m_timeout = g_timeout_add_seconds(1, UpdateDateTime, this);
    m_timeout = g_timeout_add(500, UpdateDateTime, this);
}

void TopArea::DelTimeOut() {
    if(m_timeout > 0) {
        g_source_remove(m_timeout);
        m_timeout = 0;
    }
}

void TopArea::TopAreaConfigure(GtkWidget *widget, GdkEventConfigure *event) {
    if (m_pixmapTop)
        g_object_unref(m_pixmapTop);

    m_pixmapTop = gdk_pixmap_new(widget->window,
                                 widget->allocation.width,
                                 widget->allocation.height,
                                 -1);

    m_pixmapTopBak = gdk_pixmap_new(widget->window,
                                    widget->allocation.width,
                                    widget->allocation.height,
                                    -1);

    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    gdk_gc_set_foreground(gc, g_black);
    gdk_draw_rectangle(m_pixmapTop, gc, TRUE, 0, 0, widget->allocation.width, widget->allocation.height);

    // draw spacing line
    gdk_gc_set_foreground(gc, g_gray);
    gdk_draw_line(m_pixmapTop, gc, TOP_AREA_P2, 5,  TOP_AREA_P2, 58);
    gdk_draw_line(m_pixmapTop, gc, TOP_AREA_P3, 30,  TOP_AREA_P3, 58);
    gdk_draw_line(m_pixmapTop, gc, TOP_AREA_P4, 5,  TOP_AREA_P4, 58);
    gdk_draw_line(m_pixmapTop, gc, TOP_AREA_P2,   30, TOP_AREA_P4, 30);
    gdk_draw_line(m_pixmapTop, gc, 5,   58, 839, 58);
    g_object_unref(gc);

    DrawLogo();
}

void TopArea::TopAreaExpose(GtkWidget *widget, GdkEventExpose *event) {
    gdk_draw_drawable(widget->window,
                      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                      m_pixmapTop,
                      0, 0,
                      0, 0,
                      TOP_AREA_W, TOP_AREA_H);
}

void TopArea::DrawString(const char *str, int x, int y, GdkColor *color, PangoFontDescription *font) {
    int width, height;

    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    PangoLayout *layout = gtk_widget_create_pango_layout(m_topArea, str);
    if (font)
        pango_layout_set_font_description(layout, font);
    pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
    pango_layout_get_pixel_size(layout, &width, &height);
    gdk_gc_set_foreground(gc, g_black);
    gdk_draw_rectangle(m_pixmapTop, gc, TRUE, x, y, width, height);

    gdk_gc_set_foreground(gc, color);
    gdk_draw_layout(m_pixmapTop, gc, x, y, layout);

    g_object_unref(layout);
    g_object_unref(gc);

    UpdateTopArea();
}

void TopArea::SetReadImg(bool status) {
    m_inReadImg = status;

    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    if (status) {
        gdk_draw_drawable(m_pixmapTopBak,
                          gc,
                          m_pixmapTop,
                          0, 0,
                          0, 0,
                          TOP_AREA_W, TOP_AREA_H);
        DelTimeOut();
    } else {
        gdk_draw_drawable(m_pixmapTop,
                          gc,
                          m_pixmapTopBak,
                          0, 0,
                          0, 0,
                          TOP_AREA_W, TOP_AREA_H);
        AddTimeOut();
        UpdateTopArea();
    }
    g_object_unref(gc);
}

void TopArea::DrawSnap(GdkPixbuf *pixbuf, int src_x, int src_y, int width, int height) {
    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    gdk_gc_set_function(gc, GDK_COPY);

    gdk_draw_pixbuf(m_pixmapTop,
                    gc,
                    pixbuf,
                    src_x, src_y, 0, 0,
                    width, height,
                    GDK_RGB_DITHER_NORMAL, 0, 0);
    g_object_unref(gc);

    UpdateTopArea();
}

void TopArea::ClearArea(int x, int y, int width, int height) {
    GdkGC *gc = gdk_gc_new(m_pixmapTop);
    gdk_gc_set_foreground(gc, g_black);

    gdk_draw_rectangle(m_pixmapTop, gc, TRUE, x, y, width, height);
    g_object_unref(gc);
}

void TopArea::UpdateImageParam(std::string param) {
    m_image_param = param;

    UpdateSysInfo();
}
