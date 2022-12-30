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
 
 vec4 color={1,1,1,1};

 RectBatch batch=RectBatch_Build(1000000,1000,color);

 uint64_t stress=1000000;

 for(uint64_t i=0;i<stress;i++)
 {
  float t= (float)i/(float)stress;
  float val=1-t;
  float rect[12]={-val,val,val,-val,val,val,-val,-val,val,val,val,1};
  RectBatch_Add(&batch,rect);
 }

 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }
   
  Window_Update();
   
  glClear(GL_COLOR_BUFFER_BIT);   

  RectBatch_Draw(&batch);
  SDL_GL_SwapWindow(GlobalWindow);

  // WindowTool_Update();
 
  dt=SDL_GetTicks64()-t;
  printf(" rects  drawn  %d  ---  %d millis  \n " ,batch.ssbo_entries,dt); 
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}
