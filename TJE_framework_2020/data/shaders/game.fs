varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;
varying vec4 v_color;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform float u_time;

//light
uniform vec3 u_light_direction;
uniform vec3 u_camera_pos;

void main()
{
	vec2 uv = v_uv;

    vec4 color = u_color * texture2D( u_texture, uv );
	
    //normalize vectors
    vec3 N = normalize(v_normal);
    vec3 L = normalize(u_light_direction);
    
    //light
    float NdotL = clamp( dot(N, L), 0.0, 1.0);
    //number of tones of color
    float tones = 4.0;
    NdotL = round(NdotL * tones) / tones;
    //light more white
    vec3 light = NdotL * vec3(1.0, 1.0, 1.0) * 0.8;
    //light with ambient
    light += vec3(0.2, 0.3, 0.4);
    //niebla
    //distance between camera position and where the object is
    float distance = length(v_world_position - u_camera_pos);
    distance = clamp(distance / 512.0, 0.0, 1.0);
    distance = pow(distance, 0.5);
    //cuanto mas lejos este el objeto mas tendera a ese color
    vec3 fog_color = vec3(1.0, 1.0, 1.0) * 0.8;
    //interpolacion lineal
    color.xyz = mix(color.xyz, fog_color, distance);
    //color.x = color.r equivalent!
    color.xyz *= light;
    gl_FragColor = color;
}
