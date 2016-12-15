#version 120

#ifdef GL_ES
// Set default precision to medium
precision mediump int;
precision highp float;
#endif

uniform sampler2D texture;
uniform sampler2D texture1;
uniform int pass;

varying vec2 v_texcoord;
varying float counter;
varying vec4 v_color;

const float pixels=640.*480.;



//! [0]
void main()
{
    vec4 color;

    // Set fragment color from texture
    if (pass==0){
        color=vec4(1.0f);
    }
    else if (pass==1){
        color=vec4(2.0f,0.5f,1.0f,1.0f);
    }else{
        color=vec4(1.0f,0.5f,2.0f,1.0f);
    }

    if (pass==1){
      gl_FragColor = texture2D(texture1, v_texcoord);//*(1.0-texture2D(texture1, v_texcoord*vec2(2.0f)));
      //gl_FragColor += texture2D(texture, v_texcoord)*color;
    }else{
      gl_FragColor = texture2D(texture, v_texcoord)*color;
    }
    //float c1=v_color;
    //gl_FragColor = vec4(c1,0.,0.,1.0);
    //gl_FragColor=v_color;

    //counter+=100./(640.*480.);
    //counter=0.5;

}
//! [0]

