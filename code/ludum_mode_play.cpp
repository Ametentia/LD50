#include "ludum_mode_play.h"
function void ModePlay(Game_State *state, Input *input) {
    Reset(&state->mode_arena);

    Mode_Play *play = AllocType(&state->mode_arena, Mode_Play);
    play->alloc = &state->mode_arena;
	play->rand = RandomSeed(input->ticks);

	play->back_wave_count = 1;
	play->front_wave_count = 2;

	play->back_waves[0].position = V3(0, -0.2f, -0.3f);
	play->back_waves[0].radius = 0.5f;
	play->back_waves[0].angle = 270;
	play->back_waves[0].speed = 280;
    Image_Handle back_water_texture = GetImageByName(
		&state->assets,
	   	"wave_2"
	);
	play->back_waves[0].texture = back_water_texture;

	play->front_waves[0].position = V3(0, -0.1f, 0.15f);
	play->front_waves[0].radius = 0.3f;
	play->front_waves[0].angle = 180;
	play->front_waves[0].speed = 360;
    Image_Handle front_water_texture1 = GetImageByName(
		&state->assets,
	   	"wave_1"
	);
	play->front_waves[0].texture = front_water_texture1;

	play->front_waves[1].position = V3(0, 0, 0.3f);
	play->front_waves[1].radius = 0.2f;
	play->front_waves[1].angle = 0;
	play->front_waves[1].speed = 380;
    Image_Handle front_water_texture2 = GetImageByName(
		&state->assets,
	   	"wave_0"
	);
	play->front_waves[1].texture = front_water_texture2;
	play->player.p = V2(0,-0.4);
	play->player.dim = V2(0.5,0.5);

    Image_Handle shipTexture = GetImageByName(
		&state->assets,
	   	"badship"
	);
    Image_Handle enemy_border = GetImageByName(
		&state->assets,
	   	"enemy_border"
	);
	play->enemy_spawn_time = 12;
	play->enemies[0].health = 0;
	play->enemies[0].fire_interval = 10.0;
	play->enemies[0].time_since_shot = 0;
	play->enemies[0].width = 0;
	play->enemies[0].border = enemy_border;
	Initialise(
		&(play->enemies[0].anim), 
		shipTexture,
		1,
		2,
		0.7f
	);

	play->enemies[1].health = 0;
	play->enemies[1].fire_interval = 10.0;
	play->enemies[1].time_since_shot = 0;
	play->enemies[1].width = 0;
	play->enemies[1].border = enemy_border;
	Initialise(
		&(play->enemies[1].anim), 
		shipTexture,
		1,
		2,
		0.7f
	);
    Image_Handle skipper = GetImageByName(
		&state->assets,
	   	"skipper_walking"
	);
	Initialise(
		&(play->anim), 
		skipper,
		1,
		8,
		1.0f/12
	);

    state->game_mode = GameMode_Play;
    state->play = *play;
}

function void UpdateShipHoles(
	f64 dt,
	Draw_Batch *batch,
	Ship_Hole *ship_holes
) {

}

