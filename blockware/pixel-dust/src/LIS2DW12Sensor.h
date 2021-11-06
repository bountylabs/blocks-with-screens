/**
 ******************************************************************************
 * @file    LIS2DW12Sensor.h
 * @author  CLab
 * @version V1.0.0
 * @date    15 November 2018
 * @brief   Abstract Class of an LIS2DW12 Inertial Measurement Unit (IMU) 3 axes
 *          sensor.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */


/* Prevent recursive inclusion -----------------------------------------------*/

#ifndef __LIS2DW12Sensor_H__
#define __LIS2DW12Sensor_H__


/* Includes ------------------------------------------------------------------*/

#include "Wire.h"
#include "SPI.h"
#include "lis2dw12_reg.h"

/* Defines -------------------------------------------------------------------*/

#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_2G_LOPOW1_MODE   0.976f  /**< Sensitivity value for 2g full scale, Low-power1 mode [mg/LSB] */
#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_2G_OTHER_MODES   0.244f  /**< Sensitivity value for 2g full scale, all other modes except Low-power1 [mg/LSB] */

#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_4G_LOPOW1_MODE   1.952f  /**< Sensitivity value for 4g full scale, Low-power1 mode [mg/LSB] */
#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_4G_OTHER_MODES   0.488f  /**< Sensitivity value for 4g full scale, all other modes except Low-power1 [mg/LSB] */

#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_8G_LOPOW1_MODE   3.904f  /**< Sensitivity value for 8g full scale, Low-power1 mode [mg/LSB] */
#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_8G_OTHER_MODES   0.976f  /**< Sensitivity value for 8g full scale, all other modes except Low-power1 [mg/LSB] */

#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_16G_LOPOW1_MODE  7.808f  /**< Sensitivity value for 16g full scale, Low-power1 mode [mg/LSB] */
#define LIS2DW12_ACC_SENSITIVITY_FOR_FS_16G_OTHER_MODES  1.952f  /**< Sensitivity value for 16g full scale, all other modes except Low-power1 [mg/LSB] */

/* Typedefs ------------------------------------------------------------------*/
typedef enum
{
  LIS2DW12_STATUS_OK = 0,
  LIS2DW12_STATUS_ERROR
} LIS2DW12StatusTypeDef;

typedef struct
{
  unsigned int WakeUpStatus : 1;
  unsigned int D6DOrientationStatus : 1;
  unsigned int SleepStatus : 1;
} LIS2DW12_Event_Status_t;

typedef enum
{
  LIS2DW12_HIGH_PERFORMANCE_MODE,
  LIS2DW12_LOW_POWER_MODE4,
  LIS2DW12_LOW_POWER_MODE3,
  LIS2DW12_LOW_POWER_MODE2,
  LIS2DW12_LOW_POWER_MODE1
} LIS2DW12_Operating_Mode_t;

typedef enum
{
  LIS2DW12_LOW_NOISE_DISABLE,
  LIS2DW12_LOW_NOISE_ENABLE
} LIS2DW12_Low_Noise_t;

/* Class Declaration ---------------------------------------------------------*/
   
/**
 * Abstract class of an LIS2DW12 Inertial Measurement Unit (IMU) 3 axes
 * sensor.
 */
class LIS2DW12Sensor
{
  public:
    LIS2DW12Sensor(TwoWire *i2c, uint8_t address=LIS2DW12_I2C_ADD_H);
    LIS2DW12Sensor(SPIClass *spi, int cs_pin, uint32_t spi_speed=2000000);
    LIS2DW12StatusTypeDef begin();
    LIS2DW12StatusTypeDef end();
    LIS2DW12StatusTypeDef Enable_X(void);
    LIS2DW12StatusTypeDef Disable_X(void);
    LIS2DW12StatusTypeDef ReadID(uint8_t *id);
    LIS2DW12StatusTypeDef Get_X_Axes(int32_t *acceleration);
    LIS2DW12StatusTypeDef Get_X_Sensitivity(float *sensitivity);
    LIS2DW12StatusTypeDef Get_X_AxesRaw(int16_t *value);
    LIS2DW12StatusTypeDef Get_X_ODR(float *odr);
    LIS2DW12StatusTypeDef Set_X_ODR(float odr);
    LIS2DW12StatusTypeDef Set_X_ODR_With_Mode(float odr, LIS2DW12_Operating_Mode_t mode=LIS2DW12_HIGH_PERFORMANCE_MODE, LIS2DW12_Low_Noise_t noise=LIS2DW12_LOW_NOISE_DISABLE);
    LIS2DW12StatusTypeDef Get_X_FS(float *full_scale);
    LIS2DW12StatusTypeDef Set_X_FS(float full_scale);
    LIS2DW12StatusTypeDef Enable_Wake_Up_Detection(void);
    LIS2DW12StatusTypeDef Disable_Wake_Up_Detection(void);
    LIS2DW12StatusTypeDef Set_Wake_Up_Threshold(uint8_t thr);
    LIS2DW12StatusTypeDef Set_Wake_Up_Duration(uint8_t dur);
    LIS2DW12StatusTypeDef Enable_Inactivity_Detection(void);
    LIS2DW12StatusTypeDef Disable_Inactivity_Detection(void);
    LIS2DW12StatusTypeDef Set_Sleep_Duration(uint8_t dur);
    LIS2DW12StatusTypeDef Enable_6D_Orientation(void);
    LIS2DW12StatusTypeDef Disable_6D_Orientation(void);
    LIS2DW12StatusTypeDef Set_6D_Orientation_Threshold(uint8_t thr);
    LIS2DW12StatusTypeDef Get_6D_Orientation_XL(uint8_t *xl);
    LIS2DW12StatusTypeDef Get_6D_Orientation_XH(uint8_t *xh);
    LIS2DW12StatusTypeDef Get_6D_Orientation_YL(uint8_t *yl);
    LIS2DW12StatusTypeDef Get_6D_Orientation_YH(uint8_t *yh);
    LIS2DW12StatusTypeDef Get_6D_Orientation_ZL(uint8_t *zl);
    LIS2DW12StatusTypeDef Get_6D_Orientation_ZH(uint8_t *zh);
    LIS2DW12StatusTypeDef Get_Event_Status(LIS2DW12_Event_Status_t *status);
    LIS2DW12StatusTypeDef Get_FIFO_Num_Samples(uint16_t *num_samples);
    LIS2DW12StatusTypeDef Set_FIFO_Mode(uint8_t mode);
    LIS2DW12StatusTypeDef ReadReg(uint8_t reg, uint8_t *data);
    LIS2DW12StatusTypeDef WriteReg(uint8_t reg, uint8_t data);
    
