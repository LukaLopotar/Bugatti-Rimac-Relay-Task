/**
 * @brief relay_controller.h
 * 
 * Relay controller is used for monitoring the status of all available relays
 * and act accordingly in case of an error. It also provides APIs through which 
 * it is possible to check the particular relay status (open/closed) and error
 * state (no error, constantly open, closed etc.) and it also provides a way to
 * change a particular relay to open or closed position.
 */

#ifndef RELAY_CONTROLLER_H_
#define RELAY_CONTROLLER_H_

#include <stdint.h>
#include <stdbool.h>

#include "relays_config.h"
#include "relay_constants.h"

/**
 * @brief All the possible states of the relay controller.
 */
typedef enum {
  /** The state in which relay controller starts when the system starts. */
  RELAY_CONTROLLER_INIT,
  /** Relay controller is running normally. */
  RELAY_CONTROLLER_RUNNING,
  /** Error occurred either with relay controller or one of the relays. */
  RELAY_CONTROLLER_ERROR
} RelayControllerState;

/**
 * @brief Relay controller structure.
 * 
 * @note None of these fields should be modified outside of RelayController itself.
 *       This is exposed here so that static definition of the controller is easier in the main.c file.
 *       In production code I would put this struct as a opaque struct.
 */
typedef struct {
  /** Current state of the controller. */
  RelayControllerState current_state;
  /** How many relays does controller monitor and control. */
  const uint8_t relayCount;
  /** Table of information on configuration of all the relays (NC/NO, DPO and DI lines etc.). */
  const RelayConfig *configTable;
  /** Table of information on status of all the relays (open/closed, error type etc.). */
  RelayStatus *statusTable;
} RelayController;

/**
 * @brief Checks if relay controller and relays are properly configured and starts the relay controller.
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 *
 * @return boolean - True if controller and relays are properly configured, false otherwise.
 *
 * @note This function needs to be called first before any other functions!
 */
bool RelayController_Init(RelayController *controller);

/**
 * @brief An RTOS task which monitors the status of all relays. In case that
 *        one of the relays is in error state, the relay controller opens all of
 *        the relays, sets itself to error state and, in turn, doesn't allow any
 *        of the requests to change DPO line to any of the relays anymore.
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 *
 * @return None.
 */
void RelayController_MonitorRelaysTask(RelayController *controller);

/**
 * @brief Processes a request to change the DPO level of a particular relay.
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 * @param[in]  relay_id - ID of the relay for which a DPO level needs to be changed.
 * @param[in]  request - request to either open or close the relay.
 *
 * @return boolean - True if the DPO level of the relay was changed successfully, false otherwise.
 */
bool RelayController_SetRequest(RelayController *controller, const uint8_t relay_id, const RelayRequest request);

/**
 * @brief Gets the current state of the relay (open, close, error etc.).
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 * @param[in]  relay_id - ID of the relay to get the current state.
 * @param[out]  relay_state - The current state of the relay.
 *
 * @return boolean - True if the relay state was fetched successfully, false otherwise.
 */
bool RelayController_GetRelayState(const RelayController *controller, const uint8_t relay_id, RelayState *relay_state);

/**
 * @brief Gets the current error type of the relay (no error, constantly open, welded etc.).
 *
 * @param[in,out]  controller  - Pointer to the relay controller instance.
 * @param[in]  relay_id - ID of the relay to get the current error type.
 * @param[out]  relay_error_type - The current error type of the relay.
 *
 * @return boolean - True if the relay error type was fetched successfully, false otherwise.
 */
bool RelayController_GetRelayErrorType(const RelayController *controller, const uint8_t relay_id, RelayErrorType *relay_error_type);

#endif /* RELAY_CONTROLLER_H_ */
