#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BOARD_DIM V2S(BOARD_WIDTH, BOARD_HEIGHT)
#define BOARD_X 8
#define BOARD_Y 4
#define BOARD_ORIGIN V2S(BOARD_X, BOARD_Y)
#define TET_SPAWN_X 3
#define TET_SPAWN_Y -4
#define TET_SPAWN V2S(TET_SPAWN_X, TET_SPAWN_Y)
#define TET_WIDTH 4
#define TET_HEIGHT 4

typedef enum Action_Kind
{
	Action_None = 0,

	Action_HardDrop,
	Action_SoftDrop,
	Action_MoveLeft,
	Action_MoveRight,
	Action_RotateCW,
	Action_RotateCCW,
} Action_Kind;

typedef enum State_Kind
{
	State_Spawning,
	State_Dropping,
	State_Clearing,
	State_Lost,
} State_Kind;

typedef enum Tetromino_Kind
{
	Tet_T,
	Tet_J,
	Tet_Z,
	Tet_O,
	Tet_S,
	Tet_L,
	Tet_I,
	TETROMINO_COUNT
} Tetromino_Kind;

typedef struct Game_State
{
	State_Kind state;
	u32 score;
	u32 top_score;
	u32 level;
	u32 lines;
	u32 lcg_state;
	Tetromino_Kind next_kind;
	u8 clear_vector;
	u8 drop_timer;
	u8 clear_timer;
	u8 live_tet_kind;
	u8 live_tet_orient;
	V2S live_tet_pos;
	u8 board[BOARD_HEIGHT][BOARD_WIDTH];
} Game_State;

u8 Tetrominos[TETROMINO_COUNT][4][TET_HEIGHT][TET_WIDTH] = {
	[Tet_T] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 2, 2, 2,
			0, 0, 2, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 2, 0,
			0, 2, 2, 0,
			0, 0, 2, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 2, 0,
			0, 2, 2, 2,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 2, 0,
			0, 0, 2, 2,
			0, 0, 2, 0,
		},
	},

	[Tet_J] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 1, 1, 1,
			0, 0, 0, 1,
		},
		{
			0, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 1, 0,
			0, 1, 1, 0,
		},
		{
			0, 0, 0, 0,
			0, 1, 0, 0,
			0, 1, 1, 1,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 1, 1,
			0, 0, 1, 0,
			0, 0, 1, 0,
		},
	},

	[Tet_Z] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 3, 3, 0,
			0, 0, 3, 3,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 3,
			0, 0, 3, 3,
			0, 0, 3, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 3, 3, 0,
			0, 0, 3, 3,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 3,
			0, 0, 3, 3,
			0, 0, 3, 0,
		},
	},

	[Tet_O] = {
		{
			0, 0, 0, 0,
			0, 2, 2, 0,
			0, 2, 2, 0,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 2, 2, 0,
			0, 2, 2, 0,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 2, 2, 0,
			0, 2, 2, 0,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 2, 2, 0,
			0, 2, 2, 0,
			0, 0, 0, 0,
		},
	},

	[Tet_S] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 1, 1,
			0, 1, 1, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 1, 1,
			0, 0, 0, 1,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 0, 1, 1,
			0, 1, 1, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 1, 0,
			0, 0, 1, 1,
			0, 0, 0, 1,
		},
	},

	[Tet_L] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			0, 3, 3, 3,
			0, 3, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 3, 3, 0,
			0, 0, 3, 0,
			0, 0, 3, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 3,
			0, 3, 3, 3,
			0, 0, 0, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 3, 0,
			0, 0, 3, 0,
			0, 0, 3, 3,
		},
	},

	[Tet_I] = {
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			2, 2, 2, 2,
			0, 0, 0, 0,
		},
		{
			0, 0, 2, 0,
			0, 0, 2, 0,
			0, 0, 2, 0,
			0, 0, 2, 0,
		},
		{
			0, 0, 0, 0,
			0, 0, 0, 0,
			2, 2, 2, 2,
			0, 0, 0, 0,
		},
		{
			0, 0, 2, 0,
			0, 0, 2, 0,
			0, 0, 2, 0,
			0, 0, 2, 0,
		},
	},
};

