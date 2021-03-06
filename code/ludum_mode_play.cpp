function void ModePlay(Game_State *state, Input *input) {
    Reset(&state->mode_arena);

    Mode_Play *play = AllocType(&state->mode_arena, Mode_Play);
    play->alloc     = &state->mode_arena;
	play->rand      = RandomSeed(input->ticks);

    Sound_Handle wind_handle = GetSoundByName(&state->assets, "wind");
    play->wind = PlaySound(&state->audio_state, wind_handle, PlayingSound_Looped, V2(0.3, 0.3));

    play->hits[0] = GetSoundByName(&state->assets, "hit0");
    play->hits[1] = GetSoundByName(&state->assets, "hit1");

    // Setup back waves
	play->back_wave_count = 1;

	play->back_waves[0].position = V3(0, -0.2f, 0.0f);
	play->back_waves[0].radius   = 0.5f;
	play->back_waves[0].angle    = 270;
	play->back_waves[0].speed    = 280;
	play->back_waves[0].texture  =  GetImageByName(&state->assets, "wave_2");

    // Setup front waves
    //
	play->front_wave_count = 2;

	play->front_waves[0].position = V3(0, -0.1f, 0.0f);
	play->front_waves[0].radius   = 0.3f;
	play->front_waves[0].angle    = 180;
	play->front_waves[0].speed    = 360;
	play->front_waves[0].texture  = GetImageByName(&state->assets, "wave_1");

	play->front_waves[1].position = V3(0, 0, 0.0f);
	play->front_waves[1].radius   = 0.2f;
	play->front_waves[1].angle    = 0;
	play->front_waves[1].speed    = 380;
	play->front_waves[1].texture  = GetImageByName(&state->assets, "wave_0");

    // Setup player
    //
    Player *player = &play->player;
	player->p      = V2(0.0f, -0.5f);
	player->dim    = V2(0.2f,  0.2f);

	play->droppedItems[0].active = 1;
	play->droppedItems[0].type = Item_Bucket;
	play->droppedItems[0].hitbox.dim = V2(0.2,0.2);
	play->droppedItems[0].hitbox.pos = V2(0.2, -1);
	play->droppedItems[1].active = 1;
	play->droppedItems[1].type = Item_Bucket;
	play->droppedItems[1].hitbox.dim = V2(0.2,0.2);
	play->droppedItems[1].hitbox.pos = V2(0, -1);
	play->droppedItems[2].active = 1;
	play->droppedItems[2].type = Item_Bucket;
	play->droppedItems[2].hitbox.dim = V2(0.2,0.2);
	play->droppedItems[2].hitbox.pos = V2(0.4, -1);
	play->droppedItems[3].active = 1;
	play->droppedItems[3].type = Item_Bucket;
	play->droppedItems[3].hitbox.dim = V2(0.2,0.2);
	play->droppedItems[3].hitbox.pos = V2(0.6, -1);

	Initialise(&player->anim, GetImageByName(&state->assets, "skipper_walking"), 1, 8, 1.0f / 12.0f);

    // Setup enemy ships
    //
	play->enemy_spawn_time = RandomF32(&play->rand, 10.0f, 20.0f);

    Image_Handle enemy_ship   = GetImageByName(&state->assets, "enemy_periscope");
	// cannons :)
    Image_Handle enemy_ship_dead   = GetImageByName(&state->assets, "enemy_periscope_death");
	Initialise(&play->cannon_anim, GetImageByName(&state->assets, "cannon_firing_sheet"), 6, 11, 1.0f / 12.0f);

    for (u32 it = 0; it < ArraySize(play->enemies); ++it) {
        play->enemies[it].health          = 0;
        play->enemies[it].fire_interval   = 10.0f;
        play->enemies[it].time_since_shot = 0;
        play->enemies[it].width           = 0;

        Initialise(&play->enemies[it].anim, enemy_ship, 1, 7, 1.0/14.0f);
        Initialise(&play->enemies[it].death_anim, enemy_ship_dead, 9, 6, 1.0/14.0f);
    }

    Image_Handle flag_texture = GetImageByName(&state->assets, "flag_animation");

	Initialise(&play->ship_mast_1, flag_texture, 1, 11, 1.0f / 24.0f);
	Initialise(&play->ship_mast_2, flag_texture, 1, 11, 1.0f / 24.0f);

	Initialise(&play->tentacle.animEntry, GetImageByName(&state->assets, "tentacle_wrap"), 4, 6, 1.0f / 24.0f);
	Initialise(&play->tentacle.animDeath, GetImageByName(&state->assets, "tentacle_death"), 1, 16, 1.0f / 24.0f);

    play->tentacle.dead = true;
	play->tentacle.play_tentacle_entry = 0;
	play->tentacleTimer = RandomF32(&play->rand, 90,120);
	play->ship_mast_1.current_frame = 2;

	BuildWorldHitboxes(play);

    state->game_mode = GameMode_Play;
    state->play = *play;
}

