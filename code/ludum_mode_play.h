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

#define CLOUD_SLIDE_TIME (10.0f)
// Movement
//
#define PLAYER_MOVE_SPEED (8)
#define PLAYER_AIR_STRAFE_SPEED (6)
#define PLAYER_DAMPING (25.0f)
#define PLAYER_MAX_SPEED_X (2.2f)
#define PLAYER_MAX_SPEED_Y (4.3f)

// In elementry units
//
#define MAX_SHIP_HOLES (10)
#define CLOUD_COUNT (10)

enum Ship_Layer {
    Deck_Bottom = 0,
    Deck_Middle,
	Deck_Upper
};

struct Ship_Hole {
	v3 position;
	b32 active;
	v2 hitbox_dim;
};

struct Enemy_Ship {
	s32 health;	
	f32 fire_interval;
	f32 time_since_shot;
	f32 width;
	Sprite_Animation anim;
	Image_Handle border;
};

enum Player_Flags {
    Player_OnGround   = (1 << 0),
    Player_DoubleJump = (1 << 1),
	Player_Holding	  = (1 << 2),
	Player_Flipped	  = (1 << 3)
};

enum aabbSides {
    noCollision = 0,
    leftSide = 1,
    rightSide = (1 << 1),
    topSide = (1 << 2),
    bottomSide = (1 << 3)
};

enum Collision_Type{
	Normal = 0,
	Ladder = 1,
	Trap_Door = (1 << 1),
	Cannon = (1 << 2),
	Cannon_Hole = (1 << 3),
	Tentacles = (1 << 4)
};

struct AABB{
	u32 flags;
	v2 dim;
	v2 pos;
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
	Enemy_Ship enemies[2];
	f32 time_since_enemy;
	f32 enemy_spawn_time;
	Random rand;
	Ship_Hole ship_holes[MAX_SHIP_HOLES];
	f32 ship_hole_count;
	f32 water_level;
	Sprite_Animation anim;
	Player player;
	f32 cloud_timer;
};

function int ResolveCollision(v2 posA, v2 dimA, AABB collidable);
function void UpdatePlayer(Mode_Play *play, Player *player, Input *input, Game_State *state);

#endif  // PLAY_H_
