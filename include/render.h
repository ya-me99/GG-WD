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


// ------------------------------------- RectBatch-----------------------------

typedef struct
{
 ArrayF32 data;
 uint32_t entries;
 uint32_t storage;
 uint16_t load;
 uint8_t update;
 float batch_color[4];
 GLuint ssbo,vao,ebo,vbo;
}RectBatch;



RectBatch RectBatch_Build(uint32_t ssbo_storage, uint16_t ssbo_load , float batch_color[4]);

void RectBatch_Add(RectBatch* batch, float data[12]);

void RectBatch_Draw(RectBatch* batch);

void RectBatch_ResizeSSBO(RectBatch* batch, uint64_t extra);

void RectBatch_PrintSSBO(RectBatch batch);

void RectBatch_PrintData(RectBatch batch);

void RectBatch_PrintInfo(RectBatch batch);

// ------------------------------------- Points --------------------------------

typedef float Point[7];

typedef struct 
{
 ArrayF32 data;
 uint32_t count;
 uint32_t space;
 uint32_t load;
 uint8_t  need_update;
 GLuint   vbo,vao;
}Points;

Points Points_Build(uint32_t space, uint32_t load);

void Points_Add(Points *points,Point point);

void Points_AddArray(Points *points,Point *point,uint64_t count);

void Points_Draw(Points* points);

void Points_ResizeBuffer(Points *points);

void Points_Remove();

void Points_PointUpdate(Points* points,Point point,uint32_t unit);

// ------------------------------------- Compute Shader --------------------------------- 

// ---------------------------   Spline Render -------------------------------

typedef struct
{
 Point control_points[3];
 Points spline;
 float color[4];
 float size;
 uint64_t accuracy;
}Spline;

Spline Spline_Build(Point control_points[3], float spline_color[4]);

void Spline_Update(Spline *spline);

void Spline_Draw(Spline* spline);

// ------------------------------------- Utils --------------------------------- 

void Init_Render();

uint8_t Point_Hovered(float size , vec2 pos , vec2 hovered);

int8_t IsHovered(vec2 m_pos,vec2 max,vec2 min);

uint8_t IsPoint_Rect(float px,float py,float x_top,
                     float y_top,float x_bot,float y_bot);

uint8_t ScreenToNdc(uint32_t screen_x , uint32_t screen_y , vec2 result);

uint8_t NdcToPixel(float x , float y , uint16_t* result);



#endif
