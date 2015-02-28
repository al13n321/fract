#version 410

#define BOUNDS 100.
#define TRAPS

#include <Raytracers/DE-Raytracer.frag>

// Procedurally generated set of points in space that appears somewhat random
// and allows finding the closest point in O(1).


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

tvec4 Closest(tvec4 a, tvec3 p, tvec3 g) {
	tvec3 t = p - g;
	ftype d = dot(t, t);
	return d < a.w ? tvec4(g, d) : a;
}

tvec4 Voronoi(tvec3 p) {
	tvec3 g = floor(p);
	tvec4 s = tvec4(sign(p - g - .5), 0);
	tvec4 r = tvec4(0, 0, 0, 1e10);

	r = Closest(r, p, Point(g));
	r = Closest(r, p, Point(g + s.wwz));
	r = Closest(r, p, Point(g + s.wyw));
	r = Closest(r, p, Point(g + s.xww));

	// The following lines are needed for correctness but without them performance
	// is significantly better, and artifacts are rarely noticeable.
	// Note that most artifacts with cubes are not caused by this.
	//r = Closest(r, p, Point(g + s.wyz));
	//r = Closest(r, p, Point(g + s.xwz));
	//r = Closest(r, p, Point(g + s.xyw));
	//r = Closest(r, p, Point(g + s.xyz));

	return r;
}

ftype Frame(tvec3 p) {
	vec3 t = abs(fract(p + .5) - .5);

	// Wireframe grid.
	return min(min(max(t.x,t.y), max(t.x,t.z)), max(t.y, t.z)) - 0.01;

	// Wafle.
	//return min(min(t.x, t.y), t.z);
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

	tvec4 a = Voronoi(p0.xyz);
	tvec4 b = Voronoi(rot * p0.xyz);
	if (b.w < a.w) {
		a = tvec4(irot * b.xyz, b.w);
	}
	tvec3 p = p0.xyz - a.xyz;

	// Cubes.
	ftype r = length(p - clamp(p, -Radius, Radius));

	// Spheres.
	//ftype r = sqrt(a.w) - Radius;

	// With grid.
	//return max(length(p0.xyz) - sqrt(BOUNDS), min(r, Frame(p0.xyz)));

	// Without grid.
	return max(length(p0.xyz) - sqrt(BOUNDS), r);
}

vec4 Traps(tvec4 p) {
	return vec4(0);

	// Use different color for grid.
	//return Frame(p.xyz) < 1e-2 ? vec4(0, 0, 1.5, 0) : vec4(1.5, 0, 0, 0);
}