function void UpdateRenderClouds(f64 dt, Draw_Batch *batch, Game_State *state) {
	Mode_Play *play = &state->play;

	play->cloud_timer += dt;

	if(play->cloud_timer > CLOUD_SLIDE_TIME) {
		play->cloud_timer = 0;
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
		f32 offset = play->cloud_timer + (CLOUD_SLIDE_TIME / CLOUD_COUNT) * i;

		if (offset > CLOUD_SLIDE_TIME) { offset -= CLOUD_SLIDE_TIME; }

        v3 p;
        p.x = mod * Lerp(0, 8.2f, offset / CLOUD_SLIDE_TIME);
        p.y = (9.6 / (CLOUD_COUNT * 2)) * i;
        p.z = -0.4;

		DrawQuad(batch, cloud, p, 11.2f);

        p.x = mod * Lerp(-8.2f, 0, offset / CLOUD_SLIDE_TIME);
		DrawQuad(batch, cloud, p, 11.2f);
	}
}

function f32 FindWaterLevel(Mode_Play *play, Draw_Batch *batch){
	v2 screen_bounds = (GetCameraFrustum(&(batch->game_tx)).max - GetCameraFrustum(&(batch->game_tx)).min).xy;
	return -(screen_bounds.y/2)-play->water_level + 0.08;
}

function void RenderWaterLevel(Draw_Batch *batch, Game_State *state) {
	Mode_Play *play = &state->play;

	f32 offset = Lerp(-9.3f, 0, play->cloud_timer / CLOUD_SLIDE_TIME);
	v3 pos = V3(offset, FindWaterLevel(play, batch), 0);
    Image_Handle water_top = GetImageByName(&state->assets, "water_level");
    Image_Handle water_col = GetImageByName(&state->assets, "water_colour");
	DrawQuad(batch, water_top, pos.xy, 9.3);
	offset = Lerp(0, 9.3f, play->cloud_timer / CLOUD_SLIDE_TIME);
	pos = V3(offset, FindWaterLevel(play, batch), 0);
	DrawQuad(batch, water_top, pos.xy, 9.3);
	DrawQuad(batch, water_col, V3(0, pos.y + 2.422, 0), 9.3);
}

function void UpdateShipHoles(Game_State *state, Input *input) {
	Mode_Play *play = &state->play;
	Player *player = &(play->player);
	f64 delta_time = input->delta_time;
	for(u32 i = 0; i < MAX_SHIP_HOLES; i++) {
		// Decrease the timer on the first hole
		// which is colliding with the player
		// if they are holding E
		Ship_Hole *hole = &(play->ship_holes[i]);
		if(!hole->active)
			continue;
		AABB hitbox = {};
		hitbox.pos = V2(hole->position.x, hole->position.y);
		hitbox.dim = hole->hitbox_dim;
		u32 result = ResolveCollision(player->p, player->dim, &hitbox);
		b32 colliding = result & AABB_Sides_collision;
		b32 player_holding = player->flags & Player_Holding;
		b32 player_has_plank = player->holdingFlags & Held_Plank;
		if(IsPressed(input->keys[Key_E]) && colliding && player_has_plank && player_holding) {
			hole->timer -= delta_time;

            if (!play->fix_sound) {
                Sound_Handle sound = GetSoundByName(&state->assets, "fix_hole");
                play->fix_sound = PlaySound(&state->audio_state,  sound, 0, V2(0.2, 0.2));
            }

			break;
		}
	}

    f32 rise_rate = WATER_RISE_RATE_PER_HOLE * delta_time;
	for(u32 i = 0; i < MAX_SHIP_HOLES; i++) {
		Ship_Hole *hole = &(play->ship_holes[i]);
		if (!hole->active) { continue; }

		play->water_level += rise_rate;

		if(play->water_level >= 2.28) {
			play->game_over = 1;
		}
		play->water_level = Min(play->water_level, 2.28);
		if(hole->timer < 0) {
			hole->active = 0;
			player->holdingFlags &= ~Held_Plank;
			player->flags &= ~Player_Holding;
			play->ship_hole_count--;

            if (hole->timer <= 0) { play->fix_sound = 0; }
		}
	}
}

