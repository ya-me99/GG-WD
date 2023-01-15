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
 spline_loop_max_detail=40; // Max Lines/Vertecies
 spline_loop_control_points=3;
 spline_loop_load_units=10; 
 spline_loop_load_size=(spline_loop_max_detail+1)*spline_loop_load_units; // +1 for last line
 spline_loop_load_bytes=spline_loop_load_size*2*sizeof(float);

 spline_loop_locs[0]=glGetUniformLocation( spline_loop_shader,"u_color");
}

// ---------------------------   BrushMap -------------------------------

BrushMap BrushMap_Build()
{
 BrushMap map={}; 

 map.brushes=ArrayU16_Build(100,10);
 map.storage_regions=ArrayU16_Build(100,10);

 glGenTextures(1,&map.map);
 glBindTexture(GL_TEXTURE_2D, map.map);
 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, 1024, 1024);
 glBindTexture(GL_TEXTURE_2D, 0); 

 return map;
}

uint8_t BrushMap_AddBrush(uint16_t bbox[4], GLuint src_texture , BrushMap *map)
{
  
 int16_t src_width=bbox[2]-bbox[0];
 int16_t src_height=bbox[3]-bbox[1];
 int32_t region_found=-1;
 int16_t region_width;
 int16_t region_height;
 int16_t width_diff;
 int16_t height_diff;
 
 BBoxFit fit;

 for(uint64_t i=0;i<map->storage_regions.used;i+=4)
 {
  if(map->storage_regions.array[i]==0){ continue; }

  region_width=map->storage_regions.array[i+2]-
               map->storage_regions.array[i];
                    

  region_height=map->storage_regions.array[i+3]-
                map->storage_regions.array[i+1];


  if(width_diff=region_width-src_width>0)
  {
    if(height_diff=region_height-src_height>0)
    {
     region_found=i;
     
     if(width_diff&&height_diff==0) { fit=PERFECT_FIT ; break;}
     if(height_diff==0) { fit=HEIGHT_FIT ; break; } 
     if(width_diff==0) { fit=WIDTH_FIT   ; break; }
     
     fit=POORLY_FIT;
     break;
    }
  }
 }


 uint16_t dst_x=map->storage_regions.array[region_found];
 uint16_t dst_y=map->storage_regions.array[region_found+1];


 if(fit==POORLY_FIT)
 {
  uint16_t sub_regions[8];
  
  sub_region[0]=dst_x+src_width;
  sub_region[1]=dst_y;

  sub_region[2]=map->storage_regions.array[region_found+2];
  sub_region[3]=map->storage_regions.array[region_found+3];

  sub_region[4]=dst_x;
  sub_region[5]=dst_y+src_height;

  sub_region[6]=dest_x+src_width;
  sub_region[7]=map->storage_regions.array[region_found+3];

  ArrayU16_AddArray(sub_regions,&map->storage_regions,8);
 }
 else if(fit==HEIGHT_FIT)
 {
  uint16_t sub_regions[4];
  
  sub_region[0]=dst_x+src_width;
  sub_region[1]=dst_y;

  sub_region[2]=map->storage_regions.array[region_found+2];
  sub_region[3]=map->storage_regions.array[region_found+3];
 
  ArrayU16_AddArray(sub_regions,&map->storage_regions,4);
 }
 else if(fit==WIDTH_FIT)
 { 
  uint16_t sub_regions[4];
  sub_region[0]=dst_x;
  sub_region[1]=dst_y+src_height;

  sub_region[2]=dest_x+src_width;
  sub_region[3]=map->storage_regions.array[region_found+3];

  ArrayU16_AddArray(sub_regions,&map->storage_regions,4);
 }
  

 map->storage_regions.array[region_found]=0;

 glCopyImageSubData(src_texture,GL_TEXTURE_2D,
	            0,
	            bbox[0],
                    bbox[1],
	            0,
	            map->map,
	            GL_TEXTURE_2D,
	            0,
	            dst_x,
	            dst_y,
	            0,
	            src_width,
	            src_height,
 	            0);
 
}



// ------------------------------ Brush

Brush Brush_Cricle(uint16_t radius,float border_function, float color[4])
{ 
 Brush brush={};
 
 float *data = malloc(sizeof(float)*radius*2*radius*2*4);
 uint32_t pixels_per_line=radius*2;
 uint32_t pixels_total=pixel_per_line*pixel_per_line;
 uint16 pixel_x;
 uint16 pixel_y;
 uint16_t center_x=radius;
 uint16_t center_y=radius;
 float distance;
 uint16_t row;
 float alpha;

 for(uint64_t j=0;j<pixels_per_line;j++)
 {
  row++;
   
  for(uint64_t i=0;i<pixels_per_line;i++)
  {
   pixel_x=i;
   pixel_y=row;
    
   distance=sqrt( (pixel_x-center_x)*(pixel_x-center_x) +
                  (pixel_y-center_y)*(pixel_y-center_y) );

   alpha=distance*boarder_function;

   data[i*4]=color[0];
   data[i*4+1]=color[1];
   data[i*4+2]=color[2];
   data[i*4+3]=alpha;
  }
 }

 glGenTextures(1,&brush.image);
 glBindTexture(GL_TEXTURE_2D, brush.image);
 glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, radius*2 , radius*2);
 glTexSubImage2D(GL_TEXTURE_2D​, 0,0​,0​,radius​*2,radius*2,GL_RGBA32F​,GL_FLOAT​, data); 
 glBindTexture(GL_TEXTURE_2D, 0); 

 image_bbox={0,0,radius*2,radius*2}; 

 brush.image_data=data;
 
 GLuint vao,vbo;

 float bot_xy[2];
 
 ScreenToNdc(2*radius,2*radius,bot_xy);
 
 float vertex_data[24]={
                        -1, 1, 0, 1,                // Top Left 
                        -1, bot_xy[1], 0, 0,        // Bot Left 
                        bot_xy[0],1, 1, 1,          // Top Right 
                        bot_xy[0],1, 1, 1,          // Top Right 
                        bot_xy[0],bot_xy[1],1,0,    // Bot Right
                        -1,  bot_xy[1], 0, 0,       // Bot Left 
                       };

 glGenBuffers(1,&brush.vbo);
 glGenVertexArrays(1,&brush.vao);

 glBindVertexArray(brush.vao); 
 glBindBuffer(GL_ARRAY_BUFFER,brush.vbo);
 glBufferData(GL_ARRAY_BUFFER,24*sizeof(float),
              vertex_data,GL_DYNAMIC_DRAW);
 
 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);
 glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,(void*)8);

 glEnableVertexAttribArray(0);
 glEnableVertexAttribArray(1);

 return brush;
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
 
 glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);

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

 float array[(detail+1)*2];
 
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

  } 
 
 ArrayF32_AddArray(array,&loop->lines,(detail+1)*2);
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

 glDrawArrays(GL_LINE_STRIP,0,loop->lines.used/2);
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


void GetRectDimensions(uint16_t top_x,  uint16_t top_y,
                       uint16_t bot_x,  uint16_t bot_y,
                       uint32_t* width, uint32_t* height)
{
 *width=bot_x-top_x;
 *height=bot_y-top_y;
}


void GetVboData(GLuint vbo, uint64_t storage_bytes , float* buffer)
{
  glBindBuffer(GL_ARRAY_BUFFER,vbo);
  glGetBufferSubData(GL_ARRAY_BUFFER,0,storage_bytes,buffer); 
}

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

