attribute highp vec4 VertexCoord;
attribute mediump vec2 TexCoord;
varying mediump vec2 texc;
uniform mediump mat4 mvp_matrix;
void main(void)
{
    gl_Position = mvp_matrix * VertexCoord;
    texc = TexCoord;
}

