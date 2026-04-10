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

#include "SensorGpsFailureInjector.hpp"

#include <drivers/drv_hrt.h>

SensorGpsFailureInjector::SensorGpsFailureInjector()
{
	// Cache the handle once — param_find() is a string search and is expensive.
	// The value itself is re-read each update() cycle so runtime changes take effect.
	_param_sys_failure_en = param_find("SYS_FAILURE_EN");
}

void SensorGpsFailureInjector::update()
{
	// Re-read the enable gate every cycle so toggling SYS_FAILURE_EN at runtime
	// takes effect immediately without restarting the module.
	int32_t sys_failure_en = 0;
	const bool enabled = (_param_sys_failure_en != PARAM_INVALID)
			     && (param_get(_param_sys_failure_en, &sys_failure_en) == PX4_OK)
			     && (sys_failure_en == 1);

	if (!enabled) { return; }

	vehicle_command_s vehicle_command;

	while (_vehicle_command_sub.update(&vehicle_command)) {
		const int failure_unit = static_cast<int>(lroundf(vehicle_command.param1));
		const int failure_type = static_cast<int>(lroundf(vehicle_command.param2));

		if (vehicle_command.command != vehicle_command_s::VEHICLE_CMD_INJECT_FAILURE
		    || failure_unit != vehicle_command_s::FAILURE_UNIT_SENSOR_GPS) {
			continue;
		}

		// param3: 0 = all instances, 1 = GPS0, 2 = GPS1
		const int requested_instance = static_cast<int>(lroundf(vehicle_command.param3));

		// Build a bitmask of which instances to affect directly, without a loop.
		// requested_instance=0 → all → 0b11, otherwise select the single bit.
		const uint8_t target_mask = (requested_instance == 0)
					    ? static_cast<uint8_t>((1u << GPS_MAX_INSTANCES) - 1u)
					    : static_cast<uint8_t>(1u << (requested_instance - 1));

		bool supported = false;

		switch (failure_type) {
		case vehicle_command_s::FAILURE_TYPE_OK:
			supported = true;
			PX4_INFO("CMD_INJECT_FAILURE, GPS ok (mask=0x%x)", target_mask);
			_gps_blocked_mask &= ~target_mask;
			_gps_stuck_mask   &= ~target_mask;
			_gps_wrong_mask   &= ~target_mask;
			break;

		case vehicle_command_s::FAILURE_TYPE_OFF:
			supported = true;
			PX4_INFO("CMD_INJECT_FAILURE, GPS off (mask=0x%x)", target_mask);
			_gps_blocked_mask |= target_mask;
			break;

		case vehicle_command_s::FAILURE_TYPE_STUCK:
			supported = true;
			PX4_INFO("CMD_INJECT_FAILURE, GPS stuck (mask=0x%x)", target_mask);
			_gps_stuck_mask |= target_mask;
			break;

		case vehicle_command_s::FAILURE_TYPE_WRONG:
			supported = true;
			PX4_INFO("CMD_INJECT_FAILURE, GPS wrong (mask=0x%x)", target_mask);
			_gps_wrong_mask |= target_mask;
			break;

		default:
			break;
		}

		vehicle_command_ack_s ack{};
		ack.command = vehicle_command.command;
		ack.from_external = false;
		ack.result = supported ?
			     vehicle_command_ack_s::VEHICLE_CMD_RESULT_ACCEPTED :
			     vehicle_command_ack_s::VEHICLE_CMD_RESULT_UNSUPPORTED;
		ack.timestamp = hrt_absolute_time();
		_command_ack_pub.publish(ack);
	}
}
