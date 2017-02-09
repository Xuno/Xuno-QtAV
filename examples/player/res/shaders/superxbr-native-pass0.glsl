#if defined(VERTEX)

in highp vec4 VertexCoord;
in mediump vec2 TexCoord;
out mediump vec2 texcoord0;
uniform mediump mat4 MVPMatrix;
void main(void)
{
    gl_Position = MVPMatrix * VertexCoord;
    texcoord0 = TexCoord;
}

#elif defined(FRAGMENT)


//Create FBO: 3342x2086 (3342x2086)  #2
//recompiling a shader program:
//header:
#define HOOKED_raw texture0
#define HOOKED_pos texcoord0
#define HOOKED_size texture_size0
#define HOOKED_rot texture_rot0
#define HOOKED_pt pixel_size0
#define HOOKED_tex(pos) (1.000000 * vec4(texture(HOOKED_raw, pos)).rgba)
#define HOOKED_texOff(off) HOOKED_tex(HOOKED_pos + HOOKED_pt * vec2(off))
#define MAIN_raw texture0
#define MAIN_pos texcoord0
#define MAIN_size texture_size0
#define MAIN_rot texture_rot0
#define MAIN_pt pixel_size0
#define MAIN_tex(pos) (1.000000 * vec4(texture(MAIN_raw, pos)).rgba)
#define MAIN_texOff(off) MAIN_tex(MAIN_pos + MAIN_pt * vec2(off))

//texture0
uniform sampler2D MAIN_raw;

//texture pos from vertex (texcoord0)
in mediump vec2 MAIN_pos;

//texture size (texture_size0)
uniform mediump vec2 MAIN_size;

//texture rotation (texture_rot0)
uniform mediump mat2 MAIN_rot;

//pixel size (pixel_size0)
uniform mediump vec2 MAIN_pt;


vec4 superxbr() {
vec4 i[4*4];
vec4 res;
#define i(x,y) i[(x)*4+(y)]
float luma[4*4];
#define luma(x, y) luma[(x)*4+(y)]
#define GET_SAMPLE(pos) HOOKED_texOff(pos)
#define SAMPLE4_MUL(sample4, w) ((sample4)*(w))
vec2 dir = fract(HOOKED_pos * HOOKED_size) - 0.5;
dir = transpose(HOOKED_rot) * dir;
vec2 dist = HOOKED_size * min(HOOKED_pos, vec2(1.0) - HOOKED_pos);
if (dir.x * dir.y < 0.0 && dist.x > 1.0 && dist.y > 1.0)
    return vec4(0.0);
if (dir.x < 0.0 || dir.y < 0.0 || dist.x < 1.0 || dist.y < 1.0)
    return GET_SAMPLE(-dir);
#define IDX(x, y) vec2(float(x)-1.25, float(y)-1.25)

for (int x = 0; x < 4; x++)
for (int y = 0; y < 4; y++) {
 i(x,y) = GET_SAMPLE(IDX(x,y));
 luma(x,y) = dot(i(x,y), vec4(0.2126, 0.7152, 0.0722, 0));
}

{ // step
mat4 d1 = mat4( i(0,0), i(1,1), i(2,2), i(3,3) );
mat4 d2 = mat4( i(0,3), i(1,2), i(2,1), i(3,0) );
mat4 h1 = mat4( i(0,1), i(1,1), i(2,1), i(3,1) );
mat4 h2 = mat4( i(0,2), i(1,2), i(2,2), i(3,2) );
mat4 v1 = mat4( i(1,0), i(1,1), i(1,2), i(1,3) );
mat4 v2 = mat4( i(2,0), i(2,1), i(2,2), i(2,3) );
float dw = 0.129633;
float ow = 0.175068;
vec4 dk = vec4(-dw, dw+0.5, dw+0.5, -dw);
vec4 ok = vec4(-ow, ow+0.5, ow+0.5, -ow);
vec4 d1c = SAMPLE4_MUL(d1, dk);
vec4 d2c = SAMPLE4_MUL(d2, dk);
vec4 vc = SAMPLE4_MUL(v1+v2, ok)/2.0;
vec4 hc = SAMPLE4_MUL(h1+h2, ok)/2.0;
float d_edge = 0.0;
d_edge += -1.0 * abs(luma(2,0) - luma(0,2));
d_edge -= -1.0 * abs(luma(3,2) - luma(1,0));
d_edge += 1.0 * abs(luma(1,1) - luma(0,2));
d_edge -= 1.0 * abs(luma(2,1) - luma(1,0));
d_edge += 1.0 * abs(luma(2,0) - luma(1,1));
d_edge -= 1.0 * abs(luma(3,2) - luma(2,1));
d_edge += 2.0 * abs(luma(2,1) - luma(1,2));
d_edge -= 2.0 * abs(luma(2,2) - luma(1,1));
d_edge += -1.0 * abs(luma(3,1) - luma(1,3));
d_edge -= -1.0 * abs(luma(2,3) - luma(0,1));
d_edge += 1.0 * abs(luma(2,2) - luma(1,3));
d_edge -= 1.0 * abs(luma(1,2) - luma(0,1));
d_edge += 1.0 * abs(luma(3,1) - luma(2,2));
d_edge -= 1.0 * abs(luma(2,3) - luma(1,2));
float o_edge = 0.0;
o_edge += 1.0 * abs(luma(1,0) - luma(1,1));
o_edge -= 1.0 * abs(luma(0,1) - luma(1,1));
o_edge += 2.0 * abs(luma(1,1) - luma(1,2));
o_edge -= 2.0 * abs(luma(1,1) - luma(2,1));
o_edge += 1.0 * abs(luma(1,2) - luma(1,3));
o_edge -= 1.0 * abs(luma(2,1) - luma(3,1));
o_edge += 1.0 * abs(luma(2,0) - luma(2,1));
o_edge -= 1.0 * abs(luma(0,2) - luma(1,2));
o_edge += 2.0 * abs(luma(2,1) - luma(2,2));
o_edge -= 2.0 * abs(luma(1,2) - luma(2,2));
o_edge += 1.0 * abs(luma(2,2) - luma(2,3));
o_edge -= 1.0 * abs(luma(2,2) - luma(3,2));
float str = smoothstep(0.0, 0.600000 + 1e-6, abs(d_edge));
res = mix(mix(d2c, d1c, step(0.0, d_edge)),
      mix(hc,   vc, step(0.0, o_edge)), 1.0 - str);
vec4 lo = min(min( i(1,1), i(2,1) ), min( i(1,2), i(2,2) ));
vec4 hi = max(max( i(1,1), i(2,1) ), max( i(1,2), i(2,2) ));
res = clamp(res, lo, hi);
} // step
return res;
}  // superxbr

vec4 hook() {
    return superxbr();
}

//body:
// custom hook

void main(void)
{
  gl_FragColor = hook();
  //gl_FragColor = texture(texture0, texcoord0);
}



#endif
