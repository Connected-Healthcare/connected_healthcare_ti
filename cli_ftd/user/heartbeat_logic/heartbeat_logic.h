#ifndef USER_HEARTBEATLOGIC_H_
#define USER_HEARTBEATLOGIC_H_

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

// /* POSIX Header files */
#include <unistd.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* Example/Board Header files */
#include "ti_drivers_config.h"

#include <string.h>

// Import I2C Driver definitions
#include <ti/drivers/I2C.h>

#include "tinyprintf.h"

#define WRITE_FIFO_INPUT_BYTE 0x04
#define DISABLE 0x00
#define ENABLE 0x01
#define MODE_ONE 0x01
#define MODE_TWO 0x02
#define APP_MODE 0x00
#define BOOTLOADER_MODE 0x08
#define NO_WRITE 0x00
#define INCORR_PARAM 0xEE

#define CONFIGURATION_REGISTER 0x0A
#define PULSE_MASK 0xFC
#define READ_PULSE_MASK 0x03
#define SAMP_MASK 0xE3
#define READ_SAMP_MASK 0x1C
#define ADC_MASK 0x9F
#define READ_ADC_MASK 0x60

#define ENABLE_CMD_DELAY 45000 // Microseconds
#define CMD_DELAY 6000         // Microseconds
#define MAXFAST_ARRAY_SIZE 6   // Number of bytes....
#define MAXFAST_EXTENDED_DATA 5
#define MAX30101_LED_ARRAY 12 // 4 values of 24 bit (3 byte) LED values

#define SET_FORMAT 0x00
#define READ_FORMAT                                                            \
  0x01 // Index Byte under Family Byte: READ_OUTPUT_MODE (0x11)
#define WRITE_SET_THRESHOLD 0x01 // Index Byte for WRITE_INPUT(0x14)
#define WRITE_EXTERNAL_TO_FIFO 0x00

// const uint8_t BIO_ADDRESS = 0x55;

struct bioData {

  uint32_t irLed;
  uint32_t redLed;
  uint16_t heartRate; // LSB = 0.1bpm
  uint8_t confidence; // 0-100% LSB = 1%
  uint16_t oxygen;    // 0-100% LSB = 1%
  uint8_t status; // 0: Success, 1: Not Ready, 2: Object Detectected, 3: Finger
                  // Detected
  float rValue;   // -- Algorithm Mode 2 vv
  int8_t extStatus;    // --
  uint8_t reserveOne;  // --
  uint8_t resserveTwo; // -- Algorithm Mode 2 ^^
};

struct version {
  // 3 bytes total
  uint8_t major;
  uint8_t minor;
  uint8_t revision;
};

struct sensorAttr {

  uint8_t byteWord;
  uint8_t availRegisters;
};

// Status Bytes are communicated back after every I-squared-C transmission and
// are indicators of success or failure of the previous transmission.
enum READ_STATUS_BYTE_VALUE {

  SUCCESS = 0x00,
  ERR_UNAVAIL_CMD,
  ERR_UNAVAIL_FUNC,
  ERR_DATA_FORMAT,
  ERR_INPUT_VALUE,
  ERR_TRY_AGAIN,
  ERR_BTLDR_GENERAL = 0x80,
  ERR_BTLDR_CHECKSUM,
  ERR_BTLDR_AUTH,
  ERR_BTLDR_INVALID_APP,
  ERR_UNKNOWN = 0xFF

};

// The family register bytes are the larger umbrella for all the Index and
// Write Bytes listed below. You can not reference a nestled byte without first
// referencing it's larger category: Family Register Byte.
enum FAMILY_REGISTER_BYTES {

