#if !defined(MENU_H_)
#define MENU_H_
#include "ludum_mode_play.h"

struct Mode_Menu {
    Memory_Arena *alloc;
	f32 cloud_timer;
	Wave_Layer waves[3];
};

#endif  // MENU_H_
