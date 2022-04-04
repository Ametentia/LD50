#if !defined(PLAY_H_)
#define PLAY_H_
// In world units
//
#define PLAYER_MAX_JUMP_HEIGHT (0.3f)
#define PLAYER_MAX_DOUBLE_JUMP_HEIGHT (0.28f)
#define PLAYER_MIN_JUMP_HEIGHT (0.1f)
// In seconds
//
#define PLAYER_JUMP_APEX_TIME   (0.15f)
#define PLAYER_JUMP_BUFFER_TIME (0.2f)

#define CLOUD_SLIDE_TIME (30.0f)
#define SHIP_HOLE_FIX_TIME (1.0f)

// In World units / second
//
#define WATER_RISE_RATE_PER_HOLE (1.01f)
// Movement
//
#define PLAYER_MOVE_SPEED (8)
#define PLAYER_AIR_STRAFE_SPEED (6)
#define PLAYER_DAMPING (30.0f)
#define PLAYER_MAX_SPEED_X (2.2f)
#define PLAYER_MAX_SPEED_Y (4.3f)
#define PLAYER_LADDER_CLIMB_SPEED (1)

//Map
//
#define MAX_HITBOXES (50)
#define MAX_DROPPED_ITEMS (15)
#define ITEM_DAMPING (3.0f)

// In elementry units
//
#define MAX_SHIP_HOLES (10)
#define CLOUD_COUNT    (10)

enum Ship_Layer {
    Deck_Bottom = 0,
    Deck_Middle,
	Deck_Upper
};

struct Ship_Hole {
	v3 position;
	f32 rot;
	f32 timer;
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
	b32 render_while_dead;
	Sprite_Animation death_anim;
	b32 play_dead;
};

enum Player_Flags {
    Player_OnGround   = (1 << 0),
    Player_DoubleJump = (1 << 1),
	Player_Holding	  = (1 << 2),
	Player_Flipped	  = (1 << 3),
	Player_On_Ladder  = (1 << 4),
    Player_Idle       = (1 << 5)
};

enum Player_Holding{
	Held_CannonBall = (1 << 0),
	Held_Spear = (1 << 1),
	Held_Bucket = (1 << 2),
	Held_Plank = (1 << 3),
	Held_FullBucket = (1 << 4)
};

enum AABB_Sides {
    AABB_Sides_noCollision = 0,
	AABB_Sides_collision = (1 << 0),
    AABB_Sides_leftSide = (1 << 1),
    AABB_Sides_rightSide = (1 << 2),
    AABB_Sides_topSide = (1 << 3),
    AABB_Sides_bottomSide = (1 << 4)
};

enum Collision_Type{
	Collision_Type_Normal = (1 << 0),
	Collision_Type_Ladder = (1 << 1),
	Collision_Type_Trap_Door = (1 << 2),
	Collision_Type_Cannon = (1 << 3),
	Collision_Type_Cannon_Hole = (1 << 4),
	Collision_Type_Tentacles = (1 << 5),
	Collision_Type_Cannon_Resource = (1 << 6),
	Collision_Type_Plank_Resource = (1 << 7),
	Collision_Type_Was_On_Ladder = (1 << 8),
	Collision_Type_Spears_Resource = (1 << 9),
	Collision_Type_Dropped_Item = (1 << 10)
};

enum Item_Type{
	Item_CannonBall = (1 << 0),
	Item_Spear = (1 << 1),
	Item_Bucket = (1 << 2),
	Item_Plank = (1 << 3),
	Item_FullBucket = (1 << 4)
};

struct AABB{
	u32 flags;
	v2 dim;
	v2 pos;
	v4 debugColour;
};

struct Dropped_Item{
	u32 type;
	AABB hitbox;
	v2 dp;
	b32 active;
};

struct Game_State;

struct Player{
	Sprite_Animation anim;

	u32 flags;
	u32 holdingFlags;
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
	v4 wColour;
	Player player;
	f32 cloud_timer;
	AABB hitboxes[MAX_HITBOXES];
	Dropped_Item droppedItems[MAX_DROPPED_ITEMS];
	u32 hitbox_count;
	Sprite_Animation ship_mast_1;
	Sprite_Animation ship_mast_2;
	Sprite_Animation cannon_anim;
	b32 play_cannon;
	b32 game_over;
	f64 survive_time;
};

function u32 ResolveCollision(v2 posA, v2 dimA, AABB *collidable);
function void UpdatePlayer(Mode_Play *play, Player *player, Input *input, Draw_Batch *batch);
function void UpdateRenderWaveList(f64 dt, Draw_Batch *batch, Wave_Layer *layers, u32 layer_count);

#endif  // PLAY_H_
