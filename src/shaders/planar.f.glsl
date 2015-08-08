/*** planar.f.glsl ****************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012-2014 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision mediump float;
#else
#define highp
#define mediump
#define lowp
#endif

// u_TextureN: yuv. use array?
uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
#ifdef PLANE_4
uniform sampler2D u_Texture3;
#endif //PLANE_4
varying lowp vec2 v_TexCoords0;
#ifdef MULTI_COORD
varying lowp vec2 v_TexCoords1;
varying lowp vec2 v_TexCoords2;
#ifdef PLANE_4
varying lowp vec2 v_TexCoords3;
#endif
#else
#define v_TexCoords1 v_TexCoords0
#define v_TexCoords2 v_TexCoords0
#define v_TexCoords3 v_TexCoords0
#endif //MULTI_COORD
uniform float u_opacity;
uniform float u_bpp;
uniform mat4 u_colorMatrix;
//added by xuno
uniform float u_gammaRGB;
uniform vec2  u_pix;
uniform float u_filterkernel[9];
uniform vec2 u_pixeloffsetkernel[9];
vec3 color;

//#define USED_DEBUG
//#define USED_BiCubic //moved contol to VideoShader.cpp
#ifdef USED_BiCubic
#define BICUBIC_BSPLINE  //BICUBIC_TRI, BICUBIC_BELL, BICUBIC_BSPLINE, BICUBIC_CAT
#endif
#define USED_FILTERS
#define USED_GAMMA
//added by xuno

#if defined(LA_16BITS_BE) || defined(LA_16BITS_LE)
#define LA_16BITS 1
#else
#define LA_16BITS 0
#endif
//#define LA_16BITS  (defined(LA_16BITS_BE) || defined(LA_16BITS_LE)) // why may error?

#if defined(YUV_MAT_GLSL)
//http://en.wikipedia.org/wiki/YUV calculation used
//http://www.fourcc.org/fccyvrgb.php
//GLSL: col first
// use bt601
#if defined(CS_BT709)
const mat4 yuv2rgbMatrix = mat4(1, 1, 1, 0,
                              0, -0.187, 1.8556, 0,
                              1.5701, -0.4664, 0, 0,
                              0, 0, 0, 1)
#else //BT601
const mat4 yuv2rgbMatrix = mat4(1, 1, 1, 0,
                              0, -0.344, 1.773, 0,
                              1.403, -0.714, 0, 0,
                              0, 0, 0, 1)
#endif
                        * mat4(1, 0, 0, 0,
                               0, 1, 0, 0,
                               0, 0, 1, 0,
                               0, -0.5, -0.5, 1);
#endif

//added by xuno
#ifdef BICUBIC_TRI
float Triangular(float f) {
    f = f / 2.0;
    if (f < 0.0) {
        return ( f + 1.0 );
    } else {
        return ( 1.0 - f );
    }
    return 0.0;
}
#elif defined(BICUBIC_BELL)
float BellFunc(float x) {
    float f = ( x / 2.0 ) * 1.5; // Converting -2 to +2 to -1.5 to +1.5
    if (f > -1.5 && f < -0.5) {
        return( 0.5 * pow(f + 1.5, 2.0));
    } else if ( f > -0.5 && f < 0.5 ) {
        return 3.0 / 4.0 - ( f * f );
    } else if ( ( f > 0.5 && f < 1.5 ) ) {
        return( 0.5 * pow(f - 1.5, 2.0));
    }
    return 0.0;
}
#elif defined(BICUBIC_BSPLINE)
float BSpline(float x) {
    float f = x;
    if ( f < 0.0 ){
        f = -f;
    }
    if (f >= 0.0 && f <= 1.0){
        return ( 2.0 / 3.0 ) + ( 0.5 ) * ( f* f * f ) - (f*f);
    } else if ( f > 1.0 && f <= 2.0 ) {
        return 1.0 / 6.0 * pow(( 2.0 - f  ), 3.0);
    }
    return 1.0;
}
#elif defined(BICUBIC_CAT)
float CatMullRom(float x)
{
    const float B = 0.0;
    const float C = 0.5;
    float f = x;
    if (f < 0.0) {
        f = -f;
    }
    if (f < 1.0) {
        return ( ( 12 - 9 * B - 6 * C ) * ( f * f * f ) + ( -18 + 12 * B + 6 *C ) * ( f * f ) + ( 6 - 2 * B ) ) / 6.0;
    } else if ( f >= 1.0 && f < 2.0 ) {
        return ( ( -B - 6 * C ) * ( f * f * f ) + ( 6 * B + 30 * C ) * ( f *f ) + ( - ( 12 * B ) - 48 * C  ) * f + 8 * B + 24 * C)/ 6.0;
    } else {
        return 0.0;
    }
}
#endif //CatMullRom
#if defined(USED_BiCubic)
//used it : http://www.codeproject.com/Articles/236394/Bi-Cubic-and-Bi-Linear-Interpolation-with-GLSL
vec4 BiCubic( sampler2D textureSampler, vec2 TexCoord )
{
    float texelSizeX = u_pix.x;
    float texelSizeY = u_pix.y;
    float fWidth = 1.0 / texelSizeX;
    float fHeight = 1.0 / texelSizeY;
    vec4 nSum = vec4( 0.0, 0.0, 0.0, 0.0 );
    vec4 nDenom = vec4( 0.0, 0.0, 0.0, 0.0 );
    float a = fract( TexCoord.x * fWidth ); // get the decimal part
    float b = fract( TexCoord.y * fHeight ); // get the decimal part
    for (int m=-1; m<=2;m++) {
        for (int n=-1; n<=2;n++) {
            vec4 vecData = texture2D(textureSampler,TexCoord + vec2(texelSizeX * float(m),texelSizeY * float(n)));
#if defined(BICUBIC_TRI)
            float f  = Triangular(float(m) - a);
            float f1 = Triangular ( -( float( n ) - b ) );
#elif defined(BICUBIC_BELL)
            float f  = BellFunc(float(m) - a);
            float f1 = BellFunc ( -( float( n ) - b ) );
#elif defined(BICUBIC_BSPLINE)
            float f  = BSpline(float(m) - a);
            float f1 = BSpline ( -( float( n ) - b ) );
#elif defined(BICUBIC_CAT)
            float f  = CatMullRom(float(m) - a);
            float f1 = CatMullRom ( -( float( n ) - b ) );
#endif
            vec4 vecCooef1 = vec4( f,f,f,f );
            vec4 vecCoeef2 = vec4( f1, f1, f1, f1 );
            nSum = nSum + ( vecData * vecCoeef2 * vecCooef1  );
            nDenom = nDenom + (( vecCoeef2 * vecCooef1 ));
        }
    }
    return nSum / nDenom;
}
#endif //USED_BiCubic
//added by xuno

// 10, 16bit: http://msdn.microsoft.com/en-us/library/windows/desktop/bb970578%28v=vs.85%29.aspx
void main()
{
    // FFmpeg supports 9, 10, 12, 14, 16 bits
#if LA_16BITS
    //http://stackoverflow.com/questions/22693169/opengl-es-2-0-glsl-compiling-fails-on-osx-when-using-const
    float range = exp2(u_bpp) - 1.0; // why can not be const?
#if defined(LA_16BITS_LE)
    vec2 t = vec2(1.0, 256.0)*255.0/range;
#else
    vec2 t = vec2(256.0, 1.0)*255.0/range;
#endif
#endif //LA_16BITS

//added by xuno
#if defined(USED_FILTERS)
  vec3 sum = vec3(0.0);
  for (int i=0;i<9;i++) {
#else
  int i=4;
#endif //USED_FILTERS
//added by xuno

    // 10p in little endian: yyyyyyyy yy000000 => (L, L, L, A)
    gl_FragColor = clamp(u_colorMatrix
                         * vec4(
#if LA_16BITS
//added by xuno
#if defined(USED_BiCubic)
                             dot(BiCubic(u_Texture0, v_TexCoords0+u_pixeloffsetkernel[i]).ra, t),
                             dot(BiCubic(u_Texture1, v_TexCoords1+u_pixeloffsetkernel[i]).ra, t),
                             dot(BiCubic(u_Texture2, v_TexCoords2+u_pixeloffsetkernel[i]).ra, t),
#else  //USED_BiCubic
                             dot(texture2D(u_Texture0, v_TexCoords0+u_pixeloffsetkernel[i]).ra, t),
                             dot(texture2D(u_Texture1, v_TexCoords1+u_pixeloffsetkernel[i]).ra, t),
                             dot(texture2D(u_Texture2, v_TexCoords2+u_pixeloffsetkernel[i]).ra, t),
#endif //USED_BiCubic
//added by xuno
#else  //LA_16BITS
// use r, g, a to work for both yv12 and nv12. idea from xbmc
//added by xuno
#if defined(USED_BiCubic)
                             BiCubic(u_Texture0, v_TexCoords0 + u_pixeloffsetkernel[i]).r,
                             BiCubic(u_Texture1, v_TexCoords1 + u_pixeloffsetkernel[i]).g,
                             BiCubic(u_Texture2, v_TexCoords2 + u_pixeloffsetkernel[i]).a,
#else  //USED_BiCubic
                             texture2D(u_Texture0, v_TexCoords0 + u_pixeloffsetkernel[i]).r,
                             texture2D(u_Texture1, v_TexCoords1 + u_pixeloffsetkernel[i]).g,
                             texture2D(u_Texture2, v_TexCoords2 + u_pixeloffsetkernel[i]).a,
#endif //USED_BiCubic
//added by xuno
#endif //LA_16BITS
                             1)
                         , 0.0, 1.0) * u_opacity ;

#ifdef PLANE_4
#if LA_16BITS
    gl_FragColor.a *= dot(texture2D(u_Texture3, v_TexCoords3).ra, t); //GL_LUMINANCE_ALPHA
#else //8bit
    gl_FragColor.a *= texture2D(u_Texture3, v_TexCoords3).a; //GL_ALPHA
#endif //LA_16BITS
#endif //PLANE_4


//added by xuno
#if defined(USED_FILTERS)
  color = gl_FragColor.rgb;
  sum += color * u_filterkernel[i];
 }
  gl_FragColor.rgb = sum;
#endif //USED_FILTERS

#if defined(USED_GAMMA)
  color = gl_FragColor.rgb;
  gl_FragColor.rgb = pow(color, 1.0 / vec3(u_gammaRGB));
#endif //USED_GAMMA

#if defined(USED_DEBUG)
if (t == 1.0) {
 gl_FragColor.r = 255;
}
#endif //USED_DEBUG
//added by xuno

}
