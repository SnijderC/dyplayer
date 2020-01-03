/**
 * Abstraction of basic features of the DY-SV17F mp3 player board, written for
 * Arduino, should work on other frameworks as well. Instead of DY-SV17F I will
 * from here on refer to it as the "module".
 * 
 * There are some virtual methods that MUST be overridden (serialRead and 
 * serialWrite) and one that you may override (begin)
 */
#include <stdint.h>
typedef enum  {
  USB   = 0,
  SD    = 1,
  FLASH = 2,
  FAIL  = -1
} device_t;

typedef enum  {
  FIRST_SOUND,
  LAST_SOUND
} playDirSound_t;

class DYPlayer {
  public:
  /**
   * A virtual method that can be called on after construction of the object.
   * You can use this to setup the serial port if required.
   * This is left empty so it is always safe to call it regardless of the 
   * platform.
   */
  virtual void begin(void* data);
  
  /**
   * Virtual method that should implement writing to the module via UART.
   * @param buffer of bytes to send to the module.
   * @param len of buffer.
   */
  virtual void serialWrite(unsigned char buffer[], uint8_t len);
  /** 
   * Map writing a single byte to the same method as writing a buffer of 
   * length 1.
   * Can be overridden to a function that writes directly for performance if
   * required.
   * @param uint8_t byte to write to module.
   */
  virtual void serialWrite(uint8_t byte);

  /**
   * Virtual method that should implement reading from the module via UART.
   * @param buffer to keep data received from the module.
   * @param len of buffer.
   * @returns Successful read (true), failure (false).
   */
  virtual bool serialRead(unsigned char buffer[], uint8_t len);
  
  /**
   * Calculate the sum of all bytes in a buffer as a simple "CRC".
   * @param buffer containing bytes to calculate the CRC for.
   * @param len of buffer.
   * @returns Checksum of the buffer.
   */
  uint8_t checksum(unsigned char data[], uint8_t len);
  
  /**
   * Validate data buffer with "CRC" byte (last byte should be the CRC byte).
   * @param buffer containing bytes to calculate the CRC for.
   * @param len of buffer.
   * @returns boolean indicating CRC is correct (true) or incorrect (false).
   */
  bool validateCrc(unsigned char data[], uint8_t len);
  
  /**
   * Send a command to the module, adds a CRC to the passed buffer.
   * @param buffer containing bytes to send to the module.
   * @param len of buffer.
   */
  void sendCommand(unsigned char data[], uint8_t len);

  /**
   * Get a response to a command.
   * Reads data, validates the CRC, fills the buffer if valid and returns
   * a boolean indicating success or failure.
   * @param buffer buffer for the bytes to receive.
   * @param len of buffer.
   */
  bool getResponse(unsigned char buffer[], uint8_t len);
  
  /**
   * Send a command to the module, pass a static crc. 
   * Use this to optimize speed for static commands.
   * @param buffer containing bytes to send to the module.
   * @param len of buffer.
   * @param crc Precalculated CRC byte.
   */
  void sendCommand(unsigned char data[], uint8_t len, uint8_t crc);

  /**
   * Check the current play state can be called at any time.
   * @returns Play status: 0: stop, 1: play, 2: pause.
   */
  uint8_t checkPlayState();

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
   * Play the preivous file.
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
   * @param device number USB, SD, FLASH
   * @param path of the file (asbsolute).
   * @param len of the path.
   */
  void playSpecifiedDevicePath(device_t device, unsigned char path[], uint8_t len);

  /**
   * See if communication with the module is possible.
   * Internally this does the same as `getDevice()`, the manual doesn't 
   * specify a `getDevice()` procedure, it specifies only a procedure called
   * "Check Device Online". This method returns a boolean value for 
   * convenience.
   * @returns Device online status online: true, offline: false.
   */
  bool checkDeviceOnline();

  /**
   * Get the device number the module is currently using.
   * @returns device number USB, SD, FLASH or FAIL.
   */
  device_t getDevice();

  /**
   * Set the device number the module should use.
   * Tries to set the device but no guarantee is given, use `getDevice()` to
   * check the actual current storage device.
   * @param device number USB, SD, FLASH
   */
  void setDevice(device_t device);

  /**
   * Get the amount of sound files on the current storage device.
   * @returns number of sound files.
   */
  uint16_t soundCount();

  /**
   * Get the currently playing file by number.
   * @returns number of the file currently playing.
   */
  uint16_t getPlayingSound();

  /**
   * Select previous directory and start playing the first or last song.
   * @param playDirSound_t Opt to play the FIRST_SOUND or LAST_SOUND 
   */
  void previousDir (playDirSound_t song);

  /**
   * Get number of the first song in the currently selected directory.
   * @returns number of the first song in the currently selected directory. 
   */
  uint16_t firstInDir();

  /**
   * Get the amount of sound files in the currently selected directory.
   * NOTE: Excluding files in sub directories.
   * @returns number of sound files in currently selected directory.
   */
  uint16_t soundCountDir();

  /**
   * Set the playback volume between 0 and 30.
   * Default volume if not set: 20.
   * @param uint8_t volume to set (0-30)
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
   * @param device number USB, SD, FLASH
   * @param number of the file, e.g. `1` for `00001.mp3`.
   */
  void interludeSpecified(device_t device, uint16_t number);

  /**
   * Play an interlude by device and path.
   * Note from the manual: "Music interlude" only has level 1. Continuous 
   * interlude will cover the previous interlude (the interlude will be 
   * played immediately). When the interlude is finished, it will return to
   * the first interlude breakpoint and continue to play.
   * @param device number USB:0, SD:1, FLASH:2
   * @param path of the file (asbsolute).
   * @param len of the path.
   */
  void interludeSpecifiedDevicePath(device_t device, unsigned char path[], uint8_t len);


  /**
   * Stop the interlude and continue playing.
   */
  void stopInterlude();

  /**
   * Sets the cycle mode.
   * - 0: Play all music in sequence.
   * - 1: Repeat current sound.
   * - 2: Play sound file and stop.
   * - 3: Play random sound file.
   * - 4: Repeat current directory.
   * - 5: Play random sound file in current folder.
   * - 6: Play all sound files in current folder in sequence, and stop.
   * - 7: Play all sound files on device in sequence, and stop.
   * @param mode The cycle mode to set.
   */
  void setCycleMode(uint8_t mode);

  /**
   * Set how many cycles to play when in cycle modes 0, 1 or 4 (repeat modes).
   * @param cycles The cycle count for repeat modes.
   */
  void setCycleTimes(uint16_t cycles);

  /**
   * Set the equalizer setting.
   * 0: Normal
   * 1: Pop
   * 2: Rock
   * 3: Jazz
   * 4: Classic
   * @param eq The equalizer setting.
   */
  void setEq(uint8_t eq);

  /**
   * Select a sound file without playing it.
   * @param number of the file, e.g. `1` for `00001.mp3`.
   */
  void select(uint16_t number);
};
