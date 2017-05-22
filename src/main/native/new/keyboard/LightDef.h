#ifndef __LIGHT_DEF_H__
#define __LIGHT_DEF_H__

enum ELightValue {
  LIGHT_BODYMARK  = 0x08,
  LIGHT_QUAD      = 0x09,
  LIGHT_INVERT    = 0x0A,
  LIGHT_LEFTRIGHT = 0xB,
  LIGHT_UPDOWN    = 0x0C,
  LIGHT_CW        = 0x0D,
  LIGHT_PDI       = 0x0E,
  LIGHT_PW        = 0x0F,
  LIGHT_CFM       = 0x11,
  LIGHT_D2        = 0x12,
  LIGHT_DUAL      = 0x13,
  LIGHT_M         = 0x14,
  LIGHT_FREEZE    = 0x15,
  LIGHT_FOCUS     = 0x17,
  LIGHT_ZOOM      = 0x19,
  LIGHT_POINTER   = 0x1A,
  LIGHT_MEASURE   = 0x1B,
  LIGHT_MENU      = 11,
  LIGHT_AUTO      = 13
};

#endif
