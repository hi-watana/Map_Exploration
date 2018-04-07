# Map Exploration

Map Exploration is a game program for ARM Mbed
([https://www.mbed.com/en/](https://www.mbed.com/en/)).

In Map Exploration, your goal is to find Ookayama Station, Suzukakedai Station,
or some other places. You can explore GSI Map (source: Geospatial Information
Authority of Japan, https: //maps.gsi.go.jp/development/ichiran.html), displayed
in a 512x512 (pixels) window, by using the joystick on an application board.
When you finally find the place, Mbed board tells you that, and you can start a
new exploration.

You usually use a map for looking for the place where you are going to go. In
such a situation, you will be able to grasp the total path from start to
destination. Map Exploration, however, shows only very limited area at one time,
so you have to search for some clues which lead you to the destination. The goal
of this project is to let you know such a new way of seeing maps.

Detailed information is available from `Slide.pdf`.

### Requirements

In addition to mbed compiler ([https://os.mbed.com](https://os.mbed.com)),
* C12832
  ([https://os.mbed.com/users/chris/code/C12832/](https://os.mbed.com/users/chris/code/C12832/))

is required.
