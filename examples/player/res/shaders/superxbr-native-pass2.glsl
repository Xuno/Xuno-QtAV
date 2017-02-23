#version 130
#define MAIN_raw texture0
#define MAIN_pos texcoord0
#define MAIN_size texture_size0
#define MAIN_rot texture_rot0
#define MAIN_pt pixel_size0

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

//options
uniform float sharpness = 1.0f;
uniform float edge_strength = 0.6f;

//Create FBO: 3342x2086 (3342x2086) #4
//recompiling a shader program:

//color.rgba = 1.000000 * vec4(texture(MAIN_raw, MAIN_pos)).rgba;

// scaler pre-conversion

// scaler post-conversion
// color mapping
// transparency checkerboard


void main(void)
{
 vec4 color;
 color.rgba = 1.000000 * vec4(texture(MAIN_raw, MAIN_pos)).rgba;
 bvec2 tile = lessThan(fract(gl_FragCoord.xy / 32.0), vec2(0.5));
 vec3 background = vec3(tile.x == tile.y ? 1.0 : 0.75);
 color.rgb = mix(background, color.rgb, color.a);
 gl_FragColor = color;
}
