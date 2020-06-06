varying vec2 v_uv;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	vec4 color = texture2D( u_texture, uv );
	
	//Black & White
	color.rgb = vec3(length(color) * 0.4);
	
	gl_FragColor = color;
}