    /**
     * @brief Utility function to read data.
     * @param  pBuffer: pointer to data to be read.
     * @param  RegisterAddr: specifies internal address register to be read.
     * @param  NumByteToRead: number of bytes to be read.
     * @retval 0 if ok, an error code otherwise.
     */
    uint8_t IO_Read(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToRead)
    {        
      if (dev_spi) {
        dev_spi->beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE3));

        digitalWrite(cs_pin, LOW);

        /* Write Reg Address */
        dev_spi->transfer(RegisterAddr | 0x80);
        /* Read the data */
        for (uint16_t i=0; i<NumByteToRead; i++) {
          *(pBuffer+i) = dev_spi->transfer(0x00);
        }
         
        digitalWrite(cs_pin, HIGH);

        dev_spi->endTransaction();

        return 0;
      }
		
      if (dev_i2c) {
        dev_i2c->beginTransmission(((uint8_t)(((address) >> 1) & 0x7F)));
        dev_i2c->write(RegisterAddr);
        dev_i2c->endTransmission(false);

        dev_i2c->requestFrom(((uint8_t)(((address) >> 1) & 0x7F)), (uint8_t) NumByteToRead);

        int i=0;
        while (dev_i2c->available()) {
          pBuffer[i] = dev_i2c->read();
          i++;
        }

        return 0;
      }

      return 1;
    }
    
    /**
     * @brief Utility function to write data.
     * @param  pBuffer: pointer to data to be written.
     * @param  RegisterAddr: specifies internal address register to be written.
     * @param  NumByteToWrite: number of bytes to write.
     * @retval 0 if ok, an error code otherwise.
     */
    uint8_t IO_Write(uint8_t* pBuffer, uint8_t RegisterAddr, uint16_t NumByteToWrite)
    {  
      if (dev_spi) {
        dev_spi->beginTransaction(SPISettings(spi_speed, MSBFIRST, SPI_MODE3));

        digitalWrite(cs_pin, LOW);

        /* Write Reg Address */
        dev_spi->transfer(RegisterAddr);
        /* Write the data */
        for (uint16_t i=0; i<NumByteToWrite; i++) {
          dev_spi->transfer(pBuffer[i]);
        }

        digitalWrite(cs_pin, HIGH);

        dev_spi->endTransaction();

        return 0;                    
      }
  
      if (dev_i2c) {
        dev_i2c->beginTransmission(((uint8_t)(((address) >> 1) & 0x7F)));

        dev_i2c->write(RegisterAddr);
        for (uint16_t i = 0 ; i < NumByteToWrite ; i++) {
          dev_i2c->write(pBuffer[i]);
        }

        dev_i2c->endTransmission(true);

        return 0;
      }

      return 1;
    }

  private:
    LIS2DW12StatusTypeDef Set_X_ODR_When_Enabled(float odr, LIS2DW12_Operating_Mode_t mode, LIS2DW12_Low_Noise_t noise);
    LIS2DW12StatusTypeDef Set_X_ODR_When_Disabled(float odr, LIS2DW12_Operating_Mode_t mode, LIS2DW12_Low_Noise_t noise);

    /* Helper classes. */
    TwoWire *dev_i2c;
    SPIClass *dev_spi;
    
    /* Configuration */
    uint8_t address;
    int cs_pin;
    uint32_t spi_speed;
    
    uint8_t X_isEnabled;
    float X_Last_ODR;
    LIS2DW12_Operating_Mode_t X_Last_Operating_Mode;
    LIS2DW12_Low_Noise_t X_Last_Noise;

    lis2dw12_ctx_t reg_ctx;
};

#ifdef __cplusplus
 extern "C" {
#endif
int32_t LIS2DW12_io_write( void *handle, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t nBytesToWrite );
int32_t LIS2DW12_io_read( void *handle, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t nBytesToRead );
#ifdef __cplusplus
  }
#endif

#endif
