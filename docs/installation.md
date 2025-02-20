# Installation

This project uses Platformio to compile the firmware for ESP32 boards under VSCode. Please note that installing this program and loading it onto the ESP32 board is slightly more complicated than the install process of ORM. Hence I recommend this only for those that have the basic understanding and experience for embedded systems programming, Arduino and/or Platformio. This may prove to be rather challenging for a complete beginner. There are several tutorials on YouTube as well as on the [Platformio main page](https://platformio.org/).

As a first step the platformio.ini file needs the exact board name. There is a [list](https://docs.platformio.org/en/latest/boards/index.html#espressif-32) on the platformio website. It is recommended that a dual core ESP module is used for better performance (especially if the websocket will be used). Also setting (or deleting) the flash size may be necessary if not 4MB.

Once platformio.ini is set up, upload can be performed with the VSCode platformio extension or by running `platformio run -t upload` after connecting the board via USB.

## Circuit

Basically any ESP32 development board can be used, but this project was developed and tested on the [Firebeetle 2 Dev Board](https://www.dfrobot.com/product-2231.html).

The general master branch should work with any ESP32 board (as it does not have any board specific code or feature). Please note that the sensor pin needs to be set in settings.h to the pin that is used (default is `GPIO_NUM_26`). Also if battery level measurement is used the pin that is connected to the battery (potentially via a voltage divider) should be configured. The device goes to deep sleep if no peripheral is connected and the last impulse was was since `DEEP_SLEEP_TIMEOUT`. There before entering deepsleep mode the sensor pin is set as wakeup source (taking advantage of interrupt based wakeup) so the devices starts up on starting a session automatically.

### FireBeetle 2 board

There is a specific branch (FireBeetle) for this board to take advantage of its internal connection between the battery and the ADC pin to measure battery level without additional circuitry and the RGB LED that changes color based on the measured battery level. There are additional features on this branch such as a digital switch (using a transistor) for the hall sensor power so battery consumption in deepsleep is minimal. To wake the device up a wakeup interrupt is set to a separate pin (on my machine this is connected to a reed switch on the handle chain sprocket).

Also for the this board I designed a [3D printable box](https://www.printables.com/model/441912-firebeetle-2-esp32-e-box-for-esp-rowing-monitor). You can find some pictures of the setup below.

![Firebeetle box open](imgs/firebeetle-box-open.jpg)
![Firebeetle box closed](imgs/firebeetle-box-closed.jpg)
![Firebeetle box open blinking](imgs/firebeetle-box-open-blink.jpg)

### Generic Air Rower

ESP Rowing Monitor was mainly tested and developed on a Generic Air Rower that is basically a [cheap Chinese clone](https://www.aliexpress.com/item/1005002302662579.html) of the Concept2.

The monitor provided with this rower was not very capable especially as it did not measure rotational impulses on the flywheel. So I designed a [magnet holder](https://www.printables.com/model/442340-generic-air-rower-flywheel-magnet-holder) that fits the flywheel and can hold up to 6 3x3 neodymium magnets (I use it with 3). Below are a few pictures.

![Manget holder 1](imgs/magnet-holder-bracket-1.jpg)
![Manget holder 2](imgs/magnet-holder-bracket-2.jpg)