function void UpdateRenderEnemyShip(f64 dt, Draw_Batch *batch, Mode_Play *play, Audio_State *audio) {
	b32 all_alive = 1;

	for(u8 i = 0; i < ArraySize(play->enemies); i++) {
		Enemy_Ship *enemy = &play->enemies[i];

		if (play->time_since_enemy > play->enemy_spawn_time && enemy->health <= 0) {
            if (!enemy->play_dead && !enemy->render_while_dead) {
                play->time_since_enemy = 0;
                play->enemy_spawn_time = RandomF32(&play->rand, 20, 50);
                enemy->health          = RandomU32(&play->rand, 2,  4);
            }
		}

		v3 pos   = V3(-2.0, -1.2, 0);
		v2 scale = V2(enemy->width, enemy->width);

		if (i % 2) {
			pos.x   = -pos.x;
			scale.x = -scale.x;
		}

		if (enemy->play_dead) {
			// Minus 2, we're 1 frame short on a square sprite sheet, zero indexed
            //
			u32 frames = (enemy->death_anim.rows * enemy->death_anim.cols) - 2;
			if (enemy->death_anim.current_frame == frames) {
                enemy->width -= 5 * dt;

                if (enemy->width <= 0) {
                    enemy->width     = 0;
                    enemy->play_dead = 0;
                }
			}
            else {
                UpdateAnimation(&enemy->death_anim, dt);
            }

            DrawAnimation(batch, &enemy->death_anim, pos, scale, 0);
		}

		if (enemy->health <= 0) {
			all_alive = 0;
			if (enemy->render_while_dead) {
				UpdateAnimation(&enemy->anim, dt);
				DrawAnimation(batch, &enemy->anim, pos, scale, 0);
			}

			continue;
		}

		enemy->time_since_shot += dt;
		enemy->width           += 5 * dt;

        enemy->width = Min(enemy->width, 1.8f);

		UpdateAnimation(&enemy->anim, dt);
		DrawAnimation(batch, &enemy->anim, pos, scale, 0);

		if(enemy->time_since_shot > enemy->fire_interval) {
			enemy->fire_interval = RandomF32(&(play->rand), 9, 15);
			enemy->time_since_shot = 0;
			if(play->ship_hole_count < MAX_SHIP_HOLES) {
                v2 hitbox_p   = V2(0, 0);
				v2 hitbox_dim = V2(0.3, 0.5);

				Ship_Layer ship_layer = (Ship_Layer) RandomU32(&play->rand, Deck_Bottom, Deck_Upper + 1);
				switch (ship_layer) {
					case Deck_Bottom: {
					    hitbox_p.x = RandomF32(&play->rand, -1.9, 1.7);
						hitbox_p.y = 2.2;
                    }
                    break;
					case Deck_Middle: {
						hitbox_p.x = RandomF32(&play->rand, -2.1, 2.1);
						hitbox_p.y = 1.5;
                    }
                    break;
					case Deck_Upper: {
						hitbox_p.x = RandomF32(&(play->rand), -2.1, 2.3);
						hitbox_p.y = 0.7;
                    }
                    break;
				}

				for (u32 h = 0; h < MAX_SHIP_HOLES; h++) {
                    Ship_Hole *hole = &play->ship_holes[h];

					if (!hole->active) {
                        hole->position   = V3(hitbox_p);
                        hole->hitbox_dim = hitbox_dim;
                        hole->active     = true;
						hole->timer      = SHIP_HOLE_FIX_TIME;

                        u32 index = RandomU32(&play->rand, 0, 2);
                        PlaySound(audio, play->hits[index], 0, V2(0.1, 0.1));

                        play->ship_hole_count++;
						break;
					}
				}
			}
		}
	}

	if (!all_alive) { play->time_since_enemy += dt; }
}

function void UpdateRenderWaveList(f64 dt, Draw_Batch *batch, Wave_Layer *layers, u32 layer_count) {
	for(u32 i = 0; i < layer_count; i++) {
		Wave_Layer *layer = (Wave_Layer*)(layers+i);
		layer->angle += Degrees(layer->speed) * dt;
		v2 distance = V2(layer->radius, 0);
		v2 rotated = Rotate(distance, layer->angle);
		v3 render_pos = layer->position + V3(rotated.x, rotated.y, 0);
		DrawQuad(batch, layer->texture, render_pos, 12, 0);
	}
}

