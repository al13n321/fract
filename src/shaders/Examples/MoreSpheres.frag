#version 410

#define BOUNDS 1000.0

#include <Raytracers/DE-Raytracer.frag>


uniform float Radius = 1.5;
uniform float Coef = .35;
uniform int Iterations = 10;
uniform float RotSpeed = 1;
uniform float PulseSpeed = 3;
uniform float PulseRadius = 0.1;

float DE(tvec4 p0) {
	ftype res = 1e5;
	ftype coef = 1.;
	tvec3 p = p0.xyz * .4;
	for (int i=0; i<Iterations; i++) {
		res = min(res, (length(p) - Radius - sin(Now * PulseSpeed - .3 * i) * PulseRadius * (i/2. + .5)) * coef);

		float t = RotSpeed * Now * (i + 0.5) / 2.;
		float s = sin(t);
		float c = cos(t);
		p = mat3(s, c, 0,
		         -c, s, 0,
		         0, 0, 1) * p;
		vec3 sg = sign(p);
		p -= sg;
	  t = dot(sg, vec3(1.2, 3.1, 7));
		s = sin(t);
		c = cos(t);
		p = mat3(1, 0, 0,
		         0, s, c,
		         0, -c, s) * p;
		t = dot(sg, vec3(54, 11, 91));
		s = sin(t);
		c = cos(t);
		p = mat3(s, 0, c,
		         0, 1, 0,
		         -c, 0, s) * p;
		coef *= Coef;
		p /= Coef;
	}
	return res;
}
