#### [`DY::play_state_t`](#enum-typedef-DYplaystatet) DY::DYPlayer::checkPlayState(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | [`DY::play_state_t`](#enum-typedef-DYplaystatet) | Play status: DY::STOPPED, DY::PLAYING, DY::PAUSED. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


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

number of the file, e.g. `1` for `00001.mp3`.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`. |


#### `void` DY::DYPlayer::playSpecifiedDevicePath(..)

path pointer to the path of the file (asbsolute).
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device number USB, SD, FLASH |
| __param__ | `char` | path pointer to the path of the file (asbsolute). |


#### [`DY::device_t`](#enum-typedef-DYdevicet) DY::DYPlayer::getDeviceOnline(..)

device DY::USB, DY::SD, DY::FLASH.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device DY::USB, DY::SD, DY::FLASH. |


#### [`DY::device_t`](#enum-typedef-DYdevicet) DY::DYPlayer::getPlayingDevice(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device DY::USB, DY::SD, DY::FLASH, DY::NO_DEVICE. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `void` DY::DYPlayer::setPlayingDevice(..)

device number USB, SD, FLASH
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device number USB, SD, FLASH |


#### `uint16_t` DY::DYPlayer::getSoundCount(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | `uint16_t` | number of sound files. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `uint16_t` DY::DYPlayer::getPlayingSound(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | `uint16_t` | number of the file currently playing. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `uint16_t` DY::DYPlayer::getFirstInDir(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | `uint16_t` | number of the first song in the currently selected directory. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `uint16_t` DY::DYPlayer::getSoundCountDir(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __return__ | `uint16_t` | number of sound files in currently selected directory. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `void` DY::DYPlayer::setVolume(..)

volume to set (0-30)
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | volume to set (0-30) |


#### `void` DY::DYPlayer::volumeIncrease(..)

Increase the volume.



#### `void` DY::DYPlayer::volumeDecrease(..)

Decrease the volume.



#### `void` DY::DYPlayer::interludeSpecified(..)

number of the file, e.g. `1` for `00001.mp3`.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device number USB, SD, FLASH |
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`. |


#### `void` DY::DYPlayer::interludeSpecifiedDevicePath(..)

path pointer to the path of the file (asbsolute).
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device number USB, SD, FLASH |
| __param__ | `char` | path pointer to the path of the file (asbsolute). |


#### `void` DY::DYPlayer::stopInterlude(..)

Stop the interlude and continue playing.
Will also stop the current sound from playing if interlude is not
active.



#### `void` DY::DYPlayer::setCycleMode(..)

mode The cycle mode to set.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::play_mode_t`](#enum-typedef-DYplaymodet) | mode The cycle mode to set. |


#### `void` DY::DYPlayer::setCycleTimes(..)

cycles The cycle count for repeat modes.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint16_t` | cycles The cycle count for repeat modes. |


#### `void` DY::DYPlayer::setEq(..)

