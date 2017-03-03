#version 130

in vec4 VertexCoord;
in vec2 TexCoord;
out vec2 texcoord0;
uniform mat4 MVPMatrix;
void main(void)
{
    gl_Position = MVPMatrix * VertexCoord;
    texcoord0 = TexCoord;
}
