#pragma once
#include <Arduino.h>
#include <SoftwareSerial.h>

#define ZE08_CH2O_SW_UART_RATE 9600
#define ZE08_CH2O_SW_UART_default_TX_pin 7 // 5
#define ZE08_CH2O_SW_UART_default_RX_pin 8 // 6

#define ZE08_CH2O_SERIAL_TIMEOUT 500

#define ZE08_CH2O_CMD_LEN 9
#define ZE08_CH2O_CMD_QA_MODE                        \
  {                                                  \
    0xff, 0x01, 0x78, 0x41, 0x0, 0x0, 0x0, 0x0, 0x46 \
  }
#define ZE08_CH2O_CMD_ACTIVE_MODE                    \
  {                                                  \
    0xff, 0x01, 0x78, 0x40, 0x0, 0x0, 0x0, 0x0, 0x47 \
  }

#define ZE08_CH2O_CMD_QA_READ                       \
  {                                                 \
    0xff, 0x01, 0x86, 0x0, 0x0, 0x0, 0x0, 0x0, 0x79 \
  }

#define ZE08_CH2O_HIGH_BYTE_INDEX_QA 2
#define ZE08_CH2O_LOW_BYTE_INDEX_QA 3

#define ZE08_CH2O_HIGH_BYTE_INDEX_ACTIVE 4
#define ZE08_CH2O_LOW_BYTE_INDEX_ACTIVE 5

enum ZE08_CH2O_MODE
{
  ZE08_CH2O_MODE_NONE,
  ZE08_CH2O_MODE_QA,
  ZE08_CH2O_MODE_ACTIVE,
};

enum ZE08_CH2O_RESULT
{
  ZE08_CH2O_RESULT_OK,
  ZE08_CH2O_RESULT_ERR_FB,
  ZE08_CH2O_RESULT_ERR_CRC,
  ZE08_CH2O_RESULT_ERR_TIMEOUT,
  ZE08_CH2O_RESULT_ERR_NOMODE
};

//#define ZE08_CH2O_SW_UART_DEBUG
/* Setup debug printing macros. */
#define ZE08_CH2O_SW_UART_DEBUG_PRINTER Serial
#ifdef ZE08_CH2O_SW_UART_DEBUG
#define ZE08_CH2O_SW_UART_DEBUG_PRINT(...)              \
  {                                                     \
    ZE08_CH2O_SW_UART_DEBUG_PRINTER.print(__VA_ARGS__); \
  }
#define ZE08_CH2O_SW_UART_DEBUG_PRINTLN(...)              \
  {                                                       \
    ZE08_CH2O_SW_UART_DEBUG_PRINTER.println(__VA_ARGS__); \
  }
#else
#define ZE08_CH2O_SW_UART_DEBUG_PRINT(...) \
  {                                        \
  } /**< Debug Print Placeholder if Debug is disabled */
#define ZE08_CH2O_SW_UART_DEBUG_PRINTLN(...) \
  {                                          \
  } /**< Debug Print Line Placeholder if Debug is disabled */
#endif

class ZE08_CH2O
{
public:
  ZE08_CH2O();
  ZE08_CH2O(int8_t RX_pin_in, int8_t TX_pin_in);
  void begin();
  void begin(ZE08_CH2O_MODE mode);
  uint32_t read_value();

private:
  int8_t RX_pin;
  int8_t TX_pin;
  ZE08_CH2O_MODE mode = ZE08_CH2O_MODE_NONE;
  void beginQAMode();
  void beginActiveMode();
  unsigned long lastInitTimestamp;
  void uart_write(Stream *_serial, byte data[], byte len);
  byte calcCRC(byte *data);
};
