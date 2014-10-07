uniform sampler2D main_texture;
varying vec2 fPosition;
void main(void) {
	gl_FragColor = vec4(fPosition, fPosition);
	//gl_FragColor = texture2D(main_texture, fPosition);
	//TODO: real implementation
}
