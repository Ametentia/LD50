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
	play->player.p = V2(0,-0.5);
	play->player.dim = V2(0.2,0.2);

    Image_Handle shipTexture = GetImageByName(
		&state->assets,
	   	"badship"
	);
    Image_Handle enemy_border = GetImageByName(
		&state->assets,
	   	"enemy_border"
	);
	play->enemy_spawn_time = 1;
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

	play->hitboxes[0].pos = V2(0,0.5);
	play->hitboxes[0].dim = V2(5, 0.4);
	play->hitboxes[0].flags = Normal;
	play->hitboxes[0].debugColour = V4(1,0,0,1);

	play->hitboxes[1].pos = V2(-2,0);
	play->hitboxes[1].dim = V2(0.4, 2);
	play->hitboxes[1].flags = Ladder;
	play->hitboxes[1].debugColour = V4(1,0,0,1);
	
	play->hitboxes[2].pos = V2(2,0);
	play->hitboxes[2].dim = V2(0.3, 0);
	play->hitboxes[2].flags = Trap_Door;
	play->hitboxes[2].debugColour = V4(1,0,0,1);
	
	play->hitboxes[3].pos = V2(4,-0.4);
	play->hitboxes[3].dim = V2(0.8, 0.8);
	play->hitboxes[3].flags = Cannon;
	play->hitboxes[3].debugColour = V4(1,0,0,1);
	
	play->hitboxes[4].pos = V2(-1,-0.4);
	play->hitboxes[4].dim = V2(0.1, 0.8);
	play->hitboxes[4].flags = Cannon_Hole;
	play->hitboxes[4].debugColour = V4(1,0,0,1);
	

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
			if(play->ship_hole_count < MAX_SHIP_HOLES) {
				Ship_Layer ship_layer = (Ship_Layer)RandomU32(
					&(play->rand),
					Deck_Bottom,
					Deck_Upper+1
				);
				printf("I hit %d\n", ship_layer);
				v2 hitbox_dim = V2(0.3, 0.5);
				f32 hitbox_x;
				f32 hitbox_y;
				switch(ship_layer) {
					case Deck_Bottom:
						hitbox_x = RandomF32(&(play->rand), -1.9, 1.7);
						hitbox_y = 1.63;
						break;
					case Deck_Middle:
						hitbox_x = RandomF32(&(play->rand), -2.1, 2.1);
						hitbox_y = 1.1;
						break;
					case Deck_Upper:
						hitbox_x = RandomF32(&(play->rand), -2.1, 2.3);
						hitbox_y = 0.5;
						break;
				};
				Ship_Hole *hole;
				for(u8 i = 0; i < MAX_SHIP_HOLES; i++) {
					if(!play->ship_holes[i].active) {
						hole = &(play->ship_holes[i]);
						break;
					}
				}
				// lol hanging pointer funi
				hole->position = V3(hitbox_x, hitbox_y, 3);
				hole->hitbox_dim = hitbox_dim;
				hole->active = true;
				play->ship_hole_count++;
			}

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
    Image_Handle background = GetImageByName(
		&state->assets,
	   	"background"
	);
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
	DrawQuad(
		batch,
	   	background,
		V3(0,0,-0.5),
		9.6,
		0
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
		V3(0,0.2,0),
		9.3,
		0
	);
	DrawQuad(
		batch,
	   	middle_texture,
		V3(0,0.2,0),
		9.3,
		0
	);
	UpdateRenderEnemyShip(
		input->delta_time,
		batch,
		play
	);

	UpdateAnimation(&(play->anim), input->delta_time);
	v2 player_dim = play->player.dim;
	if(play->player.flags & Player_Flipped) {
		player_dim.x = -player_dim.x;
	}
	DrawAnimation(
		batch,
	   	&(play->anim),
	   	play->player.p,
		player_dim,
		0
	);

	DrawQuad(
		batch,
		{0},
		play->hitboxes[0].pos,
		play->hitboxes[0].dim,
		0,
		play->hitboxes[0].debugColour
	);

    SetRenderTarget(batch, RenderTarget_Masked);
    DrawClear(batch, V4(0, 0, 0, 0));

	DrawQuad(
		batch,
	   	front_texture,
		V3(0,0.2,0),
		9.3,
		0
	);
	
	UpdateRenderWaveList(
		input->delta_time,
		batch,
	   	play->front_waves,
	   	play->front_wave_count
	);
	for(u32 i = 0; i < MAX_SHIP_HOLES; i++) {
		Ship_Hole hole = play->ship_holes[i];
		if(!hole.active) {
			continue;
		}
		DrawQuad(
			batch,
		   	{0},
		   	hole.position,
		   	hole.hitbox_dim,
			0
		);
	}

    //v3 mp = Unproject(&batch->game_tx, input->mouse_clip);
	v3 mp = V3(play->player.p.x, play->player.p.y, 0);

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
		player->flags |= Player_Flipped;
    }

    // Move right
    //
    if (IsPressed(input->keys[Key_D])) {
        ddp.x = on_ground ? PLAYER_MOVE_SPEED : PLAYER_AIR_STRAFE_SPEED;
		player->flags &= !Player_Flipped;
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
	
	u32 collisions = 0;
	for(u32 i = 0; i < 1; i++){
		collisions |= ResolveCollision(player->p, player->dim, &(play->hitboxes[i]));
		switch(collisions){
			case bottomSide|collision:
				player->p.y = play->hitboxes[i].pos.y - (play->hitboxes[i].dim.y/2 + player->dim.y/2) ;
				player->flags|=Player_OnGround;
				break;
			default:
			break;
		}
		if(collisions&bottomSide){
			player->dp.y=0;
		}
	}
}

function u32 ResolveCollision(v2 posA, v2 dimA, AABB *collidable){
    rect2 a_r;
    rect2 b_r;
    a_r.min = posA - (0.5f*dimA);
    a_r.max = posA + (0.5f*dimA);
    b_r.min = collidable->pos - (0.5f*collidable->dim);
    b_r.max = collidable->pos + (0.5f*collidable->dim);

    v2 overlap;
    overlap.x =  Min(b_r.max.x, a_r.max.x) - Max(b_r.min.x, a_r.min.x);
    overlap.y =  Min(b_r.max.y, a_r.max.y) - Max(b_r.min.y, a_r.min.y);

    if(overlap.x >= 0 && overlap.y >= 0){
        if(overlap.x < overlap.y){
            if(overlap.x > 0){
                if(posA.x > collidable->pos.x){
                    return collision|leftSide;
                }
                else{
                    return collision|rightSide;
                }
            }
        }
        else{
            if(overlap.x > 0){
                if(posA.y > collidable->pos.y){
                    return collision|topSide;
                }
                else{
                    return collision|bottomSide;
                }
            }
        }
    }
	if(collision){
		collidable->debugColour = V4(0,1,0,1);
	}
	else{
		collidable->debugColour = V4(1,0,0,1);
	}
    return noCollision;
}


