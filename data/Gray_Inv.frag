#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;

void main(){
	/********************
	********************/
	vec2 pos = gl_TexCoord[0].xy;
	vec4 color0 = texture2DRect(texture0, pos);

	/********************
	********************/
	vec4 color;
	if((color0.r == color0.g) && (color0.g == color0.b)){
		color = color0;
	}else{
		float val = 0.299 * color0.r + 0.587 * color0.g + 0.114 * color0.b;
		color = vec4(val, val, val, color0.a);
	}
	
	/********************
	********************/
	color.r = 1.0 - color.r;
	color.g = 1.0 - color.g;
	color.b = 1.0 - color.b;
	
	/********************
	********************/
	gl_FragColor = color;
}
