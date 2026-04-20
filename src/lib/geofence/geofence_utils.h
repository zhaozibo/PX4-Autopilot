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

/**
 * @file geofence_utils.h
 * Pure geometry functions for geofence point-in-polygon and point-in-circle tests.
 */

#pragma once

#include <matrix/math.hpp>

namespace geofence_utils
{

/**
 * Check if a point is inside a circle.
 *
 * @param center  circle center (lat, lon) in degrees
 * @param radius  circle radius in meters
 * @param point   test point (lat, lon) in degrees
 * @return true if the point is inside the circle
 */
bool insideCircle(const matrix::Vector2<double> &center, float radius,
		  const matrix::Vector2<double> &point);

/**
 * Check if two line segments intersect (excluding endpoints).
 * Works in local Cartesian coordinates (meters).
 *
 * @param p1  first segment start in local frame
 * @param p2  first segment end in local frame
 * @param v1  second segment start in local frame
 * @param v2  second segment end in local frame
 * @return true if the segments intersect
 */
bool segmentsIntersect(const matrix::Vector2f &p1, const matrix::Vector2f &p2,
		       const matrix::Vector2f &v1, const matrix::Vector2f &v2);

/**
 * Offset polygon vertices expand or inward by computing the bisector
 * of the two normalized edge directions at each vertex.
 * Works in local frame (meters).
 *
 * @param vertices_in   input vertices in local frame
 * @param num_vertices  number of vertices
 * @param margin        offset distance in meters
 * @param expand       true to expand, false to shrink
 * @param vertices_out  output array (must hold at least num_vertices elements)
 */
bool expandOrShrinkPolygon(const matrix::Vector2f *vertices_in, int num_vertices,
			   float margin, bool expand,
			   matrix::Vector2f *vertices_out);

} // namespace geofence_utils
