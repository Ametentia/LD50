#if !defined(PLAY_H_)
#define PLAY_H_

// In world units
//
#define PLAYER_MAX_JUMP_HEIGHT (0.3f)
#define PLAYER_MAX_DOUBLE_JUMP_HEIGHT (0.2f)
#define PLAYER_MIN_JUMP_HEIGHT (0.1f)
// In seconds
//
#define PLAYER_JUMP_APEX_TIME   (0.15f)
#define PLAYER_JUMP_BUFFER_TIME (0.2f)
// Movement
//
#define PLAYER_MOVE_SPEED (8)
#define PLAYER_AIR_STRAFE_SPEED (6)
#define PLAYER_DAMPING (25.0f)
#define PLAYER_MAX_SPEED_X (2.2f)
#define PLAYER_MAX_SPEED_Y (4.3f)

enum Player_Flags {
    Player_OnGround   = (1 << 0),
    Player_DoubleJump = (1 << 1),
	Player_Holding	  = (1 << 2)
};

enum aabbSides {
    noCollision=0,
    leftSide = 1,
    rightSide = (1 << 1),
    topSide = (1 << 2),
    bottomSide = (1 << 3)
};

struct Game_State;

struct Player{
	u32 flags;
    f32 last_jump_time;
    f32 last_on_ground_time;
	v2 p;
	v2 dp;
	v2 dim;
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
	Player player;
};

function int AABB(v2 posA, v2 dimA, v2 posB, v2 dimB);
function void UpdatePlayer(Mode_Play *play, Player *player, Input *input, Game_State *state);

#endif  // PLAY_H_
