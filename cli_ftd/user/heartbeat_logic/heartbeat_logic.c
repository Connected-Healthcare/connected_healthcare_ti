#include "heartbeat_logic.h"

#define DEBUG_PRINTFS 0

// Variables ------------
uint8_t bpmArr[MAXFAST_ARRAY_SIZE];
uint8_t bpmArrTwo[MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA];
uint8_t senArr[MAX30101_LED_ARRAY];
uint8_t bpmSenArr[MAXFAST_ARRAY_SIZE + MAX30101_LED_ARRAY];
uint8_t bpmSenArrTwo[MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA +
                     MAX30101_LED_ARRAY];

const uint8_t BIO_ADDRESS = 0x55;
// uint8_t _resetPin;
// uint8_t _mfioPin;
uint8_t _address = 0x55;
uint32_t _writeCoefArr[3];
uint8_t _userSelectedMode;
uint8_t _sampleRate = 100;

// Static Function Declarations
static void heartbeat__initialize_application_mode();

// Heartbeat High-level APIs

uint8_t begin() {
  heartbeat__initialize_application_mode();
  uint8_t responseByte = readByte_1(READ_DEVICE_MODE, 0x00);
  return responseByte;
}

uint8_t configBpm(uint8_t mode) {

  uint8_t statusChauf = 0;
  if (mode == MODE_ONE || mode == MODE_TWO) {
  } else
    return INCORR_PARAM;

  statusChauf = setOutputMode(ALGO_DATA); // Just the data
  if (statusChauf != SUCCESS)
    return statusChauf;

  statusChauf = setFifoThreshold(0x01); // One sample before interrupt is fired.
  if (statusChauf != SUCCESS)
    return statusChauf;

  statusChauf = agcAlgoControl(ENABLE); // One sample before interrupt is fired.
  if (statusChauf != SUCCESS)
    return statusChauf;

  statusChauf = max30101Control(ENABLE);
  if (statusChauf != SUCCESS)
    return statusChauf;

  statusChauf = maximFastAlgoControl(mode);
  if (statusChauf != SUCCESS)
    return statusChauf;

  _userSelectedMode = mode;
  _sampleRate = readAlgoSamples();

  sleep(1); // 1000 ms
  return SUCCESS;
}

// This function takes the 8 bytes from the FIFO buffer related to the wrist
// heart rate algortihm: heart rate (uint16_t), confidence (uint8_t) , SpO2
// (uint16_t), and the finger detected status (uint8_t). Note that the the
// algorithm is stated as "wrist" though the sensor only works with the finger.
// The data is loaded into the whrmFifo and returned.
struct bioData readBpm() {

  struct bioData libBpm;
  uint8_t statusChauf; // The status chauffeur captures return values.

  statusChauf = readSensorHubStatus();

  if (statusChauf == 1) { // Communication Error
    libBpm.heartRate = 0;
    libBpm.confidence = 0;
    libBpm.oxygen = 0;
    return libBpm;
  }

  numSamplesOutFifo();

  if (_userSelectedMode == MODE_ONE) {

    readFillArray(READ_DATA_OUTPUT, READ_DATA, MAXFAST_ARRAY_SIZE, bpmArr);

    // Heart Rate formatting
    libBpm.heartRate = (uint16_t)((bpmArr[0]) << 8);
    libBpm.heartRate |= (bpmArr[1]);
    libBpm.heartRate /= 10;

    // Confidence formatting
    libBpm.confidence = bpmArr[2];

    // Blood oxygen level formatting
    libBpm.oxygen = (uint16_t)(bpmArr[3] << 8);
    libBpm.oxygen |= bpmArr[4];
    libBpm.oxygen /= 10;

    //"Machine State" - has a finger been detected?
    libBpm.status = bpmArr[5];

    return libBpm;
  }

