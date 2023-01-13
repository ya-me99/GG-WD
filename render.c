#include "include/render.h"
#include "include/window.h"
 
GLuint spline_shape_shader;
GLint  spline_shape_locs[100];

GLuint  spline_loop_shader;
GLuint  spline_loop_locs[100];
uint8_t spline_loop_max_detail;
uint8_t spline_loop_load_units;
uint8_t spline_loop_control_points;
uint32_t spline_loop_load_size;
uint32_t spline_loop_load_bytes;


GENERIC_ARRAY_DEF(ArrayU8,uint8_t);
GENERIC_ARRAY_DEF(ArrayU16,uint16_t);
GENERIC_ARRAY_DEF(ArrayU32,uint32_t);
GENERIC_ARRAY_DEF(ArrayU64,uint64_t);
GENERIC_ARRAY_DEF(ArrayS8,int8_t);
GENERIC_ARRAY_DEF(ArrayS16,int16_t);
GENERIC_ARRAY_DEF(ArrayS32,int32_t);
GENERIC_ARRAY_DEF(ArrayS64,int64_t);
GENERIC_ARRAY_DEF(ArrayF32,float);
GENERIC_ARRAY_DEF(ArrayF64,double);


void Init_Render()
{
  
 // -------------------   Spline Shape Init
 
 spline_shape_shader=Shader_BuildProg("shaders/spline_shape.glsl"); 
 glUseProgram(spline_shape_shader);
 spline_shape_locs[0]=glGetUniformLocation(spline_shape_shader,"u_color");

 // -------------------   Spline Loop Init

 spline_loop_shader=Shader_BuildProg("shaders/spline_loop.glsl");
 spline_loop_max_detail=40;
 spline_loop_control_points=3;
 spline_loop_load_units=10;
 spline_loop_load_size=spline_loop_max_detail*spline_loop_load_units;
 spline_loop_load_bytes=spline_loop_load_size*2*sizeof(float);

 spline_loop_locs[0]=glGetUniformLocation( spline_loop_shader,"u_color");
 spline_loop_locs[1]=glGetUniformLocation( spline_loop_shader,"u_size");
}

// ---------------------------   Spline Array -------------------------------

SplineLoop SplineLoop_Build(float color[4],float size)
{
  SplineLoop loop={};

 loop.control_points=ArrayF32_Build(spline_loop_load_units*3,
                                    spline_loop_load_units*3);
                                     

 loop.lines=ArrayF32_Build(spline_loop_load_bytes/sizeof(float),
                           spline_loop_load_bytes/sizeof(float));
 
 loop.detail=ArrayU8_Build(spline_loop_load_units,
                           spline_loop_load_units);


 loop.size=size;

 loop.color[0]=color[0];
 loop.color[1]=color[1];
 loop.color[2]=color[2];
 loop.color[3]=color[3];


 glGenBuffers(1,&loop.vbo);
 glGenVertexArrays(1,&loop.vao);

 loop.vbo_storage=spline_loop_load_units;

 glBindVertexArray(loop.vao); 
 glBindBuffer(GL_ARRAY_BUFFER,loop.vbo);
 glBufferData(GL_ARRAY_BUFFER,spline_loop_load_bytes,
                              NULL,GL_DYNAMIC_DRAW);
 
 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);

 glEnableVertexAttribArray(0);
 
 return loop;
}

void SplineLoop_AddSpline(SplineLoop *loop,float control_points[6],uint8_t detail)
{
 
 ArrayF32_Add(control_points[0],&loop->control_points);
 ArrayF32_Add(control_points[1],&loop->control_points);
 ArrayF32_Add(control_points[2],&loop->control_points);
 ArrayF32_Add(control_points[3],&loop->control_points);
 ArrayF32_Add(control_points[4],&loop->control_points);
 ArrayF32_Add(control_points[5],&loop->control_points);


ArrayU8_Add(detail,&loop->detail);

 float array[detail*2];
 
 for(uint64_t i=0;i<=detail;i++)
 {
  float t=(float)i/detail;

  float x=(1-t)*(1-t)*control_points[0]  +
            2*t*(1-t)*control_points[2]  +
            (t*t)*control_points[4];

  float y=(1-t)*(1-t)*control_points[1]  +
            2*t*(1-t)*control_points[3]  +
             (t*t)*control_points[5];
 
   array[i*2]=x;
   array[i*2+1]=y;

   printf(" ------ x %f   ------ y %f   \n",x,y);
  } 
 
 ArrayF32_AddArray(array,&loop->lines,detail*2);
 loop->vbo_entries++;
 loop->vbo_update=1;
}

void SplineLoop_UpdateVBO(SplineLoop *loop)
{

 glBindBuffer(GL_ARRAY_BUFFER,loop->vbo);

 if(loop->vbo_entries>=loop->vbo_storage)
 {
  uint32_t storage_bytes=loop->vbo_storage*spline_loop_max_detail*2*sizeof(float);

  glBufferData(GL_ARRAY_BUFFER,storage_bytes+
                               spline_loop_load_bytes,
                               loop->lines.array,GL_DYNAMIC_DRAW);
  
  loop->vbo_storage+=spline_loop_load_units;
 }
 
 glBufferSubData(GL_ARRAY_BUFFER,0,
                 loop->lines.used*sizeof(float),
                 loop->lines.array);

 loop->vbo_update=0;
}

void SplineLoop_ExpandVBO(SplineLoop *loop);
 
void SplineLoop_Draw(SplineLoop *loop)
{

 glLineWidth(loop->size);
 
 if(loop->vbo_update){ SplineLoop_UpdateVBO(loop); }

 glUseProgram(spline_loop_shader);
 glBindVertexArray(loop->vao);
 
 glUniform4f(spline_loop_locs[0],
             loop->color[0],loop->color[1],
             loop->color[2],loop->color[3]);

 glDrawArrays(GL_LINE_LOOP,0,loop->lines.used/2);
}


