#include <base.cpp>

#include "ludum_mode_menu.cpp"
#include "ludum_mode_splash.cpp"

#include "ludum_world_hitbox.cpp"
#include "ludum_mode_play.cpp"

function void LudumUpdateRender(Game_Context *context, Input *input, Renderer_Buffer *renderer_buffer) {
    input->delta_time = Clamp(input->delta_time, 0.0, 0.2); // @Hack: Should probably be handled by the platform

    Game_State *state = context->state;

    if (!state) {
        Memory_Allocator *system_alloc = Platform->GetMemoryAllocator();

        state = AllocInline(system_alloc, Gigabytes(1), Game_State, perm_arena);
        Initialise(&state->mode_arena, system_alloc, Gigabytes(1));

        Initialise(&state->audio_state, &state->perm_arena, V2(0.2f, 0.2f));
        Initialise(&state->assets, &state->perm_arena, context->texture_queue, TextureFlag_Clamped);

        context->state = state;
    }

	if (state->game_mode == GameMode_None) {
		ModeSplash(state, input);
		// TEMP: Remove before release
		ModePlay(state, input);
	}

	switch (state->game_mode) {
		case GameMode_Splash: {
			UpdateRenderModeSplash(state, input, renderer_buffer);
			break;
		}
		case GameMode_Menu: {
			UpdateRenderModeMenu(state, input, renderer_buffer);
			break;
		}
		case GameMode_Play: {
			UpdateRenderModePlay(state, input, renderer_buffer);
			break;
		}
	}
}

