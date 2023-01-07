#include "include/render.h"
#include "include/window.h"

GLuint dummy_vao;
GLuint point_shader;
GLuint bernstein_shader;
GLuint bernstein_locs[3];
GLuint rect_batch_shader;

GLuint spline_shape_shader;
GLint spline_shape_locs[100];


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

 // -------------------   Point Init
  
 glGenVertexArrays(1,&dummy_vao);
 point_shader=Shader_BuildProg("shaders/points.glsl"); 


 // -------------------   Bernstein Init
 
 bernstein_shader=Shader_BuildProg("shaders/spline_compute.glsl"); 
 glUseProgram(bernstein_shader);
 bernstein_locs[0]=glGetUniformLocation(bernstein_shader,"control_points_x");
 bernstein_locs[1]=glGetUniformLocation(bernstein_shader,"control_points_y");
 bernstein_locs[2]=glGetUniformLocation(bernstein_shader,"control_points_z");

 // -------------------   Rect Batch Init

 rect_batch_shader=Shader_BuildProg("shaders/rect_batch.glsl");


 // -------------------   Spline Shape Init
 
 spline_shape_shader=Shader_BuildProg("shaders/spline_shape.glsl"); 
 glUseProgram(spline_shape_shader);
 spline_shape_locs[0]=glGetUniformLocation(spline_shape_shader,"u_color");
}

// ------------------------------------- SplineShape-----------------------------


