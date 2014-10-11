#version 410

uniform sampler2D main_texture;
uniform sampler2D normal_texture;
uniform sampler2D color_texture;
in vec2 fPosition;
out vec4 outColor;

void main(void) {
	vec4 v = texture(main_texture, fPosition);
	vec4 background = vec4(.4, .4, .9, 1.);	
	vec4 error_color = vec4(1, 0, 0, 1);

	vec3 normal = texture(normal_texture, fPosition).xyz;
	
	vec3 diffuse_color = vec3(.9, .9, .9);
	vec3 light_dir = normalize(vec3(1,2,3));

	vec4 foreground = vec4(diffuse_color * max(0., dot(normal, light_dir)), 1);

	outColor = mix(
		mix(background, foreground, smoothstep(0., 1., v.x)),
		error_color, smoothstep(0., 1., v.w));
}