function void UpdateDroppedItems(Game_State *state, f64 dt, Draw_Batch *batch) {
	Image_Handle cannonball_texture = GetImageByName(&state->assets, "cannonball");
	Image_Handle spear_texture = GetImageByName(&state->assets, "spear");
	Image_Handle plank_texture = GetImageByName(&state->assets, "plank");
	Image_Handle bucket_texture = GetImageByName(&state->assets, "bucket_empty");
	Image_Handle fullbucket_texture = GetImageByName(&state->assets, "bucket_full");

    // @Todo: Wet item physics
    //
	f32 gravity = (2 * PLAYER_MAX_JUMP_HEIGHT) / (PLAYER_JUMP_APEX_TIME * PLAYER_JUMP_APEX_TIME);
    v2 ddp = V2(0, gravity);
	Mode_Play *play = &(state->play);
	for(u32 i = 0; i < ArraySize(play->droppedItems); i++){
		Dropped_Item *item = &(play->droppedItems[i]);
		if(item->active){
			item->hitbox.pos += (item->dp * dt);
			item->dp += (ddp * dt);
			for(u32 j = 0; j < play->hitbox_count; j++){
				AABB *hitbox = &(play->hitboxes[j]);
				u32 result = ResolveCollision(item->hitbox.pos,item->hitbox.dim, hitbox);
				b32 no_col_flag = result & ~AABB_Sides_collision;
				if (hitbox->flags & (Collision_Type_Normal | Collision_Type_Trap_Door)) {
					v2 half_dim = 0.5f * (hitbox->dim + item->hitbox.dim);
					switch(no_col_flag) {
						case AABB_Sides_bottomSide: {
							item->dp.y = 0;
							item->hitbox.pos.y = hitbox->pos.y - half_dim.y;
						}
						break;
						default: {} break;
					}
				}
			}
			Image_Handle imageToUse = cannonball_texture;
			switch(item->type){
				case Item_Bucket:
					imageToUse = bucket_texture;
					break;
				case Item_FullBucket:
					imageToUse = fullbucket_texture;
					break;
				case Item_Plank:
					imageToUse = plank_texture;
					break;
				case Item_Spear:
					imageToUse = spear_texture;
				default:
					break;
			}
			item->dp.x *= (1.0f / (1 + (ITEM_DAMPING * dt)));
			DrawQuad(batch, imageToUse, item->hitbox.pos, item->hitbox.dim, 0);
			if(item->hitbox.pos.y > 15){
				item->active = false;
			}
		}
	}
}

function void UpdateTentacle(Game_State *state, Input *input){
	Mode_Play *play = &(state->play);
	if(play->tentacleTimer < 0 && !play->tentacle.play_tentacle_entry){
		play->tentacle.dead = 0;
		play->tentacle.play_tentacle_entry = 1;
	}
	if(play->tentacle.play_tentacle_entry){
		UpdateAnimation(&play->tentacle.animEntry, input->delta_time);
		u32 frames = play->tentacle.animEntry.rows * play->tentacle.animEntry.cols;
		if(play->tentacle.animEntry.current_frame == frames) {
			play->tentacle.animEntry.current_frame = frames-1;
			play->tentacle.play_tentacle_entry = 0;
		}
	}
	if(play->tentacle.play_tentacle_death){
		UpdateAnimation(&play->tentacle.animDeath, input->delta_time);
		u32 frames = play->tentacle.animDeath.rows * play->tentacle.animDeath.cols;
		if(play->tentacle.animDeath.current_frame == frames) {
			play->tentacle.animDeath.current_frame = 0;
			play->tentacle.play_tentacle_entry = 0;
			play->tentacle.play_tentacle_death = 0;
			play->tentacle.animEntry.current_frame = 0;
			play->tentacleTimer = RandomF32(&play->rand, 90,120);
		}
	}
}

