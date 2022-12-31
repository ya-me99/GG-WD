#version 460 core

layout(std430, binding=0) buffer name
{
 float data[];
};

out vec4 vertex_color;

void main()
{
 
 int index;
  
 switch(gl_VertexID)
 {
  case 0:index=gl_InstanceID*12;break;
  case 1:index=gl_InstanceID*12+2;break;
  case 2:index=gl_InstanceID*12+4;break;
  case 3:index=gl_InstanceID*12+6;break;
 }

 vec2 pos={data[index],data[index+1]};

 vertex_color=vec4(data[gl_InstanceID*12+8],
                   data[gl_InstanceID*12+9],
                   data[gl_InstanceID*12+10],
                   data[gl_InstanceID*12+11]);

 gl_Position=vec4(pos,0,1.0);

}

@1 BatchShader Vertex @

#version 460 core

in vec4 vertex_color;
out vec4 color;

void main()
{
 color=vertex_color;
}

@5 BatchShader Frag @
$