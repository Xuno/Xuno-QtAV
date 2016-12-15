#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision highp float;
//precision mediump float;
//precision lowp float;
#endif

#define XBR_WEIGHT 1.0

//#if (dpass == 0)
//        #define wp1   1.0
//        #define wp2   0.0
//        #define wp3   0.0
//        #define wp4   2.0
//        #define wp5  -1.0
//        #define wp6   0.0
//#elif (dpass == 1)
//        #define wp1   1.0
//        #define wp2   0.0
//        #define wp3   0.0
//        #define wp4   4.0
//        #define wp5   0.0
//        #define wp6   0.0
//#else
//        #define wp1   1.0
//        #define wp2   0.0
//        #define wp3   0.0
//        #define wp4   0.0
//        #define wp5  -1.0
//        #define wp6   0.0
//#endif


#if (dpass == 0)
#define wp1  2.0
#define wp2  1.0
#define wp3 -1.0
#define wp4  4.0
#define wp5 -1.0
#define wp6  1.0
#elif (dpass == 1)
#define wp1  2.0
#define wp2  0.0
#define wp3  0.0
#define wp4  0.0
#define wp5  0.0
#define wp6  0.0
#else
#define wp1  2.0
#define wp2  0.0
#define wp3  0.0
#define wp4  0.0
#define wp5  0.0
#define wp6  0.0
#endif



#if (dpass == 1)
        #define weight1 (XBR_WEIGHT*1.75068/10.0)
        #define weight2 (XBR_WEIGHT*1.29633/10.0/2.0)
#else
        #define weight1 (XBR_WEIGHT*1.29633/10.0)
        #define weight2 (XBR_WEIGHT*1.75068/10.0/2.0)
#endif

//#if (dpass == 0)
//        #define Get(x,y) (texture2D(tex, pos + pixel_size*vec2(x,y))[plane] * tex_mul)
//#elif (dpass == 1)
//        #define Get(x,y) (texture2D(tex, pos + pixel_size*vec2((x)+(y) - 1,(y) - (x)))[plane] * tex_mul)
//#else
//        #define Get(x,y) (texture2D(tex, pos - pixel_size*vec2(x,y))[plane] * tex_mul)
//#endif

//#define Get(x,y) (texture2D(tex, pos + pixel_size*vec2((x)+(y) - 1,(y) - (x)))[plane] * tex_mul)

//#define Get(x, y) (vec4(1., 0., 0., 1.)[plane])

#if (dpass == 0)
#define Get(x, y) (texture2D(tex, pos + (vec2(x, y) - vec2(0.25, 0.25)) * pixel_size)[plane] * tex_mul)
#elif (dpass == 1)
#define Get(x, y) (texture2D(tex, pos + (vec2((x) + (y) - 1, (y) - (x))) * pixel_size)[plane] * tex_mul)
#else
#define Get(x, y) (texture2D(tex, pos + (vec2(x, y) - vec2(0.25, 0.25)) * pixel_size)[plane] * tex_mul)
#endif



//#define wp1 2.0
//#define wp2 1.0
//#define wp3 -1.0
//#define wp4 4.0
//#define wp5 -1.0
//#define wp6 1.0
//#define weight1 (1.000000*1.29633/10.0)
//#define weight2 (1.000000*1.75068/10.0/2.0)
//#define Get(x, y) (texture2D(tex, pos + (vec2(x, y) - vec2(0.25, 0.25)) * pixel_size)[plane] * tex_mul)

uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform int dpass;
uniform highp vec2 texture_size;

 //vec2 pixel_size=vec2(2.f/1920.f,2.f/1080.f);
uniform highp vec2 pixel_size;
uniform float conf_edge_strength=0.9;
//const vec2 tex_mul=vec2(1.0);

varying highp vec2 v_texcoord;//lowp





float df(float A, float B)
{
    return abs(A-B);
}

float d_wd(float b0, float b1, float c0, float c1, float c2, float d0, float d1, float d2, float d3, float e1, float e2, float e3, float f2, float f3) {
    return (wp1*(df(c1,c2) + df(c1,c0) + df(e2,e1) + df(e2,e3)) + wp2*(df(d2,d3) + df(d0,d1)) + wp3*(df(d1,d3) + df(d0,d2)) + wp4*df(d1,d2) + wp5*(df(c0,c2) + df(e1,e3)) + wp6*(df(b0,b1) + df(f2,f3)));
}

float hv_wd(float i1, float i2, float i3, float i4, float e1, float e2, float e3, float e4)
{
    return (wp4*(df(i1,i2)+df(i3,i4)) + wp1*(df(i1,e1)+df(i2,e2)+df(i3,e3)+df(i4,e4)) + wp3*(df(i1,e2)+df(i3,e4)+df(e1,i2)+df(e3,i4)));
}

