#include <base.cpp>
#include "ludum_mode_menu.cpp"
#include "ludum_mode_splash.cpp"
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
    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;
	if(state->game_mode == GameMode_None) {
		ModeSplash(state, input);
		// TEMP: Remove before release
		ModePlay(state);
	}

    Initialise(batch, &state->assets, renderer_buffer);

    DrawClear(batch, V4(0, 0, 0, 1));
	switch(state->game_mode){
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

function int AABB(v2 posA, v2 dimA, v2 posB, v2 dimB){
    rect2 a_r;
    rect2 b_r;
    a_r.min = posA - (0.5f*dimA);
    a_r.max = posA + (0.5f*dimA);
    b_r.min = posB - (0.5f*dimB);
    b_r.max = posB + (0.5f*dimB);

    v2 overlap;
    overlap.x =  Min(b_r.max.x, a_r.max.x) - Max(b_r.min.x, a_r.min.x);
    overlap.y =  Min(b_r.max.y, a_r.max.y) - Max(b_r.min.y, a_r.min.y);

    if(overlap.x >= 0 && overlap.y >= 0){
        if(overlap.x < overlap.y){
            if(overlap.x > 0){
                if(posA.x > posB.x){
                    return leftSide;
                }
                else{
                    return rightSide;
                }
            }
        }
        else{
            if(overlap.x > 0){
                if(posA.y > posB.y){
                    return topSide;
                }
                else{
                    return bottomSide;
                }
            }
        }
    }
    return noCollision;
}


