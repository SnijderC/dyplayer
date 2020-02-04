# NOTE: Use the release branch README

Always reference the
[README.MD](https://github.com/SnijderC/dyplayer/blob/release/README.MD) in the
[`release`](https://github.com/SnijderC/dyplayer/tree/release) branch as a
source of documentation if you download the library through
[platform.io](https://platformio.org).

__Library has and probably will be under heavy development for a while. The API
could still change significantly. Stick with your major version number if you
don't want to have breaking changes.__

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
to connect 3 resistors of `10KOhm`, one from each `CON#` pin, to:

| CON pin    | Connect to | Via      |
|:-----------|:-----------|:---------|
| `CON1`     | `GND`      | `10KOhm` |
| `CON2`     | `GND`      | `10KOhm` |
| `CON3`     | `3.3V`     | `10KOhm` |

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
|Check the play state              |`0x01`|[`DY::PlayState::play_state_t` __DY::DYPlayer::checkPlayState__](#dyplaystateplaystatet-dydyplayercheckplaystate)|
|Play                              |`0x02`|[`void` __DY::DYPlayer::play__](#void-dydyplayerplay)|
|Pause                             |`0x03`|[`void` __DY::DYPlayer::pause__](#void-dydyplayerpause)|
|Stop                              |`0x04`|[`void` __DY::DYPlayer::stop__](#dyplaystatet-dydyplayerstop)|
|Previous Music                    |`0x05`|[`void` __DY::DYPlayer::previous__](#void-dydyplayerprevious)|
|Next music                        |`0x06`|[`void` __DY::DYPlayer::next__](#void-dydyplayernext)|
|Play specified music              |`0x07`|[`void` __DY::DYPlayer::playSpecified__](#void-dydyplayerplayspecified)|
|Specified device and path play    |`0x08`|[`void` __DY::DYPlayer::playSpecifiedDevicePath__](#void-dydyplayerplayspecifieddevicepath)|
|Check Device Online               |`0x09`|[`DY::Device::device_t` __DY::DYPlayer::getDeviceOnline__](#dydevicet-dydyplayergetdeviceonline)|
|Check Current Playing Device      |`0x0a`|[`DY::Device::device_t` __DY::DYPlayer::getPlayingDevice__](#dydevicet-dydyplayergetplayingdevice)|
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
|Set EQ                            |`0x1a`|[`void` __DY::DYPlayer::SetEq__](#void-dydyplayerseteq)|
|Combination play setting          |`0x1b`| Not yet implemented |
|End Combination play              |`0x1c`| Not yet implemented |
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

#### [DY::play_state_t](#typedef-enum-class-dyplay_state_t) DY::DYPlayer::checkPlayState(..)

Check the current play state can, be called at any time.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | [DY::play_state_t](#typedef-enum-class-dyplay_state_t) | Play status: A [DY::PlayState](#typedef-enum-classdyplaymodeplaymode_t) e.g DY::PlayMode::STOPPED, DY::PlayMode::PLAYING, etc  |


#### `void` DY::DYPlayer::play(..)

Play the currently selected file from the start.




#### `void` DY::DYPlayer::pause(..)

Set the play state to paused.




#### `void` DY::DYPlayer::stop(..)

Set the play state to stopped.




#### `void` DY::DYPlayer::previous(..)

Play the preivous file.




#### `void` DY::DYPlayer::next(..)

Play the next file.




#### `void` DY::DYPlayer::playSpecified(..)

Play a sound file by number, number sent as 2 bytes.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`  |


#### `void` DY::DYPlayer::playSpecifiedDevicePath(..)

Play a sound file by device and path.
Path may consist of up to 2 nested directories of 8 bytes long and a
file name of 8 bytes long excluding the extension of 4 bytes long.
If your directory names are shorter you can use more nesting. Use no
more than 36 bytes for your paths. If you require more, check the
readme, chapter: Memory use.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [DY::Device member](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::SD`  |
| __param__ | `char` | path pointer to the path of the file (asbsolute)  |


#### [DY::device_t](#typedef-enum-class-dydevice_t) DY::DYPlayer::getDeviceOnline(..)

Get the device number the module is currently using.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [`DY::Device member`](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::NO_DEVICE`  |


#### [DY::device_t](#typedef-enum-class-dydevice_t) DY::DYPlayer::getPlayingDevice(..)

Get the storage device that is currently used for playing sound files.



|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [`DY::Device member`](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH`  |


#### `void` DY::DYPlayer::setPlayingDevice(..)

Set the device number the module should use.
Tries to set the device but no guarantee is given, use `getDevice()`
to check the actual current storage device.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [DY::Device member](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::SD`  |


#### `uint16_t` DY::DYPlayer::getSoundCount(..)

Get the amount of sound files on the current storage device.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | `uint16_t` | number of sound files  |


#### `uint16_t` DY::DYPlayer::getPlayingSound(..)

Get the currently playing file by number.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | `uint16_t` | number of the file currently playing  |


#### `void` DY::DYPlayer::previousDir(..)

Select previous directory and start playing the first or last song.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `playDirSound_t` | song Play DY::PreviousDir::FIRST_SOUND o DY::PreviousDir::LAST_SOUN  |


#### `uint16_t` DY::DYPlayer::getFirstInDir(..)

Get number of the first song in the currently selected directory.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | `uint16_t` | number of the first song in the currently selected directory  |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART  |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails  |


#### `uint16_t` DY::DYPlayer::getSoundCountDir(..)

Get the amount of sound files in the currently selected directory.
NOTE: Excluding files in sub directories.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __return__ | `uint16_t` | number of sound files in currently selected directory  |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART  |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails  |


#### `void` DY::DYPlayer::setVolume(..)

Set the playback volume between 0 and 30.
Default volume if not set: 20.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | volume to set (0-30  |


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


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [DY::Device member](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::SD`  |
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`  |


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


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [DY::Device member](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::SD`  |
| __param__ | `char` | path pointer to the path of the file (asbsolute)  |


#### `void` DY::DYPlayer::stopInterlude(..)

Stop the interlude and continue playing.
Will also stop the current sound from playing if interlude is not
active.




#### `void` DY::DYPlayer::setCycleMode(..)

Sets the cycle mode.
See [play_mode_t](#typedef-enum-dyplaymodet) for modes and meaning.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `play_mode_t` | mode The cycle mode to set  |


#### `void` DY::DYPlayer::setCycleTimes(..)

Set how many cycles to play when in cycle modes 0, 1 or 4 (repeat
modes).


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint16_t` | cycles The cycle count for repeat modes  |


#### `void` DY::DYPlayer::setEq(..)

Set the equalizer setting.
See [play_mode_t](#typedef-enum-dyeqt) for settings.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | [DY::eq_t](#typedef-enum-class-dyeq_t) | eq The equalizer setting  |


#### `void` DY::DYPlayer::select(..)

Select a sound file without playing it.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`  |


#### `bool` DY::DYPlayer::validateCrc(..)

Validate data buffer with CRC byte (last byte should be the CRC byte).


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to calculate the CRC for  |
| __param__ | `uint8_t` | len of data  |
| __return__ | `bool` | boolean indicating CRC is correct (true) or incorrect (false)  |


#### `void` DY::DYPlayer::sendCommand(..)

Send a command to the module, adds a CRC to the passed buffer.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to send to the module  |
| __param__ | `uint8_t` | len of data  |


#### `void` DY::DYPlayer::sendCommand(..)

Send a command to the module, pass a static crc.
Use this to optimize speed for static commands.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to send to the module  |
| __param__ | `uint8_t` | len of data  |
| __param__ | `uint8_t` | crc Precalculated CRC byte  |


#### `bool` DY::DYPlayer::getResponse(..)

Get a response to a command.
Reads data from UART, validates the CRC, and puts it in the buffer.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | buffer pointer for the bytes to receive  |
| __param__ | `uint8_t` | len of buffer  |
| __return__ | `bool` | False on communication failure  |


#### `void` DY::DYPlayer::byPathCommand(..)

Send command with converted paths to  weird format required by the
modules.

- Any dot in a path should become a star (`*`)
- Path ending slashes should be have a star prefix, except root.

E.g.: /SONGS1/FILE1.MP3 should become: /SONGS1﹡/FILE1*MP3
NOTE: This comment uses a unicode * look-a-alike (﹡) because ﹡/ end the
comment.


|           | __Type__ | __Description__  |
|:----------|:---------|:-----------------|
| __param__ | `uint8_t` | command The command to send  |
| __param__ | [DY::device_t](#typedef-enum-class-dydevice_t) | device A [DY::Device member](#typedef-enum-classdydevicedevice_t) e.g  `DY::Device::FLASH` or `DY::Device::SD`  |
| __param__ | `char` | path of the file (asbsolute)  |


#### typedef enum class DY::device_t

Storage devices reported by module and to choose from when selecting a
storage device.

| const | value | description |
|:------|:------|:------------|
| `DY::Device::USB` | `0x00` | USB Storage device. |
| `DY::Device::SD` | `0x01` | SD Card. |
| `DY::Device::FLASH` | `0x02` | Onboard flash chip (usually winbond 32, 64Mbit flash). |
| `DY::Device::DUNNO` | `0x04` | Some boards report this as the online storage device.. |
| `DY::Device::FAIL` | `0xfe` | UART failure, can't be `-1` (so this can be uint8_t). |
| `DY::Device::NO_DEVICE` | `0xff` | No storage device is online. |

This enum class is based off uint8_t.


#### typedef enum class DY::play_state_t

The current module play state.

| const | value | description |
|:------|:------|:------------|
| `DY::PlayState::FAIL` | `-1` | UART Failure, can be a connection or a CRC problem. |
| `DY::PlayState::STOPPED` | `0` |  |
| `DY::PlayState::PLAYING` | `1` |  |
| `DY::PlayState::PAUSED` | `2` |  |

This enum class is based off int8_t.


#### typedef enum class DY::eq_t

Equalize settings.

| const | value |
|:------|:------|
| `DY::Eq::NORMAL` | `0x00` |
| `DY::Eq::POP` | `0x01` |
| `DY::Eq::ROCK` | `0x02` |
| `DY::Eq::JAZZ` | `0x03` |
| `DY::Eq::CLASSIC` | `0x04` |

This enum class is based off uint8_t.


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
- Use the [`DY::DYPlayer::getDeviceOnline`](#devicet-dydyplayergetdeviceonline)
  function to see if the device is responsive.
- See the [loading sound files](#loading-sound-files) chapter and apply the
  simplest file structure to see the wiring etc is correct before filing issues.
