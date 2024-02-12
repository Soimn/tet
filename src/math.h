typedef union F32_Bits
{
	f32 f;
	u32 bits;
} F32_Bits;

f32
Sqrt(f32 n)
{
	return (F32_Bits){ .bits = _mm_extract_ps(_mm_sqrt_ps(_mm_set1_ps(n)), 0) }.f;
}

typedef struct V2
{
	f32 x;
	f32 y;
} V2;

#define V2(X, Y) (V2){ .x = (f32)(X), .y = (f32)(Y) }

V2 V2_Scale(V2 v, f32 n) { return V2(v.x*n, v.y*n); }

V2 V2_Add     (V2 a, V2 b) { return V2(a.x + b.x, a.y + b.y); }
V2 V2_Sub     (V2 a, V2 b) { return V2(a.x - b.x, a.y - b.y); }
V2 V2_Hadamard(V2 a, V2 b) { return V2(a.x * b.x, a.y * b.y); }

f32
V2_Length(V2 v)
{
	return Sqrt(v.x*v.x + v.y*v.y);
}

typedef struct V2S
{
	s32 x;
	s32 y;
} V2S;

#define V2S(X, Y) (V2S){ .x = (s32)(X), .y = (s32)(Y) }

V2S V2S_Scale   (V2S v, s32 n) { return V2S(v.x*n, v.y*n); }
V2S V2S_InvScale(V2S v, s32 n) { return V2S(v.x/n, v.y/n); }

V2S V2S_Add     (V2S a, V2S b) { return V2S(a.x + b.x, a.y + b.y); }
V2S V2S_Sub     (V2S a, V2S b) { return V2S(a.x - b.x, a.y - b.y); }
V2S V2S_Hadamard(V2S a, V2S b) { return V2S(a.x * b.x, a.y * b.y); }
