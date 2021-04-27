# Abstracted UART Control of DY-XXXX mp3 modules

This library abstracts all features described in the manual into a C++ class.

This library __does not support__ the `ONE_Line` protocol,
[more info](#one_line-support).

Although all features are implemented and should theoretically work, only
those in the examples directory are tested. Please create an issue if you have
problems.

This library was written in a hardware independent way. Which means it should
work on any device with a serial port, e.g. any Arduino, Espressif, ARM based
boards, probably even any computer.

There are Hardware Abstraction Layers (HAL) included for [Arduino](#arduino)
and [ESP-IDF](#esp-idf), for other boards you will have to
[provide one yourself](#hal) (PR is welcome!).

## Note on upgrading from version 3.x.x to version 4.x.x

__TL;DR:__ Change all occurances of enum constants of `DY::Device`,
`DY::PlayState`, `DY::Eq`, `DY::PlayMode` and `DY::PreviousDir` from upper case
to CamelCase, e.g.: `DY::Device::FLASH` to `DY::Device::Flash`.

One of the problems with C++ I learned about, the hard way is how the namespace
can still be polluted despite namespacing everything. As macro's are handled by
the preprocessor, any mention of a macro in the source, even if obviously
intended as code (well to a human) will be interpreted as a call to a macro.

Embedded devices' headers rely on macro's heavily and they aren't generally
prefixed. So users reported collisions between constants such as `USB` (it's
actually part of an enum class: `DY::Device::USB` which makes this even more
frustrating) and a macro defined for a board that has a USB port, also named
`USB`.

The only way around it is to drop the social convention (that was arguably
already advised against) of naming constants in capitals, the compiler
doesn't care about this convention, it's just usually done for readability.


## Modules should work (not exhaustive)

| Model name | Capacity | SD Card support | Amplifier       | Voltage | Tested |
|:-----------|:---------|:---------------:|:----------------|:-------:|:------:|
| DY-SV17F   | 32Mbit   | No              | 3-5W(4Ω/8Ω)     | 5VDC    | __Yes__|
| DY-SV8F    | 64Mbit   | No              | 3-5W(4Ω/8Ω)     | 5VDC    | No     |
| DY-HV20T   | NA       | Yes, Max. 32GB  | 3-5W(4Ω/8Ω)     | 5VDC    | No     |
| DY-HV8F    | 8Mbit    | No              | 10W(8Ω)/20W(4Ω) | 6-35VDC | No     |
| DY-HV20T   | NA       | Yes, Max. 32GB  | 10W(8Ω)/20W(4Ω) | 6-35VDC | No     |
| DY-SV5W    | NA       | Yes, Max. 32GB  | 3-5W(4Ω/8Ω)     | 5VDC    | __Yes__|

NOTE: I cannot guarantee that your board will work with the library. Nor that
a specific feature will work. I only have the `DY-SV17F` in my possession to
test at the time of writing. If something does not work, make an issue and/or
send me a pull request.

## Wiring the module

If you have a board with DIP switches, set `CON3` to on, `CON1` and `CON2`
should remain off. If your board doesn't have DIP switches (e.g. `DY-SV17F`
board), you have to connect 3 resistors of `10KOhm`, one from each `CON#` pin,
to:

| CON pin    | Connect to | Via      |
|:-----------|:-----------|:---------|
| `CON1`     | `GND`      | `10KOhm` |
| `CON2`     | `GND`      | `10KOhm` |
| `CON3`     | `3.3V`     | `10KOhm` |

The 3.3V pin is exposed by the board so you don't need to provide it.

Further make these connections:

| Pin      | Connect to                       | Via                         |
|:---------|:---------------------------------|:----------------------------|
| `V?`     | `V+` (voltage depends on module) |                             |
| `GND`    | `GND`                            |                             |
| `IO0/TX` | `MCU RX`                         | `1KOhm` if using a 5V board |
| `IO1/RX` | `MCU TX`                         | `1KOhm` if using a 5V board |
| `SPK+`   | Speaker positive lead            |                             |
| `SPK-`   | Speaker negative lead            |                             |

`MCU` should be your board or microprocessor, e.g. an Arduino board.

Note the logic levels should be 3.3V or at least limited by a `1KOhm`
resistor according to the module manual, or you could damage the module.

## HAL

__Arduino tl;dr;__
If you are using Arduino, skip this chapter and go to [Arduino](#arduino).

__ESP32 tl;dr;__
If you are using ESP-IDF, skip this chapter and go to [ESP-IDF](#esp-idf).

Because the library is hardware independent you might need to add a Hardware
Abstraction Layer (HAL) which sets up the serial port and that implements a
minimum of 2 functions `serialWrite()` and `serialRead()`, there is a HAL for
Arduino included, here's a __simplified__ version of it:

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

__Note:__
*This Arduino HAL is simplied, the included HAL does `SoftwareSerial`
as well which adds some complexity that I don't think should be in an example
like this. You can find the real HAL [here](src/DYPlayerArduino.cpp).*


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
controllers with small RAM, such as Atmega328 (used in many Arduino boards),
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

To find out how to use the Arduino HAL see
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

## API

The library abstracts sending binary commands to the module. There is manual
for the module floating around the web, which I am not sure is copy righted or
not, so I won't upload it here or link to it. Just google
`DY-SV17F manual & Datasheet` and you will probably find it.

The manual doesn't clearly name any of the commands so I came up with class
method names myself. The manual does contain a command description so I
included that in the list below. Notably the "check" commands, are named as
"get" commands to incidate they retrieve information from the module. Setting
you can change are applied by "set" commands.

### Set methods

Setting or playing any sound is not confirmed by the module so if you need
confirmation that the module is doing what is expected, you need to use one of
the get functions to verify that it's doing what you told it to. E.g. when you
play a sound, you can consequently call
[`DY::DYPlayer::getPlayingSound()`](#uint16_t-dydyplayergetplayingsound) to
check that it worked.

### Get methods

Get methods will send a command to the module and wait for an answer. If an
answer is not received a timeout will expire and subsequently an they will
return `0` for sound counts,

### Command/method list

| Command (from the manual)        |byte| Method name                              |
|:---------------------------------|:--:|:-----------------------------------------|
|Check the play state              |`0x01`|[`DY::PlayState::play_state_t` __DY::DYPlayer::checkPlayState__](#dyplay_state_t-dydyplayercheckplaystate)|
|Play                              |`0x02`|[`void` __DY::DYPlayer::play__](#void-dydyplayerplay)|
|Pause                             |`0x03`|[`void` __DY::DYPlayer::pause__](#void-dydyplayerpause)|
|Stop                              |`0x04`|[`void` __DY::DYPlayer::stop__](#void-dydyplayerstop)|
|Previous Music                    |`0x05`|[`void` __DY::DYPlayer::previous__](#void-dydyplayerprevious)|
|Next music                        |`0x06`|[`void` __DY::DYPlayer::next__](#void-dydyplayernext)|
|Play specified music              |`0x07`|[`void` __DY::DYPlayer::playSpecified__](#void-dydyplayerplayspecified)|
|Specified device and path play    |`0x08`|[`void` __DY::DYPlayer::playSpecifiedDevicePath__](#void-dydyplayerplayspecifieddevicepath)|
|Check Current Playing Device      |`0x0a`|[`DY::Device::device_t` __DY::DYPlayer::getPlayingDevice__](#dydevice_t-dydyplayergetplayingdevice)|
|Switch to selected device         |`0x0b`|[`void` __DY::DYPlayer::setPlayingDevice__](#void-dydyplayersetplayingdevice)|
|Check Number Of all Music         |`0x0c`|[`uint16_t` __DY::DYPlayer::getSoundCount__](#uint16_t-dydyplayergetsoundcount)|
|Check Current Music               |`0x0d`|[`uint16_t` __DY::DYPlayer::getPlayingSound__](#uint16_t-dydyplayergetplayingsound)|
|Previous folder directory (first) |`0x0e`|[`void` __DY::DYPlayer::previousDir__](#void-dydyplayerpreviousdir)|
|Previous folder directory (last)  |`0x0f`|[`void` __DY::DYPlayer::previousDir__](#void-dydyplayerpreviousdir)|
|End playing                       |`0x10`|[`void` __DY::DYPlayer::stopInterlude__](#void-dydyplayerstopinterlude)|
|Check the first music in folder   |`0x11`|[`uint16_t` __DY::DYPlayer::getFirstInDir__](#uint16_t-dydyplayergetfirstindir)|
|Check Number of music in folder   |`0x12`|[`uint16_t` __DY::DYPlayer::getSoundCountDir__](#uint16_t-dydyplayergetsoundcountdir)|
|Volume setting                    |`0x13`|[`void` __DY::DYPlayer::setVolume__](#void-dydyplayersetvolume)|
|Volume+                           |`0x14`|[`void` __DY::DYPlayer::volumeIncrease__](#void-dydyplayervolumeincrease)|
|Volume-                           |`0x15`|[`void` __DY::DYPlayer::volumeDecrease__](#void-dydyplayervolumedecrease)|
|Select specified file to interlude|`0x16`|[`void` __DY::DYPlayer::interludeSpecified__](#void-dydyplayerinterludespecified)|
|Select specified path to interlude|`0x17`|[`void` __DY::DYPlayer::interludeSpecifiedDevicePath__](#void-dydyplayerinterludespecifieddevicepath)|
|Cycle mode setting                |`0x18`|[`void` __DY::DYPlayer::setCycleMode__](#void-dydyplayersetcyclemode)|
|Cycle times setting               |`0x19`|[`void` __DY::DYPlayer::setCycleTimes__](#void-dydyplayersetcycletimes)|
|Set EQ                            |`0x1a`|[`void` __DY::DYPlayer::setEq__](#void-dydyplayerseteq)|
|Combination play setting          |`0x1b`|[`void` __DY::DYPlayer::combinationPlay__](#void-dydyplayercombinationplay) |
|End Combination play              |`0x1c`|[`void` __DY::DYPlayer::endCombinationPlay__](#void-dydyplayerendcombinationplay)|
|Select file but do not play       |`0x1f`|[`void` __DY::DYPlayer::select__](#void-dydyplayerselect)|

UART commands are formed by:

`aa [cmd] [len] [byte_1..n] [crc]`

Where:

- `aa` Start a command
- `[cmd]` A command from the list, e.g. `01` to check the play state.
- `[len]` The length of the amount of bytes sent as an argument.
- `[byte_1..n]` All bytes uses as arguments, some argument are 1 byte, such as
  for selecting the storage device, some are 2 bytes long such as specifying a
  song by number (`uint16_t` value, between `0` and `65535`), sometimes it's a
  combination, or a path.
- `[crc]` The sum of all bytes in the entire command as a `uint8_t`.

### API documentation

#### [`DY::play_state_t`](#typedef-enum-class-dyplay_state_t) DY::DYPlayer::checkPlayState(..)

Check the current play state can, be called at any time.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | [`DY::play_state_t`](#typedef-enum-class-dyplay_state_t) |  | Play status: A [`DY::PlayState`](#typedef-enum-class-dyplay_state_t) e.g DY::PlayState::Stopped, DY::PlayState::Playing, etc  |


#### `void` DY::DYPlayer::play(..)

Play the currently selected file from the start.




#### `void` DY::DYPlayer::pause(..)

Set the play state to paused.




#### `void` DY::DYPlayer::stop(..)

Set the play state to stopped.




#### `void` DY::DYPlayer::previous(..)

Play the previous file.




#### `void` DY::DYPlayer::next(..)

Play the next file.




#### `void` DY::DYPlayer::playSpecified(..)

Play a sound file by number, number sent as 2 bytes.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `uint16_t` | `number` | number of the file, e.g. `1` for `00001.mp3`  |


#### `void` DY::DYPlayer::playSpecifiedDevicePath(..)

Play a sound file by device and path.
Path may consist of up to 2 nested directories of 8 bytes long and a
file name of 8 bytes long excluding the extension of 4 bytes long.
If your directory names are shorter you can use more nesting. Use no
more than 36 bytes for your paths. If you require more, check the
readme, chapter: Memory use.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | [`DY::device_t`](#typedef-enum-class-dydevice_t) | `device` | device A [`DY::Device member`](#typedef-enum-class-dydevice_t) e.g  `DY::Device::Flash` or `DY::Device::Sd`  |
| __param__ | `char` | `path` | path pointer to the path of the file (asbsolute)  |


#### [`DY::device_t`](#typedef-enum-class-dydevice_t) DY::DYPlayer::getPlayingDevice(..)

Get the storage device that is currently used for playing sound files.



|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | [`DY::device_t`](#typedef-enum-class-dydevice_t) |  | a [`DY::Device` member](#typedef-enum-class-dydevice_t) e.g  `DY::Device::Flash` or `DY::Device::Sd`  |


#### `void` DY::DYPlayer::setPlayingDevice(..)

Set the device number the module should use.
Tries to set the device but no guarantee is given, use `getDevice()`
to check the actual current storage device.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | [`DY::device_t`](#typedef-enum-class-dydevice_t) | `device` | device A [`DY::Device` member](#typedef-enum-class-dydevice_t) e.g  `DY::Device::Flash` or `DY::Device::Sd`  |


#### `uint16_t` DY::DYPlayer::getSoundCount(..)

Get the amount of sound files on the current storage device.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | `uint16_t` |  | number of sound files  |


#### `uint16_t` DY::DYPlayer::getPlayingSound(..)

Get the currently playing file by number.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | `uint16_t` |  | number of the file currently playing  |


#### `void` DY::DYPlayer::previousDir(..)

Select previous directory and start playing the first or last song.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `playDirSound_t` | `song` | song Play `DY::PreviousDir::FirstSound` o DY::PreviousDir::LastSoun  |


#### `uint16_t` DY::DYPlayer::getFirstInDir(..)

Get number of the first song in the currently selected directory.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | `uint16_t` |  | number of the first song in the currently selected directory  |


#### `uint16_t` DY::DYPlayer::getSoundCountDir(..)

Get the amount of sound files in the currently selected directory.
NOTE: Excluding files in sub directories.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __return__ | `uint16_t` |  | number of sound files in currently selected directory  |


#### `void` DY::DYPlayer::setVolume(..)

Set the playback volume between 0 and 30.
Default volume if not set: 20.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `uint8_t` | `volume` | volume to set (0-30  |


#### `void` DY::DYPlayer::volumeIncrease(..)

Increase the volume.




#### `void` DY::DYPlayer::volumeDecrease(..)

Decrease the volume.




#### `void` DY::DYPlayer::interludeSpecified(..)

Play an interlude file by device and number, number sent as 2 bytes.
Note from the manual: "Music interlude" only has level 1. Continuous
interlude will cover the previous interlude (the interlude will be
played immediately). When the interlude is finished, it will return to
the first interlude breakpoint and continue to play.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | [`DY::device_t`](#typedef-enum-class-dydevice_t) | `device` | device A [`DY::Device member`](#typedef-enum-class-dydevice_t) e.g  `DY::Device::Flash` or `DY::Device::Sd`  |
| __param__ | `uint16_t` | `number` | number of the file, e.g. `1` for `00001.mp3`  |


#### `void` DY::DYPlayer::interludeSpecifiedDevicePath(..)

Play an interlude by device and path.
Note from the manual: "Music interlude" only has level 1. Continuous
interlude will cover the previous interlude (the interlude will be
played immediately). When the interlude is finished, it will return to
the first interlude breakpoint and continue to play.

Path may consist of up to 2 nested directories of 8 bytes long and a
file name of 8 bytes long excluding the extension of 4 bytes long.
If your directory names are shorter you can use more nesting. Use no
more than 36 bytes for your paths. If you require more, check the
readme, chapter: Memory use.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | [`DY::device_t`](#typedef-enum-class-dydevice_t) | `device` | device A [`DY::Device member`](#typedef-enum-class-dydevice_t) e.g  `DY::Device::Flash` or `DY::Device::Sd`  |
| __param__ | `char` | `path` | path pointer to the path of the file (asbsolute)  |


#### `void` DY::DYPlayer::stopInterlude(..)

Stop the interlude and continue playing.
Will also stop the current sound from playing if interlude is not
active.




#### `void` DY::DYPlayer::setCycleMode(..)

Sets the cycle mode.
See [`DY::play_mode_t`](#typedef-enum-class-dyplay_mode_t) for modes
and meaning.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `play_mode_t` | `mode` | mode The cycle mode to set  |


#### `void` DY::DYPlayer::setCycleTimes(..)

Set how many cycles to play when in cycle modes 0, 1 or 4 (repeat
modes).


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `uint16_t` | `cycles` | cycles The cycle count for repeat modes  |


#### `void` DY::DYPlayer::setEq(..)

Set the equalizer setting.
See [`DY::eq_t`](#typedef-enum-class-dyeq_t) for settings.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | [`DY::eq_t`](#typedef-enum-class-dyeq_t) | `eq` | eq The equalizer setting  |


#### `void` DY::DYPlayer::select(..)

Select a sound file without playing it.


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `uint16_t` | `number` | number of the file, e.g. `1` for `00001.mp3`  |


#### `void` DY::DYPlayer::combinationPlay(..)

Combination play allows you to make a playlist of multiple sound files.

You could use this to combine numbers e.g.: "fourthy-two" where you
have samples for "fourthy" and "two".

This feature has a particularly curious parameters, you have to
specify the sound files by name, they have to be named by 2 numbers
and an extension, e.g.: `01.mp3` and specified by `01`. You should
pass them as an array pointer. You need to put the files into a
directory that can be called `DY`, `ZH or `XY`, you will have to check
the manual that came with your module, or try all of them. There may
well be more combinations! Also see
[Loading sound files](#loading-sound-files).

E.g.
```cpp
const char * sounds[2][3] = { "01", "02" };
DY::DYPlayer::combinationPlay(sounds, 2);
````


|           | __Type__ | __Name__ | __Description__  |
|:----------|:---------|:---------|:-----------------|
| __param__ | `char` | `sounds` | sounds An array of char[2] containing the names of sounds t play in order  |
| __param__ | `uint8_t` | `len` | len The length of the passed array  |


#### `void` DY::DYPlayer::endCombinationPlay(..)

End combination play.




#### typedef enum class DY::device_t

Storage devices reported by module and to choose from when selecting a
storage device.

| const | value | description |
|:------|:------|:------------|
| `DY::Device::Usb` | `0x00` | USB Storage device. |
| `DY::Device::Sd` | `0x01` | SD Card. |
| `DY::Device::Flash` | `0x02` | Onboard flash chip (usually winbond 32, 64Mbit flash). |
| `DY::Device::Fail` | `0xfe` | UART failure, can't be `-1` (so this can be uint8_t). |
| `DY::Device::NoDevice` | `0xff` | No storage device is online. |

This enum class is based off uint8_t.


#### typedef enum class DY::play_state_t

The current module play state.

| const | value | description |
|:------|:------|:------------|
| `DY::PlayState::Fail` | `-1` | UART Failure, can be a connection or a CRC problem. |
| `DY::PlayState::Stopped` | `0` |  |
| `DY::PlayState::Playing` | `1` |  |
| `DY::PlayState::Paused` | `2` |  |

This enum class is based off int8_t.


#### typedef enum class DY::eq_t

Equalize settings.

| const | value |
|:------|:------|
| `DY::Eq::Normal` | `0x00` |
| `DY::Eq::Pop` | `0x01` |
| `DY::Eq::Rock` | `0x02` |
| `DY::Eq::Jazz` | `0x03` |
| `DY::Eq::Classic` | `0x04` |

This enum class is based off uint8_t.

#### typedef enum class DY::play_mode_t

Play modes are basically whatever you commonly find on a media player,
i.e.:
Repeat 1, Repeat all, Repeat list (dir), playlist (by dir), random play.

The default is perhaps somewhat unexpected: `DY::PlayMode::OneOff`. Often
these modules will be used in toys or information displays where you can
press a button and hear a corresponding sound. To get default media player
behaviour, you should probably set `DY::PlayMode::Sequence` to just continue
playing the next song until all are played or skipped, then stop.


| const                       | value  | description                                                   |
| :-------------------------- | :----- | :------------------------------------------------------------ |
| `DY::PlayMode::Repeat`      | `0x00` | Play all music in sequence, and repeat.                       |
| `DY::PlayMode::RepeatOne`   | `0x01` | Repeat current sound.                                         |
| `DY::PlayMode::OneOff`      | `0x02` | Play sound file and stop.                                     |
| `DY::PlayMode::Random`      | `0x03` | Play random sound file.                                       |
| `DY::PlayMode::RepeatDir`   | `0x04` | Repeat current directory.                                     |
| `DY::PlayMode::RandomDir`   | `0x05` | Play random sound file in current folder.                     |
| `DY::PlayMode::SequenceDir` | `0x06` | Play all sound files in current folder in sequence, and stop. |
| `DY::PlayMode::Sequence`    | `0x07` | Play all sound files on device in sequence, and stop.         |

## Loading sound files

### Normal Playback

For normal playback of sound files, I recommend you name your files
sequentially and put them in the root directory of the drive unless you need
something more fancy. The numbering should be as follows:

```
00001.mp3
00002.mp3
00003.mp3
...
65535.mp3
```

#### Important: files do not play in order of named sequence

The [`DY::DYPlayer::playSpecified`](#void-dydyplayerplayspecified)
function will __not__ necessarily play the sound file named `00001.MP3` when
asked to play sound file `1`. The module will look for the first sound file
found in the filesystem. It's not using the file name, neither does it order by
file name. To be sure the sound files play in the expected order:

1. Empty the storage device.
2. Make sure there aren't any trash files on the storage device.
3. Copy the files in the order they should be played (it helps to have them
   named sequentially, sort by name and copy the files, usually they will be
   copied in sequence order.

Please don't file bugs stating there is a bug in the
[`DY::DYPlayer::playSpecified`](#void-dydyplayerplayspecified) function
because `00001.MP3` doesn't play when `1` is sent to the module. It's not a bug
in this library, it's how the module works. If you find that the above
information is not correct, or if you have suggestions for improvements, please
do create an issue..

#### By file path

You can also use paths and filenames but neither directory nor filename should
exceed 8 bytes. You can put the files in directories, but don't make a lot of
nested directories. Files in the root are played by specifying the path as
`/00001.MP3` (note the `/`).

#### Use normal paths

In the module's manual you can find that paths are required to be separated by
`*/` and `*` in place of `.`, upper case characters, etc., etc. Forget about
all of that, just supply paths as you are used to, case insensitive, no funny
character substitutions. The library will handle the required conversions.

### Combination play

For combination play, you will have to add files with names of 2 characters plus
the extension, e.g.: `01.mp3` into a directory that can be called `XY`, `ZH,
`DY`, you will have to check the manual that came with your module, or try all
of them. There may well be more options! Most manuals claim it should be `DY`
but most people reported that `XY` actually works, so try: `/DY/##.MP3` first.

If you find out what the letter combination on your board is, please add an
issue with the directory name and the module model name, so I can add it here
to help others.

## ONE_LINE support?

This library only supports UART at this time. It should be relatively straight
forward to add the `ONE_Line` protocol as found in the manual. Note however that
the entire communication protocol is different from the UART protocol. So there
is no particular benefit to adding it to this library. That said, if you plan
to implement it, we could discuss merging it with this library for convenience.

## Troubleshooting

### No communication with the board

- Check power is supplied to the module.
- Check that TX on your Arduino board connects to RX on the module and vice
  versa, check that you connected the serial lines through resistors if your
  Arduino runs on 5V. See [Wiring the module](#wiring-the-module).
- Check the configuration of the board, either by connecting the correct value
  of resistors to the correct `CON#` pins or, if your board has them, the DIP
  switches are set to the correct value. See
  [Wiring the module](#wiring-the-module).
- If you share the serial port between the computer for flashing and connecting
  to the module, disconnect the cable (FTDI or USB cable) from the board before
  booting it. Anything on the module TX line besides your Arduino board will
  prevent data from being received.
- Check there are compatible sound files on the modules storage device, maybe
  it's connecting, but doesn't have a sound to play, use
  [`DY::DYPlayer::playSpecified`](#void-dydyplayerplayspecified) to play
  sound file `00001.MP3` because it's more fault tolerant than by path
  functions.

### No sound

- Check the speaker connection.
- Check the volume control, some boards have a potentiometer you can turn.
- Test the speaker on another device and see if it is operational.
- Use the
  [`DY::DYPlayer::getPlayingDevice`](#dydevice_t-dydyplayergetplayingdevice)
  function to see if the device is responsive.
- See the [loading sound files](#loading-sound-files) chapter and apply the
  simplest file structure to see the wiring etc is correct before filing issues.
- Check there are compatible sound files on the modules storage device maybe
  it doesn't have a sound to play, use
  [`DY::DYPlayer::playSpecified`](#void-dydyplayerplayspecified) to play
  sound file `00001.MP3` because it's more fault tolerant than by path
  functions.

### Unexpected behaviour

- I asked the module to play `1` but `00001.MP3` is not played, I hear a
  different sound. This is not a bug in this library, check:
  [Important: files do not play in order of named sequence](#important-files-do-not-play-in-order-of-named-sequence)
