/**
 * @brief relays_config.c
 * 
 * This file contains the configuration table for all the available relays and
 * is meant to be manually modified if the amount of relays or their configuration
 * needs to be changed.
 *
 * For more information, see relays_config.h file.
 */

#include "relays_config.h"

const RelayConfig relaysConfigTable[NUM_OF_RELAYS] = {
  {
    .relayType = RELAY_TYPE_NORMALLY_CLOSED,
    .dpoLine = DPO_LINE_1,
    .diLine = DI_LINE_1
  },
  {
    .relayType = RELAY_TYPE_NORMALLY_OPEN,
    .dpoLine = DPO_LINE_2,
    .diLine = DI_LINE_2
  },
  {
    .relayType = RELAY_TYPE_NORMALLY_CLOSED,
    .dpoLine = DPO_LINE_3,
    .diLine = DI_LINE_3
  }
};

RelayStatus relaysStatusTable[NUM_OF_RELAYS] = {
  {
    .relayState = RELAY_STATE_UNKNOWN,
    .relayErrorType = RELAY_ERROR_TYPE_NO_ERROR,
    .timeElapsedMs = 0
  },
  {
    .relayState = RELAY_STATE_UNKNOWN,
    .relayErrorType = RELAY_ERROR_TYPE_NO_ERROR,
    .timeElapsedMs = 0
  },
  {
    .relayState = RELAY_STATE_UNKNOWN,
    .relayErrorType = RELAY_ERROR_TYPE_NO_ERROR,
    .timeElapsedMs = 0
  }
};
