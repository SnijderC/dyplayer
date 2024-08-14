/**
 * Abstraction of basic features of the DY-SV17F mp3 player board, written for
 * Arduino, should work on other frameworks as well. Instead of DY-SV17F I will
 * from here on refer to it as the "module".
 *
 * There are some virtual methods that MUST be overridden (serialRead and
 * serialWrite) and one that you may override (begin)
 */
#include <stdint.h>

#ifndef DYPLAYER_H
#define DYPLAYER_H

#ifndef DY_PATHS_IN_HEAP
#define DY_PATH_LEN 40
#endif

namespace DY
{
  /**
   * Storage devices reported by module and to choose from when selecting a
   * storage device.
   */
  typedef enum class Device : uint8_t
  {
    Usb = 0x00,     // USB Storage device.
    Sd = 0x01,      // SD Card.
    Flash = 0x02,   // Onboard flash chip (usually winbond 32, 64Mbit flash).
    Fail = 0xfe,    // UART failure, can't be `-1` (so this can be uint8_t).
    NoDevice = 0xff // No storage device is online.
  } device_t;

  /**
   * The current module play state.
   */
  typedef enum class PlayState : int8_t
  {
    Fail = -1, // UART Failure, can be a connection or a CRC problem.
    Stopped = 0,
    Playing = 1,
    Paused = 2
  } play_state_t;

  /**
   * Equalize settings.
   */
  typedef enum class Eq : uint8_t
  {
    Normal,
    Pop,
    Rock,
    Jazz,
    Classic
  } eq_t;

  /**
   * Play modes are basically whatever you commonly find on a media player,
   * i.e.:
   * Repeat 1, Repeat all, Repeat list (dir), playlist (by dir), random play.
   *
   * The default is perhaps somewhat unexpected: `DY::PlayMode::OneOff`. Often
   * these modules will be used in toys or information displays where you can
   * press a button and hear a corresponding sound. To get default media player
   * behaviour, you should probably set `DY::PlayMode::Sequence` to just continue
   * playing the next song until all are played or skipped, then stop.
   */
  typedef enum PlayMode : uint8_t
  {
    Repeat,      // Play all music in sequence, and repeat.
    RepeatOne,   // Repeat current sound.
    OneOff,      // Play sound file and stop.
    Random,      // Play random sound file.
    RepeatDir,   // Repeat current directory.
    RandomDir,   // Play random sound file in current folder.
    SequenceDir, // Play all sound files in current folder in sequence, and stop.
    Sequence     // Play all sound files on device in sequence, and stop.
  } play_mode_t;

  /**
   * The `DY::DYPlayer::previousDir()` method expects this type as its argument.
   * Imagine you would press a button on a media player that selects the
   * previous directory/playlist, do you expect it to play the first song of
   * that list, or the last one? Depending on what you find logical or on your
   * requirement, this enumeration allows you to choose what happens when you
   * go to the previous directory.
   */
  typedef enum PreviousDir : uint8_t
  {
    FirstSound, // When navigating to the previous dir, play the first sound.
    LastSound   // When navigating to the previous dir, play the last sound.
  } playDirSound_t;

  class DYPlayer
  {
  public:
    /**
     * Virtual method that should implement writing to the module via UART.
     * @param buffer pointer to bytes to send to the module.
     * @param len of buffer.
     */
    virtual void serialWrite(uint8_t *buffer, uint8_t len) = 0;
    /**
     * Map writing a single byte to the same method as writing a buffer of
     * length 1.
     * Can be overridden to a function that writes directly for performance
     * if required.
     * @param uint8_t byte to write to module.
     */
    virtual void serialWrite(uint8_t byte);

    /**
     * Virtual method that should implement reading from the module via UART.
     * @param buffer pointer to keep data received from the module.
     * @param len of buffer.
     * @return Successful read (true), failure (false).
     */
    virtual bool serialRead(uint8_t *buffer, uint8_t len) = 0;

    /**
     * Check the current play state can, be called at any time.
     * @return Play status: A [`DY::PlayState`](#typedef-enum-class-dyplay_state_t),
     *         e.g DY::PlayMode::Stopped, DY::PlayMode::Playing, etc.
     */
    play_state_t checkPlayState();

