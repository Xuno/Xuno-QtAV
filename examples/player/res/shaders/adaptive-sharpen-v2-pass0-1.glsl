#version 130

//
// Global variable definitions
//

#define HOOKED_raw texture0
#define HOOKED_pos texcoord0
#define MAIN_raw HOOKED_raw
#define MAIN_pos HOOKED_pos

#define p1 MAIN_pos
#define s0 MAIN_raw

uniform sampler2D MAIN_raw;
in vec2 MAIN_pos;


//#define HOOKED_pos MAIN_pos
//#define HOOKED_raw MAIN_raw





//
// Function declarations
//

//#define xlat_lib_saturate(x) ( clamp( x, 0.0, 1.0) )

float xlat_lib_saturate( float x) {
  return clamp( x, 0.0, 1.0);
}

vec2 xlat_lib_saturate( vec2 x) {
  return clamp( x, 0.0, 1.0);
}

vec3 xlat_lib_saturate( vec3 x) {
  return clamp( x, 0.0, 1.0);
}

vec4 xlat_lib_saturate( vec4 x) {
  return clamp( x, 0.0, 1.0);
}

mat2 xlat_lib_saturate(mat2 m) {
  return mat2( clamp(m[0], 0.0, 1.0), clamp(m[1], 0.0, 1.0));
}

mat3 xlat_lib_saturate(mat3 m) {
  return mat3( clamp(m[0], 0.0, 1.0), clamp(m[1], 0.0, 1.0), clamp(m[2], 0.0, 1.0));
}

mat4 xlat_lib_saturate(mat4 m) {
  return mat4( clamp(m[0], 0.0, 1.0), clamp(m[1], 0.0, 1.0), clamp(m[2], 0.0, 1.0), clamp(m[3], 0.0, 1.0));
}

//vec4 xlat_main( in vec2 tex );

//
// Function definitions
//
// $MinimumShaderProfile: ps_3_0

// Copyright (c) 2015-2017, bacondither
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

// First pass, MUST BE PLACED IMMEDIATELY BEFORE THE SECOND PASS IN THE CHAIN

// Adaptive sharpen - version 2017-01-22 - (requires ps >= 3.0)
// Tuned for use post resize, EXPECTS FULL RANGE GAMMA LIGHT
// https://github.com/bacondither/Adaptive-sharpen/blob/master/shaders/Adaptive-sharpen%20-%20Pass%20one.hlsl

//---------------------------------------------------------------------------------
#define w_offset 2.0         // Edge channel offset, must be the same in all passes
//---------------------------------------------------------------------------------

// Get destination pixel values
#define get(x,y)    ( xlat_lib_saturate(texture2D(s0, tex + vec2(x*(p1[0]), y*(p1[1]))).rgb) )

// Compute diff
#define b_diff(pix) ( abs(blur - c[pix]) )

vec4 hook(vec2 tex) 
{
	// Get points and clip out of range values (BTB & WTW)
	// [                c9                ]
	// [           c1,  c2,  c3           ]
	// [      c10, c4,  c0,  c5, c11      ]
	// [           c6,  c7,  c8           ]
	// [                c12               ]
	vec3 c[13] = { get( 0, 0), get(-1,-1), get( 0,-1), get( 1,-1), get(-1, 0),
	                 get( 1, 0), get(-1, 1), get( 0, 1), get( 1, 1), get( 0,-2),
	                 get(-2, 0), get( 2, 0), get( 0, 2) };

	// Blur, gauss 3x3
        vec3 blur  = (2.0*(c[2]+c[4]+c[5]+c[7]) + (c[1]+c[3]+c[6]+c[8]) + 4.0*c[0])/16.0;
        float blur_Y = (blur.r/3.0 + blur.g/3.0 + blur.b/3.0);

	// Contrast compression, center = 0.5, scaled to 1/3
	float c_comp = xlat_lib_saturate(0.266666681f + 0.9*exp2(-7.4*blur_Y));

	// Edge detection
	// Relative matrix weights
	// [          1,         ]
	// [      4,  5,  4      ]
	// [  1,  5,  6,  5,  1  ]
	// [      4,  5,  4      ]
	// [          1          ]
	float edge = length( 1.38*(b_diff(0))
	                   + 1.15*(b_diff(2) + b_diff(4)  + b_diff(5)  + b_diff(7))
	                   + 0.92*(b_diff(1) + b_diff(3)  + b_diff(6)  + b_diff(8))
	                   + 0.23*(b_diff(9) + b_diff(10) + b_diff(11) + b_diff(12)) );

	return vec4( (texture2D(s0, tex).rgb), (edge*c_comp + w_offset) );
}

void main() {
  gl_FragColor = hook(p1);
}


