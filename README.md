Part of the Twirre architecture: <https://nhlstenden-cvds.github.io/Twirre>

# TwirreArduino

* [TwirreArduino firmware](#twirrearduino-firmware)
* [TwirreSerial](#twirreserial)
* [How to Build](#how-to-build)
* [License](#license)
* [Examples](#examples)


## TwirreArduino firmware
TwirreArduino is software for the Arduino DUE which enables it to easily be used for sensor and actuator communication together with TwirreLink software. It has been designed to be easy-to-use, modular and extendable. The generic structure enables a large amount of devices to be integrated into the TwirreLink system.

## TwirreSerial
TwirreArduino includes the TwirreSerial protocol for communication between the Arduino DUE and a computer running TwirreLink software. The protocol specification can be found here: <https://github.com/NHLStenden-CVDS/Twirre/releases/download/doc-alpha/twirreSerialProtocol.pdf>

## How to Build
This project contains a project file for [Sloeber, a free and open Arduino IDE based on Eclipse][sloeber], which is the recommended way to build and flash the software.
* Start Sloeber
* Ensure Arduino DUE files are available (Arduino > Preferences > Arduino > Platforms and Boards > arduino > Arduino SAM boards... > [select latest version])
* Load project into Sloeber (File > Import... > General > Existing projects into workspace)
* Sloeber seems to store the absolute path for the compiler in the project file, which is not updated on importing the project. To fix this, right-click the project, go to "Properties" > "Arduino", and press "Apply & Close". This will update the compiler path for your system.
* Press 'upload' button (if needed, follow instructions for setting port)

## License
TwirreArduino has been made available under the MIT license (see **LICENSE**).

## Examples
Examples to be added at a later date



[sloeber]: http://eclipse.baeyens.it/index.shtml
