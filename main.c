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
 Points_Add(&test,(Point){0,0,1,0,0,1,15});


 Point pts[3]={0.0,0.0,1,0,0,1,4,
               0.5,0.5,1,0,0,1,4,
               1.0,0.0,1,0,0,1,4};
  
 Spline spline=Spline_Build(pts,(float[4]){0,0,0,1});
 
 Spline_Update(&spline);
 
 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }
   
  Window_Update();
   
  glClear(GL_COLOR_BUFFER_BIT);   
 
  //Points_Draw(&test);
   Spline_Draw(&spline);
  SDL_GL_SwapWindow(GlobalWindow);

  // WindowTool_Update();
 
  dt=SDL_GetTicks64()-t;
  //printf(" points drawn  %d  ---%d frames  \n " ,test.count , dt); 
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}
