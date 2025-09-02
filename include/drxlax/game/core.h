#ifndef GAME_CORE_H
#define GAME_CORE_H


#include <drxlax/game/controller/abstract_ctrlr.h>

int detect_shutdown_combo(controller_state_t *cstate);

void core_flag_shutdown(void);

void core_loop(void);


#endif // GAME_CORE_H
