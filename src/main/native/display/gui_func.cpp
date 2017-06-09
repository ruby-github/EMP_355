#include "display/gui_func.h"
#include "keyboard/KeyDef.h"
#include <stdlib.h>
#include <unistd.h>
#include <gdk/gdkkeysyms.h>
#include "ViewMain.h"
#include <time.h>
#include <glib.h>
#include <iostream>
#include <math.h>
#include "display/gui_global.h"
#include "display/ImageArea.h"
#include "display/MenuArea.h"
#include "sysMan/SysGeneralSetting.h"
#include <sstream>
#include <string>
#include "periDevice/ManRegister.h"
#include "sysMan/ViewSystem.h"
#include "sysMan/SysNoteSetting.h"
#include "periDevice/DCMRegister.h"

#include "utils/FakeXUtils.h"

using std::string;
using std::cout;
using std::endl;

GdkColor* g_black;
GdkColor* g_white;
GdkColor* g_red;
GdkColor* g_green;
GdkColor* g_blue;
GdkColor* g_deep;
GdkColor* g_deepGray;
GdkColor* g_gray;
GdkColor* g_lightGray;
GdkColor* g_yellow;
GdkColor* g_lightBlue;
GdkColor* g_skyBlue;

GdkColor* new_color(guint16 red, guint16 green, guint16 blue) {
    GdkColor *color = new GdkColor;
    color->red = red;
    color->green = green;
    color->blue = blue;

    GdkColormap *colormap = gdk_colormap_get_system();
    if (gdk_colormap_alloc_color(colormap, color, FALSE, TRUE) == TRUE)
        return color;
    else
        return NULL;
}

void free_color(GdkColor *color) {
    if (color)
        delete color;
    else
        return;
}

bool init_colors(void) {
    if ((g_black = new_color(0, 0, 0)) == NULL)
        return false;
    if ((g_white = new_color(65535, 65535, 65535)) == NULL)
        return false;
    if ((g_red = new_color(65535, 0, 0)) == NULL)
        return false;
    if ((g_green = new_color(0, 65535, 0)) == NULL)
        return false;
    if ((g_blue = new_color(0, 0, 65535)) == NULL)
        return false;
    if ((g_deep = new_color(42*65535/255, 47*65535/255, 58*65535/255)) == NULL)
        return false;
    if ((g_deepGray = new_color(85*65535/255, 89*65535/255, 97*65535/255)) == NULL)
        return false;
    if ((g_gray = new_color(127*65535/255, 130*65535/255, 137*65535/255)) == NULL)
        return false;
    if ((g_lightGray = new_color(170*65535/255, 172*65535/255, 176*65535/255)) == NULL)
        return false;
    if ((g_yellow = new_color(255*65535/255, 255*65535/255, 0)) == NULL)
        return false;
    if ((g_lightBlue = new_color(173*65535/255, 216*65535/255, 230*65535/255)) == NULL)
        return false;
    if ((g_skyBlue = new_color(0, 255*65535/255, 255*65535/255)) == NULL)
        return false;

    g_deep = new_color(0, 0, 0);
    g_deepGray = new_color(0, 0, 0);

    return true;
}

void free_init_colors(void) {
    delete g_black;
    delete g_white;
    delete g_red;
    delete g_green;
    delete g_blue;
    delete g_deep;
    delete g_deepGray;
    delete g_gray;
    delete g_lightGray;
    delete g_lightBlue;
    delete g_skyBlue;
    delete g_yellow;
}

/*
 * UTF8编码常见格式
   U-00000000 – U-0000007F:  0xxxxxxx
   U-00000080 – U-000007FF:  110xxxxx 10xxxxxx
   U-00000800 – U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
   U-00010000 – U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   U-00200000 – U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   U-04000000 – U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 */
void CountUTF8(const void* pBuffer, long size, int *ascii, int *utf8) {
    int a = 0;
    int u = 0;
    unsigned char* start = (unsigned char*)pBuffer;
    unsigned char* end = (unsigned char*)pBuffer + size;

    while(start < end) {
        if (*start < 0x80) {
            a++;
            start++;
        } else {
            u++;
            start += 3;
        }
    }

    *ascii = a;
    *utf8 = u;
}

void GetCurrentDate(int &Year, int &Month, int &Day) {
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    Year = now_tm->tm_year+1900;
    Month = now_tm->tm_mon+1;
    Day = now_tm->tm_mday;
}