function void UpdateRenderModePlay(Game_State *state, Input *input, Renderer_Buffer *renderer_buffer) {
    Mode_Play *play = &(state->play);

    Player *player = &play->player;
	UpdateAnimation(&player->anim, input->delta_time);

	UpdateAnimation(&play->ship_mast_1, input->delta_time);
	UpdateAnimation(&play->ship_mast_2, input->delta_time);
	play->tentacleTimer-=input->delta_time;
	UpdateTentacle(state, input);
    Draw_Batch _batch = {};
    Draw_Batch *batch = &_batch;

    // Background
    // Clouds
    // Background wave
    // Ship background
    // Ship middle
    // Ship masts
    // Cannon, reserves, barrels etc.
    //
    // Ship water  [ship mask]
    // Ship front  [circle mask]
    // Ship holes  [not masked]
    // Front waves [circle mask]
    //
    // UI Enemy ships
    //
    Initialise(batch, &state->assets, renderer_buffer);

    SetCameraTransform(batch, 0, V3(1, 0, 0), V3(0, 1, 0), V3(0, 0, 1), V3(0, 0, 15));
    DrawClear(batch, V4(0, 1, 1, 1));

    Image_Handle background     = GetImageByName(&state->assets, "background");
    Image_Handle back_texture   = GetImageByName(&state->assets, "back_layer");
    Image_Handle middle_texture = GetImageByName(&state->assets, "middle_layer");
    Image_Handle front_texture  = GetImageByName(&state->assets, "front_layer");
	Image_Handle ladder_texture = GetImageByName(&state->assets, "ladder");
	Image_Handle cannon_stack_texture = GetImageByName(&state->assets, "cannonballs");
	Image_Handle spearBarrel_texture = GetImageByName(&state->assets, "spear_barrel");
	Image_Handle wood_barrel_texture = GetImageByName(&state->assets, "wood_barrel");

	UpdatePlayer(play, &(play->player), input, batch, state);

    if (play->player.p.y >= 10) {
        // If they jump off the boat
        //
        play->game_over = true;
    }

	UpdateShipHoles(state, input);
	DrawQuad(batch, background, V3(0,0,-0.5), 9.6, 0);

	UpdateRenderClouds(input->delta_time, batch, state);
	UpdateRenderWaveList(input->delta_time, batch, play->back_waves, play->back_wave_count);

    if (play->water_changed) {
        play->water_level += (play->water_dir * input->delta_time);

        if (play->water_dir < 0) {
            if (play->water_level < play->target_water_level) {
                play->water_level   = play->target_water_level;
                play->water_changed = false;
            }
        }
        else if (play->water_dir > 0) {
            if (play->water_level > play->target_water_level) {
                play->water_level   = play->target_water_level;
                play->water_changed = false;
            }
        }
    }

	DrawQuad(batch, back_texture,   V3(0, 0.2, 0), 9.3, 0);
	DrawQuad(batch, middle_texture, V3(0, 0.2, 0), 9.3, 0);

	if (play->play_cannon) {
		UpdateAnimation(&play->cannon_anim, input->delta_time);

		u32 frames = play->cannon_anim.rows * play->cannon_anim.cols;
		if(play->cannon_anim.current_frame == frames) {
			play->play_cannon = 0;

			for(u32 i = 0; i < 2; i++) {
				Enemy_Ship *enemy = &play->enemies[i];

				if ((enemy->health <= 0) && enemy->render_while_dead) {
					enemy->play_dead = 1;

					enemy->render_while_dead        = 0;
					enemy->death_anim.current_frame = 0;
				}
			}
		}
        else if (play->cannon_anim.current_frame == 42) {
            Sound_Handle sound = GetSoundByName(&state->assets, "cannon_bang");
            PlaySound(&state->audio_state, sound, 0, V2(0.1, 0.1));
        }
	}

	// ladders
	DrawQuad(batch, ladder_texture, play->hitboxes[3].pos,  V2(0.3, 1));
	DrawQuad(batch, ladder_texture, play->hitboxes[11].pos, V2(0.3, 1));
	DrawQuad(batch, ladder_texture, play->hitboxes[15].pos, V2(0.3, 1));

	// wood barrel
	DrawQuad(batch, wood_barrel_texture, play->hitboxes[24].pos, 0.4f, 0);


	// spear barrel
	DrawQuad(batch, spearBarrel_texture, play->hitboxes[25].pos, play->hitboxes[25].dim, 0);

	v2 player_dim = player->dim;
	if(player->flags & Player_Flipped) { player_dim.x = -player_dim.x; }

	DrawAnimation(batch, &play->ship_mast_1, V3(0.2, -1,   0), V2(3, 3));
	DrawAnimation(batch, &play->ship_mast_2, V3(2.2, -0.6, 0), V2(2, 2));

	v3 cannon_pos = V3(play->hitboxes[22].pos.x, play->hitboxes[22].pos.y, (f32)0.0);
	DrawAnimation(batch, &play->cannon_anim, cannon_pos + V3(-0.8, -0.5, 0), V2(3, 3));
	DrawQuad(batch, cannon_stack_texture, play->hitboxes[23].pos, 0.7f);
	if(!play->tentacle.play_tentacle_death){
    	DrawAnimation(batch, &play->tentacle.animEntry, V3(-3.51,0.7,0), V2(3.55, 3.3));
	}
	if(play->tentacle.play_tentacle_death){
		DrawAnimation(batch, &play->tentacle.animDeath, V3(-3.51,0.7,0), V2(3.55, 3.3));
	}

#define DRAW_HITBOXES 0
#if DRAW_HITBOXES
	for(u32 i = 0; i < play->hitbox_count; i++){
        AABB *hitbox = &play->hitboxes[i];
		DrawQuadOutline(batch, hitbox->pos, hitbox->dim, 0, hitbox->debugColour, 0.01f);
	}
#endif

	for(u32 i = 0; i < play->hitbox_count; i++) {
		AABB *hitbox = &play->hitboxes[i];

		if(!(hitbox->flags & Collision_Type_Trap_Door)) { continue; }

		b32 open = hitbox->flags & Collision_Type_Was_On_Ladder;
		f32 offsetX = 0;
		f32 offsetY = 0;

        Image_Handle handle;
		if (open) {
			handle = GetImageByName(&state->assets, "trapdoor_open");
			offsetY = 0.14;
			offsetX = 0.02;
		}
        else {
		    handle = GetImageByName(&state->assets, "trapdoor_closed");
        }

		DrawQuad(batch, handle, hitbox->pos - V2(offsetX, hitbox->dim.y+offsetY), 0.4);
	}
	UpdateDroppedItems(state, input->delta_time, batch);

	if(!(player->flags & Player_Idle)) {
		DrawAnimation(batch, &player->anim, player->p, player_dim);
	}
	else {
		Image_Handle handle = GetImageByName(&state->assets, "skipper_still");
		player->anim.current_frame = 0;
		DrawQuad(batch, handle, player->p, player_dim);
	}
	if(player->flags & Player_Holding) {
		f32 flip = player->flags & Player_Flipped ? -1.0: 1.0;
		switch(player->holdingFlags) {
			case Held_CannonBall: {
    			Image_Handle cannonball = GetImageByName(&state->assets, "cannonball");
				DrawQuad(batch, cannonball, player->p+V2(0.1*flip, 0), 0.2);
				break;
			}
			case Held_Spear: {
				Image_Handle spear = GetImageByName(&state->assets, "spear");
				DrawQuad(batch, spear, player->p+V2(0.1*flip, 0), 0.3,(flip > 0) ? 0 : 0.35);
				break;
			}
			case Held_Bucket: {
    			Image_Handle bucket = GetImageByName(&state->assets, "bucket_empty");
				DrawQuad(batch, bucket, player->p+V2(0.1*flip, 0), 0.2);
				break;
			}
			case Held_FullBucket: {
				Image_Handle full_bucket = GetImageByName(&state->assets, "bucket_full");
				DrawQuad(batch, full_bucket, player->p+V2(0.1*flip, 0), 0.2);
				break;
			}
			case Held_Plank: {
				Image_Handle plank = GetImageByName(&state->assets, "plank");
				DrawQuad(batch, plank, player->p+V2(0.1*flip, 0), 0.2);
				break;
			}
		}
	}

	UpdateRenderEnemyShip(input->delta_time, batch, play, &state->audio_state);

	if (player->mask_radius > 0.0f) {
		SetRenderTarget(batch, RenderTarget_Mask0);
        DrawClear(batch, V4(0, 0, 0, 0));

        DrawQuad(batch, front_texture,  V3(0, 0.2, 0), 9.3);

        SetMaskTarget(batch, RenderTarget_Mask0);

        SetRenderTarget(batch, RenderTarget_Default);
        RenderWaterLevel(batch, state);

		SetRenderTarget(batch, RenderTarget_Mask1);
		DrawClear(batch, V4(0, 0, 0, 0));

        // Fog of war circle
        //
		DrawCircle(batch, { 0 }, player->p, player->mask_radius);

        SetMaskTarget(batch, RenderTarget_Mask1, true);
	}

    SetRenderTarget(batch, RenderTarget_Default);

	DrawQuad(batch, front_texture, V3(0, 0.2, 0), 9.3);

    SetMaskTarget(batch, RenderTarget_Default);

    Image_Handle hole_texture = GetImageByName(&state->assets, "hole");
	for (u32 i = 0; i < MAX_SHIP_HOLES; i++) {
		Ship_Hole *hole = &play->ship_holes[i];
		if (!hole->active) { continue; }

		DrawQuad(batch, hole_texture, hole->position, 0.5, hole->rot);
	}

    SetMaskTarget(batch, RenderTarget_Mask1, true);
	UpdateRenderWaveList(input->delta_time, batch, play->front_waves, play->front_wave_count);
	SetMaskTarget(batch, RenderTarget_Default);
	Image_Handle filter_handle = GetImageByName(&state->assets, "filter");
	DrawQuad(batch, filter_handle, V2(0,0), 10);
	if(play->game_over) {
        StopSound(&state->audio_state, play->wind);
        play->wind = 0;

		ModeDeath(state);
		return;
	}
}

