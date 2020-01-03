/**
 * Abstraction of basic features of the DY-SV17F mp3 player board, written for
 * Arduino, should work on other frameworks as well. Instead of DY-SV17F I will
 * from here on refer to it as the "module".
 * 
 * There are some virtual methods that MUST be overridden (serialRead and 
 * serialWrite) and one that you may override (begin)
 */

#include "dysv17f.h"

void AbstractPlayer::serialWrite(unsigned char buffer[], uint8_t len) { 
  /* do nothing */
}
void AbstractPlayer::serialWrite(uint8_t byte) {
  unsigned char buffer[1] = { byte };
  serialWrite(buffer, 1);
}
bool AbstractPlayer::serialRead(unsigned char buffer[], uint8_t len) {
    /* only return false to indicate there was no read */
    return false;
  }

uint8_t AbstractPlayer::checksum(unsigned char data[], uint8_t len) {
  uint8_t sum = 0;
  for (uint8_t i=0; i<len;i++) {
    sum = sum + data[i];
  }
  return sum;
}

bool AbstractPlayer::validateCrc(unsigned char data[], uint8_t len) {
  uint8_t crc = data[len - 1];
  uint8_t _data[len - 1] = { 0x00 };
  for (int i = 0; i < len - 1; i++) {
    _data[i] = data[i];
  }
  return checksum(_data, len - 1) == crc;
}

void AbstractPlayer::sendCommand(unsigned char data[], uint8_t len) {
  uint8_t crc = checksum(data, len);
  serialWrite(data, len);
  serialWrite(crc);
}

void AbstractPlayer::sendCommand(unsigned char data[], uint8_t len, uint8_t crc) {
  serialWrite(data, len);
  serialWrite(crc);
}

bool AbstractPlayer::getResponse(unsigned char buffer[], uint8_t len) {
  if (serialRead(buffer, len) > 0) {
    if (validateCrc(buffer, len)) {
      return true;
    }
  }
  return false;
}

uint8_t AbstractPlayer::checkPlayState() {
  uint8_t command[3] = { 0xaa, 0x01, 0x00 };
  sendCommand(command, 3, 0xab);
  unsigned char buffer[6];
  if(getResponse(buffer, 6))
    return buffer[3];
  return 0;
}

void AbstractPlayer::play() {
  uint8_t command[3] = {0xaa, 0x02, 0x00};
  sendCommand(command, 3, 0xac);
}

void AbstractPlayer::pause() {
  uint8_t command[3] = {0xaa, 0x03, 0x00};
  sendCommand(command, 3, 0xad);
}

void AbstractPlayer::stop() {
  uint8_t command[3] = {0xaa, 0x04, 0x00};
  sendCommand(command, 3, 0xae);
}

void AbstractPlayer::previous() {
  uint8_t command[3] = {0xaa, 0x05, 0x00};
  sendCommand(command, 3, 0xaf);
}

void AbstractPlayer::next() {
  uint8_t command[3] = {0xaa, 0x06, 0x00};
  sendCommand(command, 3, 0xb0);
}

void AbstractPlayer::playSpecified(uint16_t number) {
  uint8_t command[5] = { 0xaa, 0x07, 0x02, 0x00, 0x00 };
  command[3] = number >> 8; 
  command[4] = number & 0xff;
  sendCommand(command, 5);
}

void AbstractPlayer::playSpecifiedDevicePath(device_t device, unsigned char path[], uint8_t len) {
  uint8_t command[len + 4];
  command[0] = 0xaa;
  command[1] = 0x08;
  command[2] = len + 1;
  command[3] = device;
  for (uint8_t i = 0; i < len; i++) {
    command[i+4] = path[i];
  }
  sendCommand(command, len + 4);
}

bool AbstractPlayer::checkDeviceOnline() {
  return (getDevice() != -1);
}

device_t AbstractPlayer::getDevice() {
  uint8_t command[3] = { 0xaa, 0x09, 0x00 };
  sendCommand(command, 3, 0xb3);
  unsigned char buffer[5];
  if(getResponse(buffer, 5))
    return (device_t)buffer[3];
  return FAIL;
}

void AbstractPlayer::setDevice(device_t device) {
  uint8_t command[4] = { 0xaa, 0x0b, 0x01, 0x00 };
  command[3] = device;
  sendCommand(command, 4);
}

uint16_t AbstractPlayer::soundCount() {
  uint8_t command[3] = { 0xaa, 0x0c, 0x00 };
  sendCommand(command, 3, 0xb6);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

uint16_t AbstractPlayer::getPlayingSound() {
  uint8_t command[3] = { 0xaa, 0x0d, 0x00 };
  sendCommand(command, 3, 0xb7);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

// void AbstractPlayer::previousDir (playDirSound_t song);

// uint16_t AbstractPlayer::firstInDir();

// uint16_t AbstractPlayer::soundCountDir();

// void AbstractPlayer::setVolume(uint8_t volume);

// void AbstractPlayer::volumeIncrease();

// void AbstractPlayer::volumeDecrease();

// void AbstractPlayer::interludeSpecified(uint8_t device, uint16_t number);

// void AbstractPlayer::interludeSpecifiedDevicePath(uint8_t device, char path[], uint8_t len);

// void AbstractPlayer::stopInterlude();

// void AbstractPlayer::setCycleMode(uint8_t mode);

// void AbstractPlayer::setCycleTimes(uint16_t cycles);

// void AbstractPlayer::setEq(uint8_t eq);

// void AbstractPlayer::select(uint16_t number);