void GetCurrentDateTime(int &Year, int &Month, int &Day, int &Hour, int &Minute, int &Second) {
    time_t now;
    struct tm *now_tm;
    time(&now);
    now_tm = localtime(&now);
    Year = now_tm->tm_year+1900;
    Month = now_tm->tm_mon+1;
    Day = now_tm->tm_mday;
    Hour = now_tm->tm_hour;
    Minute = now_tm->tm_min;
    Second = now_tm->tm_sec;
}

void GetEntryTextForDB(GtkWidget *entry, string& text) {
    if (gtk_entry_get_text_length(GTK_ENTRY(entry))) {
        text = gtk_entry_get_text(GTK_ENTRY(entry));
        trim(text);
    } else
        text = "%";
}

bool CalcAge(const int birth_year, const int birth_month, const int birth_day, int& age, int& age_unit) {
    int cur_year, cur_month, cur_day;
    GetCurrentDate(cur_year, cur_month, cur_day);
    GDate* cur_date = g_date_new_dmy((GDateDay)cur_day, (GDateMonth)cur_month, (GDateYear)cur_year);
    if (birth_year == 0 && birth_month == 0 && birth_day == 0) {
        age = 0;
        age_unit = 0;
        g_date_free(cur_date);
        return false;
    }
    GDate* birth_date = g_date_new_dmy((GDateDay)birth_day, (GDateMonth)birth_month, (GDateYear)birth_year);
    if (g_date_compare(cur_date, birth_date) < 0) {
        g_date_free(cur_date);
        g_date_free(birth_date);
        return false;
    }

    int days = g_date_days_between(birth_date, cur_date);
    g_date_free(cur_date);
    g_date_free(birth_date);

    if (days >= 0 && days <=  100) {
        age = days;
        age_unit = 2;
        return true;
    } else if (days > 100 && days < 365) {
        if (birth_year == cur_year) {
            if (cur_day >= birth_day)
                age = cur_month - birth_month;
            else
                age = cur_month - birth_month - 1;
        } else {
            if (cur_day >= birth_day)
                age = 12 - birth_month + cur_month;
            else
                age = 12 - birth_month + cur_month - 1;
        }
        age_unit = 1;
        return true;
    } else if (days >= 365) {
        if (cur_month == birth_month) {
            if (cur_day >= birth_day)
                age = cur_year - birth_year;
            else
                age = cur_year - birth_year - 1;
        } else if (cur_month < birth_month) {
            age = cur_year - birth_year - 1;
        } else {
            age = cur_year - birth_year;
        }
        age_unit = 0;
        return true;
    } else {
        return false;
    }
}

void ChangeDateFormatToString(int iYear, int iMon, int iDay, string &sYear, string &sMon, string &sDay) {
    char buf[20];

    sprintf(buf, "%d%d%d%d", iYear/1000, (iYear%1000)/100, (iYear%100)/10, iYear%10);
    sYear = buf;
    sprintf(buf, "%d%d", iMon/10, iMon%10);
    sMon = buf;
    sprintf(buf, "%d%d", iDay/10, iDay%10);
    sDay = buf;
}

void ChangeTimeFormatToString(int iHour, int iMin, int iSec, string &sHour, string &sMin, string &sSec) {
    char buf[20];

    sprintf(buf, "%d%d", iHour/10, iHour%10);
    sHour = buf;
    sprintf(buf, "%d%d", iMin/10, iMin%10);
    sMin = buf;
    sprintf(buf, "%d%d", iSec/10, iSec%10);
    sSec = buf;

    //cout << "time is " << sHour << sMin << sSec << endl;

}

void GenerateDataString(string year, string mon, string day, string &date) {
    char buf[100];

    SysGeneralSetting sgs;
    int dateFormat = sgs.GetDateFormat();
    switch (dateFormat) {
    case 0:
        sprintf(buf, "%s-%s-%s", year.c_str(), mon.c_str(), day.c_str());
        break;
    case 1:
        sprintf(buf, "%s-%s-%s", mon.c_str(), day.c_str(), year.c_str());
        break;
    case 2:
        sprintf(buf, "%s-%s-%s", day.c_str(), mon.c_str(), year.c_str());
        break;
    }
    date = buf;
}

