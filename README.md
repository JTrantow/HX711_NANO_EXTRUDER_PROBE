# HX711_NANO_EXTRUDER_PROBE
Code for 3d printer nozzle probe using strain gauge running on Arduino Nano.


Inspired by:
https://www.youtube.com/watch?v=r3Bz-Iza5p8 Fix Your First Layer! The $10 Z-Probe for Perfect 3D Prints

https://github.com/sylim5442/Strain-Gauge-Leveling-Probe (3 years ago)

https://github.com/kasom/LoadCellZProbe Code and setup for an under-bed three load cell Z-probe, which uses an STM32 board to manage multiple HX711 inputs.

jesyblue/EVA-mod-for-load-cel-probe: A physical modification (EVA mod) and firmware (Visuino) project that routes the force through the load cell for nozzle-as-probe functionality.
palmerr23/LoadCell_ZProbe: Arduino code and instructions for a basic RepRap Z-probe setup using a standard load cell and HX711 converter. 

HX711 libraries

https://github.com/adafruit/Adafruit_HX711 Doesn't seem fully featured? 
https://github.com/bogde/HX711 Hasn't been updated in a while. Includes power_down()/power_up()
https://github.com/RobTillaart/HX711 goes into detail on various HX711 modes and settings. Looks like improvement over bogde.


Probe is setup using NOZZLE_AS_PROBE option with TARE.

  #define PROBE_TARE_TIME  200    // (ms) Time to hold tare pin
  #define PROBE_TARE_DELAY 200    // (ms) Delay after tare before
  #define PROBE_TARE_STATE HIGH   // State to write pin for tare
  #define PROBE_TARE_PIN SERVO0_PIN    // Override default pin
  #if ENABLED(PROBE_ACTIVATION_SWITCH)
    //#define PROBE_TARE_ONLY_WHILE_INACTIVE  // Fail to tare/probe if PROBE_ACTIVATION_SWITCH is active
  #endif

M48 Z-Probe Repeatability Test Results (leaving statistics run)

Mean: -0.825000 Min: -0.842 Max: -0.808 Range: 0.034 Standard Deviation: 0.010893
Mean: -0.815156 Min: -0.858 Max: -0.789 Range: 0.069 Standard Deviation: 0.015935
Mean: -0.811563 Min: -0.831 Max: -0.806 Range: 0.025 Standard Deviation: 0.007335

If I don't push triggered values (continuous stats) I get slightly better results:

Mean: -0.811563 Min: -0.831 Max: -0.806 Range: 0.025 Standard Deviation: 0.007335
Mean: -0.551250 Min: -0.567 Max: -0.533 Range: 0.034 Standard Deviation: 0.011110
Mean: -0.555312 Min: -0.561 Max: -0.536 Range: 0.025 Standard Deviation: 0.007819

Clearing at start of probe using TARE signal.

==========================================================================
M48 Z-Probe Repeatability Test

Cold test: 22 Jan 2025

Mean: 2.319531 Min: 2.314 Max: 2.323 Range: 0.009   Standard Deviation: 0.002237
Mean: 2.027188 Min: 1.867 Max: 2.183 Range: 0.316   Standard Deviation: 0.112311
Mean: 1.987969 Min: 1.984 Max: 1.994 Range: 0.009   Standard Deviation: 0.002215

Hot test: 22 Jan 2025


M48 Z-Probe Repeatability Test

Cold test: 22 Jan 2025

Mean: 2.319531 Min: 2.314 Max: 2.323 Range: 0.009   Standard Deviation: 0.002237
Mean: 2.027188 Min: 1.867 Max: 2.183 Range: 0.316   Standard Deviation: 0.112311
Mean: 1.987969 Min: 1.984 Max: 1.994 Range: 0.009   Standard Deviation: 0.002215

Mean: 0.754219 Min: 0.742 Max: 0.762 Range: 0.020 Standard Deviation: 0.004894
Mean: 0.753906 Min: 0.739 Max: 0.762 Range: 0.023 Standard Deviation: 0.005688

Hot test: 22 Jan 2025

M48 Z-Probe Repeatability Test



SENDING:G30
echo:G30
>>> set_bed_leveling_enabled  X50.00 Y50.00 Z5.00
<<< set_bed_leveling_enabled  X50.00 Y50.00 Z5.00
remember_feedrate_scaling_off: fr=66.67 100%
>>> Probe::probe_at_point  X50.00 Y50.00 Z5.00
...(50.00, 50.00, stow, 0, probe_relative)
  current_position= X50.00 Y50.00 Z5.00 :
Move to probe-relative point
>>> do_blocking_move_to  X50.00 Y50.00 Z5.00
>  X50.00 Y50.00 Z5.00
<<< do_blocking_move_to  X50.00 Y50.00 Z5.00
  current_position= X50.00 Y50.00 Z5.00 : Probe::set_deployed
