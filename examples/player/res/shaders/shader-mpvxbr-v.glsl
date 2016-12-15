#version 130

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision highp float;
#endif

uniform mat4 mvp_matrix=mat4(1.0f);
uniform int pass=0;


attribute highp vec4 VertexCoord;
attribute highp vec2 TexCoord;

varying highp vec2 v_texcoord;

void main()
 {
     // Calculate vertex position in screen space
     gl_Position = mvp_matrix * VertexCoord;

     // Pass texture coordinate to fragment shader
     // Value will be automatically interpolated to fragments inside polygon faces
     v_texcoord = TexCoord;
 }
