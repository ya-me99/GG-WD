#include "include/render.h"
#include "include/window.h"

GLuint dummy_vao;
GLuint point_shader;

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
 glGenVertexArrays(1,&dummy_vao);
 point_shader=Shader_BuildProg("shaders/points.glsl");

 
}

Points Points_Build(uint32_t units,uint32_t load)
{
 Points pts;

 pts.data=ArrayF32_Build(units*7*sizeof(float),load*7*sizeof(float));
 pts.count=0;
 pts.need_update=0;
 pts.units=units;
 pts.load=load;

 glGenBuffers(1,&pts.vbo);
 glGenVertexArrays(1,&pts.vao);

 glBindVertexArray(pts.vao); 
 glBindBuffer(GL_ARRAY_BUFFER,pts.vbo);
 glBufferData(GL_ARRAY_BUFFER,7*sizeof(float)*units,NULL,GL_DYNAMIC_DRAW);
 
 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(float),0);
 glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)8);
 glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,7*sizeof(float),(void*)24);

 glEnableVertexAttribArray(0);
 glEnableVertexAttribArray(1);
 glEnableVertexAttribArray(2);

 return pts;
}

void Points_Add(Points* points,Point point)
{
 ArrayF32_AddArray(point,&(points->data),7); 


 points->count++;
 points->need_update=1;
  
 if(points->count>points->units){
  Points_ResizeBuffer(points); 
 }
}

void Points_PointUpdate(Points* points,Point point,uint32_t unit)
{

 if(points->units<unit){ return ; }
 
 uint32_t index=unit*7;

 points->data.array[index]=point[0];
 points->data.array[index+1]=point[1];
 points->data.array[index+2]=point[2];
 points->data.array[index+3]=point[3];
 points->data.array[index+4]=point[4];
 points->data.array[index+5]=point[5];
 points->data.array[index+6]=point[6];

 //memcpy(&(),point,7*sizeof(float));
 
 points->need_update=1;
}

void Points_ResizeBuffer(Points* points)
{
  uint64_t size=7*sizeof(float)*points->load +
                7*sizeof(float)*points->count;

  glBindBuffer(GL_ARRAY_BUFFER,points->vbo);
  glBufferData(GL_ARRAY_BUFFER,size,NULL,GL_DYNAMIC_DRAW);

  points->units=points->count+points->load;
  points->need_update=1;
}

void Points_Draw(Points* points)
{

 if(points->need_update)
 { 
  glBindBuffer(GL_ARRAY_BUFFER,points->vbo);
 
  glBufferSubData( GL_ARRAY_BUFFER,
                   0,
                   sizeof(float)*points->data.used,
                   points->data.array);

  points->need_update=0;
 }

 glUseProgram(point_shader);
 glBindVertexArray(points->vao);  
 glDrawArrays(GL_POINTS,0,points->count);

}

void Points_Remove();

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