  HUB_STATUS = 0x00,
  SET_DEVICE_MODE,
  READ_DEVICE_MODE,
  OUTPUT_MODE = 0x10,
  READ_OUTPUT_MODE,
  READ_DATA_OUTPUT,
  READ_DATA_INPUT,
  WRITE_INPUT,
  WRITE_REGISTER = 0x40,
  READ_REGISTER,
  READ_ATTRIBUTES_AFE,
  DUMP_REGISTERS,
  ENABLE_SENSOR,
  READ_SENSOR_MODE,
  CHANGE_ALGORITHM_CONFIG = 0x50,
  READ_ALGORITHM_CONFIG,
  ENABLE_ALGORITHM,
  BOOTLOADER_FLASH = 0x80,
  BOOTLOADER_INFO,
  IDENTITY = 0xFF

};

// All the defines below are: 1. Index Bytes nestled in the larger category of
// the family registry bytes listed above and 2. The Write Bytes nestled even
// farther under their Index Bytes.

// Write Bytes under Family Byte: SET_DEVICE_MODE (0x01) and Index
// Byte: 0x00.
enum DEVICE_MODE_WRITE_BYTES {

  EXIT_BOOTLOADER = 0x00,
  RESET = 0x02,
  ENTER_BOOTLOADER = 0x08

};

// Write Bytes under Family Byte: OUTPUT_MODE (0x10) and Index byte: SET_FORMAT
// (0x00)
enum OUTPUT_MODE_WRITE_BYTE {

  PAUSE = 0x00,
  SENSOR_DATA,
  ALGO_DATA,
  SENSOR_AND_ALGORITHM,
  PAUSE_TWO,
  SENSOR_COUNTER_BYTE,
  ALGO_COUNTER_BYTE,
  SENSOR_ALGO_COUNTER

};

// Index Byte under the Family Byte: READ_DATA_OUTPUT (0x12)
enum FIFO_OUTPUT_INDEX_BYTE {

  NUM_SAMPLES,
  READ_DATA

};

// Index Byte under the Family Byte: READ_DATA_INPUT (0x13)
enum FIFO_EXTERNAL_INDEX_BYTE {

  SAMPLE_SIZE,
  READ_INPUT_DATA,
  READ_SENSOR_DATA,       // For external accelerometer
  READ_NUM_SAMPLES_INPUT, // For external accelerometer
  READ_NUM_SAMPLES_SENSOR

};

// Index Byte under the Family Registry Byte: WRITE_REGISTER (0x40)
enum WRITE_REGISTER_INDEX_BYTE {

  WRITE_MAX30101 = 0x03,
  WRITE_ACCELEROMETER

};

// Index Byte under the Family Registry Byte: READ_REGISTER (0x41)
enum READ_REGISTER_INDEX_BYTE {

  READ_MAX30101 = 0x03,
  READ_ACCELEROMETER

};

// Index Byte under the Family Registry Byte: READ_ATTRIBUTES_AFE (0x42)
enum GET_AFE_INDEX_BYTE {

  RETRIEVE_AFE_MAX30101 = 0x03,
  RETRIEVE_AFE_ACCELEROMETER

};

// Index Byte under the Family Byte: DUMP_REGISTERS (0x43)
enum DUMP_REGISTER_INDEX_BYTE {

  DUMP_REGISTER_MAX30101 = 0x03,
  DUMP_REGISTER_ACCELEROMETER

};

// Index Byte under the Family Byte: ENABLE_SENSOR (0x44)
enum SENSOR_ENABLE_INDEX_BYTE {

  ENABLE_MAX30101 = 0x03,
  ENABLE_ACCELEROMETER

};

// Index Byte for the Family Byte: READ_SENSOR_MODE (0x45)
enum READ_SENSOR_ENABLE_INDEX_BYTE {

  READ_ENABLE_MAX30101 = 0x03,
  READ_ENABLE_ACCELEROMETER

};

// Index Byte under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50)
enum ALGORITHM_CONFIG_INDEX_BYTE {

  SET_TARG_PERC = 0x00,
  SET_STEP_SIZE = 0x00,
  SET_SENSITIVITY = 0x00,
  SET_AVG_SAMPLES = 0x00,
  SET_PULSE_OX_COEF = 0x02,

};

// Write Bytes under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50) and the
// Index Byte: ALGORITHM_CONFIG_INDEX_BYTE - SET_TARG_PERC
enum ALGO_AGC_WRITE_BYTE {

