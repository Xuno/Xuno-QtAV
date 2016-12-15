#version 120

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision highp float;
#endif

uniform mat4 mvp_matrix=mat4(1.0f);
uniform int pass=0;

attribute vec4 a_position;
attribute vec2 a_texcoord;

varying vec2 v_texcoord;
varying float counter;
varying vec4 v_color;


//! [0]
void main()
{
    // Calculate vertex position in screen space
     gl_Position = mvp_matrix * a_position;

    // Pass texture coordinate to fragment shader
    // Value will be automatically interpolated to fragments inside polygon faces

     v_texcoord = a_texcoord;
     //counter+=0.125;//./pixels/2.;
     //counter+=1;
     //v_color=vec4(abs(a_texcoord.x)+a_texcoord.y*0.0,0.,0.,1.);

}
//! [0]
