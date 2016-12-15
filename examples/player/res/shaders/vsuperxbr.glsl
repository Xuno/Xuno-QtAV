attribute highp vec4 qt_Vertex;
attribute highp vec4 qt_MultiTexCoord0;
uniform highp mat4 qt_ModelViewProjectionMatrix;
varying highp vec4 qt_TexCoord0;

void main(void)
{
    gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
    qt_TexCoord0 = qt_MultiTexCoord0;
}

#pragma parameter XBR_EDGE_STR "Xbr - Edge Strength p0" 1.0 0.0 5.0 0.2
#pragma parameter XBR_WEIGHT "Xbr - Filter Weight" 1.0 0.00 1.50 0.01

#include "SuperXbrParams.inc"

/* COMPATIBILITY
         - HLSL compilers
         - Cg   compilers
*/

/*

        *******  Super XBR Shader  *******

        Copyright (c) 2015 Hyllian - sergiogdb@gmail.com

        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
        in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in
        all copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
        THE SOFTWARE.

*/

#if Pass == 0
        #define wp1   1.0
        #define wp2   0.0
        #define wp3   0.0
        #define wp4   2.0
        #define wp5  -1.0
        #define wp6   0.0
#elif Pass == 1
        #define wp1   1.0
        #define wp2   0.0
        #define wp3   0.0
        #define wp4   4.0
        #define wp5   0.0
        #define wp6   0.0
#else
        #define wp1   1.0
        #define wp2   0.0
        #define wp3   0.0
        #define wp4   0.0
        #define wp5  -1.0
        #define wp6   0.0
#endif

#if Pass == 1
        #define weight1 (XBR_WEIGHT*1.75068/10.0)
        #define weight2 (XBR_WEIGHT*1.29633/10.0/2.0)
#else
        #define weight1 (XBR_WEIGHT*1.29633/10.0)
        #define weight2 (XBR_WEIGHT*1.75068/10.0/2.0)
#endif

#if Pass == 0
        #define Get(x,y) (tex2D(s0, VAR.texCoord + pixel_size*float2(x,y)).xyz)
#elif Pass == 1
        #define Get(x,y) (tex2D(s0, VAR.texCoord + pixel_size*float2((x)+(y) - 1,(y) - (x))).xyz)
#else
        #define Get(x,y) (tex2D(s0, VAR.texCoord - pixel_size*float2(x,y)).xyz)
#endif

const static float3 Y = float3(.2126, .7152, .0722);

float RGBtoYUV(float3 color)
{
        return dot(color, Y);
}

float df(float A, float B)
{
        return abs(A-B);
}

float d_wd(float b0, float b1, float c0, float c1, float c2, float d0, float d1, float d2, float d3, float e1, float e2, float e3, float f2, float f3)
{
        return (wp1*(df(c1,c2) + df(c1,c0) + df(e2,e1) + df(e2,e3)) + wp2*(df(d2,d3) + df(d0,d1)) + wp3*(df(d1,d3) + df(d0,d2)) + wp4*df(d1,d2) + wp5*(df(c0,c2) + df(e1,e3)) + wp6*(df(b0,b1) + df(f2,f3)));
}

float hv_wd(float i1, float i2, float i3, float i4, float e1, float e2, float e3, float e4)
{
        return ( wp4*(df(i1,i2)+df(i3,i4)) + wp1*(df(i1,e1)+df(i2,e2)+df(i3,e3)+df(i4,e4)) + wp3*(df(i1,e2)+df(i3,e4)+df(e1,i2)+df(e3,i4)));
}

float3 min4(float3 a, float3 b, float3 c, float3 d)
{
                return min(a, min(b, min(c, d)));
}
float3 max4(float3 a, float3 b, float3 c, float3 d)
{
                return max(a, max(b, max(c, d)));
}

struct input
{
                float2 video_size;
                float2 texture_size;
                float2 output_size;
                float  frame_count;
                float  frame_direction;
                float frame_rotation;
};


struct out_vertex {
                float4 position : POSITION;
                float4 color    : COLOR;
                float2 texCoord : TEXCOORD0;
};



/*    VERTEX_SHADER    */
out_vertex main_vertex
(
                float4 position   : POSITION,
                float4 color      : COLOR,
                float2 texCoord1  : TEXCOORD0,

                uniform float4x4 modelViewProj,
                uniform input IN
)

{
        float2 tex = texCoord1;

        out_vertex OUT = {
                        mul(modelViewProj, position),
                        color,
                        tex
        };

        return OUT;
}
