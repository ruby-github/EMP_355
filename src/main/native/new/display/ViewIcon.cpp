#include "display/ViewIcon.h"

#include "Def.h"
#include "periDevice/Battery.h"

const string NetworkIconPath[] = {
  "res/icon/network-offline.png",
  "res/icon/network-idle.png"
};

const string SoundIconPath[] = {
  "res/icon/audio-muted.png",
  "res/icon/audio-on.png"
};

const string ScanIconPath[] = {
  "res/icon/scan-00.png",
  "res/icon/scan-25.png",
  "res/icon/scan-50.png",
  "res/icon/scan-75.png",
  "res/icon/scan-100.png"
};

const string ChargeIconPath[] = {
  "res/icon/charge0.jpg",
  "res/icon/charge1.jpg",
  "res/icon/charge2.jpg",
  "res/icon/charge3.jpg",
  "res/icon/charge4.jpg",
  "res/icon/charge5.jpg",
  "res/icon/charge.jpg"
};

const string ReplayIconPath = "res/icon/replay.png";
const string FlashIconPath = "res/icon/flashkey.png";
const string CdromIconPath = "res/icon/cdrom.png";

ViewIcon* ViewIcon::m_instance = NULL;

// ---------------------------------------------------------

ViewIcon* ViewIcon::GetInstance() {
  if (m_instance == NULL) {
    m_instance = new ViewIcon();
  }

  return m_instance;
}

ViewIcon::ViewIcon() {
  m_network = NULL;
  m_sound = NULL;
  m_replay = NULL;
  m_udisk = NULL;
  m_cdrom = NULL;
  m_charge = NULL;

  m_count_scanicon = 0;
}

ViewIcon::~ViewIcon() {
  if (m_instance != NULL) {
    delete m_instance;
  }

  m_instance = NULL;
}

GtkWidget* ViewIcon::Create() {
  GtkTable* table = Utils::create_table(1, 6);

  gtk_container_set_border_width(GTK_CONTAINER(table), 5);
  gtk_table_set_row_spacings(table, 1);
  gtk_table_set_col_spacings(table, 1);

  m_network = Utils::create_image(NetworkIconPath[0]);
  m_sound = Utils::create_image(SoundIconPath[0]);
  m_replay = Utils::create_image();
  m_udisk = Utils::create_image();
  m_cdrom = Utils::create_image();
  m_charge = Utils::create_image();

  gtk_table_attach_defaults(table, GTK_WIDGET(m_network), 0, 1, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_sound), 1, 2, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_replay), 2, 3, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_udisk), 3, 4, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_cdrom), 4, 5, 0, 1);
  gtk_table_attach_defaults(table, GTK_WIDGET(m_charge), 5, 6, 0, 1);

  InitCharge();

  return GTK_WIDGET(table);
}

void ViewIcon::Network(bool on) {
  string path;

  if (on) {
    path = string(CFG_RES_PATH) + NetworkIconPath[1];
  } else {
    path = string(CFG_RES_PATH) + NetworkIconPath[0];
  }

  gtk_image_set_from_file(m_network, path.c_str());
}

void ViewIcon::Sound(bool on) {
  string path;

  if (on) {
    path = string(CFG_RES_PATH) + SoundIconPath[1];
  } else {
    path = string(CFG_RES_PATH) + SoundIconPath[0];
  }

  gtk_image_set_from_file(m_sound, path.c_str());
}

void ViewIcon::Replay(bool on) {
  if (on) {
    ScanIcon(ReplayIconPath);
  } else {
    g_timeout_add(500, signal_callback_scroll_scanicon, this);
  }
}

void ViewIcon::Udisk(bool on) {
  if (on) {
    string path = string(CFG_RES_PATH) + FlashIconPath;
    gtk_image_set_from_file(m_udisk, path.c_str());
  } else {
    gtk_image_set_from_file(m_udisk, NULL);
  }
}

void ViewIcon::Cdrom(bool on) {
  if (on) {
    string path = string(CFG_RES_PATH) + CdromIconPath;
    gtk_image_set_from_file(m_cdrom, path.c_str());
  } else {
    gtk_image_set_from_file(m_cdrom, NULL);
  }
}

void ViewIcon::Printer(bool on) {
}

// ---------------------------------------------------------

void ViewIcon::ScrollScanIcon() {
  int count = GetCountScanIcon();

  string path = string(CFG_RES_PATH) + ScanIconPath[0];

  switch (count) {
  case 0:
    path = string(CFG_RES_PATH) + ScanIconPath[0];
    break;
  case 1:
    path = string(CFG_RES_PATH) + ScanIconPath[1];
    break;
  case 2:
    path = string(CFG_RES_PATH) + ScanIconPath[2];
    break;
  case 3:
    path = string(CFG_RES_PATH) + ScanIconPath[3];
    break;
  case 4:
    path = string(CFG_RES_PATH) + ScanIconPath[4];
    break;
  }

  ScanIcon(path);

  if (count < 4) {
    count++;
  } else {
    count = 0;
  }

  SetCountScanIcon(count);
}

void ViewIcon::UpdateCharge() {
  Battery battery;
  Charge(battery.GetCapacity());
}

void ViewIcon::InitCharge() {
  UpdateCharge();
  g_timeout_add(6000, signal_callback_updatecharge, this );
}

void ViewIcon::ScanIcon(const string iconName) {
  gtk_image_set_from_file(m_replay, iconName.c_str());
}

int ViewIcon::GetCountScanIcon() {
  return m_count_scanicon;
}

void ViewIcon::SetCountScanIcon(int count) {
  m_count_scanicon = count;
}

void ViewIcon::Charge(int data) {
  int value = 80 / 4;
  int count = 0;

  if (data < 10) {
    count = 0;
  } else if (data > 90) {
    count = 5;
  } else {
    count = 1 + (data - 10) / value;
  }

  string path = string(CFG_RES_PATH) + ChargeIconPath[0];

  switch (count) {
  case 0:
    path = string(CFG_RES_PATH) + ChargeIconPath[0];
    break;
  case 1:
    path = string(CFG_RES_PATH) + ChargeIconPath[1];
    break;
  case 2:
    path = string(CFG_RES_PATH) + ChargeIconPath[2];
    break;
  case 3:
    path = string(CFG_RES_PATH) + ChargeIconPath[3];
    break;
  case 4:
    path = string(CFG_RES_PATH) + ChargeIconPath[4];
    break;
  case 5:
    path = string(CFG_RES_PATH) + ChargeIconPath[5];
    break;
  }

  gtk_image_set_from_file(m_charge, path.c_str());
}
