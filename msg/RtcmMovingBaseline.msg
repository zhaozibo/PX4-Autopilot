uint64 timestamp		# time since system start (microseconds)

uint32 device_id                # unique device ID of the moving-base GPS that produced this RTCM

uint16 len                      # length of data
uint8 flags                     # LSB: 1=fragmented
uint8[300] data                 # RTCM 4072 (or equivalent) moving-baseline data for the rover GPS

uint8 ORB_QUEUE_LENGTH = 16

uint8 MAX_INSTANCES = 2
