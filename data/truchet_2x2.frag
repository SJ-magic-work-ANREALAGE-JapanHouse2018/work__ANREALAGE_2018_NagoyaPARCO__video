/************************************************************
************************************************************/
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

/************************************************************
************************************************************/
#define PI 3.14159265358979323846

uniform vec2 u_resolution;
// uniform vec2 u_mouse;
// uniform float u_time;

uniform vec2 CoordOffset;
uniform float Zoom;

// uniform int Truchet_DesignId;
uniform float Angle_0;
uniform float Angle_1;
uniform float Angle_2;
uniform float Angle_3;


/************************************************************
************************************************************/
/******************************
******************************/
vec2 rotate2D (vec2 _st, float _angle) {
    _st -= 0.5;
    _st =  mat2(cos(_angle),-sin(_angle),
                sin(_angle),cos(_angle)) * _st;
    _st += 0.5;
    return _st;
}

/******************************
******************************/
vec2 tile (vec2 _st, float _zoom) {
    _st *= _zoom;
	// return fract(_st);
	return _st;
}

/******************************
******************************/
vec2 rotateTilePattern(vec2 _st){
    //  Give each cell an index number
    //  according to its position
    float index = 0.0;
    index += step(1., mod(_st.x,2.0));
    index += step(1., mod(_st.y,2.0))*2.0;

    //      |
    //  2   |   3
    //      |
    //--------------
    //      |
    //  0   |   1
    //      |

    // Make each cell between 0.0 - 1.0
    _st = fract(_st);

	if(index == 0.0){
		_st = rotate2D(_st, Angle_0);
	}else if(index == 1.0){
		_st = rotate2D(_st, Angle_1);
    } else if(index == 2.0){
		_st = rotate2D(_st, Angle_2);
    } else if(index == 3.0){
		_st = rotate2D(_st, Angle_3);
    }
	
	/*
    // Rotate each cell according to the index
    if(index == 1.0){
        //  Rotate cell 1 by 90 degrees
        _st = rotate2D(_st,PI*0.5);
    } else if(index == 2.0){
        //  Rotate cell 2 by -90 degrees
        _st = rotate2D(_st,PI*-0.5);
    } else if(index == 3.0){
        //  Rotate cell 3 by 180 degrees
        _st = rotate2D(_st,PI);
    }
	*/

    return _st;
}

/******************************
******************************/
void main (void) {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
	st -= CoordOffset;
	
    st = tile(st, Zoom);
    st = rotateTilePattern(st);

    // Make more interesting combinations
    // st = tile(st,2.0);
    // st = rotate2D(st,-PI*u_time*0.25);
    // st = rotateTilePattern(st*2.);
    // st = rotate2D(st,PI*u_time*0.25);

    // step(st.x,st.y) just makes a b&w triangles
    // but you can use whatever design you want.
    // gl_FragColor = vec4(vec3(step(st.x,st.y)),1.0);
	gl_FragColor = vec4(vec3(smoothstep(st.x - 0.03, st.x + 0.03, st.y)), 1.0);
}

