#version 460 core

layout (location=0) in vec2 pos;

void main()
{
 gl_Position=vec4(pos,0,1);
}

@1 SplineLoop Vertex Test @
 
#version 460 core

out vec4 color;
uniform vec4 u_color;//=vec4(1,1,1,1);

void main()
{
 color=u_color;
}

@5 SplineLoop Frag @
$
