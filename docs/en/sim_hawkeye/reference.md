# Reference

Keyboard shortcuts, position data tiers, coordinate systems, and data source topics. Use this page as a lookup when you need a specific detail that doesn't fit in the feature sections.

## Keyboard Shortcuts

Press `?` at any time in Hawkeye to open the in-app help overlay with the complete keybind reference. The same information is listed below for offline reference.

### View

| Key             | Action                                                             |
| --------------- | ------------------------------------------------------------------ |
| `C`             | Cycle camera mode (Chase / FPV / Free)                             |
| `V`             | Cycle theme (Grid / Rez / Snow, or custom `.mvt` files)            |
| `H`             | Cycle HUD mode (Console / Tactical / Off)                          |
| `T`             | Cycle trail mode (off / directional / speed ribbon / drone color)  |
| `Shift+T`       | Cycle correlation overlay (off / line / curtain)                   |
| `G`             | Toggle ground track projection                                     |
| `F`             | Toggle terrain texture                                             |
| `K`             | Toggle classic (red/blue) / modern (yellow/purple) arm colors      |
| `O`             | Toggle orthographic side panel                                     |
| `Alt+1`         | Return to perspective camera                                       |
| `Alt+2`–`Alt+7` | Fullscreen ortho view (Top / Front / Left / Right / Bottom / Back) |
| `Ctrl+D`        | Toggle debug overlay                                               |
| `?`             | Toggle help overlay                                                |

### Camera motion

| Input                     | Action                                    |
| ------------------------- | ----------------------------------------- |
| Left-drag                 | Orbit camera (Chase) / look around (Free) |
| Scroll wheel              | Zoom FOV (perspective) or span (ortho)    |
| `WASDQE`                  | Fly in free camera mode                   |
| `Shift` (held, Free mode) | 3× motion boost                           |
| `Alt+Scroll`              | Zoom ortho span                           |
| Right-drag                | Pan view (ortho modes only)               |

### Vehicle selection

| Key                       | Action                                              |
| ------------------------- | --------------------------------------------------- |
| `M`                       | Cycle vehicle model within current group            |
| `Shift+M`                 | Cycle across all vehicle model groups               |
| `TAB`                     | Cycle to next vehicle (clears pins)                 |
| `1`–`9`                   | Select vehicle directly                             |
| `1`–`9` (two-digit chord) | Select drones 10–16 (press within 300 ms)           |
| `Shift+1`–`Shift+9`       | Toggle pin/unpin vehicle to HUD sidebar             |
| `Ctrl+L`                  | Toggle screen edge indicators for off-screen drones |

### Replay

| Key                             | Action                                      |
| ------------------------------- | ------------------------------------------- |
| `Space`                         | Pause / resume                              |
| `+` / `-`                       | Increase / decrease playback speed          |
| `←` / `→`                       | Seek 5 seconds                              |
| `Shift+←` / `Shift+→`           | Frame step (20 ms)                          |
| `Ctrl+Shift+←` / `Ctrl+Shift+→` | Seek 1 second                               |
| `R`                             | Restart from beginning                      |
| `Shift+L`                       | Toggle loop                                 |
| `I`                             | Toggle interpolation                        |
| `L`                             | Toggle marker label visibility              |
| `Y`                             | Toggle HDG / YAW display                    |
| `A`                             | Toggle takeoff time alignment (multi-drone) |
| `P`                             | Reopen deconfliction menu (multi-drone)     |

### Markers

| Key                   | Action                                             |
| --------------------- | -------------------------------------------------- |
| `B`                   | Drop marker at current position                    |
| `B` then `L`          | Drop marker and open label input                   |
| `Shift+B`             | Delete current marker                              |
| `[` / `]`             | Previous / next marker (selected drone)            |
| `Ctrl+[` / `Ctrl+]`   | Previous / next marker (global, across all drones) |
| `Shift+[` / `Shift+]` | Track drone from marker                            |

## Position Data Tiers

Not every ULog file contains the same position data. Some logs have authoritative home position. Some have only GPS-derived references. Some have nothing geographic at all. Hawkeye categorizes each log into one of three tiers based on what it finds.

![Position data tiers flowchart](../../assets/simulation/hawkeye/position-tiers.svg)

_<!-- 09-dia-01: flowchart showing the T1 → T2 → T3 fallback decision tree. -->_