float superxbr(sampler2D tex, vec2 pos, vec2 tex_size, vec2 pixel_size, int plane, float tex_mul)
{
//    vec2 dir = fract(pos * tex_size) - 0.5;
//    vec2 dist = tex_size * min(pos, vec2(1.0) - pos);
//    if (dir.x * dir.y < 0.0 && dist.x > 1.0 && dist.y > 1.0)
//        return 0.0;
//    if (dir.x < 0.0 || dir.y < 0.0 || dist.x < 1.0 || dist.y < 1.0)
//        return texture2D(tex, pos - dir * pixel_size)[plane] * tex_mul;

    //Skip pixels on wrong grid

#if (dpass == 0)

    vec2 dir = fract(pos * tex_size)-vec2(0.5) ;

    // Optimization: Discard (skip drawing) unused pixels, except those
    // at the edge.
    vec2 dist = tex_size * min(pos, vec2(1.0f) - pos);
    if (dir.x * dir.y < 0.0f && dist.x > 1.0f && dist.y > 1.0f){

        //return texture2D(tex, pos)[plane];
        //return vec4(0.0, 1.0, 0.0, 1.0)[plane];
              return 0.0;
    }

    if (dir.x < 0.0 || dir.y < 0.0 || dist.x < 1.0 || dist.y < 1.0)
        //return vec4(1.0, 0.0, 0.0, 1.0)[plane];
              return texture2D(tex, pos - dir * pixel_size)[plane] * tex_mul;



//    if (any(lessThan(fract( pos*tex_size) ,vec2(0.5f)))){
//        return vec4(0.0, 1.0, 0.0, 1.0)[plane] * tex_mul;
//        return texture2D(tex, pos)[plane] * tex_mul;
//    }
#else
    vec2 dir = fract(pos * tex_size/2.0) - vec2(0.5f) ;
    if (dir.x * dir.y > 0.0)
              return texture2D(tex, pos)[plane] * tex_mul;

//    vec2 dir = fract(pos*tex_size/2.0) - (0.5);
//    if ((dir.x*dir.y)>0.0){
//        //return vec4(0.0, 0.0, 1.0, 1.0)[plane] * tex_mul;
//        return texture2D(tex, pos)[plane] * tex_mul;
//    }
#endif


    float P0 = Get(-1,-1);
    float P1 = Get( 2,-1);
    float P2 = Get(-1, 2);
    float P3 = Get( 2, 2);
    float B = Get( 0,-1);
    float C = Get( 1,-1);
    float D = Get(-1, 0);
    float E = Get( 0, 0);
    float F = Get( 1, 0);
    float G = Get(-1, 1);
    float H = Get( 0, 1);
    float I = Get( 1, 1);
    float F4 = Get(2, 0);
    float I4 = Get(2, 1);
    float H5 = Get(0, 2);
    float I5 = Get(1, 2);
    float d_edge = (d_wd( D, B, G, E, C, P2, H, F, P1, H5, I, F4, I5, I4 ) - d_wd( C, F4, B, F, I4, P0, E, I, P3, D, H, I5, G, H5 ));
    float hv_edge = (hv_wd(F, I, E, H, C, I5, B, H5) - hv_wd(E, F, H, I, D, F4, G, I4));
    vec4 w1 = vec4(-weight1, weight1+0.5, weight1+0.5, -weight1);
    vec4 w2 = vec4(-weight2, weight2+0.25, weight2+0.25, -weight2);
    float c1 = dot(vec4(P2, H, F, P1), w1);
    float c2 = dot(vec4(P0, E, I, P3), w1);
    float c3 = dot(vec4( D+G, E+H, F+I, F4+I4), w2);
    float c4 = dot(vec4( C+B, F+E, I+H, I5+H5), w2);
    float limits = conf_edge_strength + 0.000001;
    float edge_strength = smoothstep(0.0, limits, abs(d_edge));
    float color =  mix(mix(c1, c2, step(0.0, d_edge)),mix(c3, c4, step(0.0, hv_edge)), 1.0 - conf_edge_strength);
    float min_sample = min(min(E, F), min(H, I));
    float max_sample = max(max(E, F), max(H, I));
    float aux = color;
    color = clamp(color, min_sample, max_sample);
    color = mix(aux, color, 1.0-2.0*abs(conf_edge_strength-0.5));
    return color;
}

void main()
{
    vec4 color;
    // superxbr (tex 0, step 1)
//    color = vec4(1.0f);
//    color[0] = superxbr(texture0, v_texcoord, texture_size, pixel_size, 0, 1.000000);
//    color[1] = superxbr(texture0, v_texcoord, texture_size, pixel_size, 1, 1.000000);
//    color[2] = superxbr(texture0, v_texcoord, texture_size, pixel_size, 2, 1.000000);
    //gl_FragColor = texture2D(texture0, v_texcoord);//*color;
//    gl_FragColor = color;
  highp  vec2  dir = fract(v_texcoord * texture_size) - vec2(0.5f) ;
  highp  vec2  dist = texture_size * min(v_texcoord, vec2(1.0f) - v_texcoord);

    if (dir.x * dir.y < 0.0f && dist.x > 1.0f && dist.y > 1.0f){
    //if (fract((v_texcoord/pixel_size).x)-0.5f == 0.f /*|| fract((v_texcoord/pixel_size).x) == 2.f*/ ){//|| ((v_texcoord*pixel_size).y >= 100.f)
        gl_FragColor = vec4(1.0f,0.f,0.f,1.f);
    }else{
        gl_FragColor = vec4(0.0f,1.f,0.f,1.f);
      //gl_FragColor = texture2D(texture0, v_texcoord);
    }


}




//! [0]
//void main0()
//{
//    vec4 color;

//    // Set fragment color from texture
//    if (dpass==0){
//        color=vec4(1.0f);
//    }
//    else if (dpass==1){
//        color=vec4(2.0f,0.5f,1.0f,1.0f);
//    }else{
//        color=vec4(1.0f,0.5f,2.0f,1.0f);
//    }

//    if (dpass==1){
//      gl_FragColor = texture2D(texture1, v_texcoord);//*(1.0-texture2D(texture1, v_texcoord*vec2(2.0f)));
//      //gl_FragColor += texture2D(texture, v_texcoord)*color;
//    }else{
//      gl_FragColor = texture2D(texture0, v_texcoord)*color;
//    }
//    //float c1=v_color;
//    //gl_FragColor = vec4(c1,0.,0.,1.0);
//    //gl_FragColor=v_color;

//    //counter+=100./(640.*480.);
//    //counter=0.5;

//}
//! [0]