// ------------------------------------- SplineShape-----------------------------

void SplineShape_SetDetail(SplineShape *shape,uint64_t detail)
{
 float *arr=malloc(detail*6*sizeof(float));

 for(uint64_t i=0;i<detail;i++)
 {
  float t=(float)i/detail;
  float n=((float)i+1)/detail;

  float x=(1-t)*(1-t)*shape->pts[0]  +
            2*t*(1-t)*shape->pts[2]  +
            (t*t)*shape->pts[4];

  float y=(1-t)*(1-t)*shape->pts[1]  +
            2*t*(1-t)*shape->pts[3]  +
             (t*t)*shape->pts[5];
 
  float x1=(1-n)*(1-n)*shape->pts[0] +
            2*n*(1-n)*shape->pts[2]  +
            (n*n)*shape->pts[4];

  float y1=(1-n)*(1-n)*shape->pts[1] +
            2*n*(1-n)*shape->pts[3]  +
             (n*n)*shape->pts[5];
   
   arr[i*6]=shape->center[0];
   arr[i*6+1]=shape->center[1];
   arr[i*6+2]=x;
   arr[i*6+3]=y;
   arr[i*6+4]=x1;
   arr[i*6+5]=y1;
  }
 
 if(shape->data) { free(shape->data); }
 shape->data=arr;
 shape->detail=detail;


 glBindBuffer(GL_ARRAY_BUFFER,shape->vbo);
 glBufferData(GL_ARRAY_BUFFER,detail*sizeof(float)*6,shape->data,GL_STATIC_DRAW);
}

SplineShape SplineShape_Build(float pts[6],uint64_t detail)
{
 SplineShape shape={ };

 shape.pts[0]=pts[0];
 shape.pts[1]=pts[1];
 shape.pts[2]=pts[2];
 shape.pts[3]=pts[3];
 shape.pts[4]=pts[4];
 shape.pts[5]=pts[5];
 
 

 shape.center[0]=pts[0]+(pts[4]-pts[0])/2;
 shape.center[1]=pts[1]+(pts[5]-pts[1])/2;


 glGenBuffers(1,&shape.vbo);
 glGenBuffers(1,&shape.ebo);
 glGenVertexArrays(1,&shape.vao);
 glBindVertexArray(shape.vao); 
 glBindBuffer(GL_ARRAY_BUFFER,shape.vbo);

 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
 glEnableVertexAttribArray(0);
  
 SplineShape_SetDetail(&shape,detail); 

 return shape;
}


void SplineShape_Draw(SplineShape shape)
{
 glUseProgram(spline_shape_shader);
 glBindVertexArray(shape.vao);

 glDrawArrays(GL_TRIANGLES,0,shape.detail*3);
}


// ------------------------------   Utils ------------------------------------

uint8_t Point_Hovered(float size , vec2 pos , vec2 hovered)
{
 vec2 width; 
 ScreenToNdc(0+(size/2),0,width);
 
 width[1]=0;
 width[0]+=1;

 // printf(" Width = %f \n ",width[0]);

 vec2 p_top_left= {};  
 p_top_left[0]=pos[0] -  (width[0] );
 p_top_left[1]=pos[1] +  (width[0] );
 

 vec2 p_bot_right={ };  
 p_bot_right[0]= pos[0] + (width[0] );
 p_bot_right[1]= pos[1] - (width[0] ); 

 // printf("p_top_left x %f  ___ p_top_left y %f    \n " , p_top_left[0] , p_top_left[1]);

 // printf("p_bot_right x %f ___ p_bot_right y %f   \n " , p_bot_right[0] , p_bot_right[1]);

 if(hovered[0]>p_bot_right[0]){ return  0;} 
 if(hovered[0]<p_top_left[0]) { return  0;} 
 if(hovered[1]>p_top_left[1]) { return  0;} 
 if(hovered[1]<p_bot_right[1]){ return  0;} 

 return 1;
}


int8_t IsHovered(vec2 m_pos,vec2 max, vec2 min)
{
 if((max[0]+1)>(m_pos[0]+1)){ return 0; }
 if((max[1]+1)<(m_pos[1]+1)){ return 0; }
 
 if((min[0]+1)<(m_pos[0]+1)){ return 0; }
 if((min[1]+1)>(m_pos[1]+1)){ return 0; }
 
 return 1;
 //s.narrow();
}


uint8_t ScreenToNdc(uint32_t screen_x , uint32_t screen_y , vec2 result)
{
 GLuint data[4];
 glGetIntegerv(GL_VIEWPORT,data);
 float x_w = data[2]/2;
 float y_w= data[3]/2;
 result[0]=(screen_x - x_w ) / x_w ;
 result[1]=-( (screen_y - y_w ) / y_w );
}

// Dosnt Work
uint8_t NdcToPixel(float x,float y, uint16_t* result)
{
 GLuint data[4];
 uint32_t x_w= data[2]/2;
 uint32_t y_w= data[3]/2;
 glGetIntegerv(GL_VIEWPORT,data);
 result[0]=x*x_w+x+x_w;
 result[1]=y*y_w+x+y_w; 
}

// Rework
uint8_t IsPoint_Rect(float px,float py,float x_top,
                     float y_top,float x_bot,float y_bot)
{
 if((y_top+1)<(py+1)){ return 0; }
 if((y_bot+1)>(py+1)){ return 0; }
 if((x_top+1)>(px+1)){ return 0; }
 if((x_bot+1)<(px+1)){ return 0; }
 return 1;
}

