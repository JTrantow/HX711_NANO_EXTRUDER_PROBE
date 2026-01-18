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

