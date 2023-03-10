#include "include/window.h"

#define BORDER_WIDTH 0.006o
#define BORDER_COLOR (vec3){.2,.0,.05}
#define CLEAR_COLOR  0.7,0.75,0.75,0

WindowState_s    GlobalWindowState;
SDL_Window*      GlobalWindow;
SDL_GLContext*   GlobalContext;
uint8_t*         GlobalKeymap;

void Init_Window()
{
 
 if (SDL_Init(SDL_INIT_VIDEO) != 0) {
  SDL_Log("Unable to initialize SDL: %s \n", SDL_GetError());
  exit(1);
 }
 
 int8_t err=SDL_GL_LoadLibrary(NULL);

 SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
 SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
 SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 
 GlobalWindow=SDL_CreateWindow(" Test ",0,0,1024,1024,SDL_WINDOW_OPENGL|
                                                      SDL_WINDOW_RESIZABLE);

 GlobalContext=SDL_GL_CreateContext(GlobalWindow);
 
 GlobalWindowState.win_size[0]=(float)1024;
 GlobalWindowState.win_size[1]=(float)1024;
 
 glClearColor(0.2,0.1,0.5,1);
 SDL_GL_SetSwapInterval(1);
 GlobalWindowState.state=WINDOW_EVENTS;

 GlobalKeymap = SDL_GetKeyboardState(NULL);
}


void Window_Update()
{ 
 //----------------------------------- Event Handeling -------------------------

 SDL_PumpEvents();
 GlobalKeymap = SDL_GetKeyboardState(NULL);
 SDL_Event event;
 
 GlobalWindowState.rm_pressed=2;
 GlobalWindowState.lm_pressed=2; 
 GlobalWindowState.sm_pressed=2;
 GlobalWindowState.m_motion=0;
 
 while(SDL_PollEvent(&event)){
  switch(event.type)
  {
   case SDL_KEYDOWN:// ------------------ KEY DOWN -----------------------------
   break;
   case SDL_MOUSEMOTION:// ------------- Mouse Motion --------------------------- 
   ScreenToNdc(event.motion.x,event.motion.y,GlobalWindowState.m_abs);
   ScreenToNdc(event.motion.xrel,event.motion.yrel,GlobalWindowState.m_rel);
   GlobalWindowState.m_motion=1;
   break;
  
   case SDL_MOUSEBUTTONDOWN:// ------------- Mouse Down -------------------------- 
   
   switch(event.button.button){ 
    case SDL_BUTTON_LEFT:GlobalWindowState.lm_pressed=1;
                         GlobalWindowState.lm_released=0;
                         GlobalWindowState.l_double_click=event.button.clicks-1;
                         break;

    case SDL_BUTTON_MIDDLE:GlobalWindowState.sm_pressed=1;
                           break;

    case SDL_BUTTON_RIGHT:GlobalWindowState.rm_pressed=1;
                          GlobalWindowState.r_double_click=event.button.clicks-1;
                          break;

    default :printf(" Bug in Mouse Button Switch ");break;
   }

   GlobalWindowState.click_xy[0]=event.button.x;
   GlobalWindowState.click_xy[1]=event.button.y;
   break;

   case SDL_MOUSEBUTTONUP:// --------------- Mouse Up   -------------------------- 
   
    switch(event.button.button){ 
     case SDL_BUTTON_LEFT:GlobalWindowState.lm_pressed=0;
                          GlobalWindowState.lm_released=1;break;
     case SDL_BUTTON_MIDDLE:GlobalWindowState.sm_pressed=0;break;
     case SDL_BUTTON_RIGHT:GlobalWindowState.rm_pressed=0;break;
     default :printf(" Bug in Mouse Button Switch ");break;
    }
   
   GlobalWindowState.released_xy[0]=event.button.x;
   GlobalWindowState.released_xy[1]=event.button.y;
   break;
  
 
   case SDL_WINDOWEVENT:// --------------- Window Event ---------------------
    switch(event.window.event)
    {
     case SDL_WINDOWEVENT_RESIZED:glViewport(0,0,event.window.data1,event.window.data2);
                                  GlobalWindowState.win_size[0]=(float)event.window.data1;
                                  GlobalWindowState.win_size[1]=(float)event.window.data2;
                                  break;

     case SDL_WINDOWEVENT_CLOSE:GlobalWindowState.close=1;break;
    };
   break;
    
   case SDL_QUIT:break;
   default:break;   
  }
 }  
}


void WindowTool_Update()
{

 switch(GlobalWindowState.state)
 {
  default :break;
 }

}

/*
void WindowEvents_Update()
{
 
 for(uint8_t i=0;i<GlobalWindowState.key_event_cnt;i++)
 {
  printf(" Event %d -------------\n ",i);
  printf(" Key Code is = %d  \n ", GlobalWindowState.key_events[i].keycode);
  printf(" Key Code is = %d  \n ", GlobalWindowState.key_events[i].repeat);
  printf(" Key Code is = %d  \n ", GlobalWindowState.key_events[i].mod);
  printf(" ------------------------------------- \n ");
 }
 
 if(GlobalWindowState.rm_pressed==1){
  printf(" Right Mouse Pressed at %f -- %f \n",GlobalWindowState.click_xy[0],
                                               GlobalWindowState.click_xy[1]);
 }
 
 if(GlobalWindowState.sm_pressed==1){
  printf(" Middle Mouse Pressed at %f -- %f \n",GlobalWindowState.click_xy[0],
                                                GlobalWindowState.click_xy[1]);
 }
 
 if(GlobalWindowState.lm_pressed==1){
  printf(" Left Mouse Pressed at %f -- %f \n",GlobalWindowState.click_xy[0],
                                              GlobalWindowState.click_xy[1]);
 }


 if(!GlobalWindowState.rm_pressed){
  printf(" Right Mouse Released at %f -- %f \n",GlobalWindowState.released_xy[0],
                                               GlobalWindowState.released_xy[1]);
 }
 
 if(!GlobalWindowState.sm_pressed){
  printf(" Middle Mouse Released at %f -- %f \n",GlobalWindowState.released_xy[0],
                                                GlobalWindowState.released_xy[1]);
 }
 
 if(!GlobalWindowState.lm_pressed){
  printf(" Left Mouse Released at %f -- %f \n",GlobalWindowState.released_xy[0],
                                              GlobalWindowState.released_xy[1]);
 }
 
}
*/