function void UpdatePlayer(Mode_Play *play, Player *player, Input *input, Draw_Batch *batch, Game_State *state) {
	f32 delta_time = input->delta_time;
	play->survive_time += delta_time;
	f32 gravity    = (2 * PLAYER_MAX_JUMP_HEIGHT) / (PLAYER_JUMP_APEX_TIME * PLAYER_JUMP_APEX_TIME);
	b32 on_ground  = (player->flags&Player_OnGround);

    f32 water_level = FindWaterLevel(play, batch);
    b32 under_water = player->p.y >= water_level;

    if (player->p.y > 0.23) {
        player->mask_radius += 8 * delta_time;
        player->mask_radius  = Min(player->mask_radius, 1.20);
    }
    else {
        player->mask_radius -= 8 * delta_time;
        player->mask_radius  = Max(player->mask_radius, 0.0);
    }

    f32 movement_multiplier = 1.0f;
	if (under_water) {
        movement_multiplier = 0.1f;
    }

    gravity *= movement_multiplier;
    v2 ddp = V2(0, gravity);

    // Attempt to jump. We will buffer this for an amount of time so if the player presses jump
    // slightly before hitting the ground it will still count
    //
    if (JustPressed(input->keys[Key_Space]) || JustPressed(input->keys[Key_K])) {
        player->last_jump_time = input->time;
		player->flags &= ~Player_On_Ladder;
		on_ground = 0;
    }

	b32 on_ladder  = player->flags & Player_On_Ladder;
	b32 up_or_down = IsPressed(input->keys[Key_S]) || IsPressed(input->keys[Key_W]);

    // Move left
    //
    if (JustPressed(input->keys[Key_A])) {
        ddp.x = (on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED) * movement_multiplier;
		player->flags |= Player_Flipped;
		player->flags &= ~Player_On_Ladder;
		player->flags &= ~Player_Idle;
    }
	else if(IsPressed(input->keys[Key_A]) && (!on_ladder || !up_or_down)) {
        ddp.x = (on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED) * movement_multiplier;
		player->flags |= Player_Flipped;
		player->flags &= ~Player_On_Ladder;
		player->flags &= ~Player_Idle;
	}

    // Move right
    //
	if(JustPressed(input->keys[Key_D])) {
        ddp.x = (on_ground ? -PLAYER_MOVE_SPEED : -PLAYER_AIR_STRAFE_SPEED) * movement_multiplier;
		player->flags &= ~Player_Flipped;
		player->flags &= ~Player_On_Ladder;
		player->flags &= ~Player_Idle;
	}
    else if (IsPressed(input->keys[Key_D]) && (!on_ladder || !up_or_down)) {
        ddp.x =(on_ground ? PLAYER_MOVE_SPEED : PLAYER_AIR_STRAFE_SPEED) * movement_multiplier;
		player->flags &= ~Player_Flipped;
		player->flags &= ~Player_On_Ladder;
		player->flags &= ~Player_Idle;
    }

	if(player->flags & Player_On_Ladder) {
		ddp.y = 0;
	}

    // If neither left or right were pressed apply damping to the player
    //
    if (IsZero(ddp.x)) {
        player->dp.x *= (1.0f / ((1 + movement_multiplier) + (PLAYER_DAMPING * delta_time)));
		player->flags |= Player_Idle;
    }

    if ((input->time - player->last_jump_time) <= PLAYER_JUMP_BUFFER_TIME) {
        b32 double_jump = (player->flags & Player_DoubleJump);

        if (on_ground || double_jump) {
            if (double_jump) {
                gravity = movement_multiplier * (2 * PLAYER_MAX_DOUBLE_JUMP_HEIGHT) / (PLAYER_JUMP_APEX_TIME * PLAYER_JUMP_APEX_TIME);

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

	if (JustPressed(input->keys[Key_E])) {
		switch (player->holdingFlags) {
			case Held_FullBucket: {
				player->holdingFlags = Held_Bucket;

				if (player->p.y > 0.23) {
					play->target_water_level = play->water_level + BUCKET_WATER_AMOUNT;
                    play->water_dir          = 1.0f;
                    play->water_changed      = true;
				}
            }
            break;
			case Held_Bucket: {
				if (player->p.y >= water_level) {
					player->holdingFlags = Held_FullBucket;

                    play->target_water_level = play->water_level - BUCKET_WATER_AMOUNT;
                    play->water_dir          = -1.0f;
                    play->water_changed      = true;
				}
            }
            break;
			default: {} break;
		}
	}

	for (u32 i = 0; i < ArraySize(play->droppedItems); i++) {
		Dropped_Item *item = &play->droppedItems[i];

		b32 result = ResolveCollision(player->p, player->dim, &item->hitbox);
		b32 colliding = result & AABB_Sides_collision;

		if (JustPressed(input->keys[Key_E]) && !(player->flags & Player_Holding) && colliding) {
			player->flags        |= Player_Holding;
			player->holdingFlags |= item->type;
			item->active          = 0;

			break;
		}
	}

	for (u32 i = 0; i < play->hitbox_count; i++) {
		AABB *hitbox = &play->hitboxes[i];

		b32 result      = ResolveCollision(player->p, player->dim, hitbox);
		b32 colliding   = result & AABB_Sides_collision;
		b32 no_col_flag = result & ~AABB_Sides_collision;

		if (!colliding && (hitbox->flags & Collision_Type_Trap_Door)) {
			hitbox->flags &= ~Collision_Type_Was_On_Ladder;
        }

		if (hitbox->flags & (Collision_Type_Normal | Collision_Type_Trap_Door)) {
			if (colliding && hitbox->flags & Collision_Type_Trap_Door && player->flags & Player_On_Ladder) {
				hitbox->flags |= Collision_Type_Was_On_Ladder;
				continue;
			}

			if (hitbox->flags & Collision_Type_Was_On_Ladder) { continue; }

            v2 half_dim = 0.5f * (hitbox->dim + player->dim);
			switch(no_col_flag) {
				case AABB_Sides_bottomSide: {
                    hitbox->debugColour = V4(0, 1, 0, 1);

					player->dp.y    = 0;
					player->p.y     = hitbox->pos.y - half_dim.y;
					player->flags  |= Player_OnGround;
					player->flags  &= ~Player_DoubleJump;
                }
                break;

				case AABB_Sides_leftSide: {
                    hitbox->debugColour = V4(0, 1, 0, 1);

					player->dp.x = 0;
					player->p.x  = hitbox->pos.x + half_dim.x;
                }
                break;
				case AABB_Sides_rightSide: {
                    hitbox->debugColour = V4(0, 1, 0, 1);

					player->dp.x = 0;
					player->p.x  = hitbox->pos.x - half_dim.x;
                }
                break;
				case AABB_Sides_topSide: {
                    hitbox->debugColour = V4(0, 1, 0, 1);

					player->dp.y = 1;
                    player->p.y  = hitbox->pos.y + half_dim.y;
                }
                break;

				default: {} break;
            }
        }

		if (colliding && (hitbox->flags & Collision_Type_Ladder)) {
			anyLadder = true;

			hitbox->debugColour = V4(0, 1, 0, 1);

			if(up_or_down && !(player->flags & Player_On_Ladder)) {
				player->p.x = hitbox->pos.x;
				player->dp.x = 0;
				player->dp.y = 0;
				player->flags |= Player_On_Ladder;
				player->flags |= Player_OnGround;
			}

			if(player->flags & Player_On_Ladder) {
				ddp.y = 0;

				if (IsPressed(input->keys[Key_W])) {
					f32 move_len = delta_time * PLAYER_LADDER_CLIMB_SPEED;
					if (player->p.y - move_len + player->dim.y / 2.0f > hitbox->pos.y - hitbox->dim.y / 2.0f){
						player->p.y -= move_len;
					}
				}
				else if (IsPressed(input->keys[Key_S])) {
					player->p.y += delta_time * PLAYER_LADDER_CLIMB_SPEED;
				}
			}
		}
		if(!colliding)
			continue;
		if(IsPressed(input->keys[Key_E])) {
			//check if any items can be picked up (-1 means no)
			s32 firstInactiveIndex = -1;
			for(s32 j = 0; j < ArraySize(play->droppedItems); j++){
				if(!play->droppedItems[j].active){
					firstInactiveIndex = j;
					break;
				}
			}

			switch(hitbox->flags) {
				case Collision_Type_Cannon: {
					if (!play->tentacle.play_tentacle_entry && player->holdingFlags & Held_Spear) {
						player->flags&= ~Player_Holding;
						player->holdingFlags&= ~Held_Spear;
						play->tentacle.play_tentacle_death = 1;
						play->tentacle.dead = 1;
						play->tentacleTimer = RandomF32(&play->rand, 90,120);
					}

					if (!(player->holdingFlags & Held_CannonBall)) { break; }
					if (!play->tentacle.dead) { break; }
					if (play->play_cannon) { break; }

					u32 enemy_index = play->enemies[0].health > play->enemies[1].health;
					if (play->enemies[enemy_index].health == 0) {
						enemy_index = 1 - enemy_index;
					}

					if (play->enemies[enemy_index].health != 0) {
						play->play_cannon               = 1;
						play->cannon_anim.current_frame = 0;

						player->flags        &= ~Player_Holding;
						player->holdingFlags &= ~Held_CannonBall;

						play->enemies[enemy_index].health--;
						play->enemies[enemy_index].render_while_dead = 1;

                        Sound_Handle sound = GetSoundByName(&state->assets, "clunk");
                        PlaySound(&state->audio_state, sound, 0, V2(0.3, 0.3));

						hitbox->debugColour = V4(0,0,1,1);
					}
					break;
				}
				case Collision_Type_Cannon_Hole:
					break;
				case Collision_Type_Cannon_Resource:
					if((!(player->flags & Player_Holding)) && firstInactiveIndex >= 0 ) {
						player->flags|=Player_Holding;
						player->holdingFlags|=Held_CannonBall;
					}
					break;
				case Collision_Type_Plank_Resource:
					if(!((player->flags & Player_Holding)) && firstInactiveIndex >= 0) {
						player->flags |= Player_Holding;
						player->holdingFlags |= Held_Plank;
					}
					break;
				case Collision_Type_Spears_Resource:
					if((!(player->flags & Player_Holding)) && firstInactiveIndex >= 0){
						player->flags |= Player_Holding;
						player->holdingFlags |= Held_Spear;
					}
					break;
				default:
					break;
			}
		}
	}

	if (IsPressed(input->keys[Key_Q]) && player->flags&Player_Holding) {
		s32 firstInactiveIndex = -1;
		for(s32 i = 0; i < ArraySize(play->droppedItems); i++){
			if(!play->droppedItems[i].active){
				firstInactiveIndex = i;
				break;
			}
		}
		if(firstInactiveIndex >= 0){
			play->droppedItems[firstInactiveIndex].type = player->holdingFlags;
			play->droppedItems[firstInactiveIndex].hitbox.dim = V2(0.2,0.2);
			play->droppedItems[firstInactiveIndex].hitbox.pos = player->p;
			play->droppedItems[firstInactiveIndex].hitbox.debugColour = V4(1,1,1,1);
			play->droppedItems[firstInactiveIndex].active = 1;
			play->droppedItems[firstInactiveIndex].dp.x = player->dp.x;

			player->holdingFlags = 0;
			player->flags&= ~Player_Holding;
		}
	}

	if (!anyLadder) { player->flags &= ~Player_On_Ladder; }
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
