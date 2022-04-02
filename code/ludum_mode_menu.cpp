#include "ludum_mode_menu.h"
function void ModeMenu(Game_State *state) {
    Reset(&state->mode_arena);

    Mode_Menu *menu = AllocType(&state->mode_arena, Mode_Menu);
    menu->alloc = &state->mode_arena;

    state->game_mode = GameMode_Menu;
    state->menu = *menu;
}

function void UpdateRenderModeMenu(Game_State *state, Input *input, Renderer_Buffer *renderer_buffer) {
    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;
    Initialise(batch, &state->assets, renderer_buffer);
    DrawClear(batch, V4(1, 0, 0, 1));
}
