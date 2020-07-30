# Mitutoyo SPC Data Reader for Arduino
Use an Arduino Nano to translate Mitutoyo Digitmatic SPC data to USB serial
for easy consumption by
[Node-RED](https://flows.nodered.org/node/node-red-node-serialport)
or any other software that accepts USB serial data.

Includes two build-time options set via `#define`:
* `HEX_DATA_OUTPUT` to output data in hexadecimal form.
* `HUMAN_READABLE_OUTPUT` to output a more human-readable form that mimics
what is shown on the instrument's built in LCD.

The directory structure here is a
[Platform.IO project](https://platformio.org/)
but the code in `src\main.cpp` should be easy to copy & paste into the standard
Arduino IDE.

References:
* Information on the 
[in-box pamphlet](https://twitter.com/Regorlas/status/1287870898398064640)
of a
[Mitutoyo Digimatic 543-783B](https://ecatalog.mitutoyo.com/ABSOLUTE-Digimatic-Indicator-ID-S-Series-543-with-Simple-Design-C1196.aspx).
* [This Instructables project](https://www.instructables.com/id/Interfacing-a-Digital-Micrometer-to-a-Microcontrol/)
which uses a Mitutoyo micrometer instead of plunge-style indicator like the
543-783B, and output to VGA which is not needed for this project.