  else if (_userSelectedMode == MODE_TWO) {
    readFillArray(READ_DATA_OUTPUT, READ_DATA,
                  MAXFAST_ARRAY_SIZE + MAXFAST_EXTENDED_DATA, bpmArrTwo);

    // Heart Rate formatting
    libBpm.heartRate = (uint16_t)((bpmArrTwo[0]) << 8);
    libBpm.heartRate |= (bpmArrTwo[1]);
    libBpm.heartRate /= 10;

    // Confidence formatting
    libBpm.confidence = bpmArrTwo[2];

    // Blood oxygen level formatting
    libBpm.oxygen = (uint16_t)((bpmArrTwo[3]) << 8);
    libBpm.oxygen |= bpmArrTwo[4];
    libBpm.oxygen /= 10.0;

    //"Machine State" - has a finger been detected?
    libBpm.status = bpmArrTwo[5];

    // Sp02 r Value formatting
    uint16_t tempVal = (uint16_t)((bpmArrTwo[6]) << 8);
    tempVal |= bpmArrTwo[7];
    libBpm.rValue = tempVal;
    libBpm.rValue /= 10.0;

    // Extended Machine State formatting
    libBpm.extStatus = bpmArrTwo[8];

    // There are two additional bytes of data that were requested but that
    // have not been implemented in firmware 10.1 so will not be saved to
    // user's data.
    return libBpm;
  }

  else {
    libBpm.heartRate = 0;
    libBpm.confidence = 0;
    libBpm.oxygen = 0;
    return libBpm;
  }
}

// Heartbeat Mid-level APIs

// Family Byte: OUTPUT_MODE (0x10), Index Byte: SET_FORMAT (0x00),
// Write Byte : outputType (Parameter values in OUTPUT_MODE_WRITE_BYTE)
uint8_t setOutputMode(uint8_t outputType) {

  if (outputType > SENSOR_ALGO_COUNTER) // Bytes between 0x00 and 0x07
    return INCORR_PARAM;

  // Check that communication was successful, not that the IC is outputting
  // correct format.
  uint8_t statusByte = writeByte_1(OUTPUT_MODE, SET_FORMAT, outputType);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;
}

// Family Byte: OUTPUT_MODE(0x10), Index Byte: WRITE_SET_THRESHOLD (0x01), Write
// byte: intThres (parameter - value betwen 0 and 0xFF). This function changes
// the threshold for the FIFO interrupt bit/pin. The interrupt pin is the MFIO
// pin which is set to INPUT after IC initialization (begin).
uint8_t setFifoThreshold(uint8_t intThresh) {

  // Checks that there was succesful communcation, not that the threshold was
  // set correctly.
  uint8_t statusByte = writeByte_1(OUTPUT_MODE, WRITE_SET_THRESHOLD, intThresh);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;
}

// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_AGC_ALGO (0x00)
// This function enables (one) or disables (zero) the automatic gain control
// algorithm.
uint8_t agcAlgoControl(uint8_t enable) {

  if (enable == 0 || enable == 1) {
  } else
    return INCORR_PARAM;

  uint8_t statusByte = enableWrite(ENABLE_ALGORITHM, ENABLE_AGC_ALGO, enable);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;
}

// Family Byte: ENABLE_SENSOR (0x44), Index Byte: ENABLE_MAX30101 (0x03), Write
// Byte: senSwitch  (parameter - 0x00 or 0x01).
// This function enables the MAX30101.
uint8_t max30101Control(uint8_t senSwitch) {

  if (senSwitch == 0 || senSwitch == 1) {
  } else
    return INCORR_PARAM;

  // Check that communication was successful, not that the sensor is enabled.
  uint8_t statusByte = enableWrite(ENABLE_SENSOR, ENABLE_MAX30101, senSwitch);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;
}

