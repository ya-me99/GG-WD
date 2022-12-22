#include "include/shader.h"

int8_t Shader_BuildProg(char* file)
{
 GLuint status;
 GLuint prog;
 GLuint shaders[6]={0,0,0,0,0,0};
 GLuint shader_cnt=0;
 
 SDL_RWops* shader=SDL_RWFromFile(file,"r");
 
 if(!shader)
 { 
  printf(" Shader Path Incorrect Path : %s \n SDL Error = %s \n ",file,SDL_GetError()); 
  return 0;
 }

 uint64_t file_size=SDL_RWsize(shader);

 char* buffer= malloc(file_size+1);
 uint64_t last=0;
 uint8_t read=0;
 
 while(1)
 {
  read=SDL_RWread(shader,&buffer[last],1,1);
  if(read==0){ printf("Error Reading File \n " ); return 0; }
  last+=read;
  
  if(buffer[last-1]=='@')
  {
  shaders[shader_cnt]=Shader_ParseBuffer(shader,buffer,last);
  shader_cnt++;
  memset(buffer,0,file_size);
  last=0;
  }
  
  if(buffer[last-1]=='$'){ break ;}
 }
  
 prog=glCreateProgram();
 
 for(int8_t i=0;i<shader_cnt;i++)
 {
  glAttachShader(prog,shaders[i]);
 }
  
 glLinkProgram(prog);
 glGetProgramiv(prog,GL_LINK_STATUS,&status);
 
 if(!status)
 {
  GLsizei log_length = 0;
  GLchar message[1024];
  glGetProgramInfoLog(prog,1024, &log_length, message);
  printf(" Error : %s \n ",message);
  return 0;
 }
 
 return prog;
}

GLuint Shader_ParseBuffer(SDL_RWops* f,char* buffer,uint64_t offset)
{
 uint16_t pos=0;
 GLuint s=0;
 SDL_RWread(f,&buffer[offset],1,1);
 uint8_t type=atoi(&buffer[offset]);
 buffer[offset-2]='\0';
 s=Shader_Build(type,buffer);

 if(s==0){ return 0 ;}
 
 while(1)
 {
  SDL_RWread(f,&buffer[pos],1,1);
  pos++;
  if(buffer[pos-1]=='@'){ break ; }
 }

 buffer[pos]='\0';
 printf(" Shader Comment is = %s \n",buffer);
 
 return s;
}

GLuint Shader_Build(uint8_t type,char* src)
{
 
 GLuint status;
 GLuint shader; 

 switch(type)
 { 
  case 1:shader=glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;
  
  case 2:shader=glCreateShader(GL_TESS_EVALUATION_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;
  case 3:shader=glCreateShader(GL_TESS_CONTROL_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;
  case 4:shader=glCreateShader(GL_GEOMETRY_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;
 
  case 5:shader=glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;

  case 6:shader=glCreateShader(GL_COMPUTE_SHADER);
                glShaderSource(shader,1,&src,NULL);
                glCompileShader(shader);
                glGetShaderiv(shader,GL_COMPILE_STATUS,&status);
                break;

  default:printf("Default Shader Type Switch \n ");break;
 }
 
 if(!status)
 {
  GLsizei log_length = 0;
  GLchar message[1024];
  glGetShaderInfoLog(shader,1024, &log_length, message);
  printf(" Error : %s shader_type %d \n",message,type);
  return 0;
 }
 
 return shader;
}