    /**
     * Play the currently selected file from the start.
     */
    void play();

    /**
     * Set the play state to paused.
     */
    void pause();

    /**
     * Set the play state to stopped.
     */
    void stop();

    /**
     * Play the previous file.
     */
    void previous();

    /**
     * Play the next file.
     */
    void next();

    /**
     * Play a sound file by number, number sent as 2 bytes.
     * @param number of the file, e.g. `1` for `00001.mp3`.
     */
    void playSpecified(uint16_t number);

    /**
     * Play a sound file by device and path.
     * Path may consist of up to 2 nested directories of 8 bytes long and a
     * file name of 8 bytes long excluding the extension of 4 bytes long.
     * If your directory names are shorter you can use more nesting. Use no
     * more than 36 bytes for your paths. If you require more, check the
     * readme, chapter: Memory use.
     * @param device A [`DY::Device member`](#typedef-enum-class-dydevice_t),
     *               e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     * @param path pointer to the path of the file (asbsolute).
     */
    void playSpecifiedDevicePath(device_t device, char *path);

    /**
     * Get the storage device that is currently used for playing sound files.
     *
     * @return a [`DY::Device` member](#typedef-enum-class-dydevice_t),
     *         e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     */
    device_t getPlayingDevice();

    /**
     * Set the device number the module should use.
     * Tries to set the device but no guarantee is given, use `getDevice()`
     * to check the actual current storage device.
     * @param device A [`DY::Device` member](#typedef-enum-class-dydevice_t),
     *               e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     */
    void setPlayingDevice(device_t device);

    /**
     * Get the amount of sound files on the current storage device.
     * @return number of sound files.
     */
    uint16_t getSoundCount();

    /**
     * Get the currently playing file by number.
     * @return number of the file currently playing.
     */
    uint16_t getPlayingSound();

    /**
     * Select previous directory and start playing the first or last song.
     * @param song Play `DY::PreviousDir::FirstSound` or
     *             DY::PreviousDir::LastSound
     */
    void previousDir(playDirSound_t song);

    /**
     * Get number of the first song in the currently selected directory.
     * @return number of the first song in the currently selected directory.
     */
    uint16_t getFirstInDir();

    /**
     * Get the amount of sound files in the currently selected directory.
     * NOTE: Excluding files in sub directories.
     * @return number of sound files in currently selected directory.
     */
    uint16_t getSoundCountDir();

    /**
     * Set the playback volume between 0 and 30.
     * Default volume if not set: 20.
     * @param volume to set (0-30)
     */
    void setVolume(uint8_t volume);

    /**
     * Increase the volume.
     */
    void volumeIncrease();

    /**
     * Decrease the volume.
     */
    void volumeDecrease();

    /**
     * Play an interlude file by device and number, number sent as 2 bytes.
     * Note from the manual: "Music interlude" only has level 1. Continuous
     * interlude will cover the previous interlude (the interlude will be
     * played immediately). When the interlude is finished, it will return to
     * the first interlude breakpoint and continue to play.
     * @param device A [`DY::Device member`](#typedef-enum-class-dydevice_t),
     *               e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     * @param number of the file, e.g. `1` for `00001.mp3`.
     */
    void interludeSpecified(device_t device, uint16_t number);

    /**
     * Play an interlude by device and path.
     * Note from the manual: "Music interlude" only has level 1. Continuous
     * interlude will cover the previous interlude (the interlude will be
     * played immediately). When the interlude is finished, it will return to
     * the first interlude breakpoint and continue to play.
     *
     * Path may consist of up to 2 nested directories of 8 bytes long and a
     * file name of 8 bytes long excluding the extension of 4 bytes long.
     * If your directory names are shorter you can use more nesting. Use no
     * more than 36 bytes for your paths. If you require more, check the
     * readme, chapter: Memory use.
     * @param device A [`DY::Device member`](#typedef-enum-class-dydevice_t),
     *               e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     * @param path pointer to the path of the file (asbsolute).
     */
    void interludeSpecifiedDevicePath(device_t device, char *path);

