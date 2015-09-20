#version 410

#define BOUNDS 100.
#define TRAPS

#include <Raytracers/DE-Raytracer.frag>


uniform float Radius = 0.02;


tvec3 Point(tvec3 p0) {
	vec3 p = vec3(p0);
	// Random matrix.
	const mat3 m = mat3(
		0.8086302229028699, 0.5227741986417075, 0.6827060411123068,
		0.5276513809626875, 0.3442914207412343, 0.37211591216017514,
		0.42050358589267633, 0.898297255815332, 0.781488075294931);

	// Random jerky movement.
	//return p + 1/3. + (1/ 3.) * fract(sin(m * p + Now * 1e-3) * 100);

	// Smooth waves.
	//return p + 1/3. + (1/ 6.) * (sin(m * p + Now * 5) + 1);

	// No movement.
	return p + 1/3. + (1/ 3.) * fract(sin(m * p) * 100);
}

tvec4 close1;
tvec4 close2;
tvec4 close3;

void Closest(tvec3 p, tvec3 g) {
	tvec3 t = p - g;
	ftype d = dot(t, t);
	if (d < close1.w) {
		close3 = close2;
		close2 = close1;
		close1 = tvec4(g, d);
	} else if (d < close2.w) {
		close3 = close2;
		close2 = tvec4(g, d);
	} else if (d < close3.w) {
		close3 = tvec4(g, d);
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

void GetClosest(tvec3 p) {
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

  close1 = close2 = close3 = vec4(0, 0, 0, 1e10);
	Voronoi(rot * p.xyz);
	close1.xyz = irot * close1.xyz;
	close2.xyz = irot * close2.xyz;
	close3.xyz = irot * close3.xyz;
	Voronoi(p.xyz);
}

ftype DE(tvec4 p0) {
	if (abs(p0.z) > 2)
		return p0.z - sign(p0.z);

	GetClosest(p0.xyz);

	float s = sign(float(close1.z));
	bool same = s == sign(close2.z);
	ftype r;
	if (!same && abs(close1.w - close2.w) < 1e-5)
		r = - dot(normalize(close2.xyz - close1.xyz), (p0.xyz - (close1.xyz + close2.xyz)*.5)) * s;
	else
		r = s * (sqrt(same ? close3.w : close2.w) - sqrt(close1.w));

	r = min(r, sqrt(close1.w) - Radius);

	return max(length(p0.xyz) - sqrt(BOUNDS), r);
}

vec4 Traps(tvec4 p) {
	//return p.z > 0 ? vec4(0, 0, 0, 1) : vec4(0, 0, 1, 0);
	GetClosest(p.xyz);
	if (sqrt(close1.w) - Radius > p.w)
		return vec4(0, 0, 0, 1);
	return close1.z > 0 ? vec4(0, 0, 1.5, 0) : vec4(1, 0, 0, 0);
}
