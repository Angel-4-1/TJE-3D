varying vec2 v_uv;
uniform sampler2D u_texture;
uniform float u_time;
uniform float u_factor;

void main()
{
	vec2 uv = v_uv;
	vec4 color = texture2D( u_texture, uv );
	//Black & White
	// color.rgb = vec3(length(color) * 0.4);

	//Distance to center screen
	float dist = length(v_uv - vec2(0.5));

	//Mancha en los bordes
	//float t = clamp(dist * 0.3, 0.0, 1.0);
	color *= 1.0 - dist * u_factor;
	//color = vec4(color.xyz, 1.0 - t);
	//color *= 1.0 - dist * u_factor;
	
	gl_FragColor = color;
}