function void UpdateRenderEnemyShip(
	f64 dt,
	Draw_Batch *batch,
	Mode_Play *play
) {
	b32 all_alive = 1;
	for(u8 i = 0; i < 2; i++) {
		Enemy_Ship *enemy = &(play->enemies[i]);
		if(play->time_since_enemy > play->enemy_spawn_time && enemy->health <= 0) {
			play->time_since_enemy = 0;
			enemy->health = 3;
			play->enemy_spawn_time = RandomF32(&(play->rand), 20, 35);
			printf("Next spawn in %f\n", play->enemy_spawn_time);
		}
		if(enemy->health <= 0) {
			all_alive = 0;
			continue;
		}
		enemy->time_since_shot += dt;
		enemy->width += 5*dt;
		v3 pos = V3(-2.4, -1.2, 0.8);
		v2 scale = V2(Min(enemy->width, 1.8f), Min(enemy->width, 1.8f));
		if(i%2) {
			pos.x = -pos.x;
			scale.x = -scale.x;
		}
		UpdateAnimation(&(enemy->anim), dt);
		DrawQuad(
			batch,
			enemy->border,
			pos,
			scale,
			0
		);
		DrawAnimation(
			batch, 
			&(enemy->anim),
			pos,
			scale,
			0
		);
		if(enemy->time_since_shot > enemy->fire_interval) {
			enemy->fire_interval = RandomF32(&(play->rand), 9, 15);
			enemy->time_since_shot = 0;
			printf("Bang %d %f!\n", i, enemy->fire_interval);
			/*
			// Upper deck Left
			DrawQuad(batch, {0}, V3(-2.1,0.5,3), V2(0.3, 0.5), 0);
			// Upper deck right
			DrawQuad(batch, {0}, V3(2.3,0.5,3), V2(0.3, 0.5), 0);
			// mid deck Left
			DrawQuad(batch, {0}, V3(-2.1,1.1,3), V2(0.3, 0.5), 0);
			// mid deck right
			DrawQuad(batch, {0}, V3(2.1,1.1,3), V2(0.3, 0.5), 0);
			// bottom deck Left
			DrawQuad(batch, {0}, V3(-1.9,1.63,3), V2(0.3, 0.5), 0);
			// bottom deck right
			DrawQuad(batch, {0}, V3(1.7,1.63,3), V2(0.3, 0.5), 0);
			*/
		}
	}
	if(!all_alive) {
		play->time_since_enemy += dt;
	}
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
			12,
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
    Image_Handle front_texture = GetImageByName(
		&state->assets,
	   	"front_layer"
	);
	
	
	UpdatePlayer(play, &(play->player), input, state);
	UpdateRenderWaveList(
		input->delta_time,
		batch,
	   	play->back_waves,
	   	play->back_wave_count
	);
	DrawQuad(
		batch,
	   	back_texture,
		V3(0,0.2,0),
		9,
		0
	);
	DrawQuad(
		batch,
	   	middle_texture,
		V3(0,0.2,0),
		9,
		0
	);
	UpdateRenderEnemyShip(
		input->delta_time,
		batch,
		play
	);

	DrawQuad(
		batch,
		{0},
		V2(0,0.4),
		V2(8,0.2),
		0,
		V4(0,0,1,1)
	);
	DrawQuad(
		batch,
		{0},
		play->player.p,
		play->player.dim,
		0,
		V4(1,1,1,1)
	);

    SetRenderTarget(batch, RenderTarget_Masked);
    DrawClear(batch, V4(0, 0, 0, 0));

	DrawQuad(
		batch,
	   	front_texture,
		V3(0,0.2,0),
		9,
		0
	);
	
	UpdateRenderWaveList(
		input->delta_time,
		batch,
	   	play->front_waves,
	   	play->front_wave_count
	);
	UpdateAnimation(&(play->anim), input->delta_time);
	DrawAnimation(batch, &(play->anim), V3(0,0.055,3), V2(0.2,0.2), 0);
	// Upper deck Left
	DrawQuad(batch, {0}, V3(-2.1,0.5,3), V2(0.3, 0.5), 0);
	// Upper deck right
	DrawQuad(batch, {0}, V3(2.3,0.5,3), V2(0.3, 0.5), 0);
	// mid deck Left
	DrawQuad(batch, {0}, V3(-2.1,1.1,3), V2(0.3, 0.5), 0);
	// mid deck right
	DrawQuad(batch, {0}, V3(2.1,1.1,3), V2(0.3, 0.5), 0);
	// bottom deck Left
	DrawQuad(batch, {0}, V3(-1.9,1.63,3), V2(0.3, 0.5), 0);
	// bottom deck right
	DrawQuad(batch, {0}, V3(1.7,1.63,3), V2(0.3, 0.5), 0);

    v3 mp = Unproject(&batch->game_tx, input->mouse_clip);

    SetRenderTarget(batch, RenderTarget_Mask);
    DrawClear(batch, V4(0, 0, 0, 0));

    DrawCircle(batch, { 0 }, mp.xy, 0.95);

    ResolveMasks(batch, false);
}