  AGC_GAIN_ID = 0x00,
  AGC_STEP_SIZE_ID,
  AGC_SENSITIVITY_ID,
  AGC_NUM_SAMP_ID,
  MAXIMFAST_COEF_ID = 0x0B

};

// Index Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51)
enum READ_ALGORITHM_INDEX_BYTE {

  READ_AGC_PERCENTAGE = 0x00,
  READ_AGC_STEP_SIZE = 0x00,
  READ_AGC_SENSITIVITY = 0x00,
  READ_AGC_NUM_SAMPLES = 0x00,
  READ_MAX_FAST_COEF = 0x02

};

// Write Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51) and Index
// Byte: READ_ALGORITHM_INDEX_BYTE - AGC
enum READ_AGC_ALGO_WRITE_BYTE {

  READ_AGC_PERC_ID = 0x00,
  READ_AGC_STEP_SIZE_ID,
  READ_AGC_SENSITIVITY_ID,
  READ_AGC_NUM_SAMPLES_ID,
  READ_MAX_FAST_COEF_ID = 0x0B

};

// Index Byte under the Family Byte: ENABLE_ALGORITHM (0x52).
enum ALGORITHM_MODE_ENABLE_INDEX_BYTE {

  ENABLE_AGC_ALGO = 0x00,
  ENABLE_WHRM_ALGO = 0x02

};

// Index Byte under the Family Byte: BOOTLOADER_FLASH (0x80).
enum BOOTLOADER_FLASH_INDEX_BYTE {

  SET_INIT_VECTOR_BYTES = 0x00,
  SET_AUTH_BYTES,
  SET_NUM_PAGES,
  ERASE_FLASH,
  SEND_PAGE_VALUE

};

// Index Byte under the Family Byte: BOOTLOADER_INFO (0x81).
enum BOOTLOADER_INFO_INDEX_BYTE {

  BOOTLOADER_VERS = 0x00,
  PAGE_SIZE

};

// Index Byte under the Family Byte: IDENTITY (0xFF).
enum IDENTITY_INDEX_BYTES {

  READ_MCU_TYPE = 0x00,
  READ_SENSOR_HUB_VERS = 0x03,
  READ_ALGO_VERS = 0x07

};

// Functions------------

// This function uses the given family, index, and write byte to enable
// the given sensor.
uint8_t enableWrite(uint8_t _familyByte, uint8_t _indexByte,
                    uint8_t _enableByte);

// This function uses the given family, index, and write byte to communicate
// with the MAX32664 which in turn communicates with downward sensors. There
// are two steps demonstrated in this function. First a write to the MCU
// indicating what you want to do, a delay, and then a read to confirm positive
// transmission.
uint8_t writeByte_1(uint8_t _familyByte, uint8_t _indexByte,
                    uint8_t _writeByte);

// This function sends is simliar to the one above and sends info to the
// MAX32664 but takes an additional uint8_t as a paramter. Again there is the
// write of the specific bytes followed by a read to confirm positive
// transmission.
// uint8_t writeByte_2(uint8_t, uint8_t, uint8_t, uint8_t);

// This function is the same as the function above and uses the given family,
// index, and write byte, but also takes a 16 bit integer as a paramter to
// communicate with the MAX32664 which in turn communicates with downward
// sensors. There are two steps demonstrated in this function. First a write to
// the MCU indicating what you want to do, a delay, and then a read to confirm
// positive transmission.
uint8_t writeByte_3(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte,
                    uint16_t _val);

// This function handles all read commands or stated another way, all
// information requests. It starts a request by writing the family byte, index
// byte, and delays 60 microseconds, during which the MAX32664 retrieves the
// requested information. An I-squared-C request is then issued, and the
// information is read and returned.
uint8_t readByte_1(uint8_t _familyByte, uint8_t _indexByte);

// This function is exactly as the one above except it accepts a Write Byte as
// a paramter. It starts a request by writing the family byte, index byte, and
// write byte to the MAX32664, delays 60 microseconds, during which
// the MAX32664 retrieves the requested information. A I-squared-C request is
// then issued, and the information is read and returned.
uint8_t readByte_2(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte);