eq The equalizer setting.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | [`DY::eq_t`](#enum-typedef-DYeqt) | eq The equalizer setting. |


#### `void` DY::DYPlayer::select(..)

number of the file, e.g. `1` for `00001.mp3`.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint16_t` | number of the file, e.g. `1` for `00001.mp3`. |


#### `bool` DY::DYPlayer::validateCrc(..)

boolean indicating CRC is correct (true) or incorrect (false).
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to calculate the CRC for. |
| __param__ | `uint8_t` | len of data. |
| __return__ | `bool` | boolean indicating CRC is correct (true) or incorrect (false). |


#### `void` DY::DYPlayer::sendCommand(..)

len of data.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to send to the module. |
| __param__ | `uint8_t` | len of data. |


#### `void` DY::DYPlayer::sendCommand(..)

crc Precalculated CRC byte.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | data pointer to bytes to send to the module. |
| __param__ | `uint8_t` | len of data. |
| __param__ | `uint8_t` | crc Precalculated CRC byte. |


#### `void` DY::DYPlayer::getResponse(..)

ExcCrcFail when CRC check on the data fails.
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | buffer pointer for the bytes to receive. |
| __param__ | `uint8_t` | len of buffer. |
| __throw__ | ExcUartFail | ExcUartFail when data can't be read from UART. |
| __throw__ | ExcCrcFail | ExcCrcFail when CRC check on the data fails. |


#### `void` DY::DYPlayer::byPathCommand(..)

path of the file (asbsolute).
|           | __Type__ | __Description__  |
|:----------|:--------:|:-----------------|
| __param__ | `uint8_t` | command The command to send. |
| __param__ | [`DY::device_t`](#enum-typedef-DYdevicet) | device number DY::USB, DY::SD, DY::FLASH. |
| __param__ | `char` | path of the file (asbsolute). |


#### enum typedef `DY::device_t`

Storage devices reported by module and to choose from when selecting a
storage device.


| const | value | description |
|:------|:------|:------------|
| `USB` | `0x00` | USB Storage device. |
| `SD` | `0x01` | SD Card. |
| `FLASH` | `0x02` | Onboard flash chip (usually winbond 32, 64Mbit flash). |
| `DUNNO` | `0x04` | Some boards report this as the online storage device.. |
| `NO_DEVICE` | `0xff` | No storage device is online. |


#### enum typedef `DY::play_state_t`

The current module play state.


| const | value |
|:------|:------|
| `STOPPED` | `0x00` |
| `PLAYING` | `0x01` |
| `PAUSED` | `0x02` |


#### enum typedef `DY::eq_t`

Equalize settings.


| const | value |
|:------|:------|
| `EQ_NORMAL` | `0x00` |
| `EQ_POP` | `0x01` |
| `EQ_ROCK` | `0x02` |
| `EQ_JAZZ` | `0x03` |
| `EQ_CLASSIC` | `0x04` |


#### enum typedef `DY::play_mode_t`

Play modes, basically whatever you commonly find on a media player, i.e.:
Repeat 1, Repeat all, Repeat list (dir), playlist (by dir), random play.

The default is perhaps somewhat unexpected: DY::PLAY_MODE_ONE_OFF. Often
these modules will be used in toys or information displays where you can
press a button and hear a corresponding sound. To get default media player
behaviour, you should probably set DY::PLAY_MODE_SEQUENCE to just continue
playing the next song until all are played or skipped, then stop.


| const | value | description |
|:------|:------|:------------|
| `PLAY_MODE_REPEAT` | `0x00` | Play all music in sequence, and repeat. |
| `PLAY_MODE_REPEAT_ONE` | `0x01` | Repeat current sound. |
| `PLAY_MODE_ONE_OFF` | `0x02` | Play sound file and stop. |
| `PLAY_MODE_RANDOM` | `0x03` | Play random sound file. |
| `PLAY_MODE_REPEAT_DIR` | `0x04` | Repeat current directory. |
| `PLAY_MODE_RANDOM_DIR` | `0x05` | Play random sound file in current folder. |
| `PLAY_MODE_SEQUENCE_DIR` | `0x06` | Play all sound files in current folder in sequence, and stop. |
| `PLAY_MODE_SEQUENCE` | `0x07` | Play all sound files on device in sequence, and stop. |


#### enum typedef `DY::playDirSound_t`

The `DY::DYPlayer::previousDir()` method expects this type as its argument.
Imagine you would press a button on a media player that selects the
previous directory/playlist, do you expect it to play the first song of
that list, or the last one? Depending on what you find logical or on your
requirement, this enumeration allows you to choose what happens when you
go to the previous directory.


| const | value | description |
|:------|:------|:------------|
| `FIRST_SOUND` | `0x00` | When navigating to the previous dir, play the first sound. |
| `LAST_SOUND` | `0x01` | When navigating to the previous dir, play the last sound. |
