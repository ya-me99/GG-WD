#ifndef RENDER
#define RENDER
#include "glad/gl.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cglm/cglm.h>
#include "shader.h"
#include "stb_image.h"
#include "da_type.h"
#include <math.h>

// ------------------------------------- Points --------------------------------

typedef struct
{
 float x,y;
 float r,g,b,a;
 float size;
}Point;

typedef struct 
{
 ArrayF32 data;
 uint32_t count;
 uint32_t units;
 uint32_t load;
 uint8_t  need_update;
 GLuint   vbo,vao;
}Points;

Points Points_Build(uint32_t units, uint32_t load);

void Points_Add(Points *points,Point point);

void Points_Draw(Points* points);

void Points_ResizeBuffer(Points *points);

void Points_Remove();

// ------------------------------------- Utils --------------------------------- 

void Init_Render();


uint8_t Point_Hovered(float size , vec2 pos , vec2 hovered);

int8_t IsHovered(vec2 m_pos,vec2 max,vec2 min);

uint8_t IsPoint_Rect(float px,float py,float x_top,
                     float y_top,float x_bot,float y_bot);

uint8_t ScreenToNdc(uint32_t screen_x , uint32_t screen_y , vec2 result);

uint8_t NdcToPixel(float x , float y , uint16_t* result);




#endif
