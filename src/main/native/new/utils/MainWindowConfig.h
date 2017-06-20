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

#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

// TOP
#define TOP_AREA_X      0
#define TOP_AREA_Y      0
#define TOP_AREA_W      (SCREEN_WIDTH - 180)
#define TOP_AREA_H      60

// HINT
#define HINT_AREA_X     0
#define HINT_AREA_Y     (SCREEN_HEIGHT - 35)
#define HINT_AREA_W     (SCREEN_WIDTH - 180)
#define HINT_AREA_H     35

// KNOB
#define KNOB_AREA_X     0
#define KNOB_AREA_Y     (SCREEN_HEIGHT - HINT_AREA_H - 105)
#define KNOB_AREA_W     (SCREEN_WIDTH - 180)
#define KNOB_AREA_H     105

// MENU
#define MENU_AREA_X     TOP_AREA_W
#define MENU_AREA_Y     0
#define MENU_AREA_W     180
#define MENU_AREA_H     (SCREEN_HEIGHT - KNOB_AREA_H - HINT_AREA_H)

// IMAGE
#define IMAGE_AREA_X    0
#define IMAGE_AREA_Y    TOP_AREA_H
#define IMAGE_AREA_W    TOP_AREA_W
#define IMAGE_AREA_H    (SCREEN_HEIGHT - TOP_AREA_H - KNOB_AREA_H - HINT_AREA_H)

// CANVAS
#define CANVAS_IMAGE_X  100
#define CANVAS_IMAGE_Y  20
#define CANVAS_AREA_X   (IMAGE_AREA_X + CANVAS_IMAGE_X)
#define CANVAS_AREA_Y   (IMAGE_AREA_Y + CANVAS_IMAGE_Y)
#define CANVAS_AREA_W   (IMAGE_AREA_W - 200)
#define CANVAS_AREA_H   (IMAGE_AREA_H - 120)

#define CANVAS_BPP      3

// BODYMARK
#define BDMK_W                60
#define BDMK_H                75
#define BDMK_PROBE_LEN        24
#define BDMK_MAX_SCALE        1.5
#define BDMK_PROBE_MAX_SCALE  1.2

#define GAP_AREA_H      2

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
#define IMAGE_CURSOR_START_X    CANVAS_AREA_X
#define IMAGE_CURSOR_START_Y    CANVAS_AREA_Y
#define IMAGE_CURSOR_FINISH_X   (IMAGE_CURSOR_START_X + CANVAS_AREA_W)
#define IMAGE_CURSOR_FINISH_Y   (IMAGE_CURSOR_START_Y + CANVAS_AREA_H)

// MENU CURSOR
#define NENU_CURSOR_START_X     MENU_AREA_X
#define NENU_CURSOR_START_Y     MENU_AREA_Y
#define NENU_CURSOR_FINISH_X    (NENU_CURSOR_START_X + MENU_AREA_W)
#define NENU_CURSOR_FINISH_Y    (NENU_CURSOR_START_Y + MENU_AREA_H)

#define BIOPSY_CURSOR_START_Y   128
#define BIOPSY_CURSOR_FINISHT_Y 213

#endif
