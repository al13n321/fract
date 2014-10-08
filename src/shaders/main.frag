uniform sampler2D main_texture;
varying vec2 fPosition;
void main(void) {
	vec4 v = texture2D(main_texture, fPosition);
	gl_FragColor = vec4(v.x, log(v.y + 1.) / 10., log(v.z + 1.) / 10., 1.);
}