void QuichSearchDateString(string year, string mon, string day, string &date) {
    char buf[100];
    sprintf(buf, "%s-%s-%s", year.c_str(), mon.c_str(), day.c_str());
    date = buf;
}

void GetDateString(int iYear, int iMon, int iDay, string &date) {
    string year, mon, day;
    ChangeDateFormatToString(iYear, iMon, iDay, year, mon, day);
    GenerateDataString(year, mon, day, date);
}

void GetTimeString(int iHour, int iMin, int iSec, string &time) {
    string h, m, s;
    char buf[100];

    ChangeTimeFormatToString(iHour, iMin, iSec, h, m, s);
    sprintf(buf, "%s:%s:%s", h.c_str(), m.c_str(), s.c_str());
    time = buf;
}

void ValidDateFormat(string &sYear, string &sMon, string &sDay) {
    int iYear = atoi(sYear.c_str());
    int iMon = atoi(sMon.c_str());
    int iDay = atoi(sDay.c_str());

    ChangeDateFormatToString(iYear, iMon, iDay, sYear, sMon, sDay);
}

/*
 * @brief get age from birthday.
 *
 * @para age[output] age in string format, 3numeric+1character, such as 080Y
 */
void GetAge(int year, int mon, int day, string &age) {
    int iAge, ageUnit;

    if (CalcAge(year, mon, day, iAge, ageUnit))
        GenAgeStr(iAge, ageUnit, age);
    else
        GenAgeStr(0, 0, age);
}

void GenAgeStr(int age, int ageUnit, string &strAge) {
    string unit;
    char buf[50];  //big because of russian

    switch (ageUnit) {
    case 0:
        unit = _("Y");
        break;
    case 1:
        unit = _("M");
        break;
    case 2:
        unit = _("D");
        break;
    default:
        unit = _("Y");
        break;
    }

    sprintf(buf, "%d%s", age, unit.c_str());
    strAge = buf;
}

void GetSexString(int iSex, string &sSex) {
    switch (iSex) {
#ifdef VET
    case 0:
        sSex = _("Female ");
        break;
    case 1:
        sSex = _("Male ");
        break;
#else
    case 0:
        sSex = _("Female");
        break;
    case 1:
        sSex = _("Male");
        break;
#endif
    case 2:
        sSex = _("Other");
        break;
    default:
        sSex = " ";
        break;
    }
}

#ifdef VET
void GetAnimalSpeciesString(int index, string &species) {
    switch (index) {
    case 0:
        species = _("Dog");
        break;
    case 1:
        species = _("Cat");
        break;
    case 2:
        species = _("Sheep");
        break;
    case 3:
        species = _("Swine");
        break;
    case 4:
        species = _("Bovine");
        break;
    case 5:
        species = _("Equine");
        break;
    case 6:
        species = _("Other");
        break;

    default:
        break;
    }
}
#endif

void GetTypeStr(string &src, string &des) {
    string str1("");
    str1 = src;
    string str2 = '"' +str1 + '"';
    string str3 = '(' + str2 +')';
    string str4 = '_' + str3;
    des = str4;
    PRINTF("********src des %s %s\n",src.c_str(),des.c_str());
}

int CalcLength(POINT p1, POINT p2) {
    int dx = p1.x - p2.x;
    int dy = p1.y - p2.y;

    int len = sqrt(dx*dx + dy*dy);

    return len;
}

void CommaToDotLocaleNumeric(char *Dot, unsigned int size) {
    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();

    if ((PL == language)||(RU == language)||(FR == language)||(DE == language)||(ES == language)) {

        for(unsigned int i=0; i<size; i++) {
            if(Dot[i]==',') {
                Dot[i]='.';
                break;
            }
        }
    }

}

void DotToCommaLocaleNumeric(char *Comma, unsigned int size) {

    SysGeneralSetting sysGS;
    int language= sysGS.GetLanguage();

    if ((PL == language)||(RU == language)||(FR == language)||(DE == language)||(ES == language)) {
        for(unsigned int i=0; i<size; i++) {
            if(Comma[i]=='.') {
                Comma[i]=',';
                break;
            }
        }
    }

}

