#if !defined(SPLASH_H_)
#define SPLASH_H_
enum Move_Type {
    MoveType_None,
    MoveType_Left,
    MoveType_Right,
    MoveType_Up,
    MoveType_Down
};

struct Block {
    v2 pos;
    v2 blockPos;
};

struct Move {
    u8 index;
    Move_Type move;    
};

struct Mode_Splash {
    Memory_Arena *alloc;
    
    u8 steps;
    f32 time;
    f32 fade_start;
    f32 move_time;
    Block blocks[8];
    Move moves[16];
    f32 timer;
    v2 lastPos;
    u8 done;
    Random rand;
};

#endif  // SPLASH_H_
