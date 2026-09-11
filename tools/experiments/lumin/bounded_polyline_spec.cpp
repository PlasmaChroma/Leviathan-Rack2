#include "BoundedPolyline.hpp"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <limits>

struct Point { float x, y; };
using Path = std::array<Point, 128>;
static void check(bool condition, const char* why) {
	if (!condition) { std::fprintf(stderr, "FAIL: %s\n", why); std::exit(1); }
}
static bool same(Point a, Point b) { return a.x == b.x && a.y == b.y; }
static double distance(Point p, Point a, Point b) {
	const double dx = double(b.x) - a.x, dy = double(b.y) - a.y;
	const double length = dx * dx + dy * dy;
	const double t = length > 0 ? std::max(0., std::min(1., ((p.x-a.x)*dx+(p.y-a.y)*dy)/length)) : 0.;
	return std::hypot(p.x - (a.x + t*dx), p.y - (a.y + t*dy));
}
static size_t verify(Path original, size_t count) {
	Path reduced = original;
	const size_t result = leviathan::render::reducePreviewPolyline(reduced, count);
	check(result >= 2 && result <= count, "bounded output count");
	check(same(original[0], reduced[0]) && same(original[count-1], reduced[result-1]), "exact endpoints");
	size_t crest = 0;
	for (size_t i=1; i<count; ++i) if (original[i].y < original[crest].y) crest=i;
	bool crestPresent = false;
	for (size_t i=0; i<result; ++i) crestPresent |= same(original[crest], reduced[i]);
	check(crestPresent, "exact crest");
	// Independent double-precision oracle checks the intervening input vertices
	// against each replacement segment, not merely the nearest output segment.
	size_t first = 0;
	for (size_t i=1; i<result; ++i) {
		size_t last=i+1==result ? count-1 : first+1;
		while (last<count && !same(original[last],reduced[i])) ++last;
		check(last<count, "retained vertices preserve source order");
		for (size_t j=first; j<=last; ++j)
			check(distance(original[j],reduced[i-1],reduced[i]) <= .02001, "centerline deviation bound");
		first=last;
	}
	return result;
}
int main() {
	Path path{};
	for (size_t i=0; i<path.size(); ++i) path[i]={float(i),5.f};
	check(verify(path,128)==2, "collinear path reduces to endpoints");
	for (int shape=0; shape<120; ++shape) {
		for (size_t i=0; i<path.size(); ++i) {
			const float x=float(i)/127.f;
			path[i]={106.f*x,24.f+(1.f+float(shape/12)*2.f)*std::sin(x*(1+shape%12)*3.14159265f)+.015f*std::cos(i*1.7f)};
		}
		verify(path,128);
	}
	for (size_t i=0; i<path.size(); ++i) path[i]={float(i%3),float(i%2)};
	verify(path,128); // Reversals must use segment distance, not infinite-line distance.
	for (auto& point:path) point={0,0};
	check(verify(path,128)==2, "duplicate points are safe");
	check(leviathan::render::reducePreviewPolyline(path,0)==0, "empty input");
	check(leviathan::render::reducePreviewPolyline(path,1)==1, "single vertex");
	check(leviathan::render::reducePreviewPolyline(path,129)==129, "oversized input is untouched");
	path[10].x=std::numeric_limits<float>::infinity();
	check(leviathan::render::reducePreviewPolyline(path,128)==128, "invalid coordinate rejected");
	std::puts("PASS: bounded reduction, endpoints/crest, reversal/degenerate cases, and independent error oracle");
}
