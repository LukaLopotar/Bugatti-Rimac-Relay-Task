/**
 * @brief simulation_config.c
 * 
 * See simulation_config.h file.
 */

#include "simulation_config.h"

uint32_t sim_tick = 1;

bool writeDpo_ReturnValueSimulation[NUM_OF_RELAYS] = {
  true, // Relay 0
  true, // Relay 1
  true // Relay 2
};
bool readDi_ReturnValueSimulation[NUM_OF_RELAYS] = {
  true, // Relay 0
  true, // Relay 1
  true // Relay 2
};
DioLevel readDi_ReturnLevelSimulation[NUM_OF_RELAYS] = {
  DIO_LEVEL_HIGH, // Relay 0
  DIO_LEVEL_HIGH, // Relay 1
  DIO_LEVEL_HIGH // Relay 2
};