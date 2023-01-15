#include "include/window.h"
#include "include/shader.h"
#include "include/render.h"



int main()
{
 Init_Window();
 Init_Render();

 glEnable(GL_PROGRAM_POINT_SIZE);
 glEnable(GL_LINE_SMOOTH);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 
 uint64_t t;
 uint64_t dt;

 float color[4]={1,0,0,1};

 float cnt[6]={0,0,0.1,1, 1,0 };
 
 SplineLoop loop=SplineLoop_Build(color,10);
 
 SplineLoop_AddSpline(&loop,cnt,30);

 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }

  Window_Update();
     
  glClear(GL_COLOR_BUFFER_BIT); 

  SplineLoop_Draw(&loop);
  
  SDL_GL_SwapWindow(GlobalWindow);

  dt=SDL_GetTicks64()-t;
 
  // WindowTool_Update();
 

  //   printf("DT = %d \n ", dt);
   
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}

