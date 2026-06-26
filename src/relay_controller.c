/**
 * @brief relay_controller.c
 * 
 * See relay_controller.h file.
 */

#include "relay_controller.h"

#include <stdio.h>

#include "simulation_config.h"
#include "hal_dio.h"
#include "relays_config.h"

#define COLOR_RED     "\033[31m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_RESET   "\033[0m"

/**
 * @brief Returns appropriate DPO level for the type of request provided and the
 *        relay type (normally closed or normally open).
 *
 * @param[in]  relay_type  - relay type (normally close or open).
 * @param[in]  relay_request  - request to either open or close the relay.
 * @param[out]  level  - DPO level needed to open or close the relay.
 *
 * @return boolean - True if DPO level was fetched successfully, false otherwise.
 */
static bool Relay_GetDioLevelForRequest(const RelayType relay_type,
    const RelayRequest relay_request, DioLevel *level) {
  /* Early exit in case of errors. */
  if (level == NULL) {
    return false;
  }

  if (relay_type >= RELAY_TYPE_COUNT) {
    return false;
  }

  if (relay_request >= RELAY_REQUEST_COUNT) {
    return false;
  }

  /* Handling of every combination of relay type and relay request. */
  if (relay_type == RELAY_TYPE_NORMALLY_OPEN) {
    *level = (relay_request == RELAY_REQUEST_OPEN ? DIO_LEVEL_LOW : DIO_LEVEL_HIGH);
    return true;
  } else if (relay_type == RELAY_TYPE_NORMALLY_CLOSED) {
    *level = (relay_request == RELAY_REQUEST_CLOSE ? DIO_LEVEL_LOW : DIO_LEVEL_HIGH);
    return true;
  }

  /* If we arrived here, that means there was some unhandled combination of
  relay type and relay request. */
  return false;
}

/**
 * @brief Helper function to set the relay controller and the relay itself to
 *        error state. It also saves the error type inside the relay status.
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 * @param[in]  relay_id  - ID of the relay to set the error.
 * @param[in]  error_type  - error type to set the relay to.
 *
 * @return None.
 */
static void RelayController_SetError(RelayController *controller,
                                     const uint8_t relay_id,
                                     const RelayErrorType error_type) {
  RelayStatus *relay_status = &controller->statusTable[relay_id];

  relay_status->relayState = RELAY_STATE_ERROR;
  relay_status->relayErrorType = error_type;
  controller->current_state = RELAY_CONTROLLER_ERROR;
}

/**
 * @brief Helper function to open all of the relays in case of an error.
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 *
 * @return None.
 */
static void RelayController_HandleErrorOpenAllRelays(RelayController *controller) {
  RelayRequest open_request = RELAY_REQUEST_OPEN;
  DioLevel level = DIO_LEVEL_UNKNOWN;

  for (uint8_t i = 0; i < controller->relayCount; i++) {
    const RelayConfig *current_relay_config = &controller->configTable[i];

    if (!Relay_GetDioLevelForRequest(current_relay_config->relayType, open_request, &level)) {
      /* At this point there is nothing else we can do if this fails. We are
      continuing since we don't know the right DPO level to set to open the relay properly. */
      continue;
    }

    HAL_Dio_WriteDpo(current_relay_config->dpoLine, level);
    controller->statusTable[i].relayState = RELAY_STATE_OPENING;
    controller->statusTable[i].timeElapsedMs = 0;
  }
}

bool RelayController_Init(RelayController *controller) {
  if (controller == NULL) {
    printf(COLOR_RED "Error: Controller is NULL.\n" COLOR_RESET);
    return false;
  }

  if (controller->configTable == NULL) {
    printf(COLOR_RED "Error: Invalid relay config table.\n" COLOR_RESET);
    controller->current_state = RELAY_CONTROLLER_ERROR;
    return false;
  }

  if (controller->statusTable == NULL) {
    printf(COLOR_RED "Error: Invalid relay status table.\n" COLOR_RESET);
    controller->current_state = RELAY_CONTROLLER_ERROR;
    return false;
  }

  if (controller->relayCount == 0 || 
      controller->relayCount > MAX_NUM_OF_RELAYS) {
    printf(COLOR_RED "Error: Invalid relay count.\n" COLOR_RESET);
    controller->current_state = RELAY_CONTROLLER_ERROR;
    return false;
  }

  if (controller->current_state != RELAY_CONTROLLER_INIT) {
    printf(COLOR_YELLOW "Warning: Unable to initialize controller, controller is already working.\n" COLOR_RESET);
    /* If controller is already working, that means it has already been initialized. */
    return true; 
  }

  for (uint8_t i = 0; i < controller->relayCount; i++) {
    const RelayConfig *current_relay_config = &controller->configTable[i];
    RelayStatus *current_relay_status = &controller->statusTable[i];
    DioLevel level = DIO_LEVEL_UNKNOWN;

    if (!HAL_Dio_ReadDi(current_relay_config->diLine, &level)) {
      RelayController_SetError(controller, i, RELAY_ERROR_TYPE_UNKNOWN_ERROR);
      RelayController_HandleErrorOpenAllRelays(controller);
      return false;
    }

    current_relay_status->relayState = (level == DIO_LEVEL_HIGH ? RELAY_STATE_CLOSED : RELAY_STATE_OPEN);
    current_relay_status->relayErrorType = RELAY_ERROR_TYPE_NO_ERROR;
    current_relay_status->timeElapsedMs = 0;
  }

  /* Here we would also start our RelaysMonitor task. */
  controller->current_state = RELAY_CONTROLLER_RUNNING;
  return true;
}

