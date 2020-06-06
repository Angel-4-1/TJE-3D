varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

uniform vec3 u_camera_pos;

// Light variables
uniform vec3 ambient_light;
uniform vec3 light_position;	
uniform vec3 light_diffuse;	
uniform vec3 light_specular;

uniform float u_material_shininess;

void main()
{
	vec2 uv = v_uv;

    vec4 color = u_color * texture2D( u_texture, uv );

	vec3 light_vector = normalize(light_position - v_world_position);
	vec3 eye_vector = normalize(v_world_position - u_camera_pos);	
	vec3 R = reflect(light_vector, v_normal);

	float LdotN = clamp( dot(light_vector, v_normal), 0.0, 1.0);
	float RdotEye = clamp( dot(R, eye_vector), 0.0, 1.0);

	vec3 Ka = vec3(1, 1, 1); //material ambient
	vec3 Kd = vec3(1, 1, 1); //material diffuse
	vec3 Ks = vec3(1, 1, 1); //material specular
	float alpha = u_material_shininess;      //material shininess

	vec3 Ia = vec3(0.7, 0.6, 0.6);//ambient_light;
	vec3 Id = vec3(0.8, 0.8, 0.8);//light_diffuse;
	vec3 Is = light_specular;

	vec3 ambient = Ka * Ia;
	vec3 diffuse = Kd * LdotN * Id;
	vec3 specular = Ks * pow(RdotEye, alpha) * Is;

	float distance = length(v_world_position - u_camera_pos);
    distance = clamp(distance / 512.0, 0.0, 1.0);
    distance = pow(distance, 0.5);

    //cuanto mas lejos este el objeto mas tendera a ese color
    vec3 fog_color = vec3(0.7, 0.4, 0.4) * 0.6;
    //interpolacion lineal
    color.xyz = mix(color.xyz, fog_color, distance);
	
	color.rgb *= specular + ambient + diffuse ;

	//set the ouput color por the pixel
	//gl_FragColor = vec4( color, 1.0 ) * 1.0;

	//color.xyz *= light;
    gl_FragColor = color;
}