// Family Byte: ENABLE_ALGORITHM (0x52), Index Byte:
// ENABLE_WHRM_ALGO (0x02)
// This function enables (one) or disables (zero) the wrist heart rate monitor
// algorithm.
uint8_t maximFastAlgoControl(uint8_t mode) {

  if (mode == 0 || mode == 1 || mode == 2) {
  } else
    return INCORR_PARAM;

  uint8_t statusByte = enableWrite(ENABLE_ALGORITHM, ENABLE_WHRM_ALGO, mode);
  if (statusByte != SUCCESS)
    return statusByte;
  else
    return SUCCESS;
}

// Family Byte: READ_ALGORITHM_CONFIG (0x51), Index Byte:
// READ_AGC_NUM_SAMPLES (0x00), Write Byte: READ_AGC_NUM_SAMPLES_ID (0x03)
// This function changes the number of samples that are averaged.
// It takes a paramater of zero to 255.
uint8_t readAlgoSamples() {

  uint8_t samples = readByte_2(READ_ALGORITHM_CONFIG, READ_AGC_NUM_SAMPLES,
                               READ_AGC_NUM_SAMPLES_ID);
  return samples;
}

// Family Byte: HUB_STATUS (0x00), Index Byte: 0x00, No Write Byte.
// The following function checks the status of the FIFO.
uint8_t readSensorHubStatus() {

  uint8_t status = readByte_1(0x00, 0x00); // Just family and index byte.
  return status;                           // Will return 0x00
}

// Family Byte: READ_DATA_OUTPUT (0x12), Index Byte: NUM_SAMPLES (0x00), Write
// Byte: NONE
// This function returns the number of samples available in the FIFO.
uint8_t numSamplesOutFifo() {

  uint8_t sampAvail = readByte_1(READ_DATA_OUTPUT, NUM_SAMPLES);
  return sampAvail;
}

// Utilities (Heartbeat Low-level APIs)

static void heartbeat__initialize_application_mode() {

  GPIO_setConfig(CONFIG_PIN_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                   GPIO_CFG_OUT_HIGH); // RST
  GPIO_setConfig(CONFIG_PIN_3, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                   GPIO_CFG_OUT_LOW); // MFIO

  GPIO_write(CONFIG_PIN_3, 1); // MFIO - Pin 26
  GPIO_write(CONFIG_PIN_2, 0); // RST - Pin 25
  // delay 10 ms
  usleep(10000);
  GPIO_write(CONFIG_PIN_2, 1); // RST
  sleep(1);
}

uint8_t enableWrite(uint8_t _familyByte, uint8_t _indexByte,
                    uint8_t _enableByte) {
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit enableWrite()\r\n");
#endif

  uint8_t number_of_bytes_to_read = 1;
  uint8_t number_of_bytes_to_write = 3;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];

  memset(writeBuffer, 0, sizeof(readBuffer));
  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;
  writeBuffer[2] = _enableByte;

  // heartbeat__i2c_transaction(readBuffer, writeBuffer,
  // number_of_bytes_to_read,number_of_bytes_to_write);

  memset(readBuffer, 0, sizeof(readBuffer));

  // initialize optional I2C bus parameters
  I2C_Params params;
  I2C_Params_init(&params);
  params.bitRate = I2C_400kHz;

  // Open I2C bus for usage
  I2C_Handle i2cHandle = I2C_open(CONFIG_I2C_0, &params);

  // Initialize slave address of transaction
  I2C_Transaction i2cTransaction = {0};

  i2cTransaction.slaveAddress = BIO_ADDRESS;
  i2cTransaction.writeBuf = writeBuffer;
  i2cTransaction.writeCount = number_of_bytes_to_write;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  bool status = I2C_transfer(i2cHandle, &i2cTransaction);
  if (status == false) {
    if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
// slave address not acknowledged
#if DEBUG_PRINTFS
      printf("Slave address t1 not acknowledged\r\n");
#endif
    }
  } else {
#if DEBUG_PRINTFS
    printf("Success\r\n");
#endif
  }

  // Close I2C
  I2C_close(i2cHandle);
  usleep(ENABLE_CMD_DELAY);
  I2C_Handle i2cHandle2 = I2C_open(CONFIG_I2C_0, &params);
  I2C_Transaction i2cTransaction2 = {0};

  i2cTransaction2.slaveAddress = 0x55;
  i2cTransaction2.writeBuf = NULL;
  i2cTransaction2.writeCount = 0;
  i2cTransaction2.readBuf = readBuffer;
  i2cTransaction2.readCount = number_of_bytes_to_read;

  bool status2 = I2C_transfer(i2cHandle2, &i2cTransaction2);
  if (status2 == false) {
    if (i2cTransaction2.status == I2C_STATUS_ADDR_NACK) {
// slave address not acknowledged
#if DEBUG_PRINTFS
      printf("Slave address t2 not acknowledged\r\n");
#endif
    }
  } else {
#if DEBUG_PRINTFS
    printf("Success\r\n");
#endif
  }
  I2C_close(i2cHandle2);
  for (int i = 0; i < number_of_bytes_to_read; i++) {
#if DEBUG_PRINTFS
    printf("%x\r\n", readBuffer[i]);
#endif
  }
