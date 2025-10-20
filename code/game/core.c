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
static uint32_t successfull_rumbles = 0;

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

static uint32_t super_primes[] = {
    127,  157,  179,  191,  211,  241,  277,  283,  331,  353,  367,  401,
    431,  461,  509,  547,  563,  587,  599,  617,  709,  739,  773,  797,
    859,  877,  919,  967,  991,  1031, 1063, 1087, 1153, 1171, 1201, 1217,
    1297, 1409, 1433, 1447, 1471, 1499, 1523, 1597, 1621,
};
static uint32_t super_position = 0;
static uint32_t expected_controllers = 0;
static uint32_t controller_is_lost = 0;

void core_loop(void) {
  dc_ctrlrs_map_state();
  controller_state_t **cstates = get_ctrlr_states();
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (cstates[i]) {
      expected_controllers |= (1 << i);
    }
  }
  printf("Expecting controllers on ports: ");
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if ((1 << i) & expected_controllers) {
      printf("%c ", 'A' + i);
    }
  }
  printf("\n");

  while (super_position < sizeof(super_primes) / sizeof(super_primes[0])) {
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
    if (framenum % super_primes[super_position] == 0) {
      for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
        if (cstates[i]) {
          // printf("ctrlr %d frame: %lu\n", i, framenum);
          purupuru_effect_t fx = {.raw = 0};
          // rumble_queued(i, fx.raw);
          fx.raw = 0x00097010;
          rumble_queued(i, fx.raw);
          // fx.fx1_intensity = framenum % 8;
        }
      }
    }
    for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
      if ((1 << i) & expected_controllers) {
        if (!cstates[i]) {
          uint32_t re_reads = 0;
          while (!cstates[i]) {
            re_reads++;
            dc_ctrlrs_map_state();
          }
          printf("Re-read controller %c %u times to recover.\n", 'A' + i,
                 re_reads);
          // dc_ctrlrs_map_state();
          if (!controller_is_lost) {
            controller_is_lost = 1;
            printf(
                "Controller lost! Increasing wait frames to attempt recovery.\n");
            printf("Increasing wait frames from %u to %u\n",
                   super_primes[super_position],
                   super_primes[super_position + 1]);
            super_position++;
          }
          printf("Warning: Expected controller %c but not found in frame %lu!\n",
                 'A' + i, framenum);
  
        } else {
          successfull_rumbles++;
          controller_is_lost = 0;
        }
      }
    }
    render_frame();
  }
  printf("Exiting core loop after %lu frames.\n", framenum);
  printf("Successful rumbles: %u\nFailed rumbles: %u", successfull_rumbles,
         sizeof(super_primes) / sizeof(super_primes[0]));
}
