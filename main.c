
#include "include/window.h"
#include "include/shader.h"
#include "include/render.h"

int main()
{
 Init_Window();
 Init_Render();

 glEnable(GL_PROGRAM_POINT_SIZE);
	  
 uint64_t t;
 uint64_t dt;

 float pts[6]={-.5,-.5,
               0,1,
               .5,-.5};

 SplineShape shape=SplineShape_Build(pts,0);
 printf(" %f %f \n ", shape.center[0],shape.center[1]);
 uint16_t counter=0;
 uint16_t lvl=1;

 while(1)
 {   
  t=SDL_GetTicks();

  if(GlobalWindowState.close) { break; }

  Window_Update();
     
  glClear(GL_COLOR_BUFFER_BIT);
  
  SplineShape_Draw(shape);
  counter++;
  if(counter==60){ lvl++ ; SplineShape_SetDetail(&shape,lvl); counter=0;
                   printf(" %d Level ------------------------  \n ",lvl);
                 }
  

  SDL_GL_SwapWindow(GlobalWindow);

  dt=SDL_GetTicks64()-t;
 
  // WindowTool_Update();
 

  printf("DT = %d \n ", dt);
   
 }
 
 SDL_DestroyWindow(GlobalWindow);
 SDL_Quit();
 return 0;
}


