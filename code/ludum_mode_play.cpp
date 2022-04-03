#include "ludum_mode_play.h"
#include "ludum_world_hitbox.cpp"
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
	play->enemy_spawn_time = 10;
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
    Image_Handle flag_texture = GetImageByName(
		&state->assets,
	   	"flag_animation"
	);
	Initialise(
		&(play->ship_mast_1), 
		flag_texture,
		1,
		11,
		1.0f/24
	);
	Initialise(
		&(play->ship_mast_2), 
		flag_texture,
		1,
		11,
		1.0f/24
	);
	play->ship_mast_1.current_frame = 2;
	BuildWorldHitboxes(play);

    state->game_mode = GameMode_Play;
    state->play = *play;
}

function void UpdateShipHoles(
	f64 dt,
	Draw_Batch *batch,
	Ship_Hole *ship_holes
) {
}

function void UpdateRenderClouds(
	f64 dt,
	Draw_Batch *batch,
	Game_State *state
) {
	Mode_Play *play = &(state->play);
	play->cloud_timer += dt;
	if(play->cloud_timer > CLOUD_SLIDE_TIME) {
		play->cloud_timer = 0;
	}
	for(s32 i = CLOUD_COUNT-1; i > -1; i--) {
		char tags[10][9] = {
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
		Image_Handle cloud = GetImageByName(
			&state->assets,
			tags[i]
		);
		f32 mod = 1;
		if(i % 2) {
			mod = -1;
		}
		f32 offset = play->cloud_timer + (CLOUD_SLIDE_TIME/CLOUD_COUNT)*i;
		if(offset > CLOUD_SLIDE_TIME) {
			 offset -= CLOUD_SLIDE_TIME;
		}
		DrawQuad(
			batch,
			cloud,
			V3(
				mod*Lerp(0, 9.6f, offset/CLOUD_SLIDE_TIME),
				(9.6/(CLOUD_COUNT*2))*i,
			   	-0.4
			),
			9.6
		);
		DrawQuad(
			batch,
			cloud,
			V3(
				mod*Lerp(-9.6f, 0, offset/CLOUD_SLIDE_TIME),
				(9.6/(CLOUD_COUNT*2))*i,
			   	-0.4
			),
			9.6
		);
	}
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
			if(play->ship_hole_count < MAX_SHIP_HOLES) {
				Ship_Layer ship_layer = (Ship_Layer)RandomU32(
					&(play->rand),
					Deck_Bottom,
					Deck_Upper+1
				);
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
	UpdateRenderClouds(
		input->delta_time,
		batch,
		state
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

	UpdateAnimation(&(play->ship_mast_1), input->delta_time);
	UpdateAnimation(&(play->ship_mast_2), input->delta_time);
	UpdateAnimation(&(play->anim), input->delta_time);
	v2 player_dim = play->player.dim;
	if(play->player.flags & Player_Flipped) {
		player_dim.x = -player_dim.x;
	}
	DrawAnimation(
		batch,
	   	&(play->ship_mast_1),
	   	V3(0.2,-1,0),
		V2(3, 3),
		0
	);
	DrawAnimation(
		batch,
	   	&(play->ship_mast_2),
	   	V3(2.2,-0.6,0),
		V2(2, 2),
		0
	);
	for(u32 i = 0; i < play->hitbox_count; i++){
		DrawQuad(
			batch,
			{0},
			play->hitboxes[i].pos,
			play->hitboxes[i].dim,
			0,
			play->hitboxes[i].debugColour
		);
	}
	DrawAnimation(
		batch,
	   	&(play->anim),
	   	play->player.p,
		player_dim,
		0
	);


	if(play->player.p.y > 0.23) {
		SetRenderTarget(batch, RenderTarget_Masked);
		DrawClear(batch, V4(0, 0, 0, 0));
	}
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


	if(play->player.p.y > 0.23) {
		SetRenderTarget(batch, RenderTarget_Mask);
		DrawClear(batch, V4(0, 0, 0, 0));

		DrawCircle(batch, { 0 }, mp.xy, 0.95);
		ResolveMasks(batch, false);
	}
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
		player->flags &= ~Player_On_Ladder;
		on_ground = 0;
    }

	b32 on_ladder = player->flags & Player_On_Ladder;
	b32 up_or_down = IsPressed(input->keys[Key_S]) || IsPressed(input->keys[Key_W]);

    // Move left
    //
    if (JustPressed(input->keys[Key_A])) {
        ddp.x = on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED;
		player->flags |= Player_Flipped;
		player->flags &= ~Player_On_Ladder;
    }
	else if(IsPressed(input->keys[Key_A]) && (!on_ladder || !up_or_down)) {
        ddp.x = on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED;
		player->flags |= Player_Flipped;
		player->flags &= ~Player_On_Ladder;
	}

    // Move right
    //
	if(JustPressed(input->keys[Key_D])) {
        ddp.x = on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED;
		player->flags &= ~Player_Flipped;
		player->flags &= ~Player_On_Ladder;
	}
    else if (IsPressed(input->keys[Key_D]) && (!on_ladder || !up_or_down)) {
        ddp.x = on_ground ? PLAYER_MOVE_SPEED : PLAYER_AIR_STRAFE_SPEED;
		player->flags &= ~Player_Flipped;
		player->flags &= ~Player_On_Ladder;
    }

	if(player->flags & Player_On_Ladder) {
		ddp.y = 0;
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
        if (player->dp.y < term_dp && player->flags & Player_On_Ladder) {
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
	b32 anyLadder = 0;
	for(u32 i = 0; i < play->hitbox_count; i++){
		AABB *hitbox = &(play->hitboxes[i]);
		u32 result = ResolveCollision(player->p, player->dim, hitbox);
		if((result == AABB_Sides_noCollision) && (hitbox->flags & Collision_Type_Trap_Door))
			hitbox->flags &= ~Collision_Type_Was_On_Ladder;
		u32 colliding = result & AABB_Sides_collision;
		u32 no_col_flag = result & ~AABB_Sides_collision;
		if(hitbox->flags & (Collision_Type_Normal | Collision_Type_Trap_Door)){
			if(hitbox->flags & Collision_Type_Trap_Door && player->flags & Player_On_Ladder) {
				hitbox->flags |= Collision_Type_Was_On_Ladder;
				continue;
			}
			if(hitbox->flags & Collision_Type_Was_On_Ladder) {
				continue;
			}
			switch(no_col_flag){
				case(AABB_Sides_bottomSide):
					play->hitboxes[i].debugColour = V4(0,1,0,1);
					player->p.y = play->hitboxes[i].pos.y - (play->hitboxes[i].dim.y/2 + player->dim.y/2);
					player->flags|=Player_OnGround;
					player->dp.y=0;
					break;
				case (AABB_Sides_leftSide):
					play->hitboxes[i].debugColour = V4(0,1,0,1);
					player->p.x = play->hitboxes[i].pos.x + (play->hitboxes[i].dim.x/2 + player->dim.x/2) ;
					player->dp.x=0;
					break;
				case (AABB_Sides_rightSide):
					play->hitboxes[i].debugColour = V4(0,1,0,1);
					player->p.x = play->hitboxes[i].pos.x - (play->hitboxes[i].dim.x/2 + player->dim.x/2) ;
					player->dp.x=0;
					break;
				case (AABB_Sides_topSide):
					play->hitboxes[i].debugColour = V4(0,1,0,1);
					player->p.x = -(play->hitboxes[i].dim.x/2 + player->dim.x/2) ;
					player->dp.y=0;
					break;
				default:
					break;
			}
		}
		if(colliding && hitbox->flags & Collision_Type_Ladder) {
			anyLadder = true;
			hitbox->debugColour = V4(0,1,0,1);
			if((IsPressed(input->keys[Key_W])
					|| IsPressed(input->keys[Key_S])) 
					&& !(player->flags & Player_On_Ladder)) {
				player->p.x = hitbox->pos.x;
				player->dp.x = 0;
				player->dp.y = 0;
				player->flags |= Player_On_Ladder;
				player->flags |= Player_OnGround;
			}
			if(player->flags & Player_On_Ladder) {
				ddp.y = 0;
				if(IsPressed(input->keys[Key_W])) {
					f32 move_len = delta_time * PLAYER_LADDER_CLIMB_SPEED;
					if(player->p.y - move_len + player->dim.y/2 > hitbox->pos.y - hitbox->dim.y/2){
						player->p.y -= move_len;
					}
				}
				else if(IsPressed(input->keys[Key_S])) {
					player->p.y += delta_time * PLAYER_LADDER_CLIMB_SPEED;
				}
			}
		}
	}
	if(!anyLadder){
		player->flags &= ~Player_On_Ladder;
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
                    return AABB_Sides_collision|AABB_Sides_leftSide;
                }
                else{
                    return AABB_Sides_collision|AABB_Sides_rightSide;
                }
            }
        }
        else{
            if(overlap.x > 0){
                if(posA.y > collidable->pos.y){
                    return AABB_Sides_collision|AABB_Sides_topSide;
                }
                else{
                    return AABB_Sides_collision|AABB_Sides_bottomSide;
                }
            }
        }
    }
	collidable->debugColour = V4(1,0,0,1);
    return AABB_Sides_noCollision;
}
