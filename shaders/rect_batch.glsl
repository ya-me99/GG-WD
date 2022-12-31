#version 460 core
layout (location=0) in vec2 pos;



void main()
{
 
 gl_Position=vec4(pos,0,1.0);

}

@1 BatchShader Vertex @

#version 460 core

out vec4 color;

void main()
{
 color=vec4(1,1,1,1);
}

@5 BatchShader Frag @
$