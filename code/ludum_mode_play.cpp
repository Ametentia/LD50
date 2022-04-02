#include "ludum_mode_play.h"
function void ModePlay(Game_State *state) {
    Reset(&state->mode_arena);

    Mode_Play *play = AllocType(&state->mode_arena, Mode_Play);
    play->alloc = &state->mode_arena;

	play->back_wave_count = 1;
	play->front_wave_count = 2;

	play->back_waves[0].position = V3(0, 0.6f, -0.3f);
	play->back_waves[0].radius = 0.5f;
	play->back_waves[0].angle = 270;
	play->back_waves[0].speed = 280;
    Image_Handle back_water_texture = GetImageByName(
		&state->assets,
	   	"wave_2"
	);
	play->back_waves[0].texture = back_water_texture;

	play->front_waves[0].position = V3(0, 0.1f, 0.15f);
	play->front_waves[0].radius = 0.4f;
	play->front_waves[0].angle = 180;
	play->front_waves[0].speed = 380;
    Image_Handle front_water_texture1 = GetImageByName(
		&state->assets,
	   	"wave_1"
	);
	play->front_waves[0].texture = front_water_texture1;

	play->front_waves[1].position = V3(0, 0.2, 0.3f);
	play->front_waves[1].radius = 0.3f;
	play->front_waves[1].angle = 0;
	play->front_waves[1].speed = 360;
    Image_Handle front_water_texture2 = GetImageByName(
		&state->assets,
	   	"wave_0"
	);
	play->front_waves[1].texture = front_water_texture2;

    state->game_mode = GameMode_Play;
    state->play = *play;
}

function void UpdateRenderWaveList(
	f64 dt,
	Draw_Batch *batch,
   	Wave_Layer *layers,
   	u32 layer_count
) {
	for(u8 i = 0; i < layer_count; i++) {
		Wave_Layer *layer = (Wave_Layer*)(layers+i);
		layer->angle += Degrees(layer->speed) * dt;
		v2 distance = V2(layer->radius, 0);
		v2 rotated = Rotate(distance, layer->angle);
		v3 render_pos = layer->position + V3(rotated.x, rotated.y, 0);
		DrawQuad(
			batch,
			layer->texture,
			render_pos,
			10,
			0
		);
	}
}

function void UpdateRenderModePlay(Game_State *state, Input *input, Renderer_Buffer *renderer_buffer) {
    Mode_Play *play = &(state->play);
    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;
    Initialise(batch, &state->assets, renderer_buffer);
    SetCameraTransform(batch, 0, V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1), V3(0, 0, 15));
    DrawClear(batch, V4(0, 1, 1, 1));
    Image_Handle back_texture = GetImageByName(
		&state->assets,
	   	"back_layer"
	);
    Image_Handle middle_texture = GetImageByName(
		&state->assets,
	   	"middle_layer"
	);
	UpdateRenderWaveList(
		input->delta_time,
		batch,
	   	play->back_waves,
	   	play->back_wave_count
	);
	DrawQuad(
		batch,
	   	back_texture,
		V3(0,0,0),
		9,
		0
	);
	DrawQuad(
		batch,
	   	middle_texture,
		V3(0,0,0),
		9,
		0
	);
	UpdateRenderWaveList(
		input->delta_time,
		batch,
	   	play->front_waves,
	   	play->front_wave_count
	);
}
