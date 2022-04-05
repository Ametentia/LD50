#include "ludum_mode_menu.h"
#include "ludum_mode_play.h"

function void ModeMenu(Game_State *state) {
    Reset(&state->mode_arena);

    Mode_Menu *menu = AllocType(&state->mode_arena, Mode_Menu);
    menu->alloc = &state->mode_arena;



		printf("done");
    // Setup front waves
    //
	menu->waves[0].position = V3(0, -0.2f, -0.3f);
	menu->waves[0].radius   = 0.5f;
	menu->waves[0].angle    = 270;
	menu->waves[0].speed    = 280;
	menu->waves[0].texture  =  GetImageByName(&state->assets, "wave_2");

	menu->waves[1].position = V3(0, -0.1f, 0.15f);
	menu->waves[1].radius   = 0.3f;
	menu->waves[1].angle    = 180;
	menu->waves[1].speed    = 360;
	menu->waves[1].texture  = GetImageByName(&state->assets, "wave_1");

	menu->waves[2].position = V3(0, 0, 0.3f);
	menu->waves[2].radius   = 0.2f;
	menu->waves[2].angle    = 0;
	menu->waves[2].speed    = 380;
	menu->waves[2].texture  = GetImageByName(&state->assets, "wave_0");
    state->game_mode = GameMode_Menu;
    state->menu = *menu;

}

function void UpdateRenderClouds(Game_State *state, Input *input, Draw_Batch *batch) {
	Mode_Menu *menu = &(state->menu);
	menu->cloud_timer += input->delta_time;

	if(menu->cloud_timer > CLOUD_SLIDE_TIME) {
		menu->cloud_timer = 0;
	}
	for(s32 i = CLOUD_COUNT - 1; i > -1; i--) {
		const char *tags[10] = {
			"clouds_0",
			"clouds_1",
			"clouds_2",
			"clouds_3",
			"clouds_4",
			"clouds_5",
			"clouds_6",
			"clouds_7",
			"clouds_8",
			"clouds_9"
		};

		Image_Handle cloud = GetImageByName(&state->assets, tags[i]);

		f32 mod    = (i % 2) ? -1 : 1;
		f32 offset = menu->cloud_timer + (CLOUD_SLIDE_TIME / CLOUD_COUNT) * i;

		if (offset > CLOUD_SLIDE_TIME) { offset -= CLOUD_SLIDE_TIME; }

        v3 p;
        p.x = mod * Lerp(0, 9.6f, offset / CLOUD_SLIDE_TIME);
        p.y = (9.6 / (CLOUD_COUNT * 2)) * i;
        p.z = -0.4;

		DrawQuad(batch, cloud, p, 9.6);

        p.x = mod * Lerp(-9.6f, 0, offset / CLOUD_SLIDE_TIME);
		DrawQuad(batch, cloud, p, 9.6);
	}
}

function void UpdateRenderModeMenu(Game_State *state, Input *input, Renderer_Buffer *renderer_buffer) {
	Mode_Menu *menu = &state->menu;

    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;

    Initialise(batch, &state->assets, renderer_buffer);
    SetCameraTransform(batch, 0, V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1), V3(0, 0, 15));

    DrawClear(batch, V4(0, 0, 0, 1));
	UpdateRenderClouds(state, input, batch);
	UpdateRenderWaveList(input->delta_time, batch, menu->waves, 3);
	DrawQuad(batch, GetImageByName(&state->assets, "title"), V3(0, -1.3f, 1), V2(3.2,2), Sin(menu->cloud_timer) / 20.0f);
	v2 screen_bounds = (GetCameraFrustum(&(batch->game_tx)).max - GetCameraFrustum(&(batch->game_tx)).min).xy;
	DrawQuad(batch, GetImageByName(&state->assets, "logo"), V3((screen_bounds.x/2)-0.9f, -(screen_bounds.y/2.0f)-0.5, 1), V2(0.5,0.5));
	DrawQuad(batch, GetImageByName(&state->assets, "play_space"), V3(0, -(screen_bounds.y/2.0f)-0.5, 1), 2);
	if(JustPressed(input->keys[Key_Space])) {
		ModePlay(state, input);
	}
}
