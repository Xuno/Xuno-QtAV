uniform sampler2D texture0;
varying mediump vec2 texc;
void main(void)
{
    vec4 color = texture2D(texture0, texc);
    gl_FragColor = vec4(color.r*0.75,color.g,color.b,color.a);
}

