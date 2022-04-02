#if !defined(LUDUM_H_)
#define LUDUM_H_

#include <base.h>
#include "ludum_mode_splash.h"
#include "ludum_mode_menu.h"
#include "ludum_mode_play.h"

enum Game_Mode {
	GameMode_None=0,
	GameMode_Splash,
	GameMode_Menu,
	GameMode_Play
};

struct Game_State {
    Memory_Arena perm_arena;
    Memory_Arena mode_arena;

    Audio_State audio_state;
    Asset_Manager assets;
	Game_Mode game_mode;
	union {
		Mode_Splash splash;
		Mode_Menu menu;
		Mode_Play play;
	};
};

struct Game_Context {
    Texture_Transfer_Queue *texture_queue;

    Game_State *state;
};
#endif  // LUDUM_H_
