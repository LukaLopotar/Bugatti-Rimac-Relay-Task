/**
 * @brief simulation_config.h
 * 
 * Config file with hardcoded values used only for the running of the simulation.
 */

#ifndef SIMULATION_CONFIG_H_
#define SIMULATION_CONFIG_H_

#include <stdbool.h>

#include "relays_config.h"
#include "hal_dio.h"
#include "relay_constants.h"

/* Periodic task simulation configuration */
#define SIM_CONFIG_TASK_STEP_MS 10
#define SIM_CONFIG_TICKS_LIMIT 7
#define RUN_SIMULATION

/* For faking hal_dio return values if RUN_SIMULATION is enabled: */
extern uint32_t sim_tick;

extern bool writeDpo_ReturnValueSimulation[NUM_OF_RELAYS];
extern bool readDi_ReturnValueSimulation[NUM_OF_RELAYS];
extern DioLevel readDi_ReturnLevelSimulation[NUM_OF_RELAYS];
#endif /* SIMULATION_CONFIG_H_ */