/**
 * Abstraction of basic features of the DY-SV17F mp3 player board, written for
 * Arduino, should work on other frameworks as well. Instead of DY-SV17F I will
 * from here on refer to it as the "module".
 * 
 * There are some virtual methods that MUST be overridden (serialRead and 
 * serialWrite) and one that you may override (begin)
 */

#include "DYPlayer.h"

void DYPlayer::serialWrite(uint8_t byte) {
  unsigned char buffer[1] = { byte };
  serialWrite(buffer, 1);
}

uint8_t DYPlayer::checksum(unsigned char data[], uint8_t len) {
  uint8_t sum = 0;
  for (uint8_t i=0; i<len;i++) {
    sum = sum + data[i];
  }
  return sum;
}

bool DYPlayer::validateCrc(unsigned char data[], uint8_t len) {
  uint8_t crc = data[len - 1];
  uint8_t _data[len - 1] = { 0x00 };
  for (int i = 0; i < len - 1; i++) {
    _data[i] = data[i];
  }
  return checksum(_data, len - 1) == crc;
}

void DYPlayer::sendCommand(unsigned char data[], uint8_t len) {
  uint8_t crc = checksum(data, len);
  serialWrite(data, len);
  serialWrite(crc);
}

void DYPlayer::sendCommand(unsigned char data[], uint8_t len, uint8_t crc) {
  serialWrite(data, len);
  serialWrite(crc);
}

bool DYPlayer::getResponse(unsigned char buffer[], uint8_t len) {
  if (serialRead(buffer, len) > 0) {
    if (validateCrc(buffer, len)) {
      return true;
    }
  }
  return false;
}

uint8_t DYPlayer::checkPlayState() {
  uint8_t command[3] = { 0xaa, 0x01, 0x00 };
  sendCommand(command, 3, 0xab);
  unsigned char buffer[6];
  if(getResponse(buffer, 6))
    return buffer[3];
  return 0;
}

void DYPlayer::play() {
  uint8_t command[3] = {0xaa, 0x02, 0x00};
  sendCommand(command, 3, 0xac);
}

void DYPlayer::pause() {
  uint8_t command[3] = {0xaa, 0x03, 0x00};
  sendCommand(command, 3, 0xad);
}

void DYPlayer::stop() {
  uint8_t command[3] = {0xaa, 0x04, 0x00};
  sendCommand(command, 3, 0xae);
}

void DYPlayer::previous() {
  uint8_t command[3] = {0xaa, 0x05, 0x00};
  sendCommand(command, 3, 0xaf);
}

void DYPlayer::next() {
  uint8_t command[3] = {0xaa, 0x06, 0x00};
  sendCommand(command, 3, 0xb0);
}

void DYPlayer::playSpecified(uint16_t number) {
  uint8_t command[5] = { 0xaa, 0x07, 0x02, 0x00, 0x00 };
  command[3] = number >> 8; 
  command[4] = number & 0xff;
  sendCommand(command, 5);
}

void DYPlayer::playSpecifiedDevicePath(device_t device, unsigned char path[], uint8_t len) {
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

bool DYPlayer::checkDeviceOnline() {
  return (getDevice() != -1);
}

device_t DYPlayer::getDevice() {
  uint8_t command[3] = { 0xaa, 0x09, 0x00 };
  sendCommand(command, 3, 0xb3);
  unsigned char buffer[5];
  if(getResponse(buffer, 5))
    return (device_t)buffer[3];
  return FAIL;
}

void DYPlayer::setDevice(device_t device) {
  uint8_t command[4] = { 0xaa, 0x0b, 0x01, 0x00 };
  command[3] = device;
  sendCommand(command, 4);
}

uint16_t DYPlayer::soundCount() {
  uint8_t command[3] = { 0xaa, 0x0c, 0x00 };
  sendCommand(command, 3, 0xb6);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

uint16_t DYPlayer::getPlayingSound() {
  uint8_t command[3] = { 0xaa, 0x0d, 0x00 };
  sendCommand(command, 3, 0xb7);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

void DYPlayer::previousDir (playDirSound_t song) {
  if (song == LAST_SOUND) {
    uint8_t command[3] = { 0xaa, 0x0e, 0x00 };
    sendCommand(command, 3, 0xb8);
  } else {
    uint8_t command[3] = { 0xaa, 0x0f, 0x00 };
    sendCommand(command, 3, 0xb9);
  }  
}

uint16_t DYPlayer::firstInDir() {
  uint8_t command[3] = { 0xaa, 0x11, 0x00 };
  sendCommand(command, 3, 0xbb);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

uint16_t DYPlayer::soundCountDir() {
  uint8_t command[3] = { 0xaa, 0x12, 0x00 };
  sendCommand(command, 3, 0xbc);
  unsigned char buffer[6];
  if (getResponse(buffer, 6))
    return (buffer[3] << 8) | buffer[4];
  return 0;
}

void DYPlayer::setVolume(uint8_t volume) {
  uint8_t command[4] = { 0xaa, 0x13, 0x01, 0x00 };
  command[3] = volume;
  sendCommand(command, 4);
}

void DYPlayer::volumeIncrease() {
  uint8_t command[3] = {0xaa, 0x14, 0x00};
  sendCommand(command, 3, 0xbe);
}

void DYPlayer::volumeDecrease() {
  uint8_t command[3] = {0xaa, 0x15, 0x00};
  sendCommand(command, 3, 0xbf);
}

void DYPlayer::interludeSpecified(device_t device, uint16_t number) {
  uint8_t command[6] = { 0xaa, 0x0b, 0x03, 0x00, 0x00, 0x00 };
  command[3] = device;
  command[4] = number >> 8; 
  command[5] = number & 0xff;
  sendCommand(command, 6);
}

void DYPlayer::interludeSpecifiedDevicePath(device_t device, unsigned char path[], uint8_t len) {
  uint8_t command[len + 4];
  command[0] = 0xaa;
  command[1] = 0x17;
  command[2] = len + 1;
  command[3] = device;
  for (uint8_t i = 0; i < len; i++) {
    command[i+4] = path[i];
  }
  sendCommand(command, len + 4);
}

void DYPlayer::stopInterlude() {
  uint8_t command[3] = {0xaa, 0x10, 0x00};
  sendCommand(command, 3, 0xba);
}

void DYPlayer::setCycleMode(uint8_t mode) {
  uint8_t command[4] = { 0xaa, 0x18, 0x01, 0x00 };
  command[3] = mode;
  sendCommand(command, 4);
}
void DYPlayer::setCycleTimes(uint16_t cycles) {
  uint8_t command[4] = { 0xaa, 0x19, 0x01, 0x00 };
  command[3] = cycles;
  sendCommand(command, 4);
}

void DYPlayer::setEq(uint8_t eq) {
  uint8_t command[4] = { 0xaa, 0x1a, 0x01, 0x00 };
  command[3] = eq;
  sendCommand(command, 4);
}

void DYPlayer::select(uint16_t number) {
  uint8_t command[5] = { 0xaa, 0x1f, 0x2, 0x00, 0x00};
  command[3] = number >> 8; 
  command[4] = number & 0xff;
  sendCommand(command, 5);
}