void SplineShape_SetDetail(SplineShape *shape,uint64_t detail)
{
 float *arr=malloc(detail*6*sizeof(float));

 for(uint64_t i=0;i<detail;i++)
 {
  float t=(float)i/detail;
  float n=((float)i+1)/detail;

  float x=(1-t)*(1-t)*shape->pts[0]     +
            2*t*(1-t)*shape->pts[2]  +
            (t*t)*shape->pts[4];

  float y=(1-t)*(1-t)*shape->pts[1]     +
            2*t*(1-t)*shape->pts[3]  +
             (t*t)*shape->pts[5];
 
  float x1=(1-n)*(1-n)*shape->pts[0]    +
            2*n*(1-n)*shape->pts[2]  +
            (n*n)*shape->pts[4];

  float y1=(1-n)*(1-n)*shape->pts[1]    +
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



// ------------------------------------- Rect_Batch-----------------------------


RectBatch RectBatch_Build(uint32_t storage, uint16_t load ,float batch_color[4])
{
 RectBatch batch={};

 glGenBuffers(1,&batch.vbo);
 glGenBuffers(1,&batch.ebo);
 glGenVertexArrays(1,&batch.vao);
 
 glBindVertexArray(batch.vao);


 glBindBuffer(GL_ARRAY_BUFFER,batch.vbo);
 glBufferData(GL_ARRAY_BUFFER,storage*sizeof(float)*12,NULL,GL_DYNAMIC_DRAW);

 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(void*)0);
 glEnableVertexAttribArray(0);
 
 batch.data=ArrayF32_Build(storage*12,load*12);
 batch.storage=storage;
 batch.load=load;
 
 batch.batch_color[0]=batch_color[0];
 batch.batch_color[1]=batch_color[1];
 batch.batch_color[2]=batch_color[2];
 batch.batch_color[3]=batch_color[3];

 return batch;
}

void RectBatch_ResizeSSBO(RectBatch* batch, uint64_t extra)
{
 batch->storage+=batch->load+extra;
 
 glBindBuffer(GL_ARRAY_BUFFER,batch->ssbo);
 
 glBufferData(GL_ARRAY_BUFFER,
              batch->storage*sizeof(float)*12,
              batch->data.array,GL_DYNAMIC_DRAW);
 
 batch->update=0;
}

void RectBatch_Add(RectBatch* batch, float data[12])
{
 ArrayF32_AddArray(data,&(batch->data),12);

 batch->entries++;
 batch->update=1;
}

void RectBatch_Draw(RectBatch* batch)
{
 glUseProgram(rect_batch_shader);
 glBindVertexArray(batch->vao); 
 
 if(batch->entries>batch->storage) { RectBatch_ResizeSSBO(batch,0); }

 if(batch->update){ 

  glBufferSubData(GL_ARRAY_BUFFER,0,
                  batch->entries*sizeof(float)*12,
                  batch->data.array);
 
  batch->update=0;
 }

 glDrawArrays(GL_TRIANGLES,0,batch->entries*6);
 
}

void RectBatch_PrintVBO(RectBatch batch)
{

 glBindBuffer(GL_ARRAY_BUFFER,batch.vbo);
 
 float buffer[batch.storage*12];

 glGetBufferSubData(GL_ARRAY_BUFFER,0,batch.entries*12*sizeof(float),buffer);

 for(uint64_t i=0;i<batch.entries*12;i++)
 {
  printf(" SSBO %d  = % f \n ",i,buffer[i]);
 }

}

void RectBatch_PrintData(RectBatch batch)
{
 for(uint64_t i=0;i<batch.data.used;i++)
 {
  printf(" Value At %d = %f ------------- \n" , batch.data.array[i]);
 }
}

void RectBatch_PrintInfo(RectBatch batch)
{
 printf(" entries = %d ------------- \n" , batch.entries);
 printf(" storage = %d ------------- \n" , batch.storage);
 printf(" load    = %d ------------- \n" , batch.load);
 printf(" update  = %d ------------- \n" , batch.update);
 printf(" ssbo    = %d ------------- \n" , batch.ssbo); 
 printf(" color   = %f %f %f %f----- \n" , batch.batch_color[0],
                                           batch.batch_color[1],
                                           batch.batch_color[2],
                                           batch.batch_color[3]); 
}

// -------------------   Points Render -----------------------------

Points Points_Build(uint32_t space,uint32_t load)
{
 Points pts;

 pts.data=ArrayF32_Build(space*7*sizeof(float),load*7*sizeof(float));
 pts.count=0;
 pts.need_update=0;
 pts.space=space;
 pts.load=load;

 glGenBuffers(1,&pts.vbo);
 glGenVertexArrays(1,&pts.vao);

 glBindVertexArray(pts.vao); 
 glBindBuffer(GL_ARRAY_BUFFER,pts.vbo);
 glBufferData(GL_ARRAY_BUFFER,7*sizeof(float)*space,NULL,GL_DYNAMIC_DRAW);
 
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
  
  if(points->count==points->space){
    Points_ResizeBuffer(points); 
  }

  ArrayF32_AddArray(point,&(points->data),7); 

  points->count++;
  points->need_update=1;
  
}

void Points_AddArray(Points *points,Point *point,uint64_t count)
{
  if(points->count+count==points->space){
    points->load+=count;
    Points_ResizeBuffer(points);
    points->load-=count;
  }

  ArrayF32_AddArray(point,&(points->data),count*7);

  points->count+=count;
  points->need_update=1;
}

void Points_PointUpdate(Points* points,Point point,uint32_t unit)
{

  if(points->space<unit){ return ; }
 
  uint32_t index=unit*7;

  points->data.array[index]=point[0];
  points->data.array[index+1]=point[1];
  points->data.array[index+2]=point[2];
  points->data.array[index+3]=point[3];
  points->data.array[index+4]=point[4];
  points->data.array[index+5]=point[5];
  points->data.array[index+6]=point[6];
 
  points->need_update=1;
}

void Points_ResizeBuffer(Points* points)
{
  uint64_t size=7*sizeof(float)*points->load +
    7*sizeof(float)*points->count;

  glBindBuffer(GL_ARRAY_BUFFER,points->vbo);
  glBufferData(GL_ARRAY_BUFFER,size,NULL,GL_DYNAMIC_DRAW);

  points->space=points->count+points->load;
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

// ---------------------------   Spline Render -------------------------------

Spline Spline_Build(Point control_points[3],float spline_color[4])
{
  Spline spline={};

  memcpy(spline.control_points,control_points,3*7*sizeof(float));  
  printf(" %f -- %f  %f -- %f  %f -- %f  \n ", spline.control_points[0][0],
	 spline.control_points[0][1],
	 spline.control_points[1][0],
	 spline.control_points[1][1],
	 spline.control_points[2][0],
	 spline.control_points[2][1]);
	                         
  spline.spline=Points_Build(1000,1000);
  Points_Add(&spline.spline,control_points[0]);
  Points_Add(&spline.spline,control_points[1]);
  Points_Add(&spline.spline,control_points[2]);

  spline.accuracy=3000;
  spline.size=10;
  
  spline.color[0]=spline_color[0];
  spline.color[1]=spline_color[1]; 
  spline.color[2]=spline_color[2];
  spline.color[3]=spline_color[3];


  return spline;
}

void Spline_Update(Spline* spline)
{
  
  float buffer[spline->accuracy*2];
  
  float p1[2]={spline->control_points[0][0],spline->control_points[0][1]};
  float p2[2]={spline->control_points[1][0],spline->control_points[1][1]};
  float p3[2]={spline->control_points[2][0],spline->control_points[2][1]};

  glUseProgram(bernstein_shader);

  glUniform2fv(bernstein_locs[0],1,p1);
  glUniform2fv(bernstein_locs[1],1,p2);
  glUniform2fv(bernstein_locs[2],1,p3);
 
  glUseProgram(bernstein_shader); 
  glDispatchCompute(1500,1,1); 
 
  glMemoryBarrier(GL_ALL_BARRIER_BITS);

  glGetTexImage(GL_TEXTURE_1D,
		0,
		GL_RGBA,
		GL_FLOAT,
		buffer);
 
  
  for(uint64_t i=0;i<spline->accuracy;i++)
    {
      Point pt={ buffer[i*2],buffer[i*2+1],
	spline->color[0],spline->color[1],
	spline->color[2],spline->color[3],
	spline->size};
    
   Points_Add(&(spline->spline),pt);
 }
 
}
  
void Spline_Draw(Spline *spline)
{

  Points_Draw(&(spline->spline));
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

