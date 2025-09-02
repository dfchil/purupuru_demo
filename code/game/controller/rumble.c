#include <drxlax/game/controller/dreamcast.h>
#include <drxlax/game/controller/rumble.h>
#include <kos/sem.h>
#include <pthread.h>
#include <stdio.h>

#define RFX_QUEUE_SIZE 0x3
volatile uint32_t _fx_queues[MAPLE_PORT_COUNT][RFX_QUEUE_SIZE]
    __attribute__((aligned(32))) = {0};
volatile uint32_t _rfx_queue_heads[MAPLE_PORT_COUNT]
    __attribute__((aligned(32))) = {0};
uint32_t _rfx_queue_tails[MAPLE_PORT_COUNT] __attribute__((aligned(32))) = {0};
static pthread_t _rfx_threads[MAPLE_PORT_COUNT] __attribute__((aligned(32)));
static pthread_mutex_t _fx_queue_mutex[MAPLE_PORT_COUNT]
    __attribute__((aligned(32)));
static pthread_cond_t _rfx_queue_cond[MAPLE_PORT_COUNT]
    __attribute__((aligned(32)));

static void *_rfx_consumer(void *arg) {
  int port = (int)(intptr_t)arg;
  while (1) {
    pthread_mutex_lock(_fx_queue_mutex + port);
    pthread_cond_wait(_rfx_queue_cond + port, _fx_queue_mutex + port);
    pthread_mutex_unlock(&_fx_queue_mutex[port]);
    while (_rfx_queue_tails[port] < _rfx_queue_heads[port]) {
      uint32_t rfx =
          _fx_queues[port][(_rfx_queue_tails[port]++) % RFX_QUEUE_SIZE];
      maple_device_t *rumble = maple_port_type(port, MAPLE_FUNC_PURUPURU);
      if (rumble != NULL) {
        purupuru_rumble_raw(rumble, rfx);
      }
    }
  }
  return NULL;
}
void rumble_queued(uint32_t ctrloffset, uint32_t raw) {
  maple_device_t *dev = maple_port_type(ctrloffset, MAPLE_FUNC_PURUPURU);
  // print_rumble_fields((purupuru_effect_t){.raw = raw});
  // purupuru_rumble_raw(dev, raw);
  // return;
  if (_rfx_queue_heads[ctrloffset] - _rfx_queue_tails[ctrloffset] ==
      RFX_QUEUE_SIZE) {
    return;
  }
  if (maple_port_type(ctrloffset, MAPLE_FUNC_PURUPURU)) {
    _fx_queues[ctrloffset][_rfx_queue_heads[ctrloffset] % RFX_QUEUE_SIZE] = raw;
    _rfx_queue_heads[ctrloffset]++;
    if (pthread_mutex_trylock(&_fx_queue_mutex[ctrloffset]) == 0) {
      pthread_cond_signal(_rfx_queue_cond + ctrloffset);
      pthread_mutex_unlock(_fx_queue_mutex + ctrloffset);
    }
  }
}

void rumble_queues_init(void) {
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    _rfx_queue_tails[i] = _rfx_queue_heads[i] = 0;
    pthread_mutex_init(_fx_queue_mutex + i, NULL);
    pthread_cond_init(_rfx_queue_cond + i, NULL);
    pthread_create(_rfx_threads + i, NULL, _rfx_consumer, (void *)(intptr_t)i);
  }
}
void rumble_queues_shutdown(void) {
  for (int i = 0; i < MAPLE_PORT_COUNT; i++) {
    pthread_cancel(_rfx_threads[i]);
    pthread_mutex_destroy(_fx_queue_mutex + i);
    pthread_cond_destroy(&_rfx_queue_cond[i]);
  }
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