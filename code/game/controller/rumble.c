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

void print_rumble_fields(uint32_t raw) {
  rumble_fields_t fields = {.raw = raw};
  printf("Rumble Fields:\n");

  printf("  .special_pulse   =  %u,\n", fields.special_pulse);
  printf("  .special_motor1  =  %u,\n", fields.special_motor1);
  printf("  .special_motor2  =  %u,\n", fields.special_motor2);

  printf("  .fx1_pulse:      =  %u,\n", fields.fx1_pulse);
  printf("  .fx1_powersave:  =  %u,\n", fields.fx1_powersave);
  printf("  .fx1_intensity:  =  %u,\n", fields.fx1_intensity);

  printf("  .fx2_lintensity: =  %u,\n", fields.fx2_lintensity);
  printf("  .fx2_pulse:      =  %u,\n", fields.fx2_pulse);
  printf("  .fx2_uintensity: =  %u,\n", fields.fx2_uintensity);
  printf("  .fx2_decay:      =  %u,\n", fields.fx2_decay);

  printf("  .duration:       =  %u,\n", fields.duration);
}
