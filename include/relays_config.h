/**
 * @brief relays_config.h
 * 
 * A header file where all the helper enums and structs are defined.
 * This file also exposes the configuration table for all the available relays.
 */

#ifndef RELAYS_CONFIG_H_
#define RELAYS_CONFIG_H_

#include <stdint.h>

#include "relay_constants.h"
#include "hal_dio.h"

/**
 * @brief Available types of relays.
 */
typedef enum {
  RELAY_TYPE_NORMALLY_CLOSED,
  RELAY_TYPE_NORMALLY_OPEN,
  RELAY_TYPE_COUNT
} RelayType;

/**
 * @brief States the relay can be in.
 */
typedef enum {
  /** The initial state before the actual state is fetched. */
  RELAY_STATE_UNKNOWN,
  RELAY_STATE_OPEN,
  /** Transitional state between previous closed state and open state.
   * This state indicates that the 30 ms of the feedback stabilization time
   * hasn't passed yet to confirm the new state of the relay. */
  RELAY_STATE_OPENING,
  /** Transitional state between previous open state and closed state.
   * This state indicates that the 30 ms of the feedback stabilization time
   * hasn't passed yet to confirm the new state of the relay. */
  RELAY_STATE_CLOSING,
  RELAY_STATE_CLOSED,
  RELAY_STATE_ERROR
} RelayState;

/**
 * @brief Types of relay errors.
 */
typedef enum {
  RELAY_ERROR_TYPE_NO_ERROR,
  /** There was an attempt to open the relay, but the relay stayed closed. */
  RELAY_ERROR_TYPE_WELDED,
  /** There was an attempt to close the relay, but the relay stayed opened. */
  RELAY_ERROR_TYPE_CONSTANTLY_OPEN,
  /** Any other error. */
  RELAY_ERROR_TYPE_UNKNOWN_ERROR
} RelayErrorType;

/**
 * @brief Relay configuration parameters.
 *
 * @note None of these fields are meant to be modified with after the initial configuration!
 */
typedef struct {
  RelayType relayType;
  DpoLine dpoLine;
  DiLine diLine;
} RelayConfig;

/**
 * @brief Relay status information.
 *
 * @note None of these fields are meant to be modified by themselves, consider them private!
 */
typedef struct {
  RelayState relayState;
  RelayErrorType relayErrorType;
  /** This will be used for tracking the 30 ms of feedback stabilization time. */
  uint8_t timeElapsedMs;
} RelayStatus;

/**
 * @brief Available types of relay requests.
 */
typedef enum {
  RELAY_REQUEST_OPEN,
  RELAY_REQUEST_CLOSE,
  RELAY_REQUEST_COUNT
} RelayRequest;

/**
 * @brief Configuration table of all of the available relays.
 */
extern const RelayConfig relaysConfigTable[NUM_OF_RELAYS];

/**
 * @brief Relay status table of all of the available relays.
 */
extern RelayStatus relaysStatusTable[NUM_OF_RELAYS];



#endif /* RELAYS_CONFIG_H_ */
