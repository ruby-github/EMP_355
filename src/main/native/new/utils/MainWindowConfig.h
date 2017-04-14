#ifndef __MAIN_WINDOW_CONFIG_H__
#define __MAIN_WINDOW_CONFIG_H__

//       |------------------------------------- 1024 --------------------------------------|
//           130       130                                          150          180
// ---   |---------|---------|-----------------------------------|---------|---------------|
//  |    |         |         |                                   |         |               |
//  | 60 |  LOGO   |Hospital |---------- Top --------------------|  Time   |   MenuLabel   |
//  |    |         |         |                                   |         |               |
//  |    |---------|---------|-----------------------------------|---------|---------------|
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
// 768   |                                Image                            | MenuNoteBook  |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |                                                                 |               |
//  |    |-----------------------------------------------------------------|---------------|
//  |    |                                                                 |               |
//  | 105|                                ShortcutMenu                     |   Reserved    |
//  |    |                                                                 |               |
//  |    |-----------------------------------------------------------------|---------------|
//  | 35 |                                   Hint                          | SystemStatus  |
// ---   |-----------------------------------------------------------------|---------------|

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

// TOP
#define TOP_AREA_X  0
#define TOP_AREA_Y  0
#define TOP_AREA_W  (SCREEN_WIDTH - 180)
#define TOP_AREA_H  60

// HINT
#define HINT_AREA_X 0
#define HINT_AREA_Y (SCREEN_HEIGHT - 35)
#define HINT_AREA_W (SCREEN_WIDTH - 180)
#define HINT_AREA_H 35

// KNOB
#define KNOB_AREA_X 0
#define KNOB_AREA_Y (SCREEN_HEIGHT - HINT_AREA_H - 105)
#define KNOB_AREA_W (SCREEN_WIDTH - 180)
#define KNOB_AREA_H 105

// MENU
#define MENU_AREA_X  TOP_AREA_W
#define MENU_AREA_Y  0
#define MENU_AREA_H (SCREEN_HEIGHT - KNOB_AREA_H - HINT_AREA_H)
#define MENU_AREA_W 180

// IMG
#define IMG_AREA_X  0
#define IMG_AREA_Y  TOP_AREA_H
#define IMG_AREA_W  (SCREEN_WIDTH - 180)
#define IMG_AREA_H  (SCREEN_HEIGHT - TOP_AREA_H - KNOB_AREA_H - HINT_AREA_H)

#define GAP_AREA_H  2

// IMAGE
#define IMAGE_X     110
#define IMAGE_Y     30
#define IMAGE_W     628
#define IMAGE_H     440

// SYSCURSOR
#define SYSCURSOR_X 0
#define SYSCUROSR_Y 60

#endif
