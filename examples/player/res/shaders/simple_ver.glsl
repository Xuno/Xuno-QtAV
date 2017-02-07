attribute highp vec4 VertexCoord;
attribute mediump vec2 TexCoord;
varying mediump vec2 texc;
uniform mediump mat4 MVPMatrix;
void main(void)
{
    gl_Position = MVPMatrix * VertexCoord;
    texc = TexCoord;
}

