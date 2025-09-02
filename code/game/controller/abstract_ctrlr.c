#include <drxlax/game/controller/abstract_ctrlr.h>

static inline uint8_t update_button_state(uint8_t prev_btnstate, int input) {
  switch (prev_btnstate) {
  case BUTTON_UP:
    return input ? BUTTON_DOWN_THIS_FRAME : BUTTON_UP;
    break;
  case BUTTON_DOWN:
    return input ? BUTTON_DOWN : BUTTON_UP_THIS_FRAME;
    break;
  case BUTTON_DOWN_THIS_FRAME:
    return input ? BUTTON_DOWN : BUTTON_UP_THIS_FRAME;
    break;
  case BUTTON_UP_THIS_FRAME:
    return input ? BUTTON_DOWN_THIS_FRAME : BUTTON_UP;
    break;
  default:
    return BUTTON_UP;
    break;
  }
}

void cont_state_onto_ctrlstate(cont_state_t *c_state,
                               controller_state_t *ctrlr) {
  if (c_state->a != ctrlr->A) {
    ctrlr->A = update_button_state(ctrlr->A, c_state->a);
  }
  if (c_state->b != ctrlr->B) {
    ctrlr->B = update_button_state(ctrlr->B, c_state->b);
  }
  if (c_state->x != ctrlr->X) {
    ctrlr->X = update_button_state(ctrlr->X, c_state->x);
  }
  if (c_state->y != ctrlr->Y) {
    ctrlr->Y = update_button_state(ctrlr->Y, c_state->y);
  }
  if (c_state->dpad_up != ctrlr->UP) {
    ctrlr->UP = update_button_state(ctrlr->UP, c_state->dpad_up);
  }
  if (c_state->dpad_down != ctrlr->DOWN) {
    ctrlr->DOWN = update_button_state(ctrlr->DOWN, c_state->dpad_down);
  }
  if (c_state->dpad_left != ctrlr->LEFT) {
    ctrlr->LEFT = update_button_state(ctrlr->LEFT, c_state->dpad_left);
  }
  if (c_state->dpad_right != ctrlr->RIGHT) {
    ctrlr->RIGHT = update_button_state(ctrlr->RIGHT, c_state->dpad_right);
  }
  if (c_state->start != ctrlr->START) {
    ctrlr->START = update_button_state(ctrlr->START, c_state->start);
  }
  ctrlr->joyx = c_state->joyx;
  ctrlr->joyy = c_state->joyy;
  ctrlr->ltrigger = c_state->ltrig;
  ctrlr->rtrigger = c_state->rtrig;
}

controller_state_t **get_ctrlr_states(void);
void read_controller(abstract_controller_t *ctrlref,
                     controller_state_t *buttons) {
  *buttons = *(get_ctrlr_states()[ctrlref->dc_ctrlr.offset]);
}
