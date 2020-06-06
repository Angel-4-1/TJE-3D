varying vec2 v_uv;
uniform sampler2D u_texture;
uniform float u_time;

void main()
{
	vec2 uv = v_uv;
	vec4 color = texture2D( u_texture, uv );
	//Black & White
	// color.rgb = vec3(length(color) * 0.4);

	//Distance to center screen
	// float dist = length(v_uv - vec2(0.5));
	//Mancha en el centro
	// color *= dist;
	//Mancha en los bordes
	// color *= 1.0 - dist;

	//Pixeles oscilan respecto el tiempo
	uv.x += sin(uv.y*20.0+u_time)*0.01;
	color = texture2D( u_texture, uv );

	//Smulacion velocidad
	//float dist = length(uv - vec2(0.5));
	//uv.x += sin(uv.y*2.0)*0.1;
	//color = texture2D( u_texture, uv );
	//color *= 1.0 - dist;
	
	gl_FragColor = color;
}