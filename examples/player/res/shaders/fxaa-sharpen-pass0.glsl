#version 130

#define HOOKED_raw texture0
#define HOOKED_pos texcoord0
#define HOOKED_size texture_size0
#define HOOKED_rot texture_rot0
#define HOOKED_pt pixel_size0
#define HOOKED_tex(pos) (1.000000 * vec4(texture(HOOKED_raw, pos)).rgba)
#define HOOKED_texOff(off) HOOKED_tex(HOOKED_pos + HOOKED_pt * vec2(off))

#define MAIN_raw HOOKED_raw
#define MAIN_pos HOOKED_pos
#define MAIN_size HOOKED_size
#define MAIN_rot HOOKED_rot
#define MAIN_pt HOOKED_pt
#define MAIN_tex HOOKED_tex
#define MAIN_texOff HOOKED_texOff

//texture0
uniform sampler2D MAIN_raw;

//texture pos from vertex (texcoord0)
in vec2 MAIN_pos;

//texture size (texture_size0)
uniform vec2 MAIN_size;

//texture rotation (texture_rot0)
uniform mat2 MAIN_rot;

//pixel size (pixel_size0)
uniform vec2 MAIN_pt;




/*
FXAA fragment shader by Timothy Lottes
http://timothylottes.blogspot.com/
GLSL version by Geeks3D
http://www.geeks3d.com/
modified and adapted to BGE by Martins Upitis
http://devlog-martinsh.blogspot.com/
modified by Simone Cingano
http://akifox.com
*/

//varying vec2 vTexCoord;

//uniform sampler2D uImage0; //redered scene texture


float width = MAIN_size.x;
float height = MAIN_size.y;

float FXAA_SUBPIX_SHIFT = 1.0/4.0;
vec2 rcpFrame = MAIN_pt;


vec3 FxaaPixelShader(vec4 posPos, sampler2D tex, vec2 rcpFrame)
{
  //posPos   // Output of FxaaVertexShader interpolated across screen
  //tex      // Input texture.
  //rcpFrame // Constant {1.0/frameWidth, 1.0/frameHeight}
  /*---------------------------------------------------------*/
  #define FXAA_REDUCE_MIN   (1.0/128.0)
  #define FXAA_REDUCE_MUL   (1.0/8.0)
  #define FXAA_SPAN_MAX     8.0
  /*---------------------------------------------------------*/
  vec3 rgbNW = texture2D(tex, posPos.zw).xyz;
  vec3 rgbNE = texture2D(tex, posPos.zw + vec2(1.0,0.0)*rcpFrame.xy).xyz;
  vec3 rgbSW = texture2D(tex, posPos.zw + vec2(0.0,1.0)*rcpFrame.xy).xyz;
  vec3 rgbSE = texture2D(tex, posPos.zw + vec2(1.0,1.0)*rcpFrame.xy).xyz;
  vec3 rgbM  = texture2D(tex, posPos.xy).xyz;
  /*---------------------------------------------------------*/
  vec3 luma = vec3(0.299, 0.587, 0.114);
  float lumaNW = dot(rgbNW, luma);
  float lumaNE = dot(rgbNE, luma);
  float lumaSW = dot(rgbSW, luma);
  float lumaSE = dot(rgbSE, luma);
  float lumaM  = dot(rgbM,  luma);
  /*---------------------------------------------------------*/
  float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
  float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
  /*---------------------------------------------------------*/
  vec2 dir;
  dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
  dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
  /*---------------------------------------------------------*/
  float dirReduce = max(
    (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
    FXAA_REDUCE_MIN);
  float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
  dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
      max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
      dir * rcpDirMin)) * rcpFrame.xy;
  /*--------------------------------------------------------*/
  vec3 rgbA = (1.0/2.0) * (
  texture2D(tex, posPos.xy + dir * (1.0/3.0 - 0.5)).xyz +
  texture2D(tex, posPos.xy + dir * (2.0/3.0 - 0.5)).xyz);
  vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
  texture2D(tex, posPos.xy + dir * (0.0/3.0 - 0.5)).xyz +
    texture2D(tex, posPos.xy + dir * (3.0/3.0 - 0.5)).xyz);
  float lumaB = dot(rgbB, luma);
  if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
  return rgbB;
}

vec4 PostFX(sampler2D tex, vec2 uv)
{
     vec4 c;
     c.a = texture2D(tex, uv.xy).a;
     uv.x=1.0-uv.x;
     vec2 rcpFrame = MAIN_pt;
     vec4 posPos = vec4(uv.st,uv.st -(rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT)));
     c.rgb = FxaaPixelShader(posPos, tex, rcpFrame);
     //c.a = 1.0; //set alpha to 1.0
     return c;
}


vec4 hook() {
    vec4 c;
    //= HOOKED_tex(HOOKED_pos);

    //c.rgb = pow(clamp(c.rgb, 0.0, 1.0), vec3(1.0/2.2));

    c=PostFX(MAIN_raw, MAIN_pos);

    return c;
}

void main(void)
{
    //gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
    gl_FragColor=hook();
    //gl_FragColor.b=1.;
}
