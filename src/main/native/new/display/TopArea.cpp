#include "display/TopArea.h"

#include <time.h>

#include "probe/ProbeMan.h"
#include "sysMan/SysGeneralSetting.h"

TopArea* TopArea::m_instance = NULL;

// ---------------------------------------------------------

TopArea* TopArea::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new TopArea();
  }

  return m_instance;
}

TopArea::TopArea() {
  m_label_hospital = NULL;
  m_label_patient_info = NULL;
  m_label_sys_info = NULL;
  m_label_tis = NULL;
  m_label_time = NULL;

  m_cell_width = 0;
  m_cell_height = 0;
  m_inReadImg = false;

  SysGeneralSetting sys;
  m_dateFormat = sys.GetDateFormat();
}

TopArea::~TopArea() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* TopArea::Create(const int width, const int height) {
  m_cell_width = width / 12;
  m_cell_height = height / 2;

  GtkTable* table = Utils::create_table(2, 12);
  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(table, 5);
  gtk_table_set_col_spacings(table, 5);

  // logo
  GtkImage* image = Utils::create_image(string(CFG_RES_PATH) + string("res/logo.png"), m_cell_width * 2 - 6, m_cell_height * 2 - 6);
  gtk_table_attach_defaults(table, GTK_WIDGET(image), 0, 2, 0, 2);

  // hospital
  m_label_hospital = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_hospital), 2, 4, 0, 2);

  gtk_misc_set_alignment(GTK_MISC(m_label_hospital), 0.5, 0.5);

  // patient info
  m_label_patient_info = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_patient_info), 4, 10, 0, 1);

  // sys info
  m_label_sys_info = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_sys_info), 4, 9, 1, 2);

  // tis
  m_label_tis = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_tis), 9, 10, 1, 2);

  // time
  m_label_time = Utils::create_label("");
  gtk_table_attach_defaults(table, GTK_WIDGET(m_label_time), 10, 12, 0, 2);

  gtk_misc_set_alignment(GTK_MISC(m_label_time), 0.5, 0.5);

  // drawing
  gtk_signal_connect(GTK_OBJECT(table), "expose_event", G_CALLBACK(signal_expose_event), this);

  return GTK_WIDGET(table);
}

void TopArea::UpdateCheckPart(const string part) {
  m_checkPart = part;
  UpdateSysInfo();
}

void TopArea::UpdateFreq(const string freq) {
  m_freq = freq;
  UpdateSysInfo();
}

void TopArea::UpdateHospitalName(const string name) {
  gtk_label_set_text(m_label_hospital, name.c_str());
  Utils::adjust_font_size(GTK_WIDGET(m_label_hospital), "", "", 24, m_cell_width * 2 - 5, m_cell_height * 2 - 5);
}

void TopArea::UpdateImageParam(const string param) {
  m_imageParam = param;
  UpdateSysInfo();
}

void TopArea::UpdatePatInfo(const string name, const string sex, const string age, const string id) {
  string tmp_name = name;
  string tmp_sex = sex;
  string tmp_age = age;
  string tmp_id = id;

  if (tmp_name.empty()) {
    tmp_name = "      ";
  }

  if (tmp_sex.empty()) {
    tmp_sex = "    ";
  }

  if (tmp_age.empty()) {
    tmp_age = "    ";
  }

  stringstream ss;
  ss << _("Name:") << " " << tmp_name
    << "    " << _("Gender:") << " " << tmp_sex << "  "
    << "    " << _("Age:") << " " << tmp_age << "  "
    << "    " << _("ID:") << " " << tmp_id;

  gtk_label_set_text(m_label_patient_info, ss.str().c_str());
}

void TopArea::UpdateProbeType(const string type) {
  m_probeType = type;
  UpdateSysInfo();
}

void TopArea::UpdateTIS(double tis) {
  stringstream ss;

  if (tis >= 0.0001) {
    double tmp = (int)(tis * 10) / 10.0;

    if (tmp < 0.4) {
      ss << "TIS < 0.4";
    } else {
      ss << "TIS " << tmp;
    }
  }

  gtk_label_set_text(m_label_tis, ss.str().c_str());
}