#if DEBUG_PRINTFS
  printf("\r\n");
#endif

  statusByte = readBuffer[0];

  return statusByte;
}

uint8_t writeByte_1(uint8_t _familyByte, uint8_t _indexByte,
                    uint8_t _writeByte) {
  // uint8_t returnByte;
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit writeByte_1()\r\n");
#endif

  uint8_t number_of_bytes_to_read = 1;
  uint8_t number_of_bytes_to_write = 3;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];

  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;
  writeBuffer[2] = _writeByte;

  heartbeat__i2c_transaction(readBuffer, writeBuffer, number_of_bytes_to_read,
                             number_of_bytes_to_write);

  statusByte = readBuffer[0];

  return statusByte;
}

// TODO: Write writeByte_2() definition here

uint8_t writeByte_3(uint8_t _familyByte, uint8_t _indexByte, uint8_t _writeByte,
                    uint16_t _val) {
  // uint8_t returnByte;
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit writeByte_1()\r\n");
#endif

  uint8_t number_of_bytes_to_read = 1;
  uint8_t number_of_bytes_to_write = 5;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];
  memset(writeBuffer, 0, sizeof(writeBuffer));
  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;
  writeBuffer[2] = _writeByte;
  writeBuffer[3] = ((_val >> 8) & 0xFF); // MSB
  writeBuffer[4] = (_val & 0xFF);        // LSB

  heartbeat__i2c_transaction(readBuffer, writeBuffer, number_of_bytes_to_read,
                             number_of_bytes_to_write);

  statusByte = readBuffer[0];
  // returnByte = readBuffer[1];

  return statusByte;
}

uint8_t readByte_1(uint8_t _familyByte, uint8_t _indexByte) {
  uint8_t returnByte;
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit readByte_1()\r\n");
#endif

  uint8_t number_of_bytes_to_read = 2;
  uint8_t number_of_bytes_to_write = 2;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];
  memset(writeBuffer, 0, sizeof(writeBuffer));

  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;

  heartbeat__i2c_transaction(readBuffer, writeBuffer, number_of_bytes_to_read,
                             number_of_bytes_to_write);

  statusByte = readBuffer[0];
  returnByte = readBuffer[1];
  if (statusByte) {
    return statusByte;
  }

  return returnByte;
}

uint8_t readByte_2(uint8_t _familyByte, uint8_t _indexByte,
                   uint8_t _writeByte) {
  uint8_t returnByte;
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit readByte_2()\r\n");
#endif

  uint8_t number_of_bytes_to_read = 2;
  uint8_t number_of_bytes_to_write = 3;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];

  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;
  writeBuffer[2] = _writeByte;

  heartbeat__i2c_transaction(readBuffer, writeBuffer, number_of_bytes_to_read,
                             number_of_bytes_to_write);

  statusByte = readBuffer[0];
  returnByte = readBuffer[1];
  if (statusByte) {
    return statusByte;
  }

  return returnByte;
}

