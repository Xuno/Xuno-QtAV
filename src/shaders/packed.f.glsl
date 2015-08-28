/*** packed.f.glsl *******************************************************************
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

#ifdef USED_BiCubic
#define BICUBIC_BSPLINE  //BICUBIC_TRI, BICUBIC_BELL, BICUBIC_BSPLINE, BICUBIC_CAT
#endif
#define USED_FILTERS
#define USED_GAMMA

uniform sampler2D u_Texture0;
varying vec2 v_TexCoords0;
uniform mat4 u_colorMatrix;
uniform float u_opacity;
uniform float u_bpp;
uniform float u_gammaRGB;
uniform vec2  u_pix;
uniform float u_filterkernel[9];
uniform vec2 u_pixeloffsetkernel[9];
vec3 color;
#ifdef PACKED_YUV
uniform mat4 u_c;

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


void main() {
#if defined(USED_FILTERS)
  vec3 sum = vec3(0.0);
  for (int i=0;i<9;i++) {
#else
   int i=4;
#endif //USED_FILTERS

#if defined(USED_BiCubic)
   vec4 c = BiCubic(u_Texture0, v_TexCoords0 + u_pixeloffsetkernel[i]);
#else  //USED_BiCubic
   vec4 c = texture2D(u_Texture0, v_TexCoords0 + u_pixeloffsetkernel[i]);
#endif //USED_BiCubic

#ifdef PACKED_YUV
   c = u_c * c;
   c.a = 1.0;
   vec4 c = texture2D(u_Texture0, v_TexCoords0);
   c = u_c * c;
#ifdef PACKED_YUV
   c.a = 1.0; // remove this, and in mat last line use 1,1,1,1
#endif //PACKED_YUV

   gl_FragColor = clamp(u_colorMatrix * c, 0.0, 1.0) * u_opacity;

#if defined(USED_FILTERS)
//added filters
   color = gl_FragColor.rgb;
   sum += color * u_filterkernel[i];
  }
  gl_FragColor.rgb = sum;
#endif //USED_FILTERS

#if defined(USED_GAMMA)
  color = gl_FragColor.rgb;
  gl_FragColor.rgb = pow(color, 1.0 / vec3(u_gammaRGB));
#endif //USED_GAMMA
}
