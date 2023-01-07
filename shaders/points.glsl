#version 460 core

layout ( location=0 ) in vec2 pos;
layout ( location=1 ) in vec4 buffer_color;
layout ( location=2 ) in float size;

out vec4 vertex_color;

void main()
{
  
 vertex_color=buffer_color;

 gl_Position=vec4(pos,0.0,1.0);
 
 gl_PointSize=2;

}

@1 PointShader Vertex @

#version 460 core

in vec4 vertex_color;
out vec4 fragment_color;

void main()
{
 fragment_color=vertex_color;
}


@5 PointsShader Frag @
$