GtkWidget *create_label(const char *str, guint width, guint height, const GdkColor* const color, PangoFontDescription *font) {
    GtkWidget *label = gtk_label_new(str);
    gtk_label_set_use_markup(GTK_LABEL(label), TRUE);
    if (width != 0 && height != 0)
        gtk_widget_set_usize(label, width, height);
    else if(width != 0 && height ==0)
        gtk_widget_set_usize(label, width, -1);
    else if(width ==0 && height !=0)
        gtk_widget_set_usize(label, -1, height);
    if (color)
        gtk_widget_modify_fg(label, GTK_STATE_NORMAL, color);
    if (font)
        gtk_widget_modify_font(label, font);
    return label;
}

GtkWidget *create_button(GtkWidget *label, guint width, guint height, const GdkColor* const color) {
    GtkWidget *button = gtk_button_new();
    if (width != 0 && height != 0)
        gtk_widget_set_usize(button, width, height);
    else if(width != 0 && height ==0)
        gtk_widget_set_usize(button, width, -1);
    else if(width ==0 && height !=0)
        gtk_widget_set_usize(button, -1, height);
    if (label)
        gtk_container_add(GTK_CONTAINER(button), label);
    if (color)
        gtk_widget_modify_bg(button, GTK_STATE_NORMAL, color);
    gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    //	gtk_button_set_focus_on_click(GTK_BUTTON(button), true);//test--2016.08.11

    modify_widget_bg(button);

    return button;
}

#include <iostream>
using namespace std;

GtkWidget* create_button_icon(GtkWidget *label, GtkWidget *icon_image) {
    int size =0;
    //if(CDCMRegister::GetMe()->IsAuthorize())
        size = 20;
    //else
    //    size = 0;

    GtkWidget *button = gtk_button_new_with_label(gtk_label_get_text(GTK_LABEL(label)));
    gtk_widget_set_size_request (button, 120, 70-size);

    /*GtkWidget *alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
    gtk_container_add (GTK_CONTAINER (button), alignment);

    GtkWidget *hbox = gtk_hbox_new (FALSE, 2);
    gtk_container_add (GTK_CONTAINER (alignment), hbox);
    //gtk_box_pack_start (GTK_BOX (hbox), icon_image, FALSE, FALSE, 0);
    if (label)
        gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);

    gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);*/
    gtk_widget_show_all(button);

    modify_widget_bg(button);

    return button;
}

GtkWidget* create_button_icon_only(GtkWidget *icon_image, guint width, guint height, const GdkColor *color) {
    GtkWidget *button = gtk_button_new();

    if (width != 0 && height != 0)
        gtk_widget_set_usize(button, width, height);
    else if(width != 0 && height ==0)
        gtk_widget_set_usize(button, width, -1);
    else if(width ==0 && height !=0)
        gtk_widget_set_usize(button, -1, height);
    //if (icon_image)
        //gtk_container_add(GTK_CONTAINER(button), icon_image);
    if (color)
        gtk_widget_modify_bg(button, GTK_STATE_NORMAL, color);

    gtk_button_set_focus_on_click(GTK_BUTTON(button), FALSE);
    gtk_widget_show_all(button);

    return button;
}

GtkWidget* create_combobox(guint width, guint height, const char *layouttype, GtkTreeModel *treemodel) {
    GtkWidget* combobox;
    if(treemodel)
        combobox = gtk_combo_box_new_with_model(treemodel);
    else
        combobox = gtk_combo_box_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer, layouttype, 0, NULL);
    if (width != 0 && height != 0)
        gtk_widget_set_usize (combobox, width, height);
    gtk_combo_box_set_active (GTK_COMBO_BOX (combobox), 0);
    g_object_unref (treemodel);

    modify_widget_bg(combobox);

    return combobox;
}

GtkWidget* create_combo_box_with_text(const int width, const int height, const vector<string> text) {
    GtkTreeIter treeIter;
    GtkListStore *liststore = gtk_list_store_new(1, G_TYPE_STRING);
    for (vector<string>::const_iterator iter = text.begin(); iter != text.end(); ++iter) {
        gtk_list_store_append(liststore, &treeIter);
        gtk_list_store_set(liststore, &treeIter, 0, _((*iter).c_str()), -1);
    }

    GtkWidget *combobox = gtk_combo_box_new_with_model(GTK_TREE_MODEL(liststore));
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combobox), renderer, "text", 0, NULL);

    if (width != 0 && height != 0)
        gtk_widget_set_usize(combobox, width, height);

    g_object_unref (liststore);

    modify_widget_bg(combobox);

    return combobox;
}

