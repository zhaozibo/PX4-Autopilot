# dds_topics.yaml — PX4 Topics Exposed to ROS 2

::: info
This document is [auto-generated](https://github.com/PX4/PX4-Autopilot/blob/main/Tools/msg/generate_msg_docs.py) from the source code.
:::

The [dds_topics.yaml](https://github.com/PX4/PX4-Autopilot/blob/main/src/modules/uxrce_dds_client/dds_topics.yaml) file specifies which uORB message definitions are compiled into the [uxrce_dds_client](../modules/modules_system.md#uxrce-dds-client) module when [PX4 is built](../middleware/uxrce_dds.md#code-generation), and hence which topics are available for ROS 2 applications to subscribe or publish (by default).

This document shows a markdown-rendered version of [dds_topics.yaml](https://github.com/PX4/PX4-Autopilot/blob/main/src/modules/uxrce_dds_client/dds_topics.yaml), listing the publications, subscriptions, and so on.

## Publications

| Topic                                    | Type                                                                                   | Rate Limit |
| ---------------------------------------- | -------------------------------------------------------------------------------------- | ---------- |
| `/fmu/out/register_ext_component_reply`  | [px4_msgs::msg::RegisterExtComponentReply](../msg_docs/RegisterExtComponentReply.md)   |
| `/fmu/out/arming_check_request`          | [px4_msgs::msg::ArmingCheckRequest](../msg_docs/ArmingCheckRequest.md)                 | 5.0        |
| `/fmu/out/mode_completed`                | [px4_msgs::msg::ModeCompleted](../msg_docs/ModeCompleted.md)                           | 50.0       |
| `/fmu/out/battery_status`                | [px4_msgs::msg::BatteryStatus](../msg_docs/BatteryStatus.md)                           | 1.0        |
| `/fmu/out/collision_constraints`         | [px4_msgs::msg::CollisionConstraints](../msg_docs/CollisionConstraints.md)             | 50.0       |
| `/fmu/out/estimator_status_flags`        | [px4_msgs::msg::EstimatorStatusFlags](../msg_docs/EstimatorStatusFlags.md)             | 5.0        |
| `/fmu/out/failsafe_flags`                | [px4_msgs::msg::FailsafeFlags](../msg_docs/FailsafeFlags.md)                           | 5.0        |
| `/fmu/out/manual_control_setpoint`       | [px4_msgs::msg::ManualControlSetpoint](../msg_docs/ManualControlSetpoint.md)           | 25.0       |
| `/fmu/out/message_format_response`       | [px4_msgs::msg::MessageFormatResponse](../msg_docs/MessageFormatResponse.md)           |
| `/fmu/out/position_setpoint_triplet`     | [px4_msgs::msg::PositionSetpointTriplet](../msg_docs/PositionSetpointTriplet.md)       | 5.0        |
| `/fmu/out/sensor_combined`               | [px4_msgs::msg::SensorCombined](../msg_docs/SensorCombined.md)                         |
| `/fmu/out/timesync_status`               | [px4_msgs::msg::TimesyncStatus](../msg_docs/TimesyncStatus.md)                         | 10.0       |
| `/fmu/out/transponder_report`            | [px4_msgs::msg::TransponderReport](../msg_docs/TransponderReport.md)                   |
| `/fmu/out/vehicle_land_detected`         | [px4_msgs::msg::VehicleLandDetected](../msg_docs/VehicleLandDetected.md)               | 5.0        |
| `/fmu/out/vehicle_attitude`              | [px4_msgs::msg::VehicleAttitude](../msg_docs/VehicleAttitude.md)                       | 50.0       |
| `/fmu/out/vehicle_control_mode`          | [px4_msgs::msg::VehicleControlMode](../msg_docs/VehicleControlMode.md)                 | 50.0       |
| `/fmu/out/vehicle_command_ack`           | [px4_msgs::msg::VehicleCommandAck](../msg_docs/VehicleCommandAck.md)                   |
| `/fmu/out/vehicle_global_position`       | [px4_msgs::msg::VehicleGlobalPosition](../msg_docs/VehicleGlobalPosition.md)           | 50.0       |
| `/fmu/out/vehicle_gps_position`          | [px4_msgs::msg::SensorGps](../msg_docs/SensorGps.md)                                   | 50.0       |
| `/fmu/out/vehicle_local_position`        | [px4_msgs::msg::VehicleLocalPosition](../msg_docs/VehicleLocalPosition.md)             | 50.0       |
| `/fmu/out/vehicle_odometry`              | [px4_msgs::msg::VehicleOdometry](../msg_docs/VehicleOdometry.md)                       | 100.0      |
| `/fmu/out/vehicle_status`                | [px4_msgs::msg::VehicleStatus](../msg_docs/VehicleStatus.md)                           | 5.0        |
| `/fmu/out/airspeed_validated`            | [px4_msgs::msg::AirspeedValidated](../msg_docs/AirspeedValidated.md)                   | 50.0       |
| `/fmu/out/vtol_vehicle_status`           | [px4_msgs::msg::VtolVehicleStatus](../msg_docs/VtolVehicleStatus.md)                   |
| `/fmu/out/home_position`                 | [px4_msgs::msg::HomePosition](../msg_docs/HomePosition.md)                             | 5.0        |
| `/fmu/out/wind`                          | [px4_msgs::msg::Wind](../msg_docs/Wind.md)                                             | 1.0        |
| `/fmu/out/gimbal_device_attitude_status` | [px4_msgs::msg::GimbalDeviceAttitudeStatus](../msg_docs/GimbalDeviceAttitudeStatus.md) | 20.0       |

## Subscriptions

| Topic                                      | Type                                                                                               |
| ------------------------------------------ | -------------------------------------------------------------------------------------------------- |
| /fmu/in/register_ext_component_request     | [px4_msgs::msg::RegisterExtComponentRequest](../msg_docs/RegisterExtComponentRequest.md)           |
| /fmu/in/unregister_ext_component           | [px4_msgs::msg::UnregisterExtComponent](../msg_docs/UnregisterExtComponent.md)                     |
| /fmu/in/config_overrides_request           | [px4_msgs::msg::ConfigOverrides](../msg_docs/ConfigOverrides.md)                                   |
| /fmu/in/arming_check_reply                 | [px4_msgs::msg::ArmingCheckReply](../msg_docs/ArmingCheckReply.md)                                 |
| /fmu/in/message_format_request             | [px4_msgs::msg::MessageFormatRequest](../msg_docs/MessageFormatRequest.md)                         |
| /fmu/in/mode_completed                     | [px4_msgs::msg::ModeCompleted](../msg_docs/ModeCompleted.md)                                       |
| /fmu/in/config_control_setpoints           | [px4_msgs::msg::VehicleControlMode](../msg_docs/VehicleControlMode.md)                             |
| /fmu/in/distance_sensor                    | [px4_msgs::msg::DistanceSensor](../msg_docs/DistanceSensor.md)                                     |
| /fmu/in/manual_control_input               | [px4_msgs::msg::ManualControlSetpoint](../msg_docs/ManualControlSetpoint.md)                       |
| /fmu/in/offboard_control_mode              | [px4_msgs::msg::OffboardControlMode](../msg_docs/OffboardControlMode.md)                           |
| /fmu/in/onboard_computer_status            | [px4_msgs::msg::OnboardComputerStatus](../msg_docs/OnboardComputerStatus.md)                       |
| /fmu/in/obstacle_distance                  | [px4_msgs::msg::ObstacleDistance](../msg_docs/ObstacleDistance.md)                                 |
| /fmu/in/sensor_optical_flow                | [px4_msgs::msg::SensorOpticalFlow](../msg_docs/SensorOpticalFlow.md)                               |
| /fmu/in/goto_setpoint                      | [px4_msgs::msg::GotoSetpoint](../msg_docs/GotoSetpoint.md)                                         |
| /fmu/in/telemetry_status                   | [px4_msgs::msg::TelemetryStatus](../msg_docs/TelemetryStatus.md)                                   |
| /fmu/in/trajectory_setpoint                | [px4_msgs::msg::TrajectorySetpoint](../msg_docs/TrajectorySetpoint.md)                             |
| /fmu/in/vehicle_attitude_setpoint          | [px4_msgs::msg::VehicleAttitudeSetpoint](../msg_docs/VehicleAttitudeSetpoint.md)                   |
| /fmu/in/vehicle_mocap_odometry             | [px4_msgs::msg::VehicleOdometry](../msg_docs/VehicleOdometry.md)                                   |
| /fmu/in/vehicle_rates_setpoint             | [px4_msgs::msg::VehicleRatesSetpoint](../msg_docs/VehicleRatesSetpoint.md)                         |
| /fmu/in/vehicle_visual_odometry            | [px4_msgs::msg::VehicleOdometry](../msg_docs/VehicleOdometry.md)                                   |
| /fmu/in/vehicle_command                    | [px4_msgs::msg::VehicleCommand](../msg_docs/VehicleCommand.md)                                     |
| /fmu/in/vehicle_command_mode_executor      | [px4_msgs::msg::VehicleCommand](../msg_docs/VehicleCommand.md)                                     |
| /fmu/in/vehicle_thrust_setpoint            | [px4_msgs::msg::VehicleThrustSetpoint](../msg_docs/VehicleThrustSetpoint.md)                       |
| /fmu/in/vehicle_torque_setpoint            | [px4_msgs::msg::VehicleTorqueSetpoint](../msg_docs/VehicleTorqueSetpoint.md)                       |
| /fmu/in/actuator_motors                    | [px4_msgs::msg::ActuatorMotors](../msg_docs/ActuatorMotors.md)                                     |
| /fmu/in/actuator_servos                    | [px4_msgs::msg::ActuatorServos](../msg_docs/ActuatorServos.md)                                     |
| /fmu/in/fixed_wing_longitudinal_setpoint   | [px4_msgs::msg::FixedWingLongitudinalSetpoint](../msg_docs/FixedWingLongitudinalSetpoint.md)       |
| /fmu/in/fixed_wing_lateral_setpoint        | [px4_msgs::msg::FixedWingLateralSetpoint](../msg_docs/FixedWingLateralSetpoint.md)                 |
| /fmu/in/longitudinal_control_configuration | [px4_msgs::msg::LongitudinalControlConfiguration](../msg_docs/LongitudinalControlConfiguration.md) |
| /fmu/in/lateral_control_configuration      | [px4_msgs::msg::LateralControlConfiguration](../msg_docs/LateralControlConfiguration.md)           |
| /fmu/in/rover_position_setpoint            | [px4_msgs::msg::RoverPositionSetpoint](../msg_docs/RoverPositionSetpoint.md)                       |
| /fmu/in/rover_speed_setpoint               | [px4_msgs::msg::RoverSpeedSetpoint](../msg_docs/RoverSpeedSetpoint.md)                             |
| /fmu/in/rover_attitude_setpoint            | [px4_msgs::msg::RoverAttitudeSetpoint](../msg_docs/RoverAttitudeSetpoint.md)                       |
| /fmu/in/rover_rate_setpoint                | [px4_msgs::msg::RoverRateSetpoint](../msg_docs/RoverRateSetpoint.md)                               |
| /fmu/in/rover_throttle_setpoint            | [px4_msgs::msg::RoverThrottleSetpoint](../msg_docs/RoverThrottleSetpoint.md)                       |
| /fmu/in/rover_steering_setpoint            | [px4_msgs::msg::RoverSteeringSetpoint](../msg_docs/RoverSteeringSetpoint.md)                       |
| /fmu/in/landing_gear                       | [px4_msgs::msg::LandingGear](../msg_docs/LandingGear.md)                                           |

## Subscriptions Multi

| Topic                       | Type                                                                 | Route Field | Max Instances |
| --------------------------- | -------------------------------------------------------------------- | ----------- | ------------- |
| /fmu/in/aux_global_position | [px4_msgs::msg::AuxGlobalPosition](../msg_docs/AuxGlobalPosition.md) | `id`        | 4             |

## Not Exported

These messages are not listed in the yaml file.
They are not build into the module, and hence are neither published or subscribed.

::: details See messages

- [CameraCapture](../msg_docs/CameraCapture.md)
- [RaptorStatus](../msg_docs/RaptorStatus.md)
- [PpsCapture](../msg_docs/PpsCapture.md)
- [TiltrotorExtraControls](../msg_docs/TiltrotorExtraControls.md)
- [EstimatorBias3d](../msg_docs/EstimatorBias3d.md)
- [MissionResult](../msg_docs/MissionResult.md)
- [EscStatus](../msg_docs/EscStatus.md)
- [QshellRetval](../msg_docs/QshellRetval.md)
- [LaunchDetectionStatus](../msg_docs/LaunchDetectionStatus.md)
- [SensorsStatusImu](../msg_docs/SensorsStatusImu.md)
- [CameraTrigger](../msg_docs/CameraTrigger.md)
- [FigureEightStatus](../msg_docs/FigureEightStatus.md)
- [ParameterSetValueResponse](../msg_docs/ParameterSetValueResponse.md)
- [VehicleMagnetometer](../msg_docs/VehicleMagnetometer.md)
- [OrbTestLarge](../msg_docs/OrbTestLarge.md)
- [ActuatorOutputs](../msg_docs/ActuatorOutputs.md)
- [RtlStatus](../msg_docs/RtlStatus.md)
- [ParameterSetUsedRequest](../msg_docs/ParameterSetUsedRequest.md)
- [GeofenceResult](../msg_docs/GeofenceResult.md)
- [PurePursuitStatus](../msg_docs/PurePursuitStatus.md)
- [NavigatorMissionItem](../msg_docs/NavigatorMissionItem.md)
- [VehicleAirData](../msg_docs/VehicleAirData.md)
- [PositionSetpoint](../msg_docs/PositionSetpoint.md)
- [SensorUwb](../msg_docs/SensorUwb.md)
- [Px4ioStatus](../msg_docs/Px4ioStatus.md)
- [GeneratorStatus](../msg_docs/GeneratorStatus.md)
- [LandingTargetPose](../msg_docs/LandingTargetPose.md)
- [MagnetometerBiasEstimate](../msg_docs/MagnetometerBiasEstimate.md)
- [SensorGnssStatus](../msg_docs/SensorGnssStatus.md)
- [DronecanNodeStatus](../msg_docs/DronecanNodeStatus.md)
- [RegisterExtComponentReplyV0](../msg_docs/RegisterExtComponentReplyV0.md)
- [SensorGnssRelative](../msg_docs/SensorGnssRelative.md)
- [Gripper](../msg_docs/Gripper.md)
- [VehicleCommandAckV0](../msg_docs/VehicleCommandAckV0.md)
- [CameraStatus](../msg_docs/CameraStatus.md)
- [SensorTemp](../msg_docs/SensorTemp.md)
- [VehicleGlobalPositionV0](../msg_docs/VehicleGlobalPositionV0.md)
- [LogMessage](../msg_docs/LogMessage.md)
- [ActuatorControlsStatus](../msg_docs/ActuatorControlsStatus.md)
- [FollowTargetStatus](../msg_docs/FollowTargetStatus.md)
- [VehicleStatusV0](../msg_docs/VehicleStatusV0.md)
- [Rpm](../msg_docs/Rpm.md)
- [EstimatorInnovations](../msg_docs/EstimatorInnovations.md)
- [DebugKeyValue](../msg_docs/DebugKeyValue.md)
- [ArmingCheckRequestV0](../msg_docs/ArmingCheckRequestV0.md)
- [MountOrientation](../msg_docs/MountOrientation.md)
- [SensorGyroFft](../msg_docs/SensorGyroFft.md)
- [SensorPreflightMag](../msg_docs/SensorPreflightMag.md)
- [TecsStatus](../msg_docs/TecsStatus.md)
- [LedControl](../msg_docs/LedControl.md)
- [OpenDroneIdSelfId](../msg_docs/OpenDroneIdSelfId.md)
- [OpenDroneIdOperatorId](../msg_docs/OpenDroneIdOperatorId.md)
- [Ekf2Timestamps](../msg_docs/Ekf2Timestamps.md)
- [FlightPhaseEstimation](../msg_docs/FlightPhaseEstimation.md)
- [QshellReq](../msg_docs/QshellReq.md)
- [ParameterResetRequest](../msg_docs/ParameterResetRequest.md)
- [RaptorInput](../msg_docs/RaptorInput.md)
- [SensorsStatus](../msg_docs/SensorsStatus.md)
- [SensorHygrometer](../msg_docs/SensorHygrometer.md)
- [GpioOut](../msg_docs/GpioOut.md)
- [DebugValue](../msg_docs/DebugValue.md)
- [EstimatorBias](../msg_docs/EstimatorBias.md)
- [UavcanParameterValue](../msg_docs/UavcanParameterValue.md)
- [NeuralControl](../msg_docs/NeuralControl.md)
- [OpenDroneIdArmStatus](../msg_docs/OpenDroneIdArmStatus.md)
- [ParameterSetValueRequest](../msg_docs/ParameterSetValueRequest.md)
- [GimbalDeviceInformation](../msg_docs/GimbalDeviceInformation.md)
- [VehicleAttitudeSetpointV0](../msg_docs/VehicleAttitudeSetpointV0.md)
- [VehicleStatusV1](../msg_docs/VehicleStatusV1.md)
- [SensorAccel](../msg_docs/SensorAccel.md)
- [HoverThrustEstimate](../msg_docs/HoverThrustEstimate.md)
- [SystemPower](../msg_docs/SystemPower.md)
- [InternalCombustionEngineControl](../msg_docs/InternalCombustionEngineControl.md)
- [LandingGearWheel](../msg_docs/LandingGearWheel.md)
- [EstimatorSensorBias](../msg_docs/EstimatorSensorBias.md)
- [HeaterStatus](../msg_docs/HeaterStatus.md)
- [GeofenceStatus](../msg_docs/GeofenceStatus.md)
- [SensorAccelFifo](../msg_docs/SensorAccelFifo.md)
- [RcParameterMap](../msg_docs/RcParameterMap.md)
- [DebugVect](../msg_docs/DebugVect.md)
- [PositionControllerStatus](../msg_docs/PositionControllerStatus.md)
- [ActuatorServosTrim](../msg_docs/ActuatorServosTrim.md)
- [RoverSpeedStatus](../msg_docs/RoverSpeedStatus.md)
- [MavlinkTunnel](../msg_docs/MavlinkTunnel.md)
- [SensorMag](../msg_docs/SensorMag.md)
- [DatamanResponse](../msg_docs/DatamanResponse.md)
- [GimbalManagerSetManualControl](../msg_docs/GimbalManagerSetManualControl.md)
- [VehicleStatusV2](../msg_docs/VehicleStatusV2.md)
- [ActuatorTest](../msg_docs/ActuatorTest.md)
- [GimbalManagerSetAttitude](../msg_docs/GimbalManagerSetAttitude.md)
- [AutotuneAttitudeControlStatus](../msg_docs/AutotuneAttitudeControlStatus.md)
- [NavigatorStatus](../msg_docs/NavigatorStatus.md)
- [RangingBeacon](../msg_docs/RangingBeacon.md)
- [VehicleOpticalFlow](../msg_docs/VehicleOpticalFlow.md)
- [GimbalManagerInformation](../msg_docs/GimbalManagerInformation.md)
- [EstimatorAidSource3d](../msg_docs/EstimatorAidSource3d.md)
- [VehicleConstraints](../msg_docs/VehicleConstraints.md)
- [CanInterfaceStatus](../msg_docs/CanInterfaceStatus.md)
- [RcChannels](../msg_docs/RcChannels.md)
- [BatteryStatusV0](../msg_docs/BatteryStatusV0.md)
- [DifferentialPressure](../msg_docs/DifferentialPressure.md)
- [GpioConfig](../msg_docs/GpioConfig.md)
- [SatelliteInfo](../msg_docs/SatelliteInfo.md)
- [VelocityLimits](../msg_docs/VelocityLimits.md)
- [VehicleLocalPositionSetpoint](../msg_docs/VehicleLocalPositionSetpoint.md)
- [VehicleAngularAccelerationSetpoint](../msg_docs/VehicleAngularAccelerationSetpoint.md)
- [GpsInjectData](../msg_docs/GpsInjectData.md)
- [CellularStatus](../msg_docs/CellularStatus.md)
- [EstimatorAidSource1d](../msg_docs/EstimatorAidSource1d.md)
- [FollowTarget](../msg_docs/FollowTarget.md)
- [EstimatorFusionControl](../msg_docs/EstimatorFusionControl.md)
- [RtlTimeEstimate](../msg_docs/RtlTimeEstimate.md)
- [VehicleRoi](../msg_docs/VehicleRoi.md)
- [EstimatorStatus](../msg_docs/EstimatorStatus.md)
- [AdcReport](../msg_docs/AdcReport.md)
- [AirspeedValidatedV0](../msg_docs/AirspeedValidatedV0.md)
- [GainCompression](../msg_docs/GainCompression.md)
- [SensorCorrection](../msg_docs/SensorCorrection.md)
- [ControlAllocatorStatus](../msg_docs/ControlAllocatorStatus.md)
- [EstimatorEventFlags](../msg_docs/EstimatorEventFlags.md)
- [AirspeedWind](../msg_docs/AirspeedWind.md)
- [EventV0](../msg_docs/EventV0.md)
- [ParameterUpdate](../msg_docs/ParameterUpdate.md)
- [ConfigOverridesV0](../msg_docs/ConfigOverridesV0.md)
- [YawEstimatorStatus](../msg_docs/YawEstimatorStatus.md)
- [EscEepromRead](../msg_docs/EscEepromRead.md)
- [EscEepromWrite](../msg_docs/EscEepromWrite.md)
- [InternalCombustionEngineStatus](../msg_docs/InternalCombustionEngineStatus.md)
- [GpioRequest](../msg_docs/GpioRequest.md)
- [TakeoffStatus](../msg_docs/TakeoffStatus.md)
- [GimbalControls](../msg_docs/GimbalControls.md)
- [FailureDetectorStatus](../msg_docs/FailureDetectorStatus.md)
- [FixedWingLateralStatus](../msg_docs/FixedWingLateralStatus.md)
- [VehicleImu](../msg_docs/VehicleImu.md)
- [DistanceSensorModeChangeRequest](../msg_docs/DistanceSensorModeChangeRequest.md)
- [DebugArray](../msg_docs/DebugArray.md)
- [GpsDump](../msg_docs/GpsDump.md)
- [UlogStreamAck](../msg_docs/UlogStreamAck.md)
- [VehicleStatusV3](../msg_docs/VehicleStatusV3.md)
- [PwmInput](../msg_docs/PwmInput.md)
- [RateCtrlStatus](../msg_docs/RateCtrlStatus.md)
- [ButtonEvent](../msg_docs/ButtonEvent.md)
- [FixedWingLateralGuidanceStatus](../msg_docs/FixedWingLateralGuidanceStatus.md)
- [HomePositionV0](../msg_docs/HomePositionV0.md)
- [VehicleLocalPositionV0](../msg_docs/VehicleLocalPositionV0.md)
- [RoverAttitudeStatus](../msg_docs/RoverAttitudeStatus.md)
- [UlogStream](../msg_docs/UlogStream.md)
- [FixedWingRunwayControl](../msg_docs/FixedWingRunwayControl.md)
- [VehicleImuStatus](../msg_docs/VehicleImuStatus.md)
- [EstimatorStates](../msg_docs/EstimatorStates.md)
- [IridiumsbdStatus](../msg_docs/IridiumsbdStatus.md)
- [VehicleAngularVelocity](../msg_docs/VehicleAngularVelocity.md)
- [NormalizedUnsignedSetpoint](../msg_docs/NormalizedUnsignedSetpoint.md)
- [DeviceInformation](../msg_docs/DeviceInformation.md)
- [RegisterExtComponentRequestV1](../msg_docs/RegisterExtComponentRequestV1.md)
- [Mission](../msg_docs/Mission.md)
- [SensorAirflow](../msg_docs/SensorAirflow.md)
- [SensorSelection](../msg_docs/SensorSelection.md)
- [ActionRequest](../msg_docs/ActionRequest.md)
- [GimbalManagerStatus](../msg_docs/GimbalManagerStatus.md)
- [GpioIn](../msg_docs/GpioIn.md)
- [OrbitStatus](../msg_docs/OrbitStatus.md)
- [UavcanParameterRequest](../msg_docs/UavcanParameterRequest.md)
- [Event](../msg_docs/Event.md)
- [EstimatorGpsStatus](../msg_docs/EstimatorGpsStatus.md)
- [TaskStackInfo](../msg_docs/TaskStackInfo.md)
- [SensorGyroFifo](../msg_docs/SensorGyroFifo.md)
- [TrajectorySetpoint6dof](../msg_docs/TrajectorySetpoint6dof.md)
- [DatamanRequest](../msg_docs/DatamanRequest.md)
- [PowerButtonState](../msg_docs/PowerButtonState.md)
- [Ping](../msg_docs/Ping.md)
- [Airspeed](../msg_docs/Airspeed.md)
- [ActuatorArmed](../msg_docs/ActuatorArmed.md)
- [RadioStatus](../msg_docs/RadioStatus.md)
- [LandingTargetInnovations](../msg_docs/LandingTargetInnovations.md)
- [BatteryInfo](../msg_docs/BatteryInfo.md)
- [VehicleAcceleration](../msg_docs/VehicleAcceleration.md)
- [OrbTestMedium](../msg_docs/OrbTestMedium.md)
- [ManualControlSwitches](../msg_docs/ManualControlSwitches.md)
- [PowerMonitor](../msg_docs/PowerMonitor.md)
- [HealthReport](../msg_docs/HealthReport.md)
- [InputRc](../msg_docs/InputRc.md)
- [SensorBaro](../msg_docs/SensorBaro.md)
- [IrlockReport](../msg_docs/IrlockReport.md)
- [RegisterExtComponentRequestV0](../msg_docs/RegisterExtComponentRequestV0.md)
- [SensorGyro](../msg_docs/SensorGyro.md)
- [TuneControl](../msg_docs/TuneControl.md)
- [EstimatorSelectorStatus](../msg_docs/EstimatorSelectorStatus.md)
- [WheelEncoders](../msg_docs/WheelEncoders.md)
- [GimbalDeviceSetAttitude](../msg_docs/GimbalDeviceSetAttitude.md)
- [ArmingCheckReplyV0](../msg_docs/ArmingCheckReplyV0.md)
- [EstimatorAidSource2d](../msg_docs/EstimatorAidSource2d.md)
- [OpenDroneIdSystem](../msg_docs/OpenDroneIdSystem.md)
- [LoggerStatus](../msg_docs/LoggerStatus.md)
- [FuelTankStatus](../msg_docs/FuelTankStatus.md)
- [Vtx](../msg_docs/Vtx.md)
- [Cpuload](../msg_docs/Cpuload.md)
- [MagWorkerData](../msg_docs/MagWorkerData.md)
- [OrbTest](../msg_docs/OrbTest.md)
- [EscReport](../msg_docs/EscReport.md)
- [PositionControllerLandingStatus](../msg_docs/PositionControllerLandingStatus.md)
- [RoverRateStatus](../msg_docs/RoverRateStatus.md)
- [FollowTargetEstimator](../msg_docs/FollowTargetEstimator.md)
- [VehicleOpticalFlowVel](../msg_docs/VehicleOpticalFlowVel.md)
- [MavlinkLog](../msg_docs/MavlinkLog.md)
  :::
