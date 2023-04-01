/*
 * Arduino library for ZE08-CH2O module
 * https://www.winsen-sensor.com/d/files/PDF/Gas%20Sensor%20Module/Formaldehyde%20Detection%20Module/ZE08-CH2O%20V1.0.pdf
 *
 * Created by Tea <icepie.dev@gmail.com> 2022-03-20
 *
 */

#include "ZE08_CH2O.h"

// public
ZE08_CH2O::ZE08_CH2O()
{
  TX_pin = ZE08_CH2O_SW_UART_default_TX_pin;
  RX_pin = ZE08_CH2O_SW_UART_default_RX_pin;
}

ZE08_CH2O::ZE08_CH2O(int8_t RX_pin_in, int8_t TX_pin_in)
{
  TX_pin = TX_pin_in;
  RX_pin = RX_pin_in;
}

// defualt mode is QA
void ZE08_CH2O::begin()
{
  this->mode = ZE08_CH2O_MODE_QA;
  beginQAMode();
}

void ZE08_CH2O::begin(ZE08_CH2O_MODE mode)
{
  this->mode = mode;
  if (mode == ZE08_CH2O_MODE_QA)
  {
    beginQAMode();
  }
  else if (mode == ZE08_CH2O_MODE_ACTIVE)
  {
    beginActiveMode();
  }
}

uint32_t ZE08_CH2O::read_value()
{
  ZE08_CH2O_RESULT sensor_flag = ZE08_CH2O_RESULT_OK;
  if (mode == ZE08_CH2O_MODE_NONE)
  {
    sensor_flag = ZE08_CH2O_RESULT_ERR_NOMODE;
    ZE08_CH2O_SW_UART_DEBUG_PRINTLN("unknow mode");
    return 0x00;
  }

  SoftwareSerial ss(RX_pin, TX_pin);
  uint8_t receive_cmd[ZE08_CH2O_CMD_LEN];

  uint16_t CH2O_value = 0;

  uint16_t high_byte_index = ZE08_CH2O_HIGH_BYTE_INDEX_ACTIVE;
  uint16_t low_byte_index = ZE08_CH2O_LOW_BYTE_INDEX_ACTIVE;

  ss.begin(ZE08_CH2O_SW_UART_RATE);

  if (this->mode == ZE08_CH2O_MODE_QA)
  {
    uint8_t cmd[ZE08_CH2O_CMD_LEN] = ZE08_CH2O_CMD_QA_READ;
    uart_write(&ss, cmd, ZE08_CH2O_CMD_LEN);
    high_byte_index = ZE08_CH2O_HIGH_BYTE_INDEX_QA;
    low_byte_index = ZE08_CH2O_LOW_BYTE_INDEX_QA;
  }

  unsigned long time = millis();
  while (ss.available() <= 0)
  {
    if (millis() - time >= ZE08_CH2O_SERIAL_TIMEOUT) // time out check
    {
      sensor_flag = ZE08_CH2O_RESULT_ERR_TIMEOUT;
      break;
    }
  }

  memset(receive_cmd, 0, ZE08_CH2O_CMD_LEN);
  ss.readBytes(receive_cmd, ZE08_CH2O_CMD_LEN);
  ss.end();

  // for (int i = 0; i < 9; i++)
  // {
  //   ZE08_CH2O_SW_UART_DEBUG_PRINT(receive_cmd[i], HEX);
  //   ZE08_CH2O_SW_UART_DEBUG_PRINTLN(" ");

  //   if (i == 8)
  //   {
  //     ZE08_CH2O_SW_UART_DEBUG_PRINTLN();
  //   }
  // }

  byte crc = calcCRC(receive_cmd); // crc
  sensor_flag = ZE08_CH2O_RESULT_OK;
  if (receive_cmd[0] != 0xFF)
    sensor_flag = ZE08_CH2O_RESULT_ERR_FB;
  if (receive_cmd[8] != crc)
    sensor_flag = ZE08_CH2O_RESULT_ERR_CRC;
  if (sensor_flag == ZE08_CH2O_RESULT_OK)
  { // calculate CH2O_value
    CH2O_value = receive_cmd[high_byte_index] * 256 + receive_cmd[low_byte_index];
  }
  else if (sensor_flag == ZE08_CH2O_SERIAL_TIMEOUT)
  {
    ZE08_CH2O_SW_UART_DEBUG_PRINTLN("timeout");
  }
  else if (sensor_flag == ZE08_CH2O_RESULT_ERR_CRC)
  {
    ZE08_CH2O_SW_UART_DEBUG_PRINTLN("CH2O sensor read CRC error");
  }
  else if (sensor_flag == ZE08_CH2O_RESULT_ERR_FB)
  {
    ZE08_CH2O_SW_UART_DEBUG_PRINTLN("CH2O read first byte error");
  }

  return CH2O_value;
}

// private
void ZE08_CH2O::beginQAMode()
{
  SoftwareSerial ss(RX_pin, TX_pin);
  ss.begin(ZE08_CH2O_SW_UART_RATE);
  uint8_t cmd[ZE08_CH2O_CMD_LEN] = ZE08_CH2O_CMD_QA_MODE; // set mode
  uart_write(&ss, cmd, ZE08_CH2O_CMD_LEN);
  ss.end();
}

void ZE08_CH2O::beginActiveMode()
{
  SoftwareSerial ss(RX_pin, TX_pin);
  ss.begin(ZE08_CH2O_SW_UART_RATE);
  uint8_t cmd[ZE08_CH2O_CMD_LEN] = ZE08_CH2O_CMD_ACTIVE_MODE; // set mode
  uart_write(&ss, cmd, ZE08_CH2O_CMD_LEN);
  ss.end();
}

void ZE08_CH2O::uart_write(Stream *_serial, byte data[], byte len)
{
  while (_serial->available() > 0)
  {
    _serial->read();
  }
  _serial->write(data, len);
  _serial->flush();
}

byte ZE08_CH2O::calcCRC(byte data[])
{
  byte i;
  byte crc = 0;
  for (i = 1; i < 8; i++)
  {
    crc += data[i];
  }
  crc = 255 - crc;
  crc++;

  return crc;
}
