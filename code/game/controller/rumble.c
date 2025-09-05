#include <drxlax/game/controller/dreamcast.h>
#include <drxlax/game/controller/rumble.h>
#include <kos/sem.h>
#include <stdio.h>

void rumble_queued(uint32_t ctrloffset, uint32_t raw) {
  maple_device_t *dev = maple_port_type(ctrloffset, MAPLE_FUNC_PURUPURU);
  purupuru_effect_t fx = (purupuru_effect_t){.raw = raw};
  purupuru_rumble(dev, &fx);
}

void rumble_queues_init(void) {
}
void rumble_queues_shutdown(void) {
}

void print_rumble_fields(purupuru_effect_t fields) {
  printf("Rumble Fields:\n");
  printf("-- 1st byte ---------------------------\n");
  printf("  .cont  (0:0)   =  %s,\n", fields.cont ? "true" : "false");
  printf("  .res   (0:1-3)   =  %u,\n", fields.res);
  printf("  .motor (0:4-7)  =  %u,\n", fields.motor);
  printf("-- 2nd byte ---------------------------\n");
  printf("  .bpow  (1:0-2)  =  %u,\n", fields.bpow);
  printf("  .div   (1:3)    =  %s,\n", fields.div ? "true" : "false");
  printf("  .fpow  (1:4-6)   =  %u,\n", fields.fpow);
  printf("  .conv  (1:7)    =  %s,\n", fields.conv ? "true" : "false");
  printf("-- 3rd byte ---------------------------\n");
  printf("  .freq  (2:0-7)  =  %u,\n", fields.freq);
  printf("-- 4th byte ---------------------------\n");
  printf("  .inc   (3:0-7)   =  %u,\n", fields.inc);
  printf("-------------------------------------\n");

  unsigned char cmdbits[9 * 4] = {0};
  for (int bidx = 0; bidx < 4; bidx++) {
    for (int j = 0; j < 8; j++) {
      cmdbits[bidx * 9 + (7-j)] = '0' + ((fields.raw >> (bidx * 8 + j)) & 1);
    }
    cmdbits[(3 - bidx) * 9 + 8] = '\n';
  }
  cmdbits[9 * 4 - 1] = 0;
  printf("  four byte pattern:\n%s\n", cmdbits);
}