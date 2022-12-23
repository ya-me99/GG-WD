#include "include/window.h"
#include "include/shader.h"
#include "include/render.h"

int main()
{
 Init_Window();
 Init_Render();

 //glEnable(GL_BLEND);
 //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
 glEnable(GL_PROGRAM_POINT_SIZE);
 
 uint64_t t;
 uint64_t dt;

 Points test=Points_Build(1000,100);

 GLuint compute_shader;
 compute_shader=Shader_BuildProg("shaders/spline_compute.glsl");

 glUseProgram(compute_shader); 

 GLuint control_point_x=glGetUniformLocation(compute_shader,"control_points_x");
 GLuint control_point_y=glGetUniformLocation(compute_shader,"control_points_y");
 GLuint control_point_z=glGetUniformLocation(compute_shader,"control_points_z");
 float p1[2]={0.0,0.0};
 float p2[2]={0.5,0.5};
 float p3[2]={1.0,0.0};
 
 glUniform2fv(control_point_x,1,p1);
 glUniform2fv(control_point_y,1,p2);
 glUniform2fv(control_point_z,1,p3);

 uint32_t uints=8000;
 float buffer[uints*8];

 
 GLuint tex_buffer;
 glGenTextures(1, &tex_buffer);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_1D, tex_buffer);
 glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, uints*2 , 0 , GL_RGBA, GL_FLOAT,buffer);
 glBindImageTexture(0,tex_buffer,0,GL_FALSE,0,GL_READ_WRITE,GL_RGBA32F);

 glUseProgram(compute_shader); 
 glDispatchCompute(uints,1,1);

  printf("Fence -------- \n ");


 glMemoryBarrier(GL_ALL_BARRIER_BITS);

 glGetTexImage(GL_TEXTURE_1D,
               0,
               GL_RGBA,
               GL_FLOAT,
               buffer);
 
  printf("Fence -------- \n ");
 
// for(uint64_t i=0;i<64000*4;i++) {  printf(" Buffer Value at %d = %f \n " ,i,buffer[i]); }

  for(uint64_t i=0;i<uints*4;i+=2) {
     Points_Add(&test,(Point){ buffer[i],buffer[i+1],1,0,0,1,10} ); }
 
 

 

 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }
   
  Window_Update();
   
  glClear(GL_COLOR_BUFFER_BIT);   
 
  Points_Draw(&test);
 
 
  SDL_GL_SwapWindow(GlobalWindow);

  // WindowTool_Update();
 
  dt=SDL_GetTicks64()-t;
  //printf(" points drawn  %d  ---%d frames  \n " ,test.count , dt); 
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}
