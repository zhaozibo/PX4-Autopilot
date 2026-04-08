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

#include <gtest/gtest.h>

#include <drivers/drv_hrt.h>
#include <lib/geo/geo.h>
#include <parameters/param.h>
#include <uORB/topics/home_position.h>
#include <uORB/topics/wind.h>

#include "navigator.h"
#include "rtl.h"

class RTLTestPeer : public RTL
{
public:
	explicit RTLTestPeer(Navigator *navigator) : RTL(navigator) {}

	loiter_point_s chooseBestLandingApproachForTest(const land_approaches_s &vtol_land_approaches)
	{
		_home_pos_sub.update();
		_wind_sub.update();
		return chooseBestLandingApproach(vtol_land_approaches);
	}
};

class RTLTest : public ::testing::Test
{
protected:
	static constexpr double kBaseLat = 47.3977419;
	static constexpr double kBaseLon = 8.5455938;
	static constexpr float kBaseAlt = 488.f;

	void SetUp() override
	{
		param_control_autosave(false);
		param_reset_all();
	}

	void publishHomePosition(const PositionYawSetpoint &position)
	{
		home_position_s home{};
		home.timestamp = hrt_absolute_time();
		home.lat = position.lat;
		home.lon = position.lon;
		home.alt = position.alt;
		home.valid_hpos = true;
		home.valid_alt = true;
		orb_advert_t home_pub = orb_advertise(ORB_ID(home_position), &home);
		orb_publish(ORB_ID(home_position), home_pub, &home);
	}

	void publishWind(float windspeed_north, float windspeed_east)
	{
		wind_s wind{};
		wind.timestamp = hrt_absolute_time();
		wind.windspeed_north = windspeed_north;
		wind.windspeed_east = windspeed_east;
		orb_advert_t wind_pub = orb_advertise(ORB_ID(wind), &wind);
		orb_publish(ORB_ID(wind), wind_pub, &wind);
	}

	static PositionYawSetpoint makePositionFromOffset(double base_lat, double base_lon, float north_m, float east_m, float alt)
	{
		MapProjection ref{base_lat, base_lon};
		PositionYawSetpoint position{};
		ref.reproject(north_m, east_m, position.lat, position.lon);
		position.alt = alt;
		position.yaw = NAN;
		return position;
	}
};

// WHY: Wind-based safe-point approach selection must evaluate the loiter-circle bearing from the
//      safe-point land location. Otherwise a remote safe point can choose the wrong approach.
// WHAT: With home at the origin, a safe point at N+100/E+100, and wind at 60 deg, the correct
//       land-relative selection is the north approach.
TEST_F(RTLTest, ChooseBestLandingApproachUsesLandLocationAsBearingOrigin)
{
	Navigator navigator;
	RTLTestPeer rtl(&navigator);

	// GIVEN: Home at the reference origin, one safe-point land location at N+100/E+100, and
	//        two valid approach loiter circles on the north and south side of that land point.
	publishHomePosition(makePositionFromOffset(kBaseLat, kBaseLon, 0.f, 0.f, kBaseAlt));
	publishWind(1.f, 1.7320508f);

	const PositionYawSetpoint land_position = makePositionFromOffset(kBaseLat, kBaseLon, 100.f, 100.f, kBaseAlt);
	const PositionYawSetpoint north_approach_position = makePositionFromOffset(kBaseLat, kBaseLon, 150.f, 100.f,
			kBaseAlt + 20.f);
	const PositionYawSetpoint south_approach_position = makePositionFromOffset(kBaseLat, kBaseLon, 50.f, 100.f,
			kBaseAlt + 20.f);

	land_approaches_s vtol_land_approaches{};
	vtol_land_approaches.land_location_lat_lon(0) = land_position.lat;
	vtol_land_approaches.land_location_lat_lon(1) = land_position.lon;
	vtol_land_approaches.approaches[0].lat = north_approach_position.lat;
	vtol_land_approaches.approaches[0].lon = north_approach_position.lon;
	vtol_land_approaches.approaches[0].height_m = north_approach_position.alt;
	vtol_land_approaches.approaches[0].loiter_radius_m = 50.f;
	vtol_land_approaches.approaches[1].lat = south_approach_position.lat;
	vtol_land_approaches.approaches[1].lon = south_approach_position.lon;
	vtol_land_approaches.approaches[1].height_m = south_approach_position.alt;
	vtol_land_approaches.approaches[1].loiter_radius_m = 50.f;

	// WHEN: chooseBestLandingApproach evaluates wind alignment for the approach block.
	const loiter_point_s selected_approach = rtl.chooseBestLandingApproachForTest(vtol_land_approaches);

	// THEN: The north approach is selected because the bearing origin is the land location,
	//       not the distant home position.
	ASSERT_TRUE(selected_approach.isValid());
	EXPECT_NEAR(selected_approach.lat, north_approach_position.lat, 1e-9);
	EXPECT_NEAR(selected_approach.lon, north_approach_position.lon, 1e-9);
	EXPECT_NEAR(selected_approach.height_m, north_approach_position.alt, 0.01f);
}
