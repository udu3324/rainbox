# rainbox research

## sound creation --- 

https://mynoise.net/NoiseMachines/rainNoiseGenerator.php
https://science.nasa.gov/earth/earth-observatory/listening-to-raindrops/

* water hitting directly on water (requires a depth of water)
* water hitting some sort of ground (tarp, or rubber)

### at what height? - 
water droplets hit their terminal velocity easily
~400mm


at what diameter of drop makes sound? what kind of sound? -
* 0.8/1.2mm dia - 13-25kHz direct water (high freq)
* 1.2/2.0 dia - quiet type
* 2.0/3.5mm dia - 1-10kHz direct water, 1-50kHz splat (low freq)




## fluid mechanics --- 

https://www.wisewell.com/blogs/news/is-deionized-water-the-same-as-distilled-water
use deionized water

focuses on creating these three diameter drops to create a dynamic sound of rain/weather

at least manage a ~2in deep water tank

water is brought from its bottom collecting tank to its upper tank, using a quiet motor with flaps 

based on the kohler rain shower head, a plate will guide water down holes using mainly gravity, not pressure




## electronics --- 

24v power supply

esp32-s3 (controls all)


### choice 1
https://zddcmotor.com/products/4245-brushless-motor (same)
https://www.aliexpress.us/item/3256807744943576.html
https://www.aliexpress.us/item/3256809335050730.html

### choice 2
https://www.underwaterthruster.com/products/3750-underwater-waterproof-and-corrosion-proof-brushless-motors-for-underwater-robots-and-rov-motors-for-unmanned-boats?variant=42650338590948
(get the esc from choice 1)
(also request them to waterproof/add cables free of charge)

brushless dc motor (preferably waterproof too but prioritize on quietness first)
brushless esc motor controller

nema 17 stepper motor
tmc2209/2240/2241 (prefer right to left) driver for quietness

https://www.aliexpress.us/item/3256806203259773.html
water level sensor (2x for top and bottom tank)

neopixel strip

https://www.aliexpress.us/item/3256809269681816.html
https://forum.arduino.cc/t/help-setting-25mm-ultrasonic-mist-maker/1333404
ultrasonic atomizer (m165d25, JHB20-17A384 or equivalent)
needs a driver controller of the same voltage and frequency




## frame/structure --- 

* tank/sides obviously has to be waterproof
* polycarbonate sides with a opening on one wall (maybe side cutouts too)
* bottom tank with a hole on the side, easy to clean and maintain
* 2020 aluminum extrusion for the frame and support
* petg prints preferred for everything
* petg or pvc flexible clear tubing