void heartbeat__i2c_transaction(uint8_t *read_buffer, uint8_t *write_buffer,
                                int read_size, int write_size) {
#if DEBUG_PRINTFS
  printf("Hit heartbeat__i2c_transaction\r\n");
#endif

  uint8_t number_of_bytes_to_read = read_size;
  uint8_t number_of_bytes_to_write = write_size;

  memset(read_buffer, 0, read_size);

  // initialize optional I2C bus parameters
  I2C_Params params;
  I2C_Params_init(&params);
  params.bitRate = I2C_400kHz;

  // Open I2C bus for usage
  I2C_Handle i2cHandle = I2C_open(CONFIG_I2C_0, &params);

  // Initialize slave address of transaction
  I2C_Transaction i2cTransaction = {0};

  i2cTransaction.slaveAddress = BIO_ADDRESS;
  i2cTransaction.writeBuf = write_buffer;
  i2cTransaction.writeCount = number_of_bytes_to_write;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  bool status = I2C_transfer(i2cHandle, &i2cTransaction);
  if (status == false) {
    if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
// slave address not acknowledged
#if DEBUG_PRINTFS
      printf("Slave address not acknowledged on i2c write\r\n");
#endif
    }
  } else {
#if DEBUG_PRINTFS
    printf("Write Success\r\n");
#endif
  }

  // Close I2C
  I2C_close(i2cHandle);
  usleep(CMD_DELAY);
  I2C_Handle i2cHandle2 = I2C_open(CONFIG_I2C_0, &params);
  I2C_Transaction i2cTransaction2 = {0};

  i2cTransaction2.slaveAddress = BIO_ADDRESS;
  i2cTransaction2.writeBuf = NULL;
  i2cTransaction2.writeCount = 0;
  i2cTransaction2.readBuf = read_buffer;
  i2cTransaction2.readCount = number_of_bytes_to_read;

  bool status2 = I2C_transfer(i2cHandle2, &i2cTransaction2);
  if (status2 == false) {
    if (i2cTransaction2.status == I2C_STATUS_ADDR_NACK) {
// slave address not acknowledged
#if DEBUG_PRINTFS
      printf("Slave address not acknowledged on i2c write\r\n");
#endif
    }
  } else {
#if DEBUG_PRINTFS
    printf("Read Success\r\n");
#endif
  }
  I2C_close(i2cHandle2);
  for (int i = 0; i < number_of_bytes_to_read; i++) {
#if DEBUG_PRINTFS
    printf("%x\r\n", read_buffer[i]);
#endif
  }
#if DEBUG_PRINTFS
  printf("\r\n");
#endif
}

uint8_t readFillArray(uint8_t _familyByte, uint8_t _indexByte,
                      uint8_t arraySize, uint8_t *array) {
  // uint8_t returnByte;
  uint8_t statusByte;

#if DEBUG_PRINTFS
  printf("Hit readFillArray()\r\n");
#endif

  uint8_t number_of_bytes_to_read = (1 + arraySize);
  uint8_t number_of_bytes_to_write = 2;

  uint8_t readBuffer[number_of_bytes_to_read];
  uint8_t writeBuffer[number_of_bytes_to_write];
  memset(writeBuffer, 0, sizeof(writeBuffer));

  writeBuffer[0] = _familyByte;
  writeBuffer[1] = _indexByte;

  heartbeat__i2c_transaction(readBuffer, writeBuffer, number_of_bytes_to_read,
                             number_of_bytes_to_write);

  statusByte = readBuffer[0];
  if (statusByte) {
    for (uint8_t i = 0; i < arraySize; i++) {
      array[i] = 0;
    }
    return statusByte; // Note: In Sparkfun driver, they return the array not
                       // statusByte
  }

  for (int i = 1; i <= arraySize; i++) {
    array[i - 1] = readBuffer[i];
  }

  return statusByte;
}
