#include <dc/maple.h>
#include <enDjinn/enj_enDjinn.h>

typedef struct {
  uint32_t framenum;
  uint32_t expected_controllers;
} rumble_test_state_t;

void render(void *data) {
  rumble_test_state_t *rt_state = (rumble_test_state_t *)data;
  maple_device_t **rumblers = enj_rumbler_get_states();
  enj_ctrlr_state_t **cstates = enj_ctrl_get_states();

  if (rt_state->framenum == 0) {
    for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
      if (cstates[i]) {
        rt_state->expected_controllers |= (1 << i);
      }
    }
  }

  if (rt_state->framenum % 31 == 0) {
    for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
      if (cstates[i] && rumblers[i]) {
        enj_rumbler_set_effect(i, 0x00072010);
      }
    }
  }

  int xpos = 50;
  int ypos = 10;

  xpos += enj_qfont_write("Expected Controllers This Run: ", xpos, ypos,
                          PVR_LIST_TR_POLY);
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (1 << i & rt_state->expected_controllers) {
      char buf[3] = {'A' + i,' ', '\0'};
      xpos += enj_qfont_write(buf, xpos, ypos, PVR_LIST_TR_POLY);
    }
  }

  xpos = 50;
  ypos = 140;

  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (((1 << i) & rt_state->expected_controllers) && !cstates[i]) {
      enj_qfont_get_sprite_hdr()->argb = 0xffff0000;  // red for errors
      printf("Warning: Expected controller %c but not found in frame %lu!\n",
             'A' + i, rt_state->framenum);
      char errstr[] = "Controller X missing!";
      errstr[11] = 'A' + i;
      enj_qfont_write(errstr, xpos, ypos, PVR_LIST_TR_POLY);
      ypos += enj_qfont_get_header()->line_height;
    }
  }
  if (ypos == 140) {
    enj_qfont_get_sprite_hdr()->argb = 0xff00ff00;  // green for success
    enj_qfont_write("All expected controllers connected.", xpos, ypos,
                    PVR_LIST_TR_POLY);
  }
  rt_state->framenum++;
}

void main_mode_updater(void *data) {
  enj_renderlist_add(PVR_LIST_TR_POLY, render, data);
}

int main(__unused int argc, __unused char **argv) {
  enj_state_defaults();
  // default soft-reset pattern is START + A + B + X + Y.
  // Lets make it easier with just START
  // START is offset 8<<1 (two bits per button)
  enj_state_set_soft_reset(BUTTON_DOWN << (8 << 1));

  if (enj_startup() != 0) {
    ENJ_DEBUG_PRINT("enDjinn startup failed, exiting\n");
    return -1;
  }
  rumble_test_state_t rt_state = {
      .framenum = 0,
  };

  enj_mode_t main_mode = {
      .name = "Main Mode",
      .mode_updater = main_mode_updater,
      .data = &rt_state,
  };
  enj_mode_push(&main_mode);
  enj_run();

  return 0;
}