void RelayController_MonitorRelaysTask(RelayController *controller) {
  if (controller == NULL) {
    printf(COLOR_RED "Error: Controller is NULL.\n" COLOR_RESET);
    return;
  }

  for (uint8_t i = 0; i < controller->relayCount; i++) {
    RelayStatus *current_relay_status = &controller->statusTable[i];

    if (current_relay_status->relayState != RELAY_STATE_OPENING &&
        current_relay_status->relayState != RELAY_STATE_CLOSING) {
      continue;
    }

    current_relay_status->timeElapsedMs += SIM_CONFIG_TASK_STEP_MS;

    if (current_relay_status->timeElapsedMs < RELAY_FEEDBACK_TIME_MS) {
      continue;
    }

    current_relay_status->timeElapsedMs = 0;

    const RelayConfig *current_relay_config = &controller->configTable[i];
    DioLevel level = DIO_LEVEL_UNKNOWN;

    if (!HAL_Dio_ReadDi(current_relay_config->diLine, &level)) {
      RelayController_SetError(controller, i, RELAY_ERROR_TYPE_UNKNOWN_ERROR);
      RelayController_HandleErrorOpenAllRelays(controller);
      return;
    }

    switch (current_relay_status->relayState) {
      case RELAY_STATE_OPENING:
        if (level == DIO_LEVEL_HIGH) {
          RelayController_SetError(controller, i, RELAY_ERROR_TYPE_WELDED);
          RelayController_HandleErrorOpenAllRelays(controller);
          return;
        }
        current_relay_status->relayState = RELAY_STATE_OPEN;
        break;

      case RELAY_STATE_CLOSING:
        if (level == DIO_LEVEL_LOW) {
          RelayController_SetError(controller, i, RELAY_ERROR_TYPE_CONSTANTLY_OPEN);
          RelayController_HandleErrorOpenAllRelays(controller);
          return;
        }
        current_relay_status->relayState = RELAY_STATE_CLOSED;
        break;

      default:
        /* Intentionally ignored since only the two state above can happen at
        this point in the function. */
    }
  }

  /* We would also have some kind of sleep here to avoid starving other tasks in the system. */
}

const char *RelayRequest_ToString(RelayRequest request) {
  switch (request) {
    case RELAY_REQUEST_OPEN:
      return "OPEN";

    case RELAY_REQUEST_CLOSE:
      return "CLOSE";

    default:
      return "INVALID";
  }
}

bool RelayController_SetRequest(RelayController *controller, const uint8_t relay_id, const RelayRequest request) {
  printf("Processing request for relay %d to %s the relay...\n", relay_id, RelayRequest_ToString(request));

  if (controller == NULL) {
    printf(COLOR_RED "Error: Controller is NULL.\n" COLOR_RESET);
    return false;
  }

  if (controller->current_state != RELAY_CONTROLLER_RUNNING) {
    printf(COLOR_RED "Error: Controller not available.\n" COLOR_RESET);
    return false;
  }

  if (relay_id >= controller->relayCount) {
    printf(COLOR_RED "Error: invalid value for relay ID provided.\n" COLOR_RESET);
    return false;
  }
  const RelayConfig *requested_relay_config = &controller->configTable[relay_id];

  DioLevel level = DIO_LEVEL_UNKNOWN;
  RelayType type = requested_relay_config->relayType;

  if (!Relay_GetDioLevelForRequest(type, request, &level)) {
    printf(COLOR_RED "Error: Unable to get DPO level required for the relay type and relay request.\n" COLOR_RESET);
    return false;
  }

  DpoLine dpo_line = requested_relay_config->dpoLine;

  if (!HAL_Dio_WriteDpo(dpo_line, level)) {
    printf(COLOR_RED "Error: Unable to set DPO line to the provided level.\n" COLOR_RESET);
    return false;
  }

  controller->statusTable[relay_id].relayState = (request == RELAY_REQUEST_OPEN ?
                                      RELAY_STATE_OPENING : RELAY_STATE_CLOSING);
  controller->statusTable[relay_id].timeElapsedMs = 0;

  return true;
}

bool RelayController_GetRelayState(const RelayController *controller, const uint8_t relay_id,
                                   RelayState *relay_state) {
  if (controller == NULL) {
    printf(COLOR_RED "Error: Controller is NULL.\n" COLOR_RESET);
    return false;
  }

  if (relay_state == NULL) {
    printf(COLOR_RED "Error: Relay state is NULL.\n" COLOR_RESET);
    return false;
  }

  if (relay_id >= controller->relayCount) {
    printf(COLOR_RED "Error: invalid value for relay ID provided.\n" COLOR_RESET);
    return false;
  }

  *relay_state = controller->statusTable[relay_id].relayState;
  return true;
}

bool RelayController_GetRelayErrorType(const RelayController *controller, const uint8_t relay_id,
                                       RelayErrorType *relay_error_type) {
  if (controller == NULL) {
    printf(COLOR_RED "Error: Controller is NULL.\n" COLOR_RESET);
    return false;
 }

  if (relay_error_type == NULL) {
    printf(COLOR_RED "Error: Relay error type is NULL.\n" COLOR_RESET);
      return false;
  }

  if (relay_id >= controller->relayCount) {
    printf(COLOR_RED "Error: invalid value for relay ID provided.\n" COLOR_RESET);
    return false;
  }

  *relay_error_type = controller->statusTable[relay_id].relayErrorType;
  return true;
}
