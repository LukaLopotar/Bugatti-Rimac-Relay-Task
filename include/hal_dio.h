/**
 * @brief hal_dio.h
 *
 * Mock of a HAL driver for controlling Digital Inputs and Digital Power Outputs
 * of the system.
 *
 * The DPO and DI lines act as a pair; in other words, if a new level is set
 * through the DPO line, the DI line should also change to that new level.
 */

#ifndef HAL_DIO_H_
#define HAL_DIO_H_

#include <stdbool.h>

/**
 * @brief Digital Input/Output level options.
 */
typedef enum {
  /** A starting value for when a new DioLevel variable is first defined. */
  DIO_LEVEL_UNKNOWN,
  DIO_LEVEL_LOW,
  DIO_LEVEL_HIGH
} DioLevel;

/**
 * @brief Available Digital Power Output lines.
 */
typedef enum {
  DPO_LINE_1,
  DPO_LINE_2,
  DPO_LINE_3,
} DpoLine;

/**
 * @brief Available Digital Input lines.
 */
typedef enum {
  DI_LINE_1,
  DI_LINE_2,
  DI_LINE_3,
} DiLine;

/**
 * @brief Writes the provided level to the chosen DPO line.
 *
 * @param[in]  line  - The chosen DPO line.
 * @param[in]  level - The level set the DPO line to.
 *
 * @return boolean - True if the provided level was changed successfully on the DPO line, false otherwise.
 */
bool HAL_Dio_WriteDpo(const DpoLine line, const DioLevel level);

/**
 * @brief Reads the current level of the DI line.
 *
 * @param[in]  line  - The chosen DI line.
 * @param[out]  level - The level the DI line is currently set to.
 *
 * @return boolean - True if the current level of DI line was read successfully, false otherwise.
 */
bool HAL_Dio_ReadDi(const DiLine line, DioLevel *level);

#endif /* HAL_DIO_H_ */