deploy=1 no_return=0
Raise Z to 10.00
do_z_clearance(10.00 [5.00 to 10.00], 0)
do_blocking_move_to_z(10.00, 4.00)
>>> do_blocking_move_to  X50.00 Y50.00 Z5.00
>  X50.00 Y50.00 Z10.00
<<< do_blocking_move_to  X50.00 Y50.00 Z10.00
>>> Probe::probe_specific_action  X50.00 Y50.00 Z10.00
<<< Probe::probe_specific_action  X50.00 Y50.00 Z10.00
>>> do_blocking_move_to  X50.00 Y50.00 Z10.00
>  X50.00 Y50.00 Z10.00
<<< do_blocking_move_to  X50.00 Y50.00 Z10.00
>>> Probe::run_z_probe  X50.00 Y50.00 Z10.00
Probe Low Point: -12.00
Taring probe
Slow Probe:
SLOW> try_to_probe(..., -12.00, 2.00, ...)
Taring probe
>>> Probe::probe_down_to_z  X50.00 Y50.00 Z10.00
do_blocking_move_to_z(-12.00, 2.00)
>>> do_blocking_move_to  X50.00 Y50.00 Z10.00
>  X50.00 Y50.00 Z-12.00
echo:busy: processing
>>> G0 Z10
SENDING:G0 Z10
echo:busy: processing
echo:busy: processing
<<< do_blocking_move_to  X50.00 Y50.00 Z-12.00
  current_position= X50.00 Y50.00 Z0.76 : sync_plan_position
<<< Probe::probe_down_to_z  X50.00 Y50.00 Z0.76
<<< Probe::run_z_probe  X50.00 Y50.00 Z0.76
  current_position= X50.00 Y50.00 Z0.76 : Probe::set_deployed
deploy=0 no_return=0
>>> Probe::probe_specific_action  X50.00 Y50.00 Z0.76
<<< Probe::probe_specific_action  X50.00 Y50.00 Z0.76
>>> do_blocking_move_to  X50.00 Y50.00 Z0.76
>  X50.00 Y50.00 Z0.76
<<< do_blocking_move_to  X50.00 Y50.00 Z0.76
Bed X: 50.00 Y: 50.00 Z: 0.76
<<< Probe::probe_at_point  X50.00 Y50.00 Z0.76
Bed X:50.00 Y:50.00 Z:0.758
restore_feedrate_and_scaling: fr=66.67 100%
>>> do_blocking_move_to  X50.00 Y50.00 Z0.76
>  X50.00 Y50.00 Z0.76
<<< do_blocking_move_to  X50.00 Y50.00 Z0.76
>>> move_z_after_probing  X50.00 Y50.00 Z0.76
<<< move_z_after_probing  X50.00 Y50.00 Z0.76
echo:M105
echo:G0 Z10
echo:M105
echo:M105
echo:M105





>>> G30
SENDING:G30
echo:G30
>>> set_bed_leveling_enabled  X50.00 Y50.00 Z10.00
<<< set_bed_leveling_enabled  X50.00 Y50.00 Z10.00
remember_feedrate_scaling_off: fr=66.67 100%
>>> Probe::probe_at_point  X50.00 Y50.00 Z10.00
...(50.00, 50.00, stow, 0, probe_relative)
  current_position= X50.00 Y50.00 Z10.00 :
Move to probe-relative point
>>> do_blocking_move_to  X50.00 Y50.00 Z10.00
>  X50.00 Y50.00 Z10.00
<<< do_blocking_move_to  X50.00 Y50.00 Z10.00
  current_position= X50.00 Y50.00 Z10.00 : Probe::set_deployed
deploy=1 no_return=0
Raise Z to 10.00
do_z_clearance(10.00 [10.00 to 10.00], 0)
>>> Probe::probe_specific_action  X50.00 Y50.00 Z10.00
<<< Probe::probe_specific_action  X50.00 Y50.00 Z10.00
>>> do_blocking_move_to  X50.00 Y50.00 Z10.00
>  X50.00 Y50.00 Z10.00
<<< do_blocking_move_to  X50.00 Y50.00 Z10.00
>>> Probe::run_z_probe  X50.00 Y50.00 Z10.00
Probe Low Point: -2.00
Taring probe
Slow Probe:
SLOW> try_to_probe(..., -2.00, 2.00, ...)
Taring probe
>>> Probe::probe_down_to_z  X50.00 Y50.00 Z10.00
do_blocking_move_to_z(-2.00, 2.00)
>>> do_blocking_move_to  X50.00 Y50.00 Z10.00
>  X50.00 Y50.00 Z-2.00
echo:busy: processing
echo:busy: processing
<<< do_blocking_move_to  X50.00 Y50.00 Z-2.00
  current_position= X50.00 Y50.00 Z1.79 : sync_plan_position
<<< Probe::probe_down_to_z  X50.00 Y50.00 Z1.79
<<< Probe::run_z_probe  X50.00 Y50.00 Z1.79
  current_position= X50.00 Y50.00 Z1.79 : Probe::set_deployed
deploy=0 no_return=0
>>> Probe::probe_specific_action  X50.00 Y50.00 Z1.79
<<< Probe::probe_specific_action  X50.00 Y50.00 Z1.79
>>> do_blocking_move_to  X50.00 Y50.00 Z1.79
>  X50.00 Y50.00 Z1.79
<<< do_blocking_move_to  X50.00 Y50.00 Z1.79
Bed X: 50.00 Y: 50.00 Z: 1.79
<<< Probe::probe_at_point  X50.00 Y50.00 Z1.79
Bed X:50.00 Y:50.00 Z:1.794
restore_feedrate_and_scaling: fr=66.67 100%
>>> do_blocking_move_to  X50.00 Y50.00 Z1.79
>  X50.00 Y50.00 Z1.79
<<< do_blocking_move_to  X50.00 Y50.00 Z1.79
>>> move_z_after_probing  X50.00 Y50.00 Z1.79
<<< move_z_after_probing  X50.00 Y50.00 Z1.79
echo:M105
echo:M105