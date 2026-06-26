#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "relay_controller.h"
#include "simulation_config.h"
#include "relays_config.h"

const char *RelayState_ToString(RelayState state) {
  switch (state) {
    case RELAY_STATE_UNKNOWN:
      return "UNKNOWN";

    case RELAY_STATE_OPEN:
      return "OPEN";

    case RELAY_STATE_OPENING:
      return "OPENING";

    case RELAY_STATE_CLOSING:
      return "CLOSING";

    case RELAY_STATE_CLOSED:
      return "CLOSED";

    case RELAY_STATE_ERROR:
      return "ERROR";

    default:
      return "INVALID";
  }
}

const char *RelayErrorType_ToString(RelayErrorType error) {
  switch (error) {
    case RELAY_ERROR_TYPE_NO_ERROR:
      return "NO ERROR";

    case RELAY_ERROR_TYPE_WELDED:
      return "WELDED";

    case RELAY_ERROR_TYPE_CONSTANTLY_OPEN:
      return "CONSTANTLY OPEN";

    case RELAY_ERROR_TYPE_UNKNOWN_ERROR:
      return "UNKNOWN ERROR";

    default:
      return "INVALID";
  }
}

const char *RelayControllerState_ToString(RelayControllerState state) {
  switch (state) {
    case RELAY_CONTROLLER_INIT:
      return "INIT";

    case RELAY_CONTROLLER_RUNNING:
      return "RUNNING";

    case RELAY_CONTROLLER_ERROR:
      return "ERROR";

    default:
      return "INVALID";
  }
}

void Simulation_SetHalDioResponses(uint32_t current_time_tick) {
  switch (current_time_tick) {
    case 1:
      readDi_ReturnLevelSimulation[0] = DIO_LEVEL_HIGH;
      readDi_ReturnLevelSimulation[1] = DIO_LEVEL_HIGH;
      readDi_ReturnLevelSimulation[2] = DIO_LEVEL_HIGH;
      break;
    case 4:
      readDi_ReturnLevelSimulation[0] = DIO_LEVEL_LOW;
      readDi_ReturnLevelSimulation[1] = DIO_LEVEL_LOW;
      readDi_ReturnLevelSimulation[2] = DIO_LEVEL_LOW;
      break;
  }
}

void Simulation_FakeRequests(RelayController *controller, uint32_t current_time_tick) {
  switch (current_time_tick) {
    case 1:
      RelayController_SetRequest(controller, 0, RELAY_REQUEST_OPEN);
      RelayController_SetRequest(controller, 1, RELAY_REQUEST_OPEN);
      break;
    case 4:
      RelayController_SetRequest(controller, 2, RELAY_REQUEST_OPEN);
      break;
  }
}

void Simulation_PrintStatus(RelayController *controller) {
  printf("Relay controller state: %s\n", RelayControllerState_ToString(controller->current_state));

  for (int i = 0; i < controller->relayCount; i++) {
    RelayState cur_relay_state = RELAY_STATE_UNKNOWN;
    RelayErrorType cur_relay_error_type = RELAY_ERROR_TYPE_NO_ERROR;
    RelayController_GetRelayState(controller, i, &cur_relay_state);
    RelayController_GetRelayErrorType(controller, i, &cur_relay_error_type);

    printf("Relay %d, state: %s, error type: %s\n", i, RelayState_ToString(cur_relay_state), RelayErrorType_ToString(cur_relay_error_type));
  }
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;

  RelayController controller = {
    .current_state = RELAY_CONTROLLER_INIT,
    .relayCount = NUM_OF_RELAYS,
    .configTable = relaysConfigTable,
    .statusTable = relaysStatusTable
  };

  RelayController_Init(&controller);

  /* This for loop here simulates the scheduler: */
  for (sim_tick = 1; sim_tick <= SIM_CONFIG_TICKS_LIMIT; sim_tick++) {
    printf("Current simulation tick: %d\n", sim_tick);
    printf("-----\n");
    Simulation_SetHalDioResponses(sim_tick);
    Simulation_FakeRequests(&controller, sim_tick);
    RelayController_MonitorRelaysTask(&controller);
    Simulation_PrintStatus(&controller);
    printf("=====\n");
  }

  return 0;
}