| Tier   | Source                                                          | Trust level          | Typical use                                                         |
| ------ | --------------------------------------------------------------- | -------------------- | ------------------------------------------------------------------- |
| **T1** | `home_position` topic                                           | High (authoritative) | PX4 logs with clean home set before arming                          |
| **T2** | `vehicle_global_position` or `vehicle_local_position` reference | Medium               | Logs where home was set mid-flight or derived from GPS              |
| **T3** | Estimated origin (first valid position sample)                  | Low                  | Logs missing both home and GPS (indoor flights, optical flow, etc.) |

### How to check the current tier

The tier badge is visible in two places:

- **HUD tier indicator**, a small T1/T2/T3 badge in the Console HUD, colored per tier
- **Debug overlay**, an explicit tier label with full description, shown when `Ctrl+D` is active

### What each tier means for multi-drone replay

- **T1 logs** compare reliably, because all drones share a well-defined geographic reference.
- **T2 logs** compare if the reference points align; if they drift, positions drift too.
- **T3 logs** have no real geographic position. Multi-drone replay with mixed tiers produces ambiguous layouts. Use the Narrow Grid deconfliction mode to collapse T3 drones into a shared view.

### Known gotchas

- **Zeroed home position**: some logs record a `home_position` topic with zeros for lat/lon/alt. Hawkeye treats this as invalid and falls back to T2.
- **Home rejected**: if the pre-scan determines that the recorded home position doesn't match observed GPS data, it's rejected and the log falls through to T2. This prevents bad home values from rendering drones at `(0,0,0)` or far from the expected area.
- **BAD REF warning**: shown in the debug overlay if the current reference frame is suspect. Usually appears alongside T3 when the estimated origin drifts from observed position data.

## Coordinate Systems

PX4 and MAVLink use the **NED** (North-East-Down) coordinate convention. Raylib uses **Y-up, right-handed**. Hawkeye converts between them at the draw layer.

![Coordinate systems](../../assets/simulation/hawkeye/coordinate-systems.svg)

_<!-- 09-dia-02: side-by-side NED vs Raylib Y-up with arrows showing the mapping. -->_

### NED (input)

| Axis | Direction                    |
| ---- | ---------------------------- |
| X    | North                        |
| Y    | East                         |
| Z    | Down (positive below origin) |

### Raylib Y-up (rendering)

| Axis | Direction                |
| ---- | ------------------------ |
| X    | Right                    |
| Y    | Up                       |
| Z    | Back (toward the camera) |

### What this means for users

- **Heading** is displayed as 0° = North, 0–360° clockwise (compass convention). This matches navigation convention and what you'd expect from a PX4 HUD.
- **Yaw** (toggled with `Y`) is displayed as ±180°, 0 = forward axis of the vehicle, counter-clockwise positive (math convention).
- **Altitude** shown in the HUD is positive up, in meters above origin. The underlying log stores it as NED Z (negative below origin); the HUD flips the sign for intuitive display.
- **Origin altitude** is typically the launch point elevation, not Mean Sea Level (MSL).

### Debug panel coordinates

The X/Y/Z values shown in the debug overlay (`Ctrl+D`) are in Raylib's Y-up draw-space coordinates, not NED. This is mostly what you want for understanding the rendered scene, but be aware that the Y value is the **altitude**, not the "forward distance" you might expect from an NED-convention tool.

## Data Sources

Hawkeye consumes telemetry from two sources: live MAVLink streams (from PX4 SITL) and ULog file replay.

### MAVLink messages (live mode)

| Message                | Purpose                                                |
| ---------------------- | ------------------------------------------------------ |
| `HEARTBEAT`            | Vehicle type, arming state, autopilot mode             |
| `HIL_STATE_QUATERNION` | Position, attitude, velocity, airspeed                 |
| `STATUSTEXT`           | Warning and log messages (severity 0–7)                |
| `HOME_POSITION`        | Authoritative home location                            |
| `GLOBAL_POSITION_INT`  | GPS latitude, longitude, altitude (fallback reference) |

### ULog topics (replay mode)

| Topic                     | Required | Purpose                                       |
| ------------------------- | -------- | --------------------------------------------- |
| `vehicle_attitude`        | yes      | Orientation quaternion                        |
| `vehicle_local_position`  | yes      | NED position and velocity                     |
| `vehicle_global_position` | no       | GPS position (enables real-world coordinates) |
| `vehicle_status`          | no       | Vehicle type, arming, flight mode transitions |
| `home_position`           | no       | Authoritative home (Tier 1 source)            |
| `airspeed_validated`      | no       | Airspeed sensor data                          |
| `logging`                 | no       | STATUSTEXT warnings (feeds the HUD ticker)    |

If a required topic is missing, Hawkeye refuses to load the log and prints an error. If optional topics are missing, the affected features silently degrade (no airspeed, no STATUSTEXT, etc.).
