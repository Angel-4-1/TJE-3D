varying vec2 v_uv;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	vec4 color = texture2D( u_texture, uv );
	//Pixeles oscilan respecto el tiempo
	uv.x += sin(uv.y*20.0+u_time)*0.01;
	color = texture2D( u_texture, uv );
	
	gl_FragColor = color;
}