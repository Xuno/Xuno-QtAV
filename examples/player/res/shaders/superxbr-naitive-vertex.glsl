#version 130

attribute vec4 VertexCoord;
attribute vec2 TexCoord;
varying vec2 texcoord0;
uniform mat4 MVPMatrix;
void main(void)
{
    gl_Position = MVPMatrix * VertexCoord;
    texcoord0 = TexCoord;
}
