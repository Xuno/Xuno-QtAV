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

// https://gist.github.com/igv/4792d0abab41d436ac1a51bb171f8c2f#file-adaptive-sharpen-2pass-glsl
// Copyright (c) 2015-2016, bacondither
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer
//    in this position and unchanged.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Adaptive sharpen - version 2016-11-19 - (requires ps >= 3.0)
// Tuned for use post resize


//!HOOK SCALED
//!BIND HOOKED

//---------------------------------------------------------------------------------
#define w_offset 2.0         // Edge channel offset, must be the same in all passes
//---------------------------------------------------------------------------------

// Get destination pixel values
#define get(x, y) ( clamp(HOOKED_texOff(vec2(x, y)*0.6).rgb, 0.0, 1.0) )

// Compute diff
#define b_diff(pix) ( abs(blur-c[pix]) )

vec4 hook() {

    // Get points and clip out of range values (BTB & WTW)
    // [                c9                ]
    // [           c1,  c2,  c3           ]
    // [      c10, c4,  c0,  c5, c11      ]
    // [           c6,  c7,  c8           ]
    // [                c12               ]
    vec3 c[13] = vec3[](get( 0, 0), get(-1,-1), get( 0,-1), get( 1,-1), get(-1, 0),
                        get( 1, 0), get(-1, 1), get( 0, 1), get( 1, 1), get( 0,-2),
                        get(-2, 0), get( 2, 0), get( 0, 2));

    // Blur, gauss 3x3
    vec3  blur   = (vec3(2)*(c[2]+c[4]+c[5]+c[7]) + (c[1]+c[3]+c[6]+c[8]) + vec3(4)*c[0])/vec3(16);
    float blur_Y = dot(blur, vec3(1.0/3.0));

    // Contrast compression, center = 0.5, scaled to 1/3
    float c_comp = clamp(0.266666681f + 0.9*exp2(-7.4*blur_Y), 0.0, 1.0);

    // Edge detection
    // Relative matrix weights
    // [          1,         ]
    // [      4,  5,  4      ]
    // [  1,  5,  6,  5,  1  ]
    // [      4,  5,  4      ]
    // [          1          ]
    float edge = length( 1.38*b_diff(0)
                       + 1.15*(b_diff(2) + b_diff(4) + b_diff(5) + b_diff(7))
                       + 0.92*(b_diff(1) + b_diff(3) + b_diff(6) + b_diff(8))
                       + 0.23*(b_diff(9) + b_diff(10) + b_diff(11) + b_diff(12)) );

    return vec4( (HOOKED_tex(HOOKED_pos).rgb), (edge*c_comp + w_offset) );
}


void main(void)
{
    //gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);
    gl_FragColor = hook();
}
