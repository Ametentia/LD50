#if !defined(DEATH_H_)
#define DEATH_H_
#include "ludum_mode_play.h"

struct Mode_Death {
    Memory_Arena *alloc;
	f32 cloud_timer;
	Wave_Layer back_wave;
	Wave_Layer waves[2];
	Sprite_Animation ship_mast_1;
	Sprite_Animation ship_mast_2;
	v2 move;
	f32 angle;
	f32 height;
	f64 survival;
};

#endif  // DEATH_H_
