/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file GaussianNoise.hpp
 *
 * Standard normal sample generators for simulation use.
 *
 * Two algorithms are provided, both drawing from N(0, 1):
 *   - generate_wgn():           Marsaglia polar method (preferred default).
 *   - generate_wgn_boxmuller(): basic Box-Muller transform.
 *
 * Callers are responsible for seeding the process-global rand() via
 * srand() if deterministic sequences are required.
 */

#pragma once

#include <math.h>
#include <stdlib.h>

#include <matrix/math.hpp>

namespace math
{

/**
 * Draw a sample from the standard normal distribution N(0, 1) using the
 * Marsaglia polar method (variant of Box-Muller).
 *
 * Prefer this function by default. It avoids trig (cos/sin) at the cost of a
 * rejection loop (acceptance probability ~ pi/4) and amortises two rand()
 * calls across two samples by caching the second half of each iteration.
 *
 * Not thread-safe: uses shared static state (phase / V1 / V2 / S) across all
 * callers and translation units. Safe for the single-threaded / serialised
 * work-queue usage in PX4 simulation modules.
 *
 * https://en.wikipedia.org/wiki/Marsaglia_polar_method
 */
inline float generate_wgn()
{
	static float V1, V2, S;
	static bool phase = true;
	float X;

	if (phase) {
		do {
			float U1 = (float)rand() / (float)RAND_MAX;
			float U2 = (float)rand() / (float)RAND_MAX;
			V1 = 2.0f * U1 - 1.0f;
			V2 = 2.0f * U2 - 1.0f;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1.0f || fabsf(S) < 1e-8f);

		X = V1 * float(sqrtf(-2.0f * float(logf(S)) / S));

	} else {
		X = V2 * float(sqrtf(-2.0f * float(logf(S)) / S));
	}

	phase = !phase;
	return X;
}

/**
 * Draw a sample from the standard normal distribution N(0, 1) using the
 * basic Box-Muller transform.
 *
 * Use this instead of generate_wgn() when any of the following matters:
 *   - Statelessness / thread-safety: no static state is kept between calls,
 *     so concurrent callers cannot corrupt each other.
 *   - Deterministic timing: exactly two rand() calls and one cos/log/sqrt
 *     per sample, no rejection loop -- constant work per invocation.
 *   - Simpler reasoning about reproducibility: the sample depends only on
 *     the next two rand() outputs, with no hidden phase/cache carried from
 *     previous calls.
 *
 * Downside: performs cosf() on every call and always issues two rand() calls
 * per sample (vs ~1.27 amortised for the polar method). On modern hardware
 * this is typically not meaningful; on platforms with an expensive libm
 * cosf() or a lock-protected rand() it can be.
 *
 * https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
 */
inline float generate_wgn_boxmuller()
{
	// rand()+1 / RAND_MAX+1 keeps U1 in (0, 1] so that logf(U1) is finite.
	// Replacing cosf by sinf gives a second independent sample, which we
	// discard for simplicity.
	const float U1 = (float)(rand() + 1) / ((float)RAND_MAX + 1.0f);
	const float U2 = (float)rand() / (float)RAND_MAX;
	return sqrtf(-2.0f * logf(U1)) * cosf(2.0f * (float)M_PI * U2);
}

/**
 * Draw a 3D vector of independent standard normal samples, each scaled by
 * the given per-axis standard deviation. Uses generate_wgn() internally.
 */
inline matrix::Vector3f noiseGauss3f(float stdx, float stdy, float stdz)
{
	return matrix::Vector3f(generate_wgn() * stdx,
				generate_wgn() * stdy,
				generate_wgn() * stdz);
}

} // namespace math
