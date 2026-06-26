/**
 * @brief hal_dio.c
 * 
 * See hal_dio.h file.
 */

#include "hal_dio.h"
#include "simulation_config.h"

bool HAL_Dio_WriteDpo(const DpoLine line, DioLevel level) {
#ifndef RUN_SIMULATION
  (void) line;
#endif
  (void) level;

  /* Here we would handle any other HAL driver calls and any potential errors the
  HAL driver can return. For the sake of this task, I will just return true here. */


#ifdef RUN_SIMULATION
  return writeDpo_ReturnValueSimulation[line];
#else
  return true;
#endif
}

bool HAL_Dio_ReadDi(const DiLine line, DioLevel *level) {
  (void) line;

  /* Here we would handle any other HAL driver calls and any potential errors the
  HAL driver can return. For the sake of this task, I will just return true here
  and set DI to high. */

#ifdef RUN_SIMULATION
  *level = readDi_ReturnLevelSimulation[line];
  return readDi_ReturnValueSimulation[line];
#else
  *level = DIO_LEVEL_HIGH;
  return true;
#endif
}