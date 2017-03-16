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
// Translator library functions
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



//
// Global variable definitions
//

//uniform vec2 p1;
//uniform sampler2D s0;

//
// Function declarations
//

//vec4 xlat_main( in vec2 tex );

//
// Function definitions
//

vec4 xlat_main(vec2 tex ) {
    vec4 orig;
    float c_edge;
    vec4 c[25];
    float maxedge;
    float sbe;
    vec2 cs;
    float c0_Y;
    float luma[25];
    vec3 dW;
    float mdiff_c0;
    float weights[12];
    float lowthrsum = 0.000000;
    float weightsum = 0.000000;
    float neg_laplace = 0.000000;
    int pix = 0;
    float x;
    float lowthr;
    float sharpen_val;
    float sharpdiff;
    int i = 0;
    int i1;
    float temp;
    int i2;
    float nmax;
    float nmin;
    float nmax_scale;
    float nmin_scale;
    float satmul;
    vec3 res;

    orig = texture2D( s0, tex);
    c_edge = (orig.a  - 2.00000);
    if ( true ){
        if ( ((c_edge > 24.0000) || (c_edge < -0.500000)) ){
            return vec4( 0.000000, 1.00000, 0.000000, 1.00000);
        }
    }
    c[0] = vec4( xlat_lib_saturate( orig.rgb  ), orig.a );
    c[1] = texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (-1.00000 * p1.y ))));
    c[2] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (-1.00000 * p1.y ))));
    c[3] = texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (-1.00000 * p1.y ))));
    c[4] = texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (0.000000 * p1.y ))));
    c[5] = texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (0.000000 * p1.y ))));
    c[6] = texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (1.00000 * p1.y ))));
    c[7] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (1.00000 * p1.y ))));
    c[8] = texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (1.00000 * p1.y ))));
    c[9] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (-2.00000 * p1.y ))));
    c[10] = texture2D( s0, (tex + vec2( (-2.00000 * p1.x ), (0.000000 * p1.y ))));
    c[11] = texture2D( s0, (tex + vec2( (2.00000 * p1.x ), (0.000000 * p1.y ))));
    c[12] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (2.00000 * p1.y ))));
    c[13] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (3.00000 * p1.y ))));
    c[14] = texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (2.00000 * p1.y ))));
    c[15] = texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (2.00000 * p1.y ))));
    c[16] = texture2D( s0, (tex + vec2( (3.00000 * p1.x ), (0.000000 * p1.y ))));
    c[17] = texture2D( s0, (tex + vec2( (2.00000 * p1.x ), (1.00000 * p1.y ))));
    c[18] = texture2D( s0, (tex + vec2( (2.00000 * p1.x ), (-1.00000 * p1.y ))));
    c[19] = texture2D( s0, (tex + vec2( (-3.00000 * p1.x ), (0.000000 * p1.y ))));
    c[20] = texture2D( s0, (tex + vec2( (-2.00000 * p1.x ), (1.00000 * p1.y ))));
    c[21] = texture2D( s0, (tex + vec2( (-2.00000 * p1.x ), (-1.00000 * p1.y ))));
    c[22] = texture2D( s0, (tex + vec2( (0.000000 * p1.x ), (-3.00000 * p1.y ))));
    c[23] = texture2D( s0, (tex + vec2( (1.00000 * p1.x ), (-2.00000 * p1.y ))));
    c[24] = texture2D( s0, (tex + vec2( (-1.00000 * p1.x ), (-2.00000 * p1.y ))));
    maxedge = (max( max( max( max( c[ 1 ].a , c[ 2 ].a ), max( c[ 3 ].a , c[ 4 ].a )), max( max( c[ 5 ].a , c[ 6 ].a ), max( c[ 7 ].a , c[ 8 ].a ))), max( max( max( c[ 9 ].a , c[ 10 ].a ), max( c[ 11 ].a , c[ 12 ].a )), c[ 0 ].a )) - 2.00000);
    sbe = ((((xlat_lib_saturate( ((((((c[ 2 ].a  + c[ 9 ].a ) + c[ 22 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ) * xlat_lib_saturate( ((((((c[ 7 ].a  + c[ 12 ].a ) + c[ 13 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) )) + (xlat_lib_saturate( ((((((c[ 4 ].a  + c[ 10 ].a ) + c[ 19 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ) * xlat_lib_saturate( ((((((c[ 5 ].a  + c[ 11 ].a ) + c[ 16 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ))) + (xlat_lib_saturate( ((((((c[ 1 ].a  + c[ 24 ].a ) + c[ 21 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ) * xlat_lib_saturate( ((((((c[ 8 ].a  + c[ 14 ].a ) + c[ 17 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ))) + (xlat_lib_saturate( ((((((c[ 3 ].a  + c[ 23 ].a ) + c[ 18 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) ) * xlat_lib_saturate( ((((((c[ 6 ].a  + c[ 20 ].a ) + c[ 15 ].a ) - 6.00000) + 0.0500000) / (abs( maxedge ) + 0.0200000)) - 0.850000) )));
    cs = mix( vec2( 0.169000, 0.253000), vec2( 0.337000, 0.504000), vec2( smoothstep( 2.00000, 3.10000, sbe)));
    c0_Y = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 0 ] * abs( c[ 0 ] )) ).rgb ) );
    luma[0] = c0_Y;
    luma[1] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 1 ] * abs( c[ 1 ] )) ).rgb ) );
    luma[2] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 2 ] * abs( c[ 2 ] )) ).rgb ) );
    luma[3] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 3 ] * abs( c[ 3 ] )) ).rgb ) );
    luma[4] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 4 ] * abs( c[ 4 ] )) ).rgb ) );
    luma[5] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 5 ] * abs( c[ 5 ] )) ).rgb ) );
    luma[6] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 6 ] * abs( c[ 6 ] )) ).rgb ) );
    luma[7] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 7 ] * abs( c[ 7 ] )) ).rgb ) );
    luma[8] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 8 ] * abs( c[ 8 ] )) ).rgb ) );
    luma[9] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 9 ] * abs( c[ 9 ] )) ).rgb ) );
    luma[10] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 10 ] * abs( c[ 10 ] )) ).rgb ) );
    luma[11] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 11 ] * abs( c[ 11 ] )) ).rgb ) );
    luma[12] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 12 ] * abs( c[ 12 ] )) ).rgb ) );
    luma[13] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 13 ] * abs( c[ 13 ] )) ).rgb ) );
    luma[14] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 14 ] * abs( c[ 14 ] )) ).rgb ) );
    luma[15] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 15 ] * abs( c[ 15 ] )) ).rgb ) );
    luma[16] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 16 ] * abs( c[ 16 ] )) ).rgb ) );
    luma[17] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 17 ] * abs( c[ 17 ] )) ).rgb ) );
    luma[18] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 18 ] * abs( c[ 18 ] )) ).rgb ) );
    luma[19] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 19 ] * abs( c[ 19 ] )) ).rgb ) );
    luma[20] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 20 ] * abs( c[ 20 ] )) ).rgb ) );
    luma[21] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 21 ] * abs( c[ 21 ] )) ).rgb ) );
    luma[22] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 22 ] * abs( c[ 22 ] )) ).rgb ) );
    luma[23] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 23 ] * abs( c[ 23 ] )) ).rgb ) );
    luma[24] = sqrt( dot( vec3( 0.255800, 0.651100, 0.0931000), xlat_lib_saturate( (c[ 24 ] * abs( c[ 24 ] )) ).rgb ) );
    dW = pow( mix( vec3( 0.500000, 1.00000, 1.41421), vec3( 0.866025, 1.00000, 0.547723), vec3( smoothstep( 0.300000, 0.800000, c_edge))), vec3( 2.00000));
    mdiff_c0 = (0.0200000 + (3.00000 * ((((abs( (luma[ 0 ] - luma[ 2 ]) ) + abs( (luma[ 0 ] - luma[ 4 ]) )) + abs( (luma[ 0 ] - luma[ 5 ]) )) + abs( (luma[ 0 ] - luma[ 7 ]) )) + (0.250000 * (((abs( (luma[ 0 ] - luma[ 1 ]) ) + abs( (luma[ 0 ] - luma[ 3 ]) )) + abs( (luma[ 0 ] - luma[ 6 ]) )) + abs( (luma[ 0 ] - luma[ 8 ]) ))))));
    weights[0] = min( (mdiff_c0 / ((((abs( (luma[ 1 ] - luma[ 24 ]) ) + abs( (luma[ 1 ] - luma[ 21 ]) )) + abs( (luma[ 1 ] - luma[ 2 ]) )) + abs( (luma[ 1 ] - luma[ 4 ]) )) + (0.500000 * (abs( (luma[ 1 ] - luma[ 9 ]) ) + abs( (luma[ 1 ] - luma[ 10 ]) ))))), dW.y );
    weights[1] = dW.x ;
    weights[2] = min( (mdiff_c0 / ((((abs( (luma[ 3 ] - luma[ 23 ]) ) + abs( (luma[ 3 ] - luma[ 18 ]) )) + abs( (luma[ 3 ] - luma[ 5 ]) )) + abs( (luma[ 3 ] - luma[ 2 ]) )) + (0.500000 * (abs( (luma[ 3 ] - luma[ 9 ]) ) + abs( (luma[ 3 ] - luma[ 11 ]) ))))), dW.y );
    weights[3] = dW.x ;
    weights[4] = dW.x ;
    weights[5] = min( (mdiff_c0 / ((((abs( (luma[ 6 ] - luma[ 4 ]) ) + abs( (luma[ 6 ] - luma[ 20 ]) )) + abs( (luma[ 6 ] - luma[ 15 ]) )) + abs( (luma[ 6 ] - luma[ 7 ]) )) + (0.500000 * (abs( (luma[ 6 ] - luma[ 10 ]) ) + abs( (luma[ 6 ] - luma[ 12 ]) ))))), dW.y );
    weights[6] = dW.x ;
    weights[7] = min( (mdiff_c0 / ((((abs( (luma[ 8 ] - luma[ 5 ]) ) + abs( (luma[ 8 ] - luma[ 7 ]) )) + abs( (luma[ 8 ] - luma[ 17 ]) )) + abs( (luma[ 8 ] - luma[ 14 ]) )) + (0.500000 * (abs( (luma[ 8 ] - luma[ 12 ]) ) + abs( (luma[ 8 ] - luma[ 11 ]) ))))), dW.y );
    weights[8] = min( (mdiff_c0 / ((((abs( (luma[ 9 ] - luma[ 2 ]) ) + abs( (luma[ 9 ] - luma[ 24 ]) )) + abs( (luma[ 9 ] - luma[ 23 ]) )) + abs( (luma[ 9 ] - luma[ 22 ]) )) + (0.500000 * (abs( (luma[ 9 ] - luma[ 1 ]) ) + abs( (luma[ 9 ] - luma[ 3 ]) ))))), dW.z );
    weights[9] = min( (mdiff_c0 / ((((abs( (luma[ 10 ] - luma[ 20 ]) ) + abs( (luma[ 10 ] - luma[ 19 ]) )) + abs( (luma[ 10 ] - luma[ 21 ]) )) + abs( (luma[ 10 ] - luma[ 4 ]) )) + (0.500000 * (abs( (luma[ 10 ] - luma[ 1 ]) ) + abs( (luma[ 10 ] - luma[ 6 ]) ))))), dW.z );
    weights[10] = min( (mdiff_c0 / ((((abs( (luma[ 11 ] - luma[ 17 ]) ) + abs( (luma[ 11 ] - luma[ 5 ]) )) + abs( (luma[ 11 ] - luma[ 18 ]) )) + abs( (luma[ 11 ] - luma[ 16 ]) )) + (0.500000 * (abs( (luma[ 11 ] - luma[ 3 ]) ) + abs( (luma[ 11 ] - luma[ 8 ]) ))))), dW.z );
    weights[11] = min( (mdiff_c0 / ((((abs( (luma[ 12 ] - luma[ 13 ]) ) + abs( (luma[ 12 ] - luma[ 15 ]) )) + abs( (luma[ 12 ] - luma[ 7 ]) )) + abs( (luma[ 12 ] - luma[ 14 ]) )) + (0.500000 * (abs( (luma[ 12 ] - luma[ 6 ]) ) + abs( (luma[ 12 ] - luma[ 8 ]) ))))), dW.z );
    weights[ 0 ] = ((max( max( ((weights[ 8 ] + weights[ 9 ]) / 4.00000), weights[ 0 ]), 0.250000) + weights[ 0 ]) / 2.00000);
    weights[ 2 ] = ((max( max( ((weights[ 8 ] + weights[ 10 ]) / 4.00000), weights[ 2 ]), 0.250000) + weights[ 2 ]) / 2.00000);
    weights[ 5 ] = ((max( max( ((weights[ 9 ] + weights[ 11 ]) / 4.00000), weights[ 5 ]), 0.250000) + weights[ 5 ]) / 2.00000);
    weights[ 7 ] = ((max( max( ((weights[ 10 ] + weights[ 11 ]) / 4.00000), weights[ 7 ]), 0.250000) + weights[ 7 ]) / 2.00000);
    for ( ; (pix < 12); ( ++pix )) {
        x = xlat_lib_saturate( (((c[ (pix + 1) ].a  - 2.00000) - 0.0100000) / 0.100000) );
        lowthr = (((x * x) * (2.97000 - (1.98000 * x))) + 0.0100000);
        neg_laplace += (pow( (luma[ (pix + 1) ] + 0.0600000), 2.40000) * (weights[ pix ] * lowthr));
        weightsum += (weights[ pix ] * lowthr);
        lowthrsum += (lowthr / 12.0000);
    }
    neg_laplace = (pow( abs( (neg_laplace / weightsum) ), 0.416667) - 0.0600000);
    sharpen_val = (1.00000 / ((0.400000 * pow( abs( c_edge ), 3.50000)) + 0.500000));
    sharpdiff = ((c0_Y - neg_laplace) * (((lowthrsum * sharpen_val) * 0.800000) + 0.0100000));
    for ( ; (i < 3); ( ++i )) {
        i1 = i;
        for ( ; (i1 < (24 - i)); i1 += 2) {
            temp = luma[ i1 ];
            luma[ i1 ] = min( luma[ i1 ], luma[ (i1 + 1) ]);
            luma[ (i1 + 1) ] = max( temp, luma[ (i1 + 1) ]);
        }
        i2 = (24 - i);
        for ( ; (i2 > i); i2 -= 2) {
            temp = luma[ i ];
            luma[ i ] = min( luma[ i ], luma[ i2 ]);
            luma[ i2 ] = max( temp, luma[ i2 ]);
            temp = luma[ (24 - i) ];
            luma[ (24 - i) ] = max( luma[ (24 - i) ], luma[ (i2 - 1) ]);
            luma[ (i2 - 1) ] = min( temp, luma[ (i2 - 1) ]);
        }
    }
    nmax = ((max( (luma[ 22 ] + (luma[ 23 ] * 2.00000)), (c0_Y * 3.00000)) + luma[ 24 ]) / 4.00000);
    nmin = ((min( (luma[ 2 ] + (luma[ 1 ] * 2.00000)), (c0_Y * 3.00000)) + luma[ 0 ]) / 4.00000);
    nmax_scale = min( ((nmax - c0_Y) + min( 0.00300000, (1.00010 - nmax))), 0.100000);
    nmin_scale = min( ((c0_Y - nmin) + min( 0.00900000, (0.000100000 + nmin))), 0.100000);
    sharpdiff = (pow( ((cs.x  * pow( abs( max( sharpdiff, 0.000000) ), 0.750000)) + (abs( (1.00000 - cs.x ) ) * pow( abs( (((exp( ((2.00000 * min( abs( max( sharpdiff, 0.000000) ), (nmax_scale * 24.0000))) / nmax_scale) ) - 1.00000) / (exp( ((2.00000 * min( abs( max( sharpdiff, 0.000000) ), (nmax_scale * 24.0000))) / nmax_scale) ) + 1.00000)) * nmax_scale) ), 0.750000))), 1.33333) - pow( ((cs.y  * pow( abs( min( sharpdiff, 0.000000) ), 0.750000)) + (abs( (1.00000 - cs.y ) ) * pow( abs( (((exp( ((2.00000 * min( abs( min( sharpdiff, 0.000000) ), (nmin_scale * 24.0000))) / nmin_scale) ) - 1.00000) / (exp( ((2.00000 * min( abs( min( sharpdiff, 0.000000) ), (nmin_scale * 24.0000))) / nmin_scale) ) + 1.00000)) * nmin_scale) ), 0.750000))), 1.33333));
    satmul = ((xlat_lib_saturate( (c0_Y + sharpdiff) ) + 0.0200000) / (c0_Y + 0.0200000));
    res = ((c0_Y + sharpdiff) + ((c[ 0 ].rgb  - c0_Y) * satmul));
    vec4 r=vec4( ( false ) ? ( (orig.rgb  + (res - c[ 0 ].rgb )) ) : ( res ), 1.00000);
    r.r=1;
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
