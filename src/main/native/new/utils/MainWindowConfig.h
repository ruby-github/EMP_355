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
#define MENU_AREA_W 180
#define MENU_AREA_H (SCREEN_HEIGHT - KNOB_AREA_H - HINT_AREA_H)

// IMG
#define IMG_AREA_X  0
#define IMG_AREA_Y  TOP_AREA_H
#define IMG_AREA_W  (SCREEN_WIDTH - 180)
#define IMG_AREA_H  (SCREEN_HEIGHT - TOP_AREA_H - KNOB_AREA_H - HINT_AREA_H)

// IMAGE
#define IMAGE_X     100
#define IMAGE_Y     30
#define IMAGE_W     644
#define IMAGE_H     448

#define GAP_AREA_H  2

// CURSOR

// SYSCURSOR
#define SYSCURSOR_X             (MENU_AREA_X + MENU_AREA_W / 2)
#define SYSCUROSR_Y             (MENU_AREA_Y + MENU_AREA_H / 2)

// SCREEN CURSOR
#define SCREEN_CURSOR_START_X   0
#define SCREEN_CURSOR_START_Y   0
#define SCREEN_CURSOR_FINISH_X  (SCREEN_CURSOR_START_X + SCREEN_WIDTH)
#define SCREEN_CURSOR_FINISH_Y  (SCREEN_CURSOR_START_Y + SCREEN_HEIGHT)

// IMAGE CURSOR
#define IMAGE_CURSOR_START_X    (IMG_AREA_X + IMAGE_X)
#define IMAGE_CURSOR_START_Y    (IMG_AREA_Y + IMAGE_Y)
#define IMAGE_CURSOR_FINISH_X   (IMAGE_CURSOR_START_X + IMAGE_W)
#define IMAGE_CURSOR_FINISH_Y   (IMAGE_CURSOR_START_Y + IMAGE_H)

// MENU CURSOR
#define NENU_CURSOR_START_X     MENU_AREA_X
#define NENU_CURSOR_START_Y     MENU_AREA_Y
#define NENU_CURSOR_FINISH_X    (NENU_CURSOR_START_X + MENU_AREA_W)
#define NENU_CURSOR_FINISH_Y    (NENU_CURSOR_START_Y + MENU_AREA_H)

#define BIOPSY_CURSOR_START_Y   128
#define BIOPSY_CURSOR_FINISHT_Y 213

#endif
