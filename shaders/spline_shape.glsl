#version 460 core

layout (location=0) in vec2 pos;

void main()
{
 gl_Position=vec4(pos,0,1);
}

@1 SplineShape Vertex Test @
 
#version 460 core

uniform vec4 u_color=vec4(1,1,1,1);
out vec4 color;

void main()
{
 color=vec4(1,1,1,1);
}

@5 SplineShape Frag @
$

