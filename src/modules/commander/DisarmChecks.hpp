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
 * @file DisarmChecks.hpp
 *
 * Helper class to verify that components transition to disarmed state
 * after the flight controller disarms.
 */

#pragma once

#include <px4_platform_common/defines.h>
#include <px4_platform_common/time.h>
#include <uORB/Subscription.hpp>
#include <uORB/topics/esc_status.h>

using namespace time_literals;

class DisarmChecks
{
public:
	DisarmChecks() = default;
	~DisarmChecks() = default;

	/**
	 * Start the disarm verification process.
	 * Call this immediately after disarming.
	 */
	void start();

	/**
	 * Update the verification state and report failures.
	 * Call this periodically from the main loop.
	 * @param mavlink_log_pub mavlink log publisher for console messages
	 */
	void update(orb_advert_t *mavlink_log_pub);

	/**
	 * Check if verification is still pending.
	 */
	bool isPending() const { return _pending; }

private:
	static constexpr hrt_abstime TIMEOUT{1_s};

	/**
	 * Check if all ESCs have disarmed.
	 * @return true if all ESCs are disarmed (or no data available)
	 */
	bool checkEscDisarmed();

	// Future checks can be added here:
	// bool checkParachuteDisarmed();

	bool _pending{false};
	hrt_abstime _deadline{0};

	uORB::Subscription _esc_status_sub{ORB_ID(esc_status)};
};