uint8_t readFillArray(uint8_t _familyByte, uint8_t _indexByte,
                      uint8_t arraySize, uint8_t *array);

// Functions ------------

// Family Byte: READ_DEVICE_MODE (0x02) Index Byte: 0x00, Write Byte: 0x00
// The following function initializes the sensor. To place the MAX32664 into
// application mode, the MFIO pin must be pulled HIGH while the board is held
// in reset for 10ms. After 50 addtional ms have elapsed the board should be
// in application mode and will return two bytes, the first 0x00 is a
// successful communcation byte, followed by 0x00 which is the byte indicating
// which mode the IC is in.
// uint8_t begin(TwoWire &wirePort = Wire);
uint8_t begin();

// Family Byte: HUB_STATUS (0x00), Index Byte: 0x00, No Write Byte.
// The following function checks the status of the FIFO.
uint8_t readSensorHubStatus();

// This function sets very basic settings to get sensor and biometric data.
// The biometric data includes data about heartrate, the confidence
// level, SpO2 levels, and whether the sensor has detected a finger or not.
uint8_t configBpm(uint8_t mode);

// This function takes the 8 bytes from the FIFO buffer related to the wrist
// heart rate algortihm: heart rate (uint16_t), confidence (uint8_t) , SpO2
// (uint16_t), and the finger detected status (uint8_t). Note that the the
// algorithm is stated as "wrist" though the sensor only works with the finger.
// The data is loaded into the whrmFifo and returned.
struct bioData readBpm();

// This function reads the CONFIGURATION_REGISTER (0x0A), bits [1:0] from the
// MAX30101 Sensor. It returns one of the four settings in microseconds.
uint16_t readPulseWidth();

// Family Byte: ENABLE_SENSOR (0x44), Index Byte: ENABLE_MAX30101 (0x03), Write
// Byte: senSwitch (parameter - 0x00 or 0x01).
// This function enables the MAX30101.
uint8_t max30101Control(uint8_t);

// Family Byte: READ_SENSOR_MODE (0x45), Index Byte: READ_ENABLE_MAX30101 (0x03)
// This function checks if the MAX30101 is enabled or not.
uint8_t readMAX30101State();

// Family Byte: OUTPUT_MODE (0x10), Index Byte: SET_FORMAT (0x00),
// Write Byte : outputType (Parameter values in OUTPUT_MODE_WRITE_BYTE)
uint8_t setOutputMode(uint8_t outputType);

// Family Byte: OUTPUT_MODE, Index Byte: WRITE_SET_THRESHOLD, Write byte:
// intThres (parameter - value betwen 0 and 0xFF). This function changes the
// threshold for the FIFO interrupt bit/pin. The interrupt pin is the MFIO pin
// which is set to INPUT after IC initialization (begin).
uint8_t setFifoThreshold(uint8_t intThresh);

// Family Byte: READ_DATA_OUTPUT (0x12), Index Byte: NUM_SAMPLES (0x00), Write
// Byte: NONE
// This function returns the number of samples available in the FIFO.
uint8_t numSamplesOutFifo();

// Family Byte: READ_ALGORITHM_CONFIG (0x51), Index Byte:
// READ_AGC_NUM_SAMPLES (0x00), Write Byte: READ_AGC_NUM_SAMPlES_ID (0x03)
// This function changes the number of samples that are averaged.
// It takes a paramater of zero to 255.
uint8_t readAlgoSamples();

// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_AGC_ALGO (0x00)
// This function enables (one) or disables (zero) the automatic gain control
// algorithm.
uint8_t agcAlgoControl(uint8_t enable);

// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_WHRM_ALGO (0x02)
// This function enables (one) or disables (zero) the wrist heart rate monitor
// algorithm.
uint8_t maximFastAlgoControl(uint8_t mode);

void heartbeat__i2c_transaction(uint8_t *read_buffer, uint8_t *write_buffer,
                                int read_size, int write_size);

#endif