    /**
     * Stop the interlude and continue playing.
     * Will also stop the current sound from playing if interlude is not
     * active.
     */
    void stopInterlude();

    /**
     * Sets the cycle mode.
     * See [`DY::play_state_t`](#typedef-enum-class-dyplay_state_t) for modes
     * and meaning.
     * @param mode The cycle mode to set.
     */
    void setCycleMode(play_mode_t mode);

    /**
     * Set how many cycles to play when in cycle modes 0, 1 or 4 (repeat
     * modes).
     * @param cycles The cycle count for repeat modes.
     */
    void setCycleTimes(uint16_t cycles);

    /**
     * Set the equalizer setting.
     * See [`DY::eq_t`](#typedef-enum-class-dyeq_t) for settings.
     * @param eq The equalizer setting.
     */
    void setEq(eq_t eq);

    /**
     * Select a sound file without playing it.
     * @param number of the file, e.g. `1` for `00001.mp3`.
     */
    void select(uint16_t number);

    /**
     * Combination play allows you to make a playlist of multiple sound files.
     *
     * You could use this to combine numbers e.g.: "fourthy-two" where you
     * have samples for "fourthy" and "two".
     *
     * This feature has a particularly curious parameters, you have to
     * specify the sound files by name, they have to be named by 2 numbers
     * and an extension, e.g.: `01.mp3` and specified by `01`. You should
     * pass them as an array pointer. You need to put the files into a
     * directory that can be called `DY`, `ZH or `XY`, you will have to check
     * the manual that came with your module, or try all of them. There may
     * well be more combinations! Also see
     * [Loading sound files](#loading-sound-files).
     *
     * E.g.
     * ```cpp
     * const char * sounds[2][3] = { "01", "02" };
     * DY::DYPlayer::combinationPlay(sounds, 2);
     * ````
     * @param sounds An array of char[2] containing the names of sounds to
     *               play in order.
     * @param len The length of the passed array.
     */
    void combinationPlay(char *sounds[], uint8_t len);

    /**
     * End combination play.
     */
    void endCombinationPlay();

  private:
    /**
     * Calculate the sum of all bytes in a buffer as a simple "CRC".
     * @param data pointer to bytes to calculate the CRC for.
     * @param len of buffer.
     * @return Checksum of the buffer.
     */
    uint8_t inline checksum(uint8_t *data, uint8_t len);

    /**
     * Validate data buffer with CRC byte (last byte should be the CRC byte).
     * @param data pointer to bytes to calculate the CRC for.
     * @param len of data.
     * @return boolean indicating CRC is correct (true) or incorrect (false).
     */
    bool validateCrc(uint8_t *data, uint8_t len);

    /**
     * Send a command to the module, adds a CRC to the passed buffer.
     * @param data pointer to bytes to send to the module.
     * @param len of data.
     */
    void sendCommand(uint8_t *data, uint8_t len);

    /**
     * Send a command to the module, pass a static crc.
     * Use this to optimize speed for static commands.
     * @param data pointer to bytes to send to the module.
     * @param len of data.
     * @param crc Precalculated CRC byte.
     */
    void sendCommand(uint8_t *data, uint8_t len, uint8_t crc);

    /**
     * Get a response to a command.
     * Reads data from UART, validates the CRC, and puts it in the buffer.
     * @param buffer pointer for the bytes to receive.
     * @param len of buffer.
     * @return False on communication failure.
     */
    bool getResponse(uint8_t *buffer, uint8_t len);

    /**
     * Send command with converted paths to  weird format required by the
     * modules.
     *
     * - Any dot in a path should become a star (`*`)
     * - Path ending slashes should be have a star prefix, except root.
     *
     * E.g.: /SONGS1/FILE1.MP3 should become: /SONGS1﹡/FILE1*MP3
     * NOTE: This comment uses a unicode * look-a-alike (﹡) because ﹡/ end the
     * comment.
     * @param command The command to send.
     * @param device A [DY::Device member](#typedef-enum-class-dydevice_t),
     *               e.g  `DY::Device::Flash` or `DY::Device::Sd`.
     * @param path of the file (asbsolute).
     */
    void byPathCommand(uint8_t command, device_t device, char *path);
  };
}

#endif