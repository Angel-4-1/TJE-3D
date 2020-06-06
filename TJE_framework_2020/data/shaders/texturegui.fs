
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

uniform vec4 u_tex_range; //startx, starty, w, h

void main()
{
	vec2 uv = v_uv;
	
	//uv.x = (width) * uv.x + startx;
	uv.x = (u_tex_range.z) * uv.x + u_tex_range.x;
	//uv.y = (height) * uv.y + starty;
	uv.y = (u_tex_range.w) * uv.y + u_tex_range.y;

	vec4 color = u_color * texture2D( u_texture, uv );
	if(color.a <= 0.0)
		discard;
	gl_FragColor = color;
}
