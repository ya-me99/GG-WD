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

 for(uint64_t i=0;i<500000;i++)
 {
  Points_Add(&test,(Point){0,0,1,0,0,1,15});
 }
  
 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }
   
  Window_Update();
   
  
  glClear(GL_COLOR_BUFFER_BIT);  

  for(uint64_t i=0;i<500000;i++)
  {
   float p=(0.5+i)/500000;
   Points_PointUpdate(&test,(Point){p,p,p,p,p,1,p*100},i);
  }
 
 
  Points_Draw(&test);
 
  SDL_GL_SwapWindow(GlobalWindow);

  // WindowTool_Update();
 
  dt=SDL_GetTicks64()-t;
  
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}
