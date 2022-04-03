function void AddHitBox(
	Mode_Play *play,
	v2 pos,
	v2 dim,
	u32 flags
   	) {
	play->hitboxes[play->hitbox_count].pos = pos;
	play->hitboxes[play->hitbox_count].dim = dim;
	play->hitboxes[play->hitbox_count].flags = flags;
	play->hitboxes[play->hitbox_count].debugColour = V4(1,1,1,1);
	play->hitbox_count = play->hitbox_count+1;
}
function void BuildWorldHitboxes(Mode_Play *play) {
	// above deck floor left
	AddHitBox(
		play,
	 	V2(-1 ,0.38),
		V2(1.8f, 0.1),
		Collision_Type_Normal
	);
	// above deck floor right
	AddHitBox(
		play,
	 	V2(1.8,0.38f),
		V2(2.9f, 0.1f),
		Collision_Type_Normal
	);
	// above deck floor trap door
	AddHitBox(
		play,
	 	V2(0.125,0.38f),
		V2(0.45, 0.1f),
		Collision_Type_Trap_Door
	);
	// private cabin floor
	AddHitBox(
		play,
	 	V2(-2.8,0),
		V2(1.4, 0.1),
		Collision_Type_Normal
	);
	// left wall above deck
	AddHitBox(
		play,
	 	V2(-3.4f,0.4),
		V2(0.3, 1),
		Collision_Type_Normal
	);
	// top deck floor
	AddHitBox(
		play,
	 	V2(0.1f,1),
		V2(6.1, 0.1),
		Collision_Type_Normal
	);
	// middle deck floor
	AddHitBox(
		play,
	 	V2(0,1.7),
		V2(5.8, 0.1),
		Collision_Type_Normal
	);
}
