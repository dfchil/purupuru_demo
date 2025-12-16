#include <dc/maple.h>
#include <enDjinn/enj_enDjinn.h>
#include <dc/video.h>

typedef struct {
  uint32_t framenum;
  uint32_t expected_controllers;
  int8_t xy_joy_ringbuffer[10][2];
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
        // enj_rumbler_set_effect(i, 0x00072010);
        enj_rumbler_set_effect(i, 0x011a1010);
      }
    }
  }

  int xpos = 50;
  int ypos = 10;

  xpos += enj_qfont_write("Expected Controllers This Run: ", xpos, ypos,
                          PVR_LIST_PT_POLY);
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (1 << i & rt_state->expected_controllers) {
      char buf[3] = {'A' + i, ' ', '\0'};
      xpos += enj_qfont_write(buf, xpos, ypos, PVR_LIST_PT_POLY);
    }
  }

  xpos = 50;
  ypos = 140;

  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    if (((1 << i) & rt_state->expected_controllers) && !cstates[i]) {
      enj_qfont_get_sprite_hdr()->argb = 0xffff0000; // red for errors
      printf("Warning: Expected controller %c but not found in frame %lu!\n",
             'A' + i, rt_state->framenum);
      char errstr[] = "Controller X missing!";
      errstr[11] = 'A' + i;
      enj_qfont_write(errstr, xpos, ypos, PVR_LIST_PT_POLY);
      ypos += enj_qfont_get_header()->line_height;
    }
  }
  if (ypos == 140) {
    enj_qfont_get_sprite_hdr()->argb = 0xff00ff00; // green for success
    enj_qfont_write("All expected controllers connected.", xpos, ypos,
                    PVR_LIST_PT_POLY);
    ypos += enj_qfont_get_header()->line_height * 3;
  }

  if (cstates[0] != NULL) {

    if (cstates[0]->buttons.A != BUTTON_DOWN) {
      enj_qfont_write("Button A on controller A not pressed!", xpos, ypos,
                      PVR_LIST_PT_POLY);
    }
    enj_qfont_write("+", cstates[0]->joyx + (vid_mode->width>>1), cstates[0]->joyy + (vid_mode->height>>1),
                    PVR_LIST_PT_POLY);
    rt_state->xy_joy_ringbuffer[rt_state->framenum % 10][0] =
        cstates[0]->joyx;
    rt_state->xy_joy_ringbuffer[rt_state->framenum % 10][1] =
        cstates[0]->joyy;
  } else if (cstates[0] != NULL) {
    enj_qfont_write("Button A on controller A is pressed.", xpos, ypos,
                    PVR_LIST_PT_POLY);
  }

  int same_joy_values = 0;
  for (int i = 0; i < 10; i++) {
    if (rt_state->xy_joy_ringbuffer[(rt_state->framenum - i) % 10][0] == cstates[0]->joyx &&
        rt_state->xy_joy_ringbuffer[(rt_state->framenum - i) % 10][1] == cstates[0]->joyy) {
      same_joy_values++;
    }
  }
  if (same_joy_values >= 3) {

    ypos += enj_qfont_get_header()->line_height;
    enj_qfont_write("Joystick on controller A seems stuck!", xpos, ypos,
                    PVR_LIST_PT_POLY);
  }

  rt_state->framenum++;
}

void main_mode_updater(void *data) {
  enj_renderlist_add(PVR_LIST_PT_POLY, render, data);
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
