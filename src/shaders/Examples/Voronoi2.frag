#version 410

#define BOUNDS 100.

#include <Raytracers/DE-Raytracer.frag>

// This was an attempt to use two nearest points, but it didn't work well.


uniform float Radius = 0.1;

tvec3 Point(tvec3 p) {
	// Random matrix.
	const mat3 m = mat3(
		0.8086302229028699, 0.5227741986417075, 0.6827060411123068,
		0.5276513809626875, 0.3442914207412343, 0.37211591216017514,
		0.42050358589267633, 0.898297255815332, 0.781488075294931);

	// Random jerky movement.
	//return p + 1/3. + (1/ 3.) * fract(sin(m * p + Now * 1e-3) * 100);

	// Smooth waves.
	return p + 1/3. + (1/ 6.) * (sin(m * p + Now * 5) + 1);
}

tvec4 close1;
tvec4 close2;

void Closest(tvec3 p, tvec3 g) {
	tvec3 t = p - g;
	ftype d = dot(t, t);
	if (d < close1.w) {
		close2 = close1;
		close1 = tvec4(g, d);
	} else if (d < close2.w) {
		close2 = tvec4(g, d);
	}
}

void Voronoi(tvec3 p) {
	tvec3 g = floor(p);
	tvec4 s = tvec4(sign(p - g - .5), 0);

	Closest(p, Point(g));
	Closest(p, Point(g + s.wwz));
	Closest(p, Point(g + s.wyw));
	Closest(p, Point(g + s.xww));
}

ftype Metaballs(tvec3 p, tvec3 a, tvec3 b) {
	return min(min(length(p-a), length(p-b)) - Radius,
		dot(b-a,b-a) > Radius*Radius * 36 ? 1e10 :
		distance(p, a + (b-a)*clamp((dot(b-a,p-a)/dot(b-a,b-a)),0,1)) - Radius / 5
	);
}

float DE(tvec4 p0) {
	// Random rotation matrix.
	const mat3 rot = mat3(
		0.15520525,  0.97091064,  0.18232899,
		0.89788923, -0.21560822,  0.38380727,
		-0.41195419, -0.10414234,  0.90523374);
	// Its inverse.
	const mat3 irot = mat3(
		0.15520525,  0.89788923, -0.41195419,
    0.97091064, -0.21560822, -0.10414234,
    0.18232899,  0.38380727,  0.90523374);

  close1 = close2 = vec4(0, 0, 0, 1e10);
	Voronoi(rot * p0.xyz);
	close1.xyz = irot * close1.xyz;
	close2.xyz = irot * close2.xyz;
	Voronoi(p0.xyz);

	ftype r = Metaballs(p0.xyz, close1.xyz, close2.xyz);

	return max(length(p0.xyz) - sqrt(BOUNDS), r);
}