#if 0
PangoLayout * create_pango_text(GtkWidget *widget,
                                const char *text,
                                int width,
                                PangoFontDescription *font,
                                const GdkColor* const color) {
    PangoLayout *layout;
    PangoAttrList *attrs;
    PangoAttribute *attr;

    layout = gtk_widget_create_pango_layout(widget, text);

    if (width != 0)
        pango_layout_set_width(layout, width);
    if (font)
        pango_layout_set_font_description(layout, font);
    if (color) {
        attrs = pango_attr_list_new();
        attr = pango_attr_foreground_new(color->red, color->green, color->blue);
        pango_attr_list_change(attrs, attr);
        pango_layout_set_attributes(layout, attrs);
    }

    return layout;	//layout not be free
}
#endif

GtkWidget* create_pixmap(const gchar *filename) {
    GtkWidget *pixmap;

    if (!filename || !filename[0])
        return gtk_image_new ();

    if (g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE) {
        g_warning ("Couldn't find pixmap file: %s", filename);
        return gtk_image_new ();
    }
    pixmap = gtk_image_new_from_file(filename);
    return pixmap;
}



GdkColor* MeasureColorConvert(int colorIndex) {
    if (colorIndex == 0)
        return g_red;
    else if (colorIndex == 1)
        return g_yellow;
    else if (colorIndex == 2)
        return g_green;
    else if (colorIndex == 3)
        return g_blue;
    else if (colorIndex == 4)
        return g_white;
    else if(colorIndex == 5)
        return g_black;
    else
        return NULL;
}

int CountLines(string& text) {
    int count = 1;
    string::size_type pos = 0;
    while ((pos = text.find_first_of("\n", pos)) != string::npos) {
        ++pos;
        ++count;
    }
    return count;
}

// Remove leading and trailing whitespace
void trim(string& str) {
    static const char whitespace[] = " \n\t\v\r\f";
    str.erase(0, str.find_first_not_of(whitespace));
    str.erase(str.find_last_not_of(whitespace) + 1);
}

// 返回Combobox的index值, 失败返回-1
int GetComboBoxIndex(GtkComboBox *combobox, string name) {
    GtkTreeIter iter;
    GtkTreeModel* model = gtk_combo_box_get_model(GTK_COMBO_BOX (combobox));
    char *new_name = 0;
    string str_name;
    if (gtk_tree_model_get_iter_first (model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &new_name, -1);
        str_name = new_name;
        delete new_name;
        new_name = 0;
        while (str_name != name) {
            if (gtk_tree_model_iter_next(model, &iter)) {
                gtk_tree_model_get(model, &iter, 0, &new_name, -1);
                str_name = new_name;
                delete new_name;
                new_name = 0;
            } else {
                PRINTF("Do not exist in combobox\n");
                return -1;
            }
        }
        GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
        char *path_str = gtk_tree_path_to_string(path);
        gtk_tree_path_free(path);
        int ret = atoi(path_str);
        delete path_str;
        return ret;
    } else {
        return -1;
    }
}



PangoFontDescription* AdaptStringFont(const char *font_family, const char *font_style, int font_size, PangoLayout *layout, const char *str, const int max_width) {
    char font_des[128];
    int width = 0;
    int height = 0;
    int size = font_size;
    PangoFontDescription *font = NULL;
    do {
        sprintf(font_des, "%s, %s, %d", font_family, font_style, size);
        font = pango_font_description_from_string(font_des);
        pango_layout_set_font_description(layout, font);
        pango_layout_set_alignment(layout, PANGO_ALIGN_LEFT);
        pango_layout_get_pixel_size(layout, &width, &height);
        --size;
    } while (width > max_width);
    return font;
}

void SetTheme(const char *rc_path) {
    gtk_rc_parse(rc_path);
    gtk_rc_reparse_all();
    gtk_rc_reset_styles(gtk_settings_get_default());
}




void modify_widget_bg(GtkWidget* widget) {
    GdkColor* bg_color = new GdkColor();
    gdk_color_parse("green", bg_color);

    gtk_widget_modify_bg(widget, GTK_STATE_ACTIVE, bg_color);
    gtk_widget_modify_bg(widget, GTK_STATE_PRELIGHT, bg_color);
    gtk_widget_modify_bg(widget, GTK_STATE_SELECTED, bg_color);
}
