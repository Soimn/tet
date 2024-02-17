#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

typedef struct Game_State
{
	u32 score;
	u32 top_score;
	u32 level;
	u32 lines;
	u8 board[BOARD_HEIGHT][BOARD_WIDTH];
} Game_State;

void
BlitGame(Game_State* state)
{
		V2S board_origin = V2S(8, 4);
		V2S board_dim    = V2S(BOARD_WIDTH, BOARD_HEIGHT);
		BlitFrame(V2S_Sub(board_origin, V2S(0, 3)), V2S(board_dim.x, 1));
		BlitString(V2S_Add(board_origin, V2S(2, -2)), "LINES-");
		BlitInt(V2S_Add(board_origin, V2S(2 + sizeof("LINES-")-1, -2)), state->lines, 3);
		BlitFrame(board_origin, board_dim);

		V2S score_origin = V2S_Add(board_origin, V2S(board_dim.x+2, -2));
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
				BlitSprite(V2S_Add(board_origin, V2S(1 + i, 1 + j)), V2S(sprite_x, 4));
			}
		}
}

void
Init(Game_State* state)
{
	*state = (Game_State){
		.score     = 0,
		.top_score = 0,
		.level     = 1,
		.lines     = 0,
		.board     = {1},
	};
}

void
Tick(Game_State* state)
{
	for (u8 i = 0, carry = state->board[BOARD_HEIGHT-1][BOARD_WIDTH-1]; i < BOARD_WIDTH*BOARD_HEIGHT; ++i)
	{
		u8 new_carry = ((u8*)state->board)[i];
		((u8*)state->board)[i] = carry;
		carry = new_carry;
	}

	BlitGame(state);
}
