/****************************************************************************
 *
 *   Copyright (c) 2013-2026 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#include "geofence_utils.h"
#include <lib/geo/geo.h>

namespace geofence_utils
{

bool insidePolygon(const matrix::Vector2<double> *vertices, int num_vertices,
		   const matrix::Vector2<double> &point)
{
	/**
	 * Adaptation of algorithm originally presented as
	 * PNPOLY - Point Inclusion in Polygon Test
	 * W. Randolph Franklin (WRF)
	 * Only supports non-complex polygons (not self intersecting)
	 */
	bool c = false;

	for (int i = 0, j = num_vertices - 1; i < num_vertices; j = i++) {
		if (((vertices[i](1) >= point(1)) != (vertices[j](1) >= point(1))) &&
		    (point(0) <= (vertices[j](0) - vertices[i](0)) * (point(1) - vertices[i](1)) /
		     (vertices[j](1) - vertices[i](1)) + vertices[i](0))) {
			c = !c;
		}
	}

	return c;
}
bool insidePolygon(const matrix::Vector2f *vertices, int num_vertices,
		   const matrix::Vector2f &point)
{
	/**
	 * Adaptation of algorithm originally presented as
	 * PNPOLY - Point Inclusion in Polygon Test
	 * W. Randolph Franklin (WRF)
	 * Only supports non-complex polygons (not self intersecting)
	 */
	bool c = false;

	for (int i = 0, j = num_vertices - 1; i < num_vertices; j = i++) {
		if (((vertices[i](1) >= point(1)) != (vertices[j](1) >= point(1))) &&
		    (point(0) <= (vertices[j](0) - vertices[i](0)) * (point(1) - vertices[i](1)) /
		     (vertices[j](1) - vertices[i](1)) + vertices[i](0))) {
			c = !c;
		}
	}

	return c;
}

bool insideCircle(const matrix::Vector2<double> &center, float radius,
		  const matrix::Vector2<double> &point)
{
	float dist = get_distance_to_next_waypoint(point(0), point(1), center(0), center(1));
	return dist < radius;
}

bool segmentsIntersect(const matrix::Vector2f &p1, const matrix::Vector2f &p2,
		       const matrix::Vector2f &v1, const matrix::Vector2f &v2)
{
	double d1x = p2(0) - p1(0), d1y = p2(1) - p1(1);
	double d2x = v2(0) - v1(0), d2y = v2(1) - v1(1);
	double denom = d1x * d2y - d1y * d2x;

	if (fabs(denom) < 1e-15) {
		return false;
	}

	double d3x = v1(0) - p1(0), d3y = v1(1) - p1(1);
	double t = (d3x * d2y - d3y * d2x) / denom;
	double u = (d3x * d1y - d3y * d1x) / denom;

	return t > 0.0 && t < 1.0 && u > 0.0 && u < 1.0;
}

bool expandOrShrinkPolygon(const matrix::Vector2f *vertices_in, int num_vertices,
			   float margin, bool expand,
			   matrix::Vector2f *vertices_out)
{
	for (int i = 0; i < num_vertices; i++) {
		int prev = (i + num_vertices - 1) % num_vertices;
		int next = (i + 1) % num_vertices;

		const matrix::Vector2f edge_prev = vertices_in[prev] - vertices_in[i];
		const matrix::Vector2f edge_next = vertices_in[next] - vertices_in[i];

		if (edge_prev.norm() < FLT_EPSILON || edge_next.norm() < FLT_EPSILON) {
			return false;
		}

		// Normalized directions from vertex to its neighbors
		const matrix::Vector2f to_prev = edge_prev.normalized();
		const matrix::Vector2f to_next = edge_next.normalized();

		// Bisector points inward (toward the polygon interior)
		matrix::Vector2f bisector = to_prev + to_next;

		if (bisector.length() < FLT_EPSILON) {
			// this happens when all three vertices are exactly on one line
			bisector = matrix::Vector2f(-to_prev(1), to_prev(0)); // rotate 90 degrees to get a perpendicular direction

		} else {
			bisector.normalize();
		}

		const matrix::Vector2f test_point = vertices_in[i] + bisector;
		const bool test_point_inside = geofence_utils::insidePolygon(vertices_in, num_vertices, test_point);

		float direction = 1.f;

		if (expand) {
			direction = (test_point_inside) ? -1.f : 1.f;

		} else {
			direction = (test_point_inside) ? 1.f : -1.f;
		}

		vertices_out[i] = vertices_in[i] + bisector * margin * direction;
	}

	return true;
}

} // namespace geofence_utils
