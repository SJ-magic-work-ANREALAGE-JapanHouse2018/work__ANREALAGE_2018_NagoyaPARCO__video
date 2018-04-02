#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;

uniform vec2 u_resolution;
uniform vec2 CoordOffset;
uniform float Zoom;

uniform vec2 OfsDirVector_R;
uniform vec2 OfsDirVector_G;
uniform vec2 OfsDirVector_B;

/********************
0:none.
1:LR
2:UpDown
3:Diag
********************/
uniform int mirror = 0;

vec2 get_mirrored(vec2 st);

void main(){
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
	st -= CoordOffset;
    st = fract(st * Zoom);
	st = get_mirrored(st);
	
	vec2 pos_R		= (st + OfsDirVector_R) * u_resolution;
	vec4 color_R	= texture2DRect(texture0, pos_R);

	vec2 pos_G		= (st + OfsDirVector_G) * u_resolution;
	vec4 color_G	= texture2DRect(texture0, pos_G);

	vec2 pos_B		= (st + OfsDirVector_B) * u_resolution;
	vec4 color_B	= texture2DRect(texture0, pos_B);

	
	vec4 color;
	color.r = color_R.r;
	color.g = color_G.g;
	color.b = color_B.b;
	color.a = 1.0;
	
	gl_FragColor = color;
}

vec2 get_mirrored(vec2 st)
{
	if(mirror == 0)			{/* none. */}
	else if(mirror == 1)	{st.x = 1.0 - st.x;}
	else if(mirror == 2)	{st.y = 1.0 - st.y;}
	else if(mirror == 3)	{st = 1.0 - st;}
	
	return st;
}
