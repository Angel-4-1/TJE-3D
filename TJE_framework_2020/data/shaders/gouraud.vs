//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;
uniform vec3 camera_position;

// Light variables
uniform vec3 ambient_light;
uniform vec3 light_position;	
uniform vec3 light_diffuse;	
uniform vec3 light_specular;

// Material properties
uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_shininess;

//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 v_light_intensity;

//here create uniforms for all the data we need here

void main()
{	
	//convert local coordinate to world coordinates
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz;

	//convert local normal to world coordinates
	vec3 wNormal = normalize( (model * vec4(gl_Normal.xyz , 0.0)).xyz );

	//pass them to the pixel shader interpolated
	v_wPos = wPos;
	v_wNormal = wNormal;

	//in GOURAUD compute the color here and pass it to the pixel shader
	vec3 light_vector = normalize(light_position - wPos);
	vec3 eye_vector = normalize(wPos - camera_position);	
	// Reflect incident light_vector according to wNormal
	vec3 R = reflect(light_vector, wNormal);

	vec3 Ka = material_ambient;
	vec3 Kd = material_diffuse;
	vec3 Ks = material_specular;
	float alpha = material_shininess;

	vec3 Ia = ambient_light;
	vec3 Id = light_diffuse;
	vec3 Is = light_specular;

	float LdotN = clamp( dot(light_vector, wNormal), 0.0, 1.0);
	float RdotEye = clamp( dot(R, eye_vector), 0.0, 1.0);

	vec3 ambient = Ka * Ia;
	vec3 diffuse = Kd * LdotN * Id;
	vec3 specular = Ks * pow(RdotEye, alpha) * Is;
	
	v_light_intensity = ambient + diffuse + specular;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}