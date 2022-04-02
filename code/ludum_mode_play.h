#if !defined(PLAY_H_)
#define PLAY_H_

struct Ship_Hole {
	v3 position;
	b32 active;
	v3 hitbox_dimensions;
};

struct Enemy_Ship {
	s32 health;	
	f32 fire_interval;
	f32 time_since_shot;
	f32 width;
	Sprite_Animation anim;
	Image_Handle border;
};

struct Wave_Layer {
	v3 position;
	f32 radius;
	f32 angle;
	f32 speed;
	Image_Handle texture;
};


struct Mode_Play {
    Memory_Arena *alloc;
	u32 back_wave_count;
	Wave_Layer back_waves[3];
	u32 front_wave_count;
	Wave_Layer front_waves[3];
	Enemy_Ship enemies[2];
	f32 time_since_enemy;
	f32 enemy_spawn_time;
	Random rand;
	Ship_Hole ship_holes[10];
	f32 water_level;
	Sprite_Animation anim;
};

enum aabbSides {
    noCollision=0,
    leftSide=1,
    rightSide=1<<1,
    topSide=1<<2,
    bottomSide=1<<3
};

function int AABB(v2 posA, v2 dimA, v2 posB, v2 dimB);

#endif  // PLAY_H_
