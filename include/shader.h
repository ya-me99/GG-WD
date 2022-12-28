#ifndef SHADER
#define SHADER

#include <SDL2/SDL.h>
#include <SDL2/SDL_rwops.h>
#include <stdio.h>
#include <stdlib.h>
#include <epoxy/gl.h>
#include <epoxy/glx.h>
#include <cglm/cglm.h>


int8_t Shader_BuildProg(char* file);

GLuint Shader_Build(uint8_t type,char* src);

GLuint Shader_ParseBuffer(SDL_RWops* f,char* buffer,uint64_t offset);

#endif 
