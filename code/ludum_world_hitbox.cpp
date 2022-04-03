function AABB* AddHitBox(
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
	return &(play->hitboxes[play->hitbox_count-1]);
}
function void BuildWorldHitboxes(Mode_Play *play) {
	// above deck floor left
	AddHitBox(
		play,
	 	V2(-1.3 ,0.38),
		V2(1.2f, 0.1),
		Collision_Type_Normal
	);
	// above deck floor right
	AddHitBox(
		play,
	 	V2(1.45,0.38f),
		V2(3.4f, 0.1f),
		Collision_Type_Normal
	);
	// above deck floor trap door
	AABB *trap_door = AddHitBox(
		play,
	 	V2(-0.5,0.38f),
		V2(0.45, 0.1f),
		Collision_Type_Trap_Door
	);
	play->trap_doors[0] = trap_door;
	// First ladder
	AddHitBox(
		play,
	 	V2(-0.5, 0.58f),
		V2(0.2, 0.6f),
		Collision_Type_Ladder
	);
	// private cabin floor
	AddHitBox(
		play,
	 	V2(-2.8,0),
		V2(1.4, 0.1),
		Collision_Type_Normal
	);
	// private cabin stairs
	AddHitBox(
		play,
	 	V2(-1.9, 0.3),
		V2(0.2, 0.2),
		Collision_Type_Normal
	);
	// private cabin stairs
	AddHitBox(
		play,
	 	V2(-2.1, 0.15),
		V2(0.2, 0.2),
		Collision_Type_Normal
	);
	// left wall above deck
	AddHitBox(
		play,
	 	V2(-3.4f,0.4),
		V2(0.3, 1),
		Collision_Type_Normal
	);
	// top deck floor left
	AddHitBox(
		play,
	 	V2(-1,1),
		V2(4.5, 0.1),
		Collision_Type_Normal
	);
	// top deck floor right
	AddHitBox(
		play,
	 	V2(2.6,1),
		V2(1.6, 0.1),
		Collision_Type_Normal
	);
	// above deck floor trap door
	trap_door = AddHitBox(
		play,
	 	V2(1.6, 1),
		V2(0.45, 0.1f),
		Collision_Type_Trap_Door
	);
	play->trap_doors[1] = trap_door;
	// second ladder
	AddHitBox(
		play,
	 	V2(1.6, 1.2f),
		V2(0.2, 0.6f),
		Collision_Type_Ladder
	);
	// middle deck floor left
	AddHitBox(
		play,
	 	V2(-1.8 ,1.7),
		V2(1.8, 0.1),
		Collision_Type_Normal
	);
	// middle deck floor trap door
	trap_door = AddHitBox(
		play,
	 	V2(-0.7, 1.7),
		V2(0.45, 0.1f),
		Collision_Type_Trap_Door
	);
	play->trap_doors[2] = trap_door;
	// middle deck floor right
	AddHitBox(
		play,
	 	V2(1.4 ,1.7),
		V2(3.7, 0.1),
		Collision_Type_Normal
	);
	// third ladder
	AddHitBox(
		play,
	 	V2(-0.7, 1.9f),
		V2(0.2, 0.6f),
		Collision_Type_Ladder
	);
	// bottom deck floor
	AddHitBox(
		play,
	 	V2(0, 2.4),
		V2(5.6, 0.1),
		Collision_Type_Normal
	);
	// Left wall
	AddHitBox(
		play,
	 	V2(-2.8, 1.7),
		V2(0.2, 1.5),
		Collision_Type_Normal
	);
	// Right wall
	AddHitBox(
		play,
	 	V2(3.1, 1.7),
		V2(0.2, 1.5),
		Collision_Type_Normal
	);
	// Right wall
	AddHitBox(
		play,
	 	V2(2.3, 2.2),
		V2(0.2, 1.5),
		Collision_Type_Normal
	);
}
