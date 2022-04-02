#if !defined(PLAY_H_)
#define PLAY_H_

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