u8 TetrominoFirstNonEmptyLine[TETROMINO_COUNT][4] = {
	[Tet_T] = { 2, 1, 1, 1 },
	[Tet_J] = { 2, 1, 1, 1 },
	[Tet_Z] = { 2, 1, 2, 1 },
	[Tet_O] = { 1, 1, 1, 1 },
	[Tet_S] = { 2, 1, 2, 1 },
	[Tet_L] = { 2, 1, 1, 1 },
	[Tet_I] = { 2, 0, 2, 0 },
};

void
BlitTet(V2S pos, Tetromino_Kind kind, u8 orientation)
{
	for (umm j = 0; j < TET_HEIGHT; ++j)
	{
		for (umm i = 0; i < TET_WIDTH; ++i)
		{
			u8 tet_val = Tetrominos[kind][orientation][j][i];
			if (tet_val == 0) continue;
			else
			{
				u8 sprite = tet_val - 1;

				V2S cell_pos = V2S_Add(pos, V2S(i, j));

				if (cell_pos.y <= BOARD_Y) continue;
				else
				{
					BlitSprite(cell_pos, V2S(5 + sprite, 4));
				}
			}
		}
	}
}

void
BlitGame(Game_State* state)
{
		BlitFrame(V2S_Sub(BOARD_ORIGIN, V2S(0, 3)), V2S(BOARD_WIDTH, 1));
		BlitString(V2S_Add(BOARD_ORIGIN, V2S(2, -2)), "LINES-");
		BlitInt(V2S_Add(BOARD_ORIGIN, V2S(2 + sizeof("LINES-")-1, -2)), state->lines, 3);
		BlitFrame(BOARD_ORIGIN, BOARD_DIM);

		V2S score_origin = V2S_Add(BOARD_ORIGIN, V2S(BOARD_WIDTH+2, -2));
		V2S score_dim    = V2S(6, 7);
		BlitFrame(score_origin, score_dim);
		BlitString(V2S_Add(score_origin, V2S(1, 2)), "TOP");
		BlitInt(V2S_Add(score_origin, V2S(1, 3)), state->top_score, 6);
		BlitString(V2S_Add(score_origin, V2S(1, 5)), "SCORE");
		BlitInt(V2S_Add(score_origin, V2S(1, 6)), state->score, 6);

		V2S next_origin = V2S_Add(score_origin, V2S(0, score_dim.y+3));
		V2S next_dim    = V2S(6, 5);
		BlitFrame(next_origin, next_dim);
		BlitString(V2S_Add(next_origin, V2S(2, 1)), "NEXT");
		BlitTet(V2S_Add(next_origin, V2S(2, 2)), state->next_kind, 0);

		V2S level_origin = V2S_Add(next_origin, V2S(0, next_dim.y+3));
		BlitFrame(level_origin, V2S(6, 2));
		BlitString(V2S_Add(level_origin, V2S(1, 1)), "LEVEL");
		BlitInt(V2S_Add(level_origin, V2S(3, 2)), state->level, 2);

		for (umm j = 0; j < BOARD_HEIGHT; ++j)
		{
			for (umm i = 0; i < BOARD_WIDTH; ++i)
			{
				if (state->board[j][i] == 0) continue;

				umm sprite_x = 5 + (umm)(state->board[j][i] - 1);
				ASSERT(sprite_x < SPRITESHEET_WIDTH);
				BlitSprite(V2S_Add(BOARD_ORIGIN, V2S(1 + i, 1 + j)), V2S(sprite_x, 4));
			}
		}
}

u32
RandomU32(Game_State* state)
{
	state->lcg_state *= 1664525;
	state->lcg_state += 1013904223;
	return state->lcg_state;
}

