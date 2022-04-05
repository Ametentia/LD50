#include "ludum_mode_death_scene.h"

function void ModeDeath(Game_State *state) {
	Mode_Play play = state->play;

    Reset(&state->mode_arena);
    Mode_Death *death = AllocType(&state->mode_arena, Mode_Death);
    death->alloc = &state->mode_arena;

	death->back_wave.position = play.back_waves[0].position;
	death->back_wave.radius   = play.back_waves[0].radius;
	death->back_wave.angle    = play.back_waves[0].angle;
	death->back_wave.speed    = play.back_waves[0].speed;
	death->back_wave.texture  = GetImageByName(&state->assets, "wave_2");

	death->waves[0].position = play.front_waves[0].position;
	death->waves[0].radius   = play.front_waves[0].radius;
	death->waves[0].angle    = play.front_waves[0].angle;
	death->waves[0].speed    = play.front_waves[0].speed;
	death->waves[0].texture  = GetImageByName(&state->assets, "wave_1");

	death->waves[1].position = play.front_waves[1].position;
	death->waves[1].radius   = play.front_waves[1].radius;
	death->waves[1].angle    = play.front_waves[1].angle;
	death->waves[1].speed    = play.front_waves[1].speed;
	death->waves[1].texture  = GetImageByName(&state->assets, "wave_0");
	death->cloud_timer = play.cloud_timer;
	death->ship_mast_1 = play.ship_mast_1;
	death->ship_mast_2 = play.ship_mast_2;
	death->survival = play.survive_time;

    state->game_mode = GameMode_Death;
    state->death = *death;
}

function void UpdateRenderCloudsDeath(Game_State *state, Input *input, Draw_Batch *batch) {
	Mode_Death *death = &(state->death);
	death->cloud_timer += input->delta_time;

	if(death->cloud_timer > CLOUD_SLIDE_TIME) {
		death->cloud_timer = 0;
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
		f32 offset = death->cloud_timer + (CLOUD_SLIDE_TIME / CLOUD_COUNT) * i;

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

function void UpdateRenderModeDeath(Game_State *state, Input *input, Renderer_Buffer *renderer_buffer) {
	Mode_Death *death = &state->death;

    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;

    Initialise(batch, &state->assets, renderer_buffer);
    SetCameraTransform(batch, 0, V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1), V3(0, 0, 15));

	UpdateAnimation(&death->ship_mast_1, input->delta_time);
	UpdateAnimation(&death->ship_mast_2, input->delta_time);

    DrawClear(batch, V4(0, 0, 0, 1));
	if(death->angle < (-Pi32/8)*3) {
		death->height += 1.1*input->delta_time;
	}
	else {
		death->height -= 1*input->delta_time;
	}
	f32 drop = death->height;
	v2 pos = V2(0-sin(death->angle*2), 0.2 + drop);
	UpdateRenderCloudsDeath(state, input, batch);
	UpdateRenderWaveList(input->delta_time, batch, &death->back_wave, 1);
    Image_Handle back_texture   = GetImageByName(&state->assets, "back_layer");
    Image_Handle middle_texture = GetImageByName(&state->assets, "middle_layer");
    Image_Handle front_texture  = GetImageByName(&state->assets, "front_layer");
	DrawQuad(batch, back_texture,   pos, 9.3, death->angle);
	DrawQuad(batch, middle_texture, pos, 9.3, death->angle);
    DrawQuad(batch, front_texture, pos, 9.3, death->angle);
	DrawAnimation(batch, &death->ship_mast_1, pos+Rotate(V2(0.2, -1.2), death->angle), V2(3, 3), death->angle);
	DrawAnimation(batch, &death->ship_mast_2, pos+Rotate(V2(2.2, -0.8), death->angle), V2(2, 2), death->angle);
	UpdateRenderWaveList(input->delta_time, batch, death->waves, 2);
	Image_Handle filter_handle = GetImageByName(&state->assets, "filter");
	DrawQuad(batch, filter_handle, V2(0,0), 10);
	death->angle = Max(death->angle - 0.8*input->delta_time, -Pi32/2);
	if(death->angle < (-Pi32/8)*3) {
		v2 score_pos = pos - V2(0, 5.5);
		score_pos.y = Min(score_pos.y, 0);
		u32 survive_time = (u32)death->survival;
		const char *numbers[10] = {
			"0",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9"
		};
		for(s32 i = 0; i < 7; i++) {
			Image_Handle number_handle  = GetImageByName(&state->assets, numbers[survive_time%10]);
			survive_time = survive_time/10;
			DrawQuad(batch, number_handle, score_pos - V2((i-3)*0.5, 0.9), 0.5);
		}
		DrawQuad(batch, GetImageByName(&state->assets, "congratulations"), score_pos - V2(0, 1.6), 3);
		DrawQuad(batch, GetImageByName(&state->assets, "you_survived"), score_pos - V2(0, 1.25), 2);
		DrawQuad(batch, GetImageByName(&state->assets, "seconds"), score_pos - V2(0, 0.5), 2);
	}
	if(pos.y - 5.5f > 0) {
		DrawQuad(batch, GetImageByName(&state->assets, "continue_space"), V2(0, 2), 2);
		if(JustPressed(input->keys[Key_Space])) {
			ModeMenu(state);
		}
	}
}
