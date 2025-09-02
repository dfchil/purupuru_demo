#ifndef DRXLAX_GAME_CONTROLLER_CORE_H
#define DRXLAX_GAME_CONTROLLER_CORE_H

#include <stdint.h>
#include <dc/maple/controller.h>

typedef enum {
  A = 0,
  B = 1,
  C = 2,
  D = 3,
  E = 4,
  F = 5,
  G = 6,
  H = 7,
} port_name_e;

typedef enum {
  BUTTON_UP = 0b00,
  BUTTON_DOWN = 0b01,
  BUTTON_UP_THIS_FRAME = 0b10,
  BUTTON_DOWN_THIS_FRAME = 0b11,
} button_state_e;

typedef struct {
  union {
    uint8_t ABXY;
    struct {
      uint8_t A : 2;
      uint8_t B : 2;
      uint8_t X : 2;
      uint8_t Y : 2;
    };
  };
  union {
    uint8_t dpad;
    struct {
      uint8_t UP : 2;
      uint8_t DOWN : 2;
      uint8_t LEFT : 2;
      uint8_t RIGHT : 2;
    };
  };
  int8_t joyx;
  int8_t joyy;
  uint8_t ltrigger;
  uint8_t rtrigger;
  struct {
    uint16_t START : 2;
    uint16_t rumble : 1;
    uint16_t vmu : 1;
    uint16_t portnum : 4;
    uint16_t : 8;
  };

  cont_state_t *state;
} controller_state_t;


#endif // DRXLAX_GAME_CONTROLLER_CORE_H