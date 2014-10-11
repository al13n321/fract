#version 410

uniform sampler2D main_texture;
in vec2 fPosition;
out vec4 col;

void main(void) {
	vec4 v = texture(main_texture, fPosition);
	if (v.w > 0.0) {
		gl_FragColor = vec4(1, 0, 0, 1);
		return;
	}
	vec4 background = vec4(.4, .4, .9, 1.);
	vec4 foreground = vec4(0., v.y / 10., v.z / 10., 0);
	gl_FragColor = mix(background, foreground, smoothstep(0., 1., v.x));
}
