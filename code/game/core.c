#include <arch/arch.h>
#include <drxlax/game/controller/dreamcast.h>
#include <drxlax/game/controller/rumble.h>
#include <drxlax/render/core.h>
#include <stdio.h>

#include <dc/maple/purupuru.h>

static struct {
  uint32_t end_of_sequence : 1;
  uint32_t shut_down : 1;
  uint32_t : 30;
} flags;

static uint32_t framenum = 0;

int detect_shutdown_combo(controller_state_t *cstate) {
  if ((cstate->START & BUTTON_DOWN) && (cstate->A & BUTTON_DOWN)) {
    if ((cstate->START == BUTTON_DOWN_THIS_FRAME) ||
        (cstate->A == BUTTON_DOWN_THIS_FRAME)) {
      return 1;
    }
  }
  return 0;
}

void core_flag_shutdown(void) { flags.shut_down = 1; }

int check_c1_exit(void) {
  if (flags.shut_down) {
    return 1;
  }
  return 0;
}


static uint32_t expected_controllers = 0;


void core_loop(void) {
  dc_ctrlrs_map_state();
  controller_state_t **cstates = get_ctrlr_states();
  for(int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if(cstates[i]) {
      expected_controllers |= (1 << i);
    }
  }
  printf("Expecting controllers on ports: ");
  for(int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if((1 << i) & expected_controllers) {
      printf("%c ", 'A' + i);
    }
  }
  printf("\n");

  while (1) {
    framenum++;
    dc_ctrlrs_map_state();
    cstates = get_ctrlr_states();
    for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
      if (detect_shutdown_combo(cstates[i])) {
        core_flag_shutdown();
      }
    }
    if (check_c1_exit()) {
      break;
    }
    if (framenum % 31 == 0) {
      for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
        if (cstates[i]) {
          // printf("ctrlr %d frame: %lu\n", i, framenum);
          purupuru_effect_t fx = {.raw = 0x00072010};
          // fx.fx1_intensity = framenum % 8;
          rumble_queued(i, fx.raw);
        }
      }
    }

    for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
      if (((1 << i) & expected_controllers) && !cstates[i]) {
        printf("Warning: Expected controller %c but not found in frame %lu!\n", 'A' + i, framenum);
      } 
    }
    render_frame();
  }
}
