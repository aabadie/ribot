## RiBot hardware setup

### Required elements

Here is the list of elements used to setup RiBot:
* 1 [MDF Wood](https://en.wikipedia.org/wiki/Medium-density_fibreboard) panel
  (300mm x 600mm size, 3mm thin).
* 2 [90mm wheels](http://www.robotshop.com/en/pololu-wheel-90-10mm-black-pair.html)
* A pair of [mounting hubs](http://www.robotshop.com/en/pololu-universal-aluminum-6mm-mounting-hub.html)
* 2 [Metal balls](http://www.robotshop.com/en/pololu-ball-caster-3-8-in-metal-ball.html)
* 2 gear motors [with encoder](https://www.pololu.com/product/2824) or
  [without encoder](https://www.pololu.com/product/1104) (Note: those motors are
  a bit noisy, RiBot might use different ones in the future)
* 2 [metal gear brackets](http://www.robotshop.com/en/pololu-37d-mm-metal-gearmotor-bracket.html)
* A [dual channel DC motor driver](http://www.robotshop.com/en/10a-5-30v-dual-channel-dc-motor-driver.html)
* A 12V [NiMh battery](http://www.robotshop.com/en/120v-2800mah-rechargeable-nimh-battery-pack.html)
* A [voltage regulator](http://www.robotshop.com/en/5v-3a-step-down-voltage-regulator.html)
* A 4 channel [level shifter](https://www.adafruit.com/product/757)
* 2 [terminal block connectors (3 points)](http://www.wago.us/products/terminal-blocks-and-connectors/installation-connectors/compact-splicing-connectors-for-all-conductor-types-222-series/overview/)
* A set of wires
* Screws with bolts : M5-80mm x 5, M2.5-20mm x 8, M2.5-8mm x 8
* A board that runs RIOT and with support for UART, PWM and GPIO. RiBot has
  been successfully tested with:
  * A [ST Nucleo-f446](http://www.st.com/en/evaluation-tools/nucleo-f446re.html)
  board running the
  [bluetooth communication firmware](https://github.com/aabadie/ribot/tree/master/bt-fw).
  * An [Atmel SAMR21 Xplained Pro](http://www.atmel.com/tools/ATSAMR21-XPRO.aspx)
    board running the
    [CoAP controlled firmware](https://github.com/aabadie/ribot/tree/master/coap-fw).

### Setup steps

1. Cut the mechanical parts of RiBot with a laser cutter, the MDF panels and from
  [pdf schematics](https://github.com/aabadie/ribot/blob/master/docs/design/RiBot2.pdf).
2. Assemble all elements (TODO: pictures required):
    * Use glue to build 8 spacers (9mm thin) from the 3mm thin disks
    * Stare the control board (Nucleo or Samr21) on the top panel using the 4
      remaining 20mm screws and 9mm spacers
    * Stare the metals balls to the bottom panel
    * Using the spacers and the 20mm screws, stare the dual channel DC driver
      board on the bottom panel
    * Stare the gear bracket on the bottom panel using the 8mm screws
    * Stare the DC motors on the gear bracket
    * Assemble the vertical partitions parts on the bottom panel
    * Put the battery in the back space
    * Ensure the DC motors wires are accessible from the front space (with the
      motor driver board)
    * Put the top panel and stare it with the M5 screws
    * Setup the wheels with the mounting hubs
3. [TODO: give wiring schematics] Wire up the boards, the battery and DC motor drivers
   Be very careful with shortcuts and verify wiring twice before powering
   everything : the boards (nucleo, atmel) are only tolerant to **5V** and the
   battery gives 12V to DC motors.
   On the DC motor driver, `M1` corresponds to the right motor and `M2` to the
   left motor.
   Use the terminal block connectors to split the battery cable in 2 cables
   (2 GND and 2 14V) : 1 `GND` goes to the `B-` on the DC Motor Driver, 1 14 V
   goes to `B+`. The other `GND` goes to the voltage regulator, the other 14V
   as well.
   On the motor driver, the `DIR1` and `DIR2` pins only works woth 5V levels,
   thus the need for the level shifter (as the control boards works at 3.3V):
   * With the nucleo connect `DIR1` to `D4`, `DIR2` to D7, `PWM1` to `D5` and
     `PWM2` to `D6`,
   * With the Atmel connect `DIR1` to `PA18`, `DIR2` to `PA19`, `PWM1` to
     `PA6` and `PWM2` to `PA7`.


#### License

RiBot documentation is under CC BY-NC license: http://creativecommons.org/licenses/by-nc/3.0/
