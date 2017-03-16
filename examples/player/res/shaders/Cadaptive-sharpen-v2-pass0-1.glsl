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

vec4 xlat_main(vec2 tex ) {
    vec3 c[13];
    vec3 blur;
    float blur_Y;
    float c_comp;
    float edge;

    c[0] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (0.000000 * p1.y )))).rgb  );
    c[1] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (-1.00000 * p1.y )))).rgb  );
    c[2] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (-1.00000 * p1.y )))).rgb  );
    c[3] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (-1.00000 * p1.y )))).rgb  );
    c[4] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (0.000000 * p1.y )))).rgb  );
    c[5] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (0.000000 * p1.y )))).rgb  );
    c[6] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (1.00000 * p1.y )))).rgb  );
    c[7] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (1.00000 * p1.y )))).rgb  );
    c[8] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (1.00000 * p1.y )))).rgb  );
    c[9] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (-2.00000 * p1.y )))).rgb  );
    c[10] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (-2.00000 * p1.x ), (0.000000 * p1.y )))).rgb  );
    c[11] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (2.00000 * p1.x ), (0.000000 * p1.y )))).rgb  );
    c[12] = xlat_lib_saturate( texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (2.00000 * p1.y )))).rgb  );
    blur = ((((2.00000 * (((c[ 2 ] + c[ 4 ]) + c[ 5 ]) + c[ 7 ])) + (((c[ 1 ] + c[ 3 ]) + c[ 6 ]) + c[ 8 ])) + (4.00000 * c[ 0 ])) / 16.0000);
    blur_Y = (((blur.x  / 3.00000) + (blur.y  / 3.00000)) + (blur.z  / 3.00000));
    c_comp = xlat_lib_saturate( (0.266667 + (0.900000 * exp2( (-7.40000 * blur_Y) ))) );
    edge = length( ((((1.38000 * abs( (blur - c[ 0 ]) )) + (1.15000 * (((abs( (blur - c[ 2 ]) ) + abs( (blur - c[ 4 ]) )) + abs( (blur - c[ 5 ]) )) + abs( (blur - c[ 7 ]) )))) + (0.920000 * (((abs( (blur - c[ 1 ]) ) + abs( (blur - c[ 3 ]) )) + abs( (blur - c[ 6 ]) )) + abs( (blur - c[ 8 ]) )))) + (0.230000 * (((abs( (blur - c[ 9 ]) ) + abs( (blur - c[ 10 ]) )) + abs( (blur - c[ 11 ]) )) + abs( (blur - c[ 12 ]) )))) );
    vec4 r= vec4(texture2D( s0, tex).rgb , ((edge * c_comp) + 2.00000));
    return r;
}


//
// User varying
//


//
// Translator's entry point
//
void main() {
  gl_FragColor = xlat_main(p1);
}


