#ifndef GAME_CONTROLLER_ABSTRACT_H
#define GAME_CONTROLLER_ABSTRACT_H
#include <drxlax/game/controller/core.h>

/**
 * button 4 states
 * 0 - not pressed
 * 1 - pressed this frame
 * 2 - pressed continuously
 * 3 - released this frame
 */

typedef enum controller_type {
  NO_CONTROLLER_E = 0,
  DREAMCAST_STD_E = 1,
  SERIAL_CABLE_E = 2,
  NETWORK_E = 3,
  REPLAY_E = 4,
  AIBOT_E = 5,
} controller_type_e;

typedef struct {
  int offset;
} dc_controller_t;

typedef struct {
  struct {
    controller_type_e type : 4;
    port_name_e port : 4;
    uint32_t _ : 24;
  };
  dc_controller_t dc_ctrlr;
} abstract_controller_t;


void cont_state_onto_ctrlstate(cont_state_t *c_state,
                               controller_state_t *ctrlr);
#endif // GAME_CONTROLLER_ABSTRACT_H