function void UpdatePlayer(Mode_Play *play, Player *player, Input *input, Game_State *state){
	f32 delta_time = input->delta_time;
	f32 gravity = (2 * PLAYER_MAX_JUMP_HEIGHT) / (PLAYER_JUMP_APEX_TIME * PLAYER_JUMP_APEX_TIME);
    v2 ddp = V2(0, gravity);
	b32 on_ground = (player->flags&Player_OnGround);

    // Attempt to jump. We will buffer this for an amount of time so if the player presses jump
    // slightly before hitting the ground it will still count
    //
    if (JustPressed(input->keys[Key_Space])) {
        player->last_jump_time = input->time;
		on_ground = 0;
    }

    // Move left
    //
    if (IsPressed(input->keys[Key_A])) {
        ddp.x = on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED;
    }

    // Move right
    //
    if (IsPressed(input->keys[Key_D])) {
        ddp.x = on_ground ? PLAYER_MOVE_SPEED : PLAYER_AIR_STRAFE_SPEED;
    }

    // If neither left or right were pressed apply damping to the player
    //
    if (IsZero(ddp.x)) {
        player->dp.x     *= (1.0f / (1 + (PLAYER_DAMPING * delta_time)));
    }

    if ((input->time - player->last_jump_time) <= PLAYER_JUMP_BUFFER_TIME) {
        b32 double_jump = (player->flags & Player_DoubleJump);

        if (on_ground || double_jump) {
            if (double_jump) {
                gravity = (2 * PLAYER_MAX_DOUBLE_JUMP_HEIGHT) / (PLAYER_JUMP_APEX_TIME * PLAYER_JUMP_APEX_TIME);

                player->dp.y   = -Sqrt(2 * gravity * PLAYER_MAX_DOUBLE_JUMP_HEIGHT);
                player->flags &= ~Player_DoubleJump;
            }
            else {
                player->dp.y   = -Sqrt(2 * gravity * PLAYER_MAX_JUMP_HEIGHT);
                player->flags |= Player_DoubleJump;
            }

            player->flags &= ~Player_OnGround;
            player->last_jump_time = 0;
        }
    }

    if (!IsPressed(input->keys[Key_Space]) && (player->dp.y < 0)) {
        f32 initial_dp_sq = (2 * gravity * PLAYER_MAX_JUMP_HEIGHT);
        f32 limit_dp_sq   = (2 * gravity * (PLAYER_MAX_JUMP_HEIGHT - PLAYER_MIN_JUMP_HEIGHT));

        f32 term_dp = -Sqrt(initial_dp_sq - limit_dp_sq);
        if (player->dp.y < term_dp) {
            player->dp.y = term_dp;
        }
    }

    // Limit x speed
    //
    if (Abs(player->dp.x) > PLAYER_MAX_SPEED_X) {
        player->dp.x *= (PLAYER_MAX_SPEED_X / Abs(player->dp.x));
    }

    // Limit y only in downward direction to prevent jumps from being cut short
    //
    if (player->dp.y > PLAYER_MAX_SPEED_Y) {
        player->dp.y *= (PLAYER_MAX_SPEED_Y / player->dp.y);
    }

    player->p  += (player->dp * delta_time);
    player->dp += (ddp * delta_time);
	
	int result = AABB(player->p, player->dim,V2(0,0.4),V2(8,0.2));
	switch(result){
		case bottomSide:
			player->p.y= 0.4-((player->dim.y)/2 + 0.1);
			player->flags|=Player_OnGround;
			break;
		default:
		break;
	}
	if(result&bottomSide){
		player->dp.y=0;
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


