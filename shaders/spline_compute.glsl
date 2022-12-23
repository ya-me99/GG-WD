#version 430

layout(local_size_x = 1 ) in;
layout(rgba32f, binding = 0) uniform image1D img_output;


uniform vec2 control_points_x; //=vec2(0,0);
uniform vec2 control_points_y; // =vec2(0.5,0.5);
uniform vec2 control_points_z; //=vec2(1,0);

void main()
{
 
 float samples=float(gl_NumWorkGroups.x);
 int pos=int(gl_WorkGroupID.x);
 float t1=pos/samples;
 float t2=t1+1/samples;

 vec2 p1=(1-t1)*(1-t1)*control_points_x +
          2*t1*(1-t1)*control_points_y  +
          (t1*t1)*control_points_z;

 vec2 p2=(1-t2)*(1-t2)*control_points_x +
          2*t2*(1-t2)*control_points_y  +
         (t2*t2)*control_points_z;


 vec4 pixel=vec4(p1,p2);

 imageStore(img_output,pos,pixel);
}

@6 Compute Shader @
$