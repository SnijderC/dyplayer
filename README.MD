# NOTE: Use the release branch README

Always reference the
[README.MD](https://github.com/SnijderC/dyplayer/blob/release/README.MD) in the
[`release`](https://github.com/SnijderC/dyplayer/tree/release) branch as a
source of documentation if you download the library through
[platform.io](https://platformio.org).

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

There are Hardware Abstraction Layers (HAL) included for [Arduino](#arduino)
and [ESP-IDF](#esp-idf), for other boards you will have to
[provide one yourself](#hal) (PR is welcome!).

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

## HAL

__Arduino tl;dr;__
If you are using Arduino, skip this chapter and go to [Arduino](#arduino).

__ESP32 tl;dr;__
If you are using ESP-IDF, skip this chapter and go to [ESP-IDF](#esp-idf).

Because the library is hardware independent you might need to add a Hardware
Abstraction Layer (HAL) which sets up the serial port and that implements a
minimum of 2 functions `serialWrite()` and `serialRead()`, this is the included
Arduino example:

``` C++
// player.hpp
#include <Arduino.h>
#include "DYPlayer.h"
namespace DY {
  class Player: public DYPlayer {
    public:
      HardwareSerial *port;
      Player();
      Player(HardwareSerial* port);
      void begin();
      void serialWrite(uint8_t *buffer, uint8_t len);
      bool serialRead(uint8_t *buffer, uint8_t len);
  };
}


//player.cpp
#include "player.hpp"
#include "DYPlayerArduino.h"

namespace DY {
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
}

```

Steps:

1. Define a class that extends the `DY::DYPlayer` class.
1. Define constructors that set up the serial port. On some platforms you will
   need to setup your serial port after some other things are initialized, e.g.
   on Arduino, then define an additional `DY::Player::begin()` (or
   e.g. `DY::Player::init()`) to finish initialisation.
1. Define functions for `serialWrite()` and `serialRead()` according to the
   board and the framework you use.

## Memory use

This library uses a little memory as possible to play nice with micro
controllers with small RAM, such as Atmega238 (used in many Arduino boards),
which has 2K RAM.

To keep memory usage low, avoid using the functions that take `char *path`
arguments. If you do not intend to play sounds by file name, you can skip the
rest of this chapter. If you do, keep reading.

The `char *path` arguments will always use more RAM than the `uint16_t`
arguments, obviously but this is compounded by an odd requirement of the player
modules. I.e. the paths to files on flash/SD card have to be defined different
than usual, e.g.
`/SONGS/IN/A/PATH/00001.MP3` should be specified as:
`/SONGS*/IN*/A*/PATH*/00001*MP3`

Analysing this:

- Paths normally end in `/` but an additional `*` is required;
- except for the root level.
- Period in before the extension should be replaced by `*` as well.
- The new path is 4 bytes longer than the specified path.

The conversion is done by the library but this means that the path is allocated
twice in memory, once by you, once by the library and the latter needs to have
more capacity (in this case 4 bytes). The libray can keep the second string's
memory in 2 ways: in heap or in stack memory.

Stack memory is not dynamic, i.e.: the required amount of bytes should be known
at compile time, which means more than the expected amount of bytes should
already be reserved, which is wasteful. Aside from being wastelful, the path
could be really short (most likely, e.g. `/00001.MP3` or something like
`/SFX/00001.MP3`), or it could be really long..

Putting the path in dynamically assigned heap memory fixes all of that, the
library can count the amount of `/` in the path and make a variable exactly
long enough for the converted path. However we should always be wary of
[Heap fragmentation](https://cpp4arduino.com/2018/11/06/what-is-heap-fragmentation.html).
In short to assign memory, a contgious block of it needs to be available.
Assigning chunks of memory and then freeing them leaves holes in the memory that
may be be too small to use again later. This will gradually lead to problems.
You may need to reset your device after several minutes, hours or days because
the program can't allocate heap memory any more.

So, by default the library will reserve stack memory. The amount is based on
some assumptions:

- The manual of the sound modules states that paths and file names may be up to
  8 characters long.
- The library assumes that you will not nest more than 2 directories.
- Extentions can be `.MP3` or `.WAV`, so always 4 bytes.

So we come to:

```
/ dir */ dir */ file * ext

1 + 8 + 2 + 8 + 2 + 8 + 1 + 4 = 34
```

Let's round that up to `40` and you could even have some more nesting as long as
the directory names are small enough.

The library will therefore define `DY_PATH_LEN` as `40`, you can override that
if you need more, or if you want to save a few bytes of precious memory. Note
that `40` is the maximum length of the path __after conversion__ to the funcky
format required by the module. The amount of bytes you may use with this default
is set at `36`.

Alternatively, if you have a more capable device and/or you can use virtual
memory, you can define `DY_PATHS_IN_HEAP` to use heap memory instead of reserved
stack memory.

NOTE: On Arduino, you can wrap your strings in `F()` to tell the compiler you
want the string stored in flash, as opposed to RAM (default), which will save
you even more RAM.

## Arduino

Because this is included, on Arduino you can just include the
`DYPlayerArduino.h` header file and use the module.

You may use any Serial port on the board, you will have to pass it to the
`begin()` function. Like this:

```c++
DY::Player player(&Serial2);
```

To use the Arduino HAL see
[PlaySoundByNumber.ino](examples/PlaySoundByNumber/PlaySoundByNumber.ino).

## ESP-IDF

Because this is included, on Arduino you can just include the
`DYPlayerESP32.h` header file and use the module.

You may use any Serial port on the board, and any 2 pins for TX and RX.
E.g.: user `UART_NUM_2` on pins TX: 18, RX: 19:

```c++
DY::Player player(UART_NUM_2, 18, 19);
```

You can find an example here:
[PlaySounds.cpp](examples/esp32/PlaySounds.cpp).

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