void
Init(Game_State* state)
{
	*state = (Game_State){
		.state            = State_Spawning,
		.clear_vector     = 0,
		.score            = 0,
		.top_score        = 0,
		.level            = 1,
		.lines            = 0,
		.lcg_state        = 1337,
		.next_kind        = 0,
		.drop_timer       = 0,
		.clear_timer      = 0,
		.live_tet_kind    = 0,
		.live_tet_orient  = 0,
		.live_tet_pos     = TET_SPAWN,
		.board            = {0},
	};

	state->next_kind = RandomU32(state) % TETROMINO_COUNT;
}

void
Tick(Game_State* state, Action_Kind input)
{
	bool should_flash = false;

	if (state->state == State_Spawning)
	{
		state->live_tet_kind   = state->next_kind;
		state->live_tet_orient = 0;
		state->live_tet_pos    = TET_SPAWN;

		state->next_kind = RandomU32(state) % TETROMINO_COUNT;

		state->state = State_Dropping;
	}
	else if (state->state == State_Clearing)
	{
		if (state->clear_timer >= BOARD_WIDTH)
		{
			state->lines += __popcnt(state->clear_vector);

			for (umm move_dst = state->live_tet_pos.y;;)
			{
				while ((state->clear_vector&1) == 0)
				{
					state->clear_vector >>= 1;
					++move_dst;
				}

				u8 move = __popcnt(state->clear_vector & ~(state->clear_vector+1));

				ASSERT(move != 0);
				ASSERT(move_dst >= state->live_tet_pos.y && move_dst < state->live_tet_pos.y + TET_HEIGHT);

				for (smm j = move_dst-1; j >= 0; --j)
				{
					for (umm i = 0; i < BOARD_WIDTH; ++i)
					{
						state->board[j+move][i] = state->board[j][i];
					}
				}

				move_dst += move;

				state->clear_vector >>= move;

				if (state->clear_vector == 0) break;
				else                          continue;
			}

			state->clear_timer  = 0;
			state->clear_vector = 0;

			state->state = State_Spawning;
		}
		else
		{
			ASSERT(BOARD_WIDTH % 2 == 0 && state->clear_timer < BOARD_WIDTH);
			for (umm i = 0; i < TET_HEIGHT; ++i)
			{
				if ((state->clear_vector & (1 << i)) != 0)
				{
					state->board[state->live_tet_pos.y+i][(BOARD_WIDTH/2-1) - state->clear_timer/2] = 0;
					state->board[state->live_tet_pos.y+i][BOARD_WIDTH/2     + state->clear_timer/2] = 0;
				}
			}

			if (state->clear_vector == 0xF && state->clear_timer % 4 == 0) should_flash = true;
			state->clear_timer += 1;
		}
	}
	else if (state->state == State_Dropping)
	{
		V2S tet_move  = V2S(0, 0);
		u8 tet_orient = state->live_tet_orient;

		if (state->drop_timer++ == 32)
		{
			state->drop_timer = 0;

			tet_move.y = 1;
		}

		switch (input)
		{
			case Action_HardDrop:
			{
				state->live_tet_kind = (state->live_tet_kind+1)%TETROMINO_COUNT;
			} break;

			case Action_SoftDrop:  tet_move.y =  1;                           break;
			case Action_MoveLeft:  tet_move.x = -1;                           break;
			case Action_MoveRight: tet_move.x =  1;                           break;
			case Action_RotateCW:  tet_orient = (state->live_tet_orient+1)%4; break;
			case Action_RotateCCW: tet_orient = (state->live_tet_orient+3)%4; break;
		}

		bool orient_changed = false;
		if (tet_orient != state->live_tet_orient)
		{
			for (umm j = 0; j < TET_HEIGHT; ++j)
			{
				for (umm i = 0; i < TET_WIDTH; ++i)
				{
					if (Tetrominos[state->live_tet_kind][tet_orient][j][i] == 0) continue;
					else
					{
						V2S board_pos = V2S_Add(state->live_tet_pos, V2S(i, j));
						if (board_pos.x < 0 || board_pos.x >= BOARD_WIDTH || board_pos.y < 0 ||
								state->board[board_pos.y][board_pos.x] != 0)
						{
							goto orient_invalid;
						}
					}
				}
			}

			state->live_tet_orient = tet_orient;
			orient_changed         = true;
	orient_invalid:;
		}

		bool should_lock = false;
		if (tet_move.x != 0 || tet_move.y != 0)
		{
			bool move_x_valid = true;
			for (umm j = 0; j < TET_HEIGHT; ++j)
			{
				for (umm i = 0; i < TET_WIDTH; ++i)
				{
					if (Tetrominos[state->live_tet_kind][state->live_tet_orient][j][i] == 0) continue;
					else
					{
						V2S board_pos = V2S_Add(state->live_tet_pos, V2S(i, j));

						if (board_pos.x + tet_move.x < 0 || board_pos.x + tet_move.x >= BOARD_WIDTH ||
								board_pos.y >= 0 && state->board[board_pos.y][board_pos.x + tet_move.x] != 0)
						{
							move_x_valid = false;
						}
					}
				}
			}

			if (move_x_valid) state->live_tet_pos.x += tet_move.x;

			bool move_y_valid = true;
			for (umm j = 0; j < TET_HEIGHT; ++j)
			{
				for (umm i = 0; i < TET_WIDTH; ++i)
				{
					if (Tetrominos[state->live_tet_kind][state->live_tet_orient][j][i] == 0) continue;
					else
					{
						V2S board_pos = V2S_Add(state->live_tet_pos, V2S(i, j));

						if (board_pos.y + tet_move.y >= BOARD_HEIGHT ||
								board_pos.y + tet_move.y >= 0 && state->board[board_pos.y + tet_move.y][board_pos.x] != 0)
						{
							move_y_valid = false;
						}
					}
				}
			}

			if (move_y_valid) state->live_tet_pos.y += tet_move.y;
			else              should_lock = (tet_move.y != 0);
		}

		if (should_lock)
		{
			if (state->live_tet_pos.y + TetrominoFirstNonEmptyLine[state->live_tet_kind][state->live_tet_orient] < 0)
			{
				state->state = State_Lost;
			}
			else
			{
				state->clear_vector = 0;
				for (umm j = 0; j < TET_HEIGHT; ++j)
				{
					if (state->live_tet_pos.y+j < 0) continue;
					else
					{
						for (umm i = 0; i < TET_WIDTH; ++i)
						{
							state->board[state->live_tet_pos.y+j][state->live_tet_pos.x+i] |= Tetrominos[state->live_tet_kind][state->live_tet_orient][j][i];
						}

						if (state->live_tet_pos.y+j < BOARD_HEIGHT)
						{
							bool line_filled = true;
							for (umm i = 0; i < BOARD_WIDTH; ++i)
							{
								if (state->board[state->live_tet_pos.y+j][i] == 0)
								{
									line_filled = false;
								}
							}

							if (line_filled) state->clear_vector |= (1 << j);
						}
					}
				}
			}

			state->drop_timer = 0;

			if (state->clear_vector != 0) state->state = State_Clearing;
			else                          state->state = State_Spawning;
		}
	}

	BlitGame(state);

	if (state->state == State_Dropping)
	{
		BlitTet(V2S_Add(V2S(BOARD_X+1, BOARD_Y+1), state->live_tet_pos), state->live_tet_kind, state->live_tet_orient);
	}

	if (should_flash)
	{
		for (umm j = 0; j < BOARD_HEIGHT; ++j)
		{
			for (umm i = 0; i < BOARD_WIDTH; ++i)
			{
				BlitSprite(V2S_Add(BOARD_ORIGIN, V2S(1 + i, 1 + j)), V2S(6, 4));
			}
		}
	}

	if (state->state == State_Lost)
	{
		BlitString(V2S_Add(BOARD_ORIGIN, V2S(BOARD_WIDTH/2-3,BOARD_HEIGHT/2-3)), "YOU LOST");
	}
}