string TopArea::GetCheckPart() {
  return m_checkPart;
}

string TopArea::GetHospitalName() {
  return gtk_label_get_text(m_label_hospital);
}

string TopArea::GetProbeType() {
  return m_probeType;
}

string TopArea::GetTIS() {
  return gtk_label_get_text(m_label_tis);
}

bool TopArea::GetReadImg() {
  return m_inReadImg;
}

void TopArea::SetDateFormat(int dateFormat) {
  m_dateFormat = dateFormat;
}

void TopArea::SetReadImg(bool status) {
  m_inReadImg = status;

  if (status) {
    gtk_widget_queue_draw(GTK_WIDGET(m_label_sys_info));
    gtk_widget_queue_draw(GTK_WIDGET(m_label_tis));
  }
}

void TopArea::AddTimeOut() {
  g_timeout_add(500, signal_callback_update_datetime, this);
}

// ---------------------------------------------------------

void TopArea::DrawingExpose(GtkWidget* widget, GdkEventExpose* event) {
  cairo_t* cr = gdk_cairo_create(widget->window);

  cairo_set_source_rgb(cr, 255, 255, 255);
  cairo_set_line_width (cr, 1);

  cairo_move_to(cr, m_cell_width * 4, m_cell_height);
  cairo_line_to(cr, m_cell_width * 10, m_cell_height);

  cairo_move_to(cr, 2, m_cell_height * 2 + 2);
  cairo_line_to(cr, m_cell_width * 12, m_cell_height * 2 + 2);

  cairo_move_to(cr, m_cell_width * 4, 2);
  cairo_line_to(cr, m_cell_width * 4, m_cell_height * 2 + 2);

  cairo_move_to(cr, m_cell_width * 9, m_cell_height);
  cairo_line_to(cr, m_cell_width * 9, m_cell_height * 2 + 2);

  cairo_move_to(cr, m_cell_width * 10, 2);
  cairo_line_to(cr, m_cell_width * 10, m_cell_height * 2 + 2);

  cairo_stroke(cr);
  cairo_destroy(cr);
}

void TopArea::UpdateSysInfo() {
  stringstream ss;

  if (!m_imageParam.empty()) {
    ss << m_imageParam << "    ";
  }

  string probeType = ProbeMan::GetInstance()->VerifyProbeName(m_probeType);

  if (probeType.empty()) {
    ss << _("No Probe");
  } else {
    ss << probeType;
  }

  ss << "    ";
  ss << m_checkPart;

  gtk_label_set_text(m_label_sys_info, ss.str().c_str());
}

void TopArea::DrawDateTime() {
  time_t now;
  time(&now);

  tm* timeinfo = localtime(&now);

  string week;

  switch (timeinfo->tm_wday) {
  case 0:
    week = _("Sun.");
    break;
  case 1:
    week = _("Mon.");
    break;
  case 2:
    week = _("Tues.");
    break;
  case 3:
    week = _("Wed.");
    break;
  case 4:
    week = _("Thur.");
    break;
  case 5:
    week = _("Fri.");
    break;
  case 6:
    week = _("Sat.");
    break;
  default:
    break;
  }

  char buffer_date[20] = {0};

  switch (m_dateFormat) {
  case 0:
    strftime(buffer_date, 20, "%Y-%m-%d", timeinfo);
    break;
  case 1:
    strftime(buffer_date, 20, "%m-%d-%Y", timeinfo);
    break;
  case 2:
    strftime(buffer_date, 20, "%d-%m-%Y", timeinfo);
    break;
  default:
    strftime(buffer_date, 20, "%Y-%m-%d", timeinfo);
    break;
  }

  char buffer_time[20] = {0};
  strftime(buffer_time, 20, "%H:%M:%S", timeinfo);

  stringstream ss;
  ss << buffer_date << " " << week << "\n" << buffer_time;

  gtk_label_set_text(m_label_time, ss.str().c_str());
}
