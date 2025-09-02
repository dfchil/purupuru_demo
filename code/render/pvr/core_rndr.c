#include <dc/pvr.h>

void render_frame(void) {
  pvr_wait_ready();
  pvr_scene_begin();

  

  pvr_scene_finish();
}
