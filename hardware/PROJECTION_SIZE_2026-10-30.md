# Smaller projection — investigation, 30 October 2026

Owner reports the previous installation projected roughly one metre wide at two metres distance, whereas the present projection looks smaller. No current measured width, active profile/status or old analog gain has been supplied, so the cause is **not established**.

## Confirmed from code and the current circuit

1. Legacy receiver `firmware/legacy/uno-candidate/LaserShow/Laser.cpp` calls `setGain2x(channel, 0)` on both DAC channels. Its `DAC_MCP4X.cpp` sets the 1× bit when the flag is false.
2. Current S3 `Geometry.h::dacWord()` uses `0x3000` / `0xb000`; bit 13 is also 1×. There is no demonstrated gain-halving difference between these code paths.
3. MCP4822 internal reference is 2.048 V. At 1× the nominal code increment is 0.5 mV and full-scale code 4095 is 2.0475 V. Its 5 V supply is not a command for 0…5 V output. [Microchip datasheet, register/gain and transfer function](https://ww1.microchip.com/downloads/en/devicedoc/20002249b.pdf).
4. Old panel defaults were scale 0.25. Version 0.1.5 imports older profiles at scale 1.0 and angle 0°. A scale of 0.25 saved in 0.1.5 still shrinks coordinate excursions to one quarter.
5. Current analog resistors 47k feedback / 10k input give magnitude 4.7×; nominal full-span output is 2.0475 × 4.7 = **9.62325 Vpp**. The transfer is `Vout = 5.7*Vref - 4.7*Vdac`. Owner reported Vref approximately 0.79 V. Supply ±15 V does not determine closed-loop gain by itself. [Current assembly measurements](ANALOG_BRINGUP_2026-10-29.md).
6. Unit-scale full recorded show coordinates before output orientation span X 400…4073 and Y 356…3279. Individual scenes have their own margins. Frame spans 1…4095 at angle 0°, scale 1.0. Hence full Frame is not the same physical width as every artwork scene.
7. New 0.1.4 arbitrary-angle fitting reduces applied scale at intermediate angles to keep corners inside the DAC canvas. At 45°: requested 1.0 → applied ~0.707. At 0°: 1.0. This new feature cannot explain observations made on 0.1.3 or earlier.

## Direct next evidence

- Read `status`: exact firmware, saved scale, orientation; angle and appliedScale.
- Use **Frame + F** at scale 1.0 and angle 0°. Record width, height, distance from the mirrors to the surface, and the size at speeds 100% and 25%. For comparisons with an older installation, use the same drawing and distance.
- `diag` reports commanded lit coordinate extents and nominal predicted voltage span. These are software observations, **not actual analog measurements**.
- If software span is full but physical size remains small, compare actual DAC and analog signal span and identify the old feedback/input resistors. A static multimeter reading at the centre cannot establish a moving waveform's peak-to-peak span.

No analog wiring, resistor ratio, SPI clock, DAC gain or laser power is changed by this investigation. Doubling DAC gain without recalculating the offset would change the output centre by approximately −4.8 V at the old midpoint; it is not a verified correction.
