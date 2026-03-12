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
 * @file DisarmChecks.cpp
 */

#include "DisarmChecks.hpp"
#include "commander_helper.h"

#include <px4_platform_common/log.h>
#include <px4_platform_common/events.h>

void DisarmChecks::start()
{
	_pending = true;
	_deadline = hrt_absolute_time() + TIMEOUT;
}

void DisarmChecks::update(orb_advert_t *mavlink_log_pub)
{
	if (!_pending) {
		return;
	}

	if (hrt_absolute_time() >= _deadline) {
		_pending = false;

		// Run all verification checks at timeout
		if (!checkEscDisarmed()) {
			if (mavlink_log_pub) {
				mavlink_log_critical(mavlink_log_pub, "ESC disarm failure: ESCs still armed\t");
			}

			events::send(events::ID("commander_esc_disarm_failure"),
			{events::Log::Error, events::LogInternal::Warning},
			"ESC disarm failure: one or more ESCs failed to disarm");

			tune_negative(true);
		}

		// Future checks:
		// if (!checkParachuteDisarmed()) { ... }
	}
}

bool DisarmChecks::checkEscDisarmed()
{
	esc_status_s esc_status;

	if (_esc_status_sub.copy(&esc_status)) {
		return esc_status.esc_armed_flags == 0;
	}

	// No ESC status data available - assume OK
	return true;
}
