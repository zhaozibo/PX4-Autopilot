/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#pragma once

#include <parameters/param.h>
#include <uORB/Subscription.hpp>
#include <uORB/Publication.hpp>
#include <uORB/topics/vehicle_command.h>
#include <uORB/topics/vehicle_command_ack.h>

/**
 * GPS failure injector for SITL simulation.
 *
 * Listens for VEHICLE_CMD_INJECT_FAILURE commands targeting FAILURE_UNIT_SENSOR_GPS
 * and maintains per-instance state bitmasks that SensorGpsSim consults before
 * each publish to suppress or corrupt GPS data.
 *
 * Mirrors the design of FailureInjector (motor failures) in commander.
 * Requires SYS_FAILURE_EN = 1.
 *
 * Instance mapping (param3):
 *   0 = all instances
 *   1 = GPS instance 0 (primary)
 *   2 = GPS instance 1 (secondary)
 */
class SensorGpsFailureInjector
{
public:
	static constexpr int GPS_MAX_INSTANCES = 2;

	SensorGpsFailureInjector();

	/**
	 * Poll vehicle_command and update failure state bitmasks.
	 * Must be called once per SensorGpsSim::Run() cycle.
	 */
	void update();

	/** Returns true if the given GPS instance should stop publishing (FAILURE_TYPE_OFF). */
	bool isBlocked(int instance) const { return (_gps_blocked_mask >> instance) & 1u; }

	/** Returns true if the given GPS instance should freeze on its last position (FAILURE_TYPE_STUCK). */
	bool isStuck(int instance) const { return (_gps_stuck_mask >> instance) & 1u; }

	/** Returns true if the given GPS instance should publish corrupted position data (FAILURE_TYPE_WRONG). */
	bool isWrong(int instance) const { return (_gps_wrong_mask >> instance) & 1u; }

private:
	uORB::Subscription _vehicle_command_sub{ORB_ID(vehicle_command)};
	uORB::Publication<vehicle_command_ack_s> _command_ack_pub{ORB_ID(vehicle_command_ack)};

	// Cached handle from param_find() — avoids repeated string searches.
	// The actual value is re-read in update() so runtime changes take effect.
	param_t _param_sys_failure_en{PARAM_INVALID};

	uint8_t _gps_blocked_mask{0}; ///< FAILURE_TYPE_OFF  — stop publishing
	uint8_t _gps_stuck_mask{0};   ///< FAILURE_TYPE_STUCK — freeze last position
	uint8_t _gps_wrong_mask{0};   ///< FAILURE_TYPE_WRONG — corrupt position
};
