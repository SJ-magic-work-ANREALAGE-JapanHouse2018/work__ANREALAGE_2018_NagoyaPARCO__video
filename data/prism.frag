#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;

uniform vec2 u_resolution;

uniform vec2 OfsDirVector_R;
uniform vec2 OfsDirVector_G;
uniform vec2 OfsDirVector_B;

void main(){
	vec2 pos_R		= gl_TexCoord[0].xy + OfsDirVector_R * u_resolution.x;
	vec4 color_R	= texture2DRect(texture0, pos_R);
	
	vec2 pos_G		= gl_TexCoord[0].xy + OfsDirVector_G * u_resolution.x;
	vec4 color_G	= texture2DRect(texture0, pos_G);
	
	vec2 pos_B		= gl_TexCoord[0].xy + OfsDirVector_B * u_resolution.x;
	vec4 color_B	= texture2DRect(texture0, pos_B);
	
	vec4 color;
	color.r = color_R.r;
	color.g = color_G.g;
	color.b = color_B.b;
	color.a = 1.0;
	
	gl_FragColor = color;
}
