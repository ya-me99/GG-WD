#ifndef WINDOW
#define WINDOW
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include "render.h"
#include <cglm/cglm.h>
#include "da_type.h"

typedef enum
{
 WINDOW_EVENTS,
}WindowState;

typedef struct
{
 WindowState state;
 uint8_t rm_pressed;
 uint8_t lm_pressed;
 uint8_t lm_released;
 uint8_t sm_pressed;
 uint8_t l_double_click;
 uint8_t r_double_click;
 uint8_t m_motion;
 vec2 m_abs;
 vec2 m_rel;
 vec2 click_xy;
 vec2 released_xy;
 int64_t shape_focus_id;
 int64_t shape_hover_id;
 uint8_t close;
 uint8_t resized;
 vec2 win_size;
}WindowState_s;

extern WindowState_s      GlobalWindowState;
extern SDL_Window*        GlobalWindow;
extern SDL_GLContext*     GlobalContext;
extern uint8_t*           GlobalKeymap;

void Init_Window();

void Window_Update();

void WindowTool_Update();

void WindowEvents_Update();

#endif
