# Abstracted UART Control of DY-XXXX mp3 modules

This library abstracts almost all features described in the manual into a C++
class, except: 

- Combination play (simultaneous sound file playing), the protocol is rather
  awkward, where most commands require a `uint16_t` and some a path string, 
  this command requires the number as a string, I think. I currently have no use
  for this feature, you could add it via PR if you do (I would appreciate it).
  Note that "interlude" should work.

This library also __does not support__ the `ONE_Line` protocol, 
[more info](#one_line-support). 

Although all features are implemented and should theoretically work, only 
those in the examples directory are tested. Please create an issue if you have
problems with any of the implemented features.

This library was written in a hardware independent way. Which means it should
work on any device with a serial port, e.g. any Arduino, Espressif, ARM based 
boards, probably even any computer. 

## HAL

__Arduino tl;dr;__
If you are using Arduino, skip this chapter and go to [Arduino](#arduino).

Because the library is hardware independent you might need to add a Hardware
Abstraction Layer (HAL) which sets up the serial port and that implements a 
minimum of 2 functions `serialWrite()` and `serialRead()`, this is the included
Arduino example: 

``` C++
// player.hpp
#include <Arduino.h>
#include "DYPlayer.h"

class Player: public DYPlayer {
    public:
        HardwareSerial *port;
        Player();
        Player(HardwareSerial* port);
        void begin();
        void serialWrite(uint8_t *buffer, uint8_t len);
        bool serialRead(uint8_t *buffer, uint8_t len);
};

//player.cpp
#include "player.hpp"

#include "DYPlayerArduino.h"
Player::Player() {
    this->port = &Serial;
}
Player::Player(HardwareSerial* port) {
    this->port = port;
}    
void Player::begin() {
    port->begin(9600);
}
void Player::serialWrite(uint8_t *buffer, uint8_t len) {
    port->write(buffer, len);
}
bool Player::serialRead(uint8_t *buffer, uint8_t len) {
    // Serial.setTimeout(1000); // Default timeout 1000ms.
    if(port->readBytes(buffer, len) > 0) {
        return true;
    }
    return false;
}
```

Steps: 

1. Define a class that extends the `DYPlayer` class.
1. Define constructors that set up the serial port. On some platforms you will
   need to setup your serial port after some other things are initialized, e.g.
   on Arduino, then define an additional `Player::begin()` (or 
   e.g. `Player::init()`) to finish initialisation.
1. Define functions for `serialWrite()` and `serialRead()` according to the 
   board and the framework you use.

## Arduino

Because this is included, on Arduino you can just include the
`DYPlayerArduino.h` header file and use the module.

You may use any Serial port on the board, you will have to pass it to the begin
function. The argument should be of type `void*` because the virtual function
is hardware independent, it doesn't have the Arduino `HardwareSerial` type. So
cast it to (e.g. `Player.begin((HardwareSerial*) Serial2)`).

To use the Arduino HAL see [PlaySound.ino](examples/PlaySound/PlaySound.ino).

## Modules should work (not exhaustive)

| Model name | Capacity | SD Card support | Amplifier       | Voltage | Tested |
|:-----------|:---------|:---------------:|:----------------|:-------:|:------:|
| DY-SV17F   | 32Mbit   | No              | 3-5W(4Ω/8Ω)     | 5VDC    | Yes    |
| DY-SV8F    | 64Mbit   | No              | 3-5W(4Ω/8Ω)     | 5VDC    | No     |
| DY-HV20T   | NA       | Yes, Max. 32GB  | 3-5W(4Ω/8Ω)     | 5VDC    | No     |
| DY-HV8F    | 8Mbit    | No              | 10W(8Ω)/20W(4Ω) | 6-35VDC | No     |
| DY-HV20T   | NA       | Yes, Max. 32GB  | 10W(8Ω)/20W(4Ω) | 6-35VDC | No     |

NOTE: I cannot guarantee that your board will work with the library. Nor that
a specific feature will work. I only have the `DY-SV17F` in my possession to
test at the time of writing. If something does not work, make an issue and/or
send me a pull request. 

## Wiring the module

If you have a board with DIP switches, set `CON3` to on, `CON1` and `CON2`
should remain off. If you have no DIP switches (e.g. `DY-SV17F` board), you have
to connect 3 resistors, one from each `CON` pin, to:

| CON pin    | Connect to |
|:-----------|:-----------|
| `CON1`     | `3.3V`     |
| `CON2`     | `GND`      |
| `CON3`     | `GND`      |

The 3.3V pin is exposed by the board so you don't need to provide it.

Further make these connections:

| Pin      | Connect to                       |
|:---------|:---------------------------------|
| `V?`     | `V+` (voltage depends on module) |
| `GND`    | `GND`                            |
| `IO0/TX` | `MCU RX`                         |
| `IO1/RX` | `MCU TX`                         |
| `SPK+`   | Speaker positive lead            |
| `SPK-`   | Speaker negative lead            |

`MCU` should be your board or microprocessor, e.g. an Arduino board.

## Loading sound files

I recommend you name your files sequentially and put them in the root directory
of the drive unless you need something more fancy. The numbering should be as
follows:

```
00001.mp3
00002.mp3
00003.mp3
...
65535.mp3
```

That way the simplest functions that allow you to play a sound by `uint16_t`
should work.

You can also use paths and filenames but neither path nor filename should 
exceed 8 bytes. See the manual of your board for more specifics. If you have any
problems playing files, please try the simple way before filing issues.

## ONE_LINE support?

This library only supports UART at this time. It should be relatively straight
forward to add the `ONE_Line` protocol as found in the manual. Note however that
the entire communication protocol is different from the UART protocol. So there
is no particular benefit to adding it to this library. That said, if you plan
to implement it, we could discuss merging it with this library for convenience.

## Troubleshooting

### No sound

- Check the volume control, some boards have a potentiometer you can turn.
- Check the speaker connection.
- Test the speaker on another device and see if it is operational.
- Use the `checkDevice()` function to see if the device is responsive.
- See the [loading sound files](#loading-sound-files) chapter and apply the 
  simplest file structure to see the wiring etc is correct before filing issues.

