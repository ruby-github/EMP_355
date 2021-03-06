#include "probe/ProbeSocket.h"

#include "Def.h"

enum EPROBELIST355 {
  P35C50LP, P70L40JP, P75L40JP, P70L60JP, P90L40JP,
  P65C10LP, P35C20IP, P65C15EP, P30P16BP, P10L25KP,
  P65C10IP
};

string g_probe_list[NUM_PROBE] = {
  "35C50L", "70L40J", "75L40J", "70L60J", "90L40J",
  "65C10L", "35C20I", "65C15E", "30P16B", "10L25K",
  "65C10I"
};

// class func
const int ProbeSocket::FREQ2D_SUM[NUM_PROBE] = {
  7, 4, 4, 5, 5,
  3, 4, 3, 6, 3,
  3
};

ProbeSocket::FreqPair ProbeSocket::FREQ2D[NUM_PROBE][MAX_2D_FREQ] = {
  {{40,40},   {50,50},    {60,60},    {70,70},    {80,80},    {90,90},    {100,100},  {0,0},  {0,0},  {0,0}}, // 35C50L
  {{100,100}, {130,130},  {150,150},  {180,180},  {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 70L40J
  {{100,100}, {130,130},  {150,150},  {180,180},  {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 75L40J
  {{100,100}, {130,130},  {150,150},  {170,170},  {180,180},  {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 70L60J
  {{120,120}, {150,150},  {180,180},  {200,200},  {240,240},  {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 90L40J
  {{100,100}, {130,130},  {150,150},  {0,0},      {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 65C10L
  {{50,50},   {70,70},    {80,80},    {100,100},  {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 35C20I
  {{100,100}, {130,130},  {160,160},  {0,0},      {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 65C15E
  {{40,40},   {50,50},    {56,56},    {70,70},    {80,80},    {100,100},  {0,0},      {0,0},  {0,0},  {0,0}}, // 30P16B
  {{160,160}, {200,200},  {240,240},  {0,0},      {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 10L25K
  {{100,100}, {130,130},  {150,150},  {0,0},      {0,0},      {0,0},      {0,0},      {0,0},  {0,0},  {0,0}}, // 65C10I
};

const int ProbeSocket::FREQ_DOPPLER_SUM[NUM_PROBE] = {2, 2,2, 2, 2, 2, 2, 2, 2, 2, 2}; //2
const int ProbeSocket::FREQ_DOPPLER[NUM_PROBE][MAX_DOP_FREQ] = {
  {54, 66},   {118, 144}, {118, 144}, {118, 144}, {0, 0},
  {98, 116},  {76, 86},   {98, 116},  {48, 56},   {150,180},
  {98, 116}
};

const int ProbeSocket::BAND_PASS_FILTER_FC_DOPPLER[NUM_PROBE][MAX_DOP_FREQ] = {
  {54, 66},   {118, 144}, {118,144},  {118, 144}, {0, 0},
  {98, 116},  {76, 86},   {98, 116},  {56, 64},   {150,180},
  {98, 116}
};

const int ProbeSocket::BAND_PASS_FILTER_FC1_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC] = {
  {
    {25, 20, 18, 18, 18}, {30, 23, 18, 18, 18}, {32, 25, 18, 18, 18}, {32, 25, 20, 20, 18}, {32, 25, 20, 20, 18},
    {33, 30, 20, 20, 18}, {33, 30, 20, 20, 18}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //35C50L
  {
    {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L40J
  {
    {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L40J
  {
    {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L60J
  {
    {80, 50, 40, 35, 30}, {80, 50, 40, 35, 30}, {80, 50, 40, 35, 30}, {80, 50, 40, 35, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //90L40J
  {
    {30, 28, 25, 22, 20}, {35, 26, 20, 20, 20}, {45, 40, 35, 28, 20}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C10L
  {
    {32, 25, 24, 20, 16}, {32, 25, 24, 20, 16}, {32, 25, 24, 20, 16}, {32, 25, 24, 20, 16}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //35C20I
  {
    {45, 38, 32, 28, 24}, {52, 49, 36, 32, 25}, {51, 48, 38, 36, 32}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C15E
  {
    {36, 25, 26, 22, 19}, {36, 25, 26, 22, 19}, {36, 25, 26, 22, 19}, {38, 30, 25, 22, 18},{38, 30, 25, 22, 18},
    {38, 30, 25, 22, 18}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //30P16B
  {
    {16, 20, 24, 26, 26}, {16, 20, 24, 26, 26}, {65, 55, 35, 30, 25}, {25, 20, 24, 26, 26}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //10L25K
  {
    {35, 26, 20, 20, 20}, {35, 26, 20, 20, 20}, {35, 26, 20, 20, 20}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C10I
};

const int ProbeSocket::BAND_PASS_FILTER_FC2_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC] = {
  {
    {60, 55, 28, 26, 24}, {65, 58, 36, 32, 30}, {67, 60, 36, 32, 30}, {67, 60, 45, 30, 30}, {67, 60, 45, 30, 30},
    {68, 65, 45, 30, 30}, {67, 60, 45, 30, 30}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //35C50L
  {
    {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L40J
  {
    {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L40J
  {
    {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {164, 150, 78, 58, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //75L60J
  {
    {150, 140, 100, 70, 50}, {150, 140, 100, 70, 50}, {150, 140, 100, 70, 50}, {150, 140, 100, 70, 50}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //90L40J
  {
    {100, 90, 75, 60, 50}, {131, 100, 80, 69, 58}, {125, 100, 75, 62, 50}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C10L
  {
    {72, 60, 49, 48, 40}, {72, 60, 49, 48, 40}, {72, 60, 49, 48, 40}, {72, 60, 49, 48, 40}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //35C20I
  {
    {119, 100, 84, 68, 54}, {119, 100, 84, 68, 54}, {124, 103, 87, 70, 56}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C15E
  {
    {56, 47, 48, 42, 40}, {56, 47, 48, 42, 40}, {56, 47, 48, 42, 40}, {58, 52, 48, 42, 40},{58, 52, 48, 42, 40},
    {58, 52, 48, 42, 40}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //30P16B
  {
    {164, 150, 78, 58, 30}, {145, 120, 85, 60, 40}, {164, 150, 78, 58, 30}, {170, 150, 78, 58, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //10L25K
  {
    {131, 100, 80, 69, 58}, {131, 100, 80, 69, 58}, {131, 100, 80, 69, 58}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, //65C10I
};

const int ProbeSocket::DYNAMIC_FILTER_FC_BASE_FREQ[NUM_PROBE][MAX_2D_FREQ][MAX_FC] = {
  {
    {50, 50, 48, 44, 32}, {50, 50, 48, 44, 32}, {50, 50, 48, 44, 32}, {50, 50, 48, 44, 32}, {50, 50, 48, 44, 32},
    {50, 50, 48, 44, 32}, {50, 50, 48, 44, 32}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 35C50L
  {
    {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 75L40J
  {
    {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 75L40J
  {
    {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {80, 70, 60, 50, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 75L60J
  {
    {100, 90, 80, 70, 60}, {100, 90, 80, 70, 60}, {100, 90, 80, 70, 60}, {100, 90, 80, 70, 60}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 90L40J
  {
    {60, 60, 60, 50, 50}, {80, 70, 60, 55, 50}, {60, 60, 60, 50, 50}, {0, 0, 0, 0, 0},{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 65C10L
  {
    {50, 50, 40, 40, 30}, {50, 50, 40, 40, 30}, {50, 50, 40, 40, 30}, {50, 50, 40, 40, 30},{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 35C20I
  {
    {70, 65, 65, 60, 55}, {70, 65, 65, 60, 55}, {80, 75, 60, 60, 55}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 65C15E
  {
    {45, 45, 40, 35, 35}, {45, 45, 40, 35, 35}, {45, 45, 40, 35, 35}, {45, 45, 40, 35, 35}, {45, 45, 40, 35, 35},
    {45, 45, 40, 35, 35}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 30P16B
  {
    {80, 70, 60, 50, 30}, {70, 60, 60, 50, 40}, {85, 70, 60, 50, 30}, {85, 70, 60, 50, 30}, {0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 10L25K
  {
    {80, 70, 60, 55, 50}, {80, 70, 60, 55, 50}, {80, 70, 60, 55, 50}, {0, 0, 0, 0, 0},{0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0}
  }, // 65C10I
};

const int ProbeSocket::FREQ_THI_SUM[NUM_PROBE] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4};
const int ProbeSocket::FREQ_THI[NUM_PROBE][MAX_HARMONIC_FREQ] = {
  {40, 45, 50},   {160, 200, 240},  {160,200,240},  {90, 100, 120}, {100, 110, 120},
  {120,160,200},  {40, 45, 50},     {80, 90, 100},  {34, 40, 45},   {180, 200, 240},
  {80, 90, 100}
};

const int ProbeSocket::BAND_PASS_FILTER_FC1[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC] = {
  //{{34, 30, 27, 22, 18}, {39, 38, 33, 29, 26}, {45, 40, 34, 30, 25}}, //35C50L
  {{34, 30, 27, 22, 18}, {38, 35, 29, 22, 21}, {45, 40, 34, 30, 25}}, //35C50L
  {{40, 40, 38, 35, 32}, {45, 40, 38, 35, 32},{48, 40, 42, 38, 35}}, //75L40J
  {{40, 40, 38, 35, 32}, {45, 40, 38, 35, 32},{48, 40, 42, 38, 35}}, //75L40J
  {{73, 73, 68, 66, 64}, {82, 79, 69, 69, 67}, {99, 95, 92, 92, 99}}, //75L60J
  {{73, 73, 68, 66, 64}, {82, 79, 69, 69, 67}, {99, 95, 92, 92, 99}}, //90L40J
  {{38, 36, 33, 30, 25}, {40, 38, 34, 30, 25}, {48, 45, 40, 36, 30}},  //65C10L
  //{{22, 19, 17, 16, 14}, {24, 23, 20, 18, 16}, {26, 24, 20, 18, 15}}, //35C20I
  {{32, 25, 24, 20, 16}, {32, 25, 24, 20, 16}, {32, 25, 24, 20, 16}}, //35C20I
  //{{64, 55, 54, 52, 49}, {69, 63, 67, 64, 63}, {84, 82, 70, 59, 54}}, //65C15E
  {{60, 52, 42, 40, 36}, {64, 56, 50, 46, 42}, {68, 60, 52, 48, 45}}, //65C15E
  // {{38, 36, 33, 27, 25}, {45, 39, 36, 29, 27}, {46, 41, 38, 34, 29}}, //30P16B
  {{30, 28, 28, 24, 21}, {38, 36, 33, 27, 25}, {45, 39, 36, 29, 27}}, //30P16B
  //{{73, 73, 68, 66, 64}, {82, 79, 69, 69, 67}, {99, 95, 92, 92, 99}}, //55L60G
  {{60, 50, 40, 35, 30}, {65, 55, 40, 35, 30},{70, 60, 40, 35, 30}},  //10L25K
  {{51, 44, 33, 28, 24}, {53, 45, 34, 28, 24}, {55, 47, 35, 29, 24}}, //65C10I
};

const int ProbeSocket::BAND_PASS_FILTER_FC2[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC] = {
  //{{52, 46, 41, 34, 26}, {55, 53, 47, 41, 36}, {59, 54, 46, 40, 33}}, //35C50L
  {{52, 46, 41, 34, 26}, {60, 58, 56, 45, 42}, {59, 54, 46, 40, 33}}, //35C50L
  {{140, 130, 90,90, 88},{140, 140, 93, 93, 90},{150, 138, 130, 110, 90}}, //75L40J
  {{140, 130, 90,90, 88},{140, 140, 93, 93, 90},{150, 138, 130, 110, 90}}, //75L40J
  {{97, 97, 91, 88, 86}, {110, 103, 93, 93, 90}, {131, 126, 122, 122, 131}}, //75L60J
  {{97, 97, 91, 88, 86}, {110, 103, 93, 93, 90}, {131, 126, 122, 122, 131}}, //90L40J
  {{140, 116, 100, 70, 60}, {140, 100, 80, 68, 60}, {142, 115, 95, 75, 60}}, //65C10L
  //{{65, 57, 51, 41, 34}, {71, 68, 60, 53, 47}, {78, 71, 60, 53, 44}}, //35C20I
  {{72, 60, 49, 48, 40}, {72, 60, 49, 48, 40}, {72, 60, 49, 48, 40}}, //35C20I
  //{{92, 78, 71, 66, 63}, {97, 90, 84, 78, 77}, {111, 108, 98, 87, 81}}, //65C15E
  {{110, 94, 74, 69, 65}, {110, 98, 74, 69, 65}, {112, 99, 74, 69, 65}}, //65C15E
  //{{54, 50, 47, 37, 35}, {59, 53, 48, 39, 37}, {62, 55, 50, 46, 39}}, //30P16B
  {{50, 50, 46, 38, 38}, {54, 50, 47, 37, 35}, {59, 53, 48, 39, 37}}, //30P16B
  //{{97, 97, 91, 88, 86}, {110, 103, 93, 93, 90}, {131, 126, 122, 122, 131}}, //55L60G
  {{120, 100, 75, 55, 50}, {135, 95, 75, 55, 50}, {140, 100, 75, 55, 50}},//10L25K
  {{97, 90, 78, 67, 59}, {99, 91, 79, 67, 59}, {101, 93, 80, 68, 59}}, //65C10I
};

const int ProbeSocket::DYNAMIC_FILTER_FC[NUM_PROBE][MAX_HARMONIC_FREQ][MAX_FC] = {
  //{{23, 23, 20, 18, 18}, {25, 25, 25, 25, 25}, {26, 26, 26, 26, 26}}, //35C50L
  {{23, 23, 20, 18, 18}, {40, 40, 38, 30, 30}, {26, 26, 26, 26, 26}}, //35C50L
  {{60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}},//75L40J
  {{60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}},//75L40J
  {{60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}}, //75L60J
  {{60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}},  //90L40J
  {{60, 60, 60, 60, 50}, {60, 60, 60, 60, 50}, {60, 60, 60, 60, 50}},  //65C10L
  //{{23, 23, 20, 20, 18}, {25, 25, 25, 25, 25}, {26, 26, 26, 26, 26}}, //35C20I
  {{50, 50, 40, 40, 30}, {50, 50, 40, 40, 30}, {50, 50, 40, 40, 30}}, //35C20I
  {{70, 60, 60, 50, 50}, {70, 60, 60, 50, 50}, {70, 60, 60, 50, 50}}, //65C15E
  // {{23, 23, 20, 18, 18}, {25, 24, 21, 18, 16}, {25, 25, 25, 25, 25}}, //30P16B
  {{15, 15, 15, 15, 15}, {23, 23, 20, 18, 18}, {25, 24, 21, 18, 16}}, //30P16B
  //{{65, 65, 60, 60, 60}, {65, 65, 60, 60, 60}, {65, 60, 60, 60, 60}},  //55L60G
  {{60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}, {60, 60, 60, 60, 60}}, //10L25K
  {{80, 75, 70, 65, 50}, {80, 75, 70, 65, 50}, {80, 75, 70, 65, 50}}, //65C10I
};

IoCtrl ProbeSocket::m_io;

void ProbeSocket::ReadProbePara() {
  unsigned char para[PARA_LEN];
  const int offset = 0;

  ///> read probe parameter to para
  memset(para, 0, PARA_LEN);
  m_io.ReadOneProbe(m_cmdRead, para, PARA_LEN);

  ///> judge if probe parameter is valid
  unsigned char type = para[offset + 10];

  if ((type == 'C') || (type == 'L') || (type == 'N') || (type == 'T') || (type == 'P') || (type == 'V')
          || (type == 'c') || (type == 'l') || (type == 'n') || (type == 't') || (type == 'p') || (type == 'v')) { // probe is valid
      m_para.exist = TRUE;
      int i;
      for( i = 0; i < 10; i ++) {
          m_para.model[i] = para[offset + i];
      }
      m_para.model[i] = '\0';

      m_para.type = type;

      m_para.array = para[offset + 11] * 100 + para[offset + 12];

      m_para.lines = para[offset + 13] * 100 + para[offset + 14];

      m_para.width = para[offset + 15] * 100 + para[offset + 16];

      m_para.r = para[offset + 17];

      m_para.widthPhase = para[offset + 18] * 100 + para[offset + 19];
      m_para.freqRange[0] = para[offset + 20] * 100 + para[offset + 21];
      m_para.freqRange[1] = para[offset + 22] * 100 + para[offset + 23];

      m_para.depth = para[offset + 24] * 100 + para[offset + 25];

      GetFreq2D(m_para.model, m_para.type, m_para.vecFreqRange);

      PRINTF("================read Probe radius: %d\n", m_para.r);

      for (i = 0; i < MAX_DOP_FREQ; i ++) {
          m_para.freqDop[i] =  para[offset + i* 2 + 26] * 100 + para[offset + i * 2 + 27];
      }

      //get harmonic freq
      GetHarmonicFreq(m_para.model, m_para.type);

      //volume probe
      m_para.volumeR = para[offset + 36] * 100 + para[offset + 37];

      m_para.volumeAngle = para[offset + 38] * 100 + para[offset + 39];
  } else { // not valid
      PRINTF("probe is not existed\n");
      m_para.exist = FALSE;
      m_para.type= 'F';
  }
  //--verify factory flag
  int j;
  char factoryFlag[4];
  for( j = 0; j < 3; j ++) {
      factoryFlag[j] = para[offset + 60 + j];
  }
  factoryFlag[j] = '\0';
  strcpy(m_para.factoryFlag, (GetProbeFactoryFlag(factoryFlag, m_para.model)).c_str());
}

void ProbeSocket::ActiveSocket() {
  m_io.SetInterface(m_socketAddr);
}

void ProbeSocket::ActiveHV(bool on) {
  m_io.HighVoltage(on);
}

void ProbeSocket::GetProbePara(ProbePara &para) {
  para = m_para;
}

void ProbeSocket::SetProbePara(ProbePara para) {
  m_para = para;
}

void ProbeSocket::WriteProbePara(unsigned char* buf, unsigned int len) {
  m_io.ProbeWrite(m_cmdRead, buf, len);
}

int ProbeSocket::GetProbeIndex(char* model, unsigned char probeType) {
  unsigned char type = probeType;
  PRINTF("********type %c\n",type);

  // find probe's index
  PRINTF("----------------model = %s\n", model);
  int index = 0;
  for (index = 0; index < NUM_PROBE; index ++) {
      PRINTF("list model = %s\n", g_probe_list[index].c_str());
      if (strcmp(model, g_probe_list[index].c_str()) == 0) {
          break;
      }
  }

  PRINTF("index = %d\n", index);
  if (index == NUM_PROBE) {
      if ((type == 'C') || (type == 'V')) {
          index = CONVEX_35R50; // convex
      } else if ((type == 'N') && (type == 'L') && (type == 'T')) {
          index = TINYCONVEX_35R20; // low freq tiny convex
      } else if ((type == 'c') || (type == 'n') || (type == 'v')) {
          index = TINYCONVEX_65R15; // high freq tiny convex
      } else if (type == 't') {
          index = TV_65R10; //TV
      } else if (type == 'l') {
          index = LINEAR_75W40; //linear
      } else if ((type == 'P') || (type == 'p')) {
          index = PHASE_30P16A; //phase
      } else {
          index = CONVEX_35R50; //convex
      }

      if (index >= NUM_PROBE)
          index = 0;
  }

  return index;
}

void ProbeSocket::GetFreq2D(char* model, unsigned char probeType, vector<FreqPair> &freqPair) {
  // get pair of freq
  int index = GetProbeIndex(model, probeType);
  freqPair.clear();

  for (int i = 0; i < MAX_2D_FREQ; i ++) {
    PRINTF("emit = %d, rece = %d\n", FREQ2D[index][i].emit, FREQ2D[index][i].receive);
    if ((FREQ2D[index][i].emit == 0) || (FREQ2D[index][i].receive == 0))
        return;

    freqPair.push_back(FREQ2D[index][i]);
  }
}

void ProbeSocket::GetHarmonicFreq(char* model, unsigned char probeType) {
  // get pair of freq
  int index = GetProbeIndex(model, probeType);

  ASSERT(index >= NUM_PROBE);
  // 现在只需要在界面上显示3个谐波频率，为了以后需求，程序里面是支持4个谐波频率。
  for (int i = 0; i < MAX_HARMONIC_FREQ; i ++) {
    if (FREQ_THI[index][i] == 0) {
      return;
    }

    m_para.thiFreq[i] = FREQ_THI[index][i];
    PRINTF("valule:%d index:%d\n", FREQ_THI[index][i], index);
  }
}

string ProbeSocket::GetProbeFactoryFlag(char* flag, char* model) {
  string strFlag=flag;
  if(strFlag.empty()) {
    int i;
    for (i = 0; i < NUM_PROBE; i ++) {
      if (strcmp(model, g_probe_list[i].c_str()) == 0) {
        break;
      }
    }

    int index = i;
    if (index >= NUM_PROBE) { //not found
      strFlag="EMP";
    } else {
      switch((EPROBELIST355)index) {
      case P35C50LP:
      case P70L40JP:
      case P75L40JP:
      case P65C10LP:
        strFlag="APX";
        break;
      case P65C15EP:
      case P30P16BP:
        strFlag="JR";
        break;
      case P10L25KP:
        strFlag="SNR";
        break;
      case P35C20IP:
      case P65C10IP:
      case P70L60JP:
      case P90L40JP:
      default:
        strFlag="EMP";
        break;
      }
    }
  } else {
    if(!((strFlag == "EMP")||(strFlag == "SNR")||(strFlag == "NDK")||(strFlag == "APX")||(strFlag == "OSC")||(strFlag == "JR"))) {
      strFlag="EMP";
    }
  }

  return strFlag;
}
