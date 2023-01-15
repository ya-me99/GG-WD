#ifndef RENDER
#define RENDER
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <cglm/cglm.h>
#include "shader.h"
#include "stb_image.h"
#include "da_type.h"
#include <math.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>

// ---------------------------   BrushMap ---------------------

typedef enum
{
 PERFECT_FIT,
 WIDTH_FIT,
 HEIGHT_FIT,
 POORLY_FIT,
}BBoxFit;

typedef struct
{
 ArrayU16 brushes;
 ArrayU16 storage_regions; 
 GLuint map;
}BrushMap;

BrushMap BrushMap_Build();

uint8_t BrushMap_AddBrush();

// ---------------------------   Brush -------------------------------

typedef struct
{
 GLuint image;
 float* image_data;
 uint16_t image_bbox[4];
 GLuint vao,vbo;
}Brush;

Brush Brush_Build();

Brush Brush_Cricle(uint16_t radius,float border_function, float color[4]);

// ------------------------------------- SplineShape-----------------------------


typedef struct
{
 float* data;
 float pts[6];
 float center[2];
 float color[4];
 GLuint detail;
 GLuint vao,vbo,ebo;
}SplineShape;

SplineShape SplineShape_Build(float cnt_pts[6],uint64_t detail);

void SplineShape_Draw(SplineShape shape);

void SplineShape_SetDetail(SplineShape *shape, uint64_t detail);

 
// ------------------------------------- Compute Shader --------------------------------- 

typedef struct
{
 ArrayF32 control_points;
 ArrayF32 lines;
 ArrayU8  detail;
 uint32_t vbo_entries;
 uint32_t vbo_storage;
 uint8_t vbo_update;
 GLuint vao,vbo;
 float color[4];
 float size;
}SplineLoop;


SplineLoop SplineLoop_Build(float color[4],float size);

void SplineLoop_AddSpline(SplineLoop *loop, float control_points[6],uint8_t detail);

void SplineLoop_Update(SplineLoop *loop);

void SplineLoop_Draw(SplineLoop *loop);

// ------------------------------------- Utils --------------------------------- 

void Init_Render();

void GetVboData(GLuint vbo, uint64_t storage_bytes,float* buffer);

void GetRectDimensions(uint16_t top_x,  uint16_t top_y,
                       uint16_t bot_x,  uint16_t bot_y,
                       uint32_t* width, uint32_t* height);


uint8_t Point_Hovered(float size , vec2 pos , vec2 hovered);

int8_t IsHovered(vec2 m_pos,vec2 max,vec2 min);

uint8_t IsPoint_Rect(float px,float py,float x_top,
                     float y_top,float x_bot,float y_bot);

uint8_t ScreenToNdc(uint32_t screen_x , uint32_t screen_y , vec2 result);

uint8_t NdcToPixel(float x , float y , uint16_t* result);



#endif
