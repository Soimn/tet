#define UNICODE
#define NOMINMAX            1
#define WIN32_LEAN_AND_MEAN 1
#define WIN32_MEAN_AND_LEAN 1
#define VC_EXTRALEAN        1
#include <windows.h>
#include <timeapi.h>
#undef NOMINMAX
#undef WIN32_LEAN_AND_MEAN
#undef WIN32_MEAN_AND_LEAN
#undef VC_EXTRALEAN
#undef far
#undef near

#include <intrin.h>

typedef signed __int8  s8;
typedef signed __int16 s16;
typedef signed __int32 s32;
typedef signed __int64 s64;

#define S8_MIN  ((s8) 0x80)
#define S16_MIN ((s16)0x8000)
#define S32_MIN ((s32)0x80000000)
#define S64_MIN ((s64)0x8000000000000000DLL)

#define S8_MAX  ((s8) 0x7F)
#define S16_MAX ((s16)0x7FFF)
#define S32_MAX ((s32)0x7FFFFFFF)
#define S64_MAX ((s64)0x7FFFFFFFFFFFFFFFDLL)

typedef unsigned __int8  u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

#define U8_MAX  ((u8) 0xFF)
#define U16_MAX ((u16)0xFFFF)
#define U32_MAX ((u32)0xFFFFFFFF)
#define U64_MAX ((u64)0xFFFFFFFFFFFFFFFFULL)

typedef s64 smm;
typedef u64 umm;

#define SMM_MIN ((smm)S64_MIN)
#define SMM_MAX ((smm)S64_MAX)
#define UMM_MAX ((umm)U64_MAX)

typedef u8 bool;
#define true 1
#define false 0

typedef float f32;
typedef double f64;

#define ASSERT(EX) ((EX) ? 1 : (__debugbreak(), *(volatile int*)0 = 0))
#define NOT_IMPLEMENTED ASSERT(!"NOT_IMPLEMENTED")

#define ARRAY_SIZE(A) (sizeof(A)/sizeof(0[A]))

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define NORMAL_WINDOW_STYLES (WS_OVERLAPPEDWINDOW | WS_VISIBLE)
#define FULLSCREEN_WINDOW_STYLES (WS_VISIBLE)

#define SCREEN_WIDTH 256
#define SCREEN_HEIGHT 224
#define CELL_SIZE 8
#define SCREEN_GRID_X 32
#define SCREEN_GRID_Y 28

#define SPRITESHEET_WIDTH 8
#define SPRITESHEET_HEIGHT 6
#define SPRITESHEET_CELL_SIZE 8

#include "math.h"

struct
{
  HWND window;

  bool is_fullscreen;
  HMONITOR monitor;
  MONITORINFO monitor_info;
  WINDOWPLACEMENT normal_placement;

	V2S scaled_screen_origin;
	V2S scaled_screen_dim;
	u32 backbuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
	u32 spritesheet[SPRITESHEET_HEIGHT*SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE*SPRITESHEET_CELL_SIZE];
} PlatformState = {0};

void
RefitToMonitor()
{
  PlatformState.monitor = MonitorFromWindow(PlatformState.window, MONITOR_DEFAULTTOPRIMARY);

  PlatformState.monitor_info = (MONITORINFO){ .cbSize = sizeof(MONITORINFO) };
  BOOL monitor_info_ok = GetMonitorInfoW(PlatformState.monitor, &PlatformState.monitor_info);
  ASSERT(monitor_info_ok); // TODO

  V2S monitor_res = V2S(PlatformState.monitor_info.rcMonitor.right  - PlatformState.monitor_info.rcMonitor.left,
                        PlatformState.monitor_info.rcMonitor.bottom - PlatformState.monitor_info.rcMonitor.top);

  if (PlatformState.is_fullscreen)
  {
    SetWindowLongW(PlatformState.window, GWL_STYLE, FULLSCREEN_WINDOW_STYLES);
    SetWindowPos(PlatformState.window, HWND_TOP, PlatformState.monitor_info.rcMonitor.left,
                 PlatformState.monitor_info.rcMonitor.top, monitor_res.x, monitor_res.y,
                 SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }
  else
  {
    SetWindowLongW(PlatformState.window, GWL_STYLE, NORMAL_WINDOW_STYLES);
    SetWindowPos(PlatformState.window, 0, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
  }

	RECT client_rect;
	GetClientRect(PlatformState.window, &client_rect);
	V2S client_dim = V2S(client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);

	s32 scaled_screen_x;
	s32 scaled_screen_y;
	if (client_dim.x < client_dim.y)
	{
		scaled_screen_x = client_dim.x;
		scaled_screen_y = (SCREEN_HEIGHT*client_dim.x)/SCREEN_WIDTH;
	}
	else
	{
		scaled_screen_x = (SCREEN_WIDTH*client_dim.y)/SCREEN_HEIGHT;
		scaled_screen_y = client_dim.y;
	}

	PlatformState.scaled_screen_origin = V2S_InvScale(V2S_Sub(client_dim, V2S(scaled_screen_x, scaled_screen_y)), 2);
	PlatformState.scaled_screen_dim    = V2S(scaled_screen_x, scaled_screen_y);
}

void
ToggleFullscreen()
{
  if (PlatformState.is_fullscreen)
  {
    PlatformState.is_fullscreen = false;

    SetWindowPlacement(PlatformState.window, &PlatformState.normal_placement);
  }
  else
  {
    PlatformState.normal_placement = (WINDOWPLACEMENT){ .length = sizeof(WINDOWPLACEMENT) };
    GetWindowPlacement(PlatformState.window, &PlatformState.normal_placement);

    PlatformState.is_fullscreen = true;
  }

  RefitToMonitor();
}

LRESULT
Wndproc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
  LRESULT result;

  if (msg == WM_CLOSE)
  {
    DestroyWindow(window);
    PostQuitMessage(0);
    result = 0;
  }
  else if (msg == WM_SIZE || msg == WM_EXITSIZEMOVE)
  {
    RefitToMonitor();
		InvalidateRect(PlatformState.window, 0, TRUE);
    result = 0;
  }
  else if (msg == WM_PAINT)
  {
    PAINTSTRUCT paint = {0};
    HDC dc = BeginPaint(window, &paint);

    // TODO

    EndPaint(window, &paint);
    result = 0;
  }
  else result = DefWindowProcW(window, msg, wparam, lparam);

  return result;
}

void
BlitSprite(V2S pos, V2S sprite)
{
	ASSERT(pos.x >= 0 && pos.x < SCREEN_WIDTH/SPRITESHEET_CELL_SIZE);
	ASSERT(pos.y >= 0 && pos.y < SCREEN_WIDTH/SPRITESHEET_CELL_SIZE);
	ASSERT(sprite.x >= 0 && sprite.x < SPRITESHEET_WIDTH);
	ASSERT(sprite.y >= 0 && sprite.y < SPRITESHEET_HEIGHT);

	for (umm j = 0; j < SPRITESHEET_CELL_SIZE; ++j)
	{
		for (umm i = 0; i < SPRITESHEET_CELL_SIZE; ++i)
		{
			u32 color = PlatformState.spritesheet[(sprite.y*SPRITESHEET_CELL_SIZE + j)*SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE + (sprite.x*SPRITESHEET_CELL_SIZE + i)];

			PlatformState.backbuffer[(pos.y*SPRITESHEET_CELL_SIZE + j)*SCREEN_WIDTH + (pos.x*SPRITESHEET_CELL_SIZE + i)] = color;
		}
	}
}

void
BlitChar(V2S pos, u8 c)
{
	ASSERT(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '-');

	umm index = ((u8)(c - 0x30) < 10 ? 26 + (c-0x30) : (c == '-' ? 36 : ((c&0x1F) - 1)));

	BlitSprite(pos, V2S(index%SPRITESHEET_WIDTH, index/SPRITESHEET_WIDTH));
}

void
BlitFrame(V2S origin, V2S dim)
{
	BlitSprite(V2S(origin.x +       0, origin.y +       0), V2S(1, 5));
	BlitSprite(V2S(origin.x + dim.x+1, origin.y +       0), V2S(3, 5));
	BlitSprite(V2S(origin.x +       0, origin.y + dim.y+1), V2S(0, 5));
	BlitSprite(V2S(origin.x + dim.x+1, origin.y + dim.y+1), V2S(2, 5));

	for (umm i = 1; i < dim.x+1; ++i)
	{
		BlitSprite(V2S(origin.x + i, origin.y +       0), V2S(7, 5));
		BlitSprite(V2S(origin.x + i, origin.y + dim.y+1), V2S(5, 5));
	}

	for (umm i = 1; i < dim.y+1; ++i)
	{
		BlitSprite(V2S(origin.x +       0, origin.y + i), V2S(4, 5));
		BlitSprite(V2S(origin.x + dim.x+1, origin.y + i), V2S(6, 5));
	}
}

void
BlitString(V2S origin, char* string)
{
	for (umm i = 0; string[i] != 0; ++i)
	{
		if (string[i] == ' ') continue;
		BlitChar(V2S(origin.x + i, origin.y), string[i]);
	}
}

void
BlitInt(V2S origin, umm integer, umm length)
{
	for (umm i = length-1; i < length; --i)
	{
		BlitChar(V2S(origin.x + i, origin.y), '0' + (integer%10));
		integer /= 10;
	}
}

#include "tet.c"

int
wWinMain(HINSTANCE instance, HINSTANCE pre_instance, PWSTR cmd_line, int show_cmd)
{
  int exit_code = 0;

	{
		HANDLE bitmap = LoadImageW(0, L"spritesheet.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		ASSERT(bitmap != 0);

		BITMAP bitmap_info;
		int got_obj = GetObject(bitmap, sizeof(BITMAP), &bitmap_info);
		ASSERT(got_obj != 0);

		ASSERT(bitmap_info.bmWidth == SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE);
		ASSERT(bitmap_info.bmHeight == SPRITESHEET_HEIGHT*SPRITESHEET_CELL_SIZE);
		ASSERT(bitmap_info.bmBitsPixel == 24);

		for (umm j = 0; j < SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE; ++j)
		{
			for (umm i = 0; i < SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE; ++i)
			{
				u32 color = ((u32)((u8*)bitmap_info.bmBits)[j*bitmap_info.bmWidthBytes + i*3 + 0] << 0) |
				            ((u32)((u8*)bitmap_info.bmBits)[j*bitmap_info.bmWidthBytes + i*3 + 1] << 8) |
				            ((u32)((u8*)bitmap_info.bmBits)[j*bitmap_info.bmWidthBytes + i*3 + 2] << 16);

				PlatformState.spritesheet[((SPRITESHEET_HEIGHT*SPRITESHEET_CELL_SIZE-1)-j)*SPRITESHEET_WIDTH*SPRITESHEET_CELL_SIZE + i] = color;
			}
		}

		DeleteObject(bitmap);
	}

	WNDCLASSW window_class = {
			.style         = CS_OWNDC,
			.lpfnWndProc   = Wndproc,
			.cbClsExtra    = 0,
			.cbWndExtra    = 0,
			.hInstance     = instance,
			.hIcon         = 0,
			.hCursor       = 0,
			.hbrBackground = CreateSolidBrush(0),
			.lpszMenuName  = 0,
			.lpszClassName = L"TET",
	};

	ATOM window_atom = RegisterClassW(&window_class);
	ASSERT(window_atom != 0); // TODO

	PlatformState.window = CreateWindowW(L"TET", L"TET", NORMAL_WINDOW_STYLES & ~WS_VISIBLE,
																			 CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
																			 0, 0, 0, 0);
	ASSERT(PlatformState.window != 0); // TODO
	
	PlatformState.normal_placement = (WINDOWPLACEMENT){ .length = sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement(PlatformState.window, &PlatformState.normal_placement);

	RefitToMonitor();

  ShowWindow(PlatformState.window, SW_SHOW);
	InvalidateRect(PlatformState.window, 0, TRUE);

	timeBeginPeriod(1);

	LARGE_INTEGER perf_freq;
	QueryPerformanceFrequency(&perf_freq);

	LARGE_INTEGER flip_time;
	QueryPerformanceCounter(&flip_time);

	Game_State game_state = {0};
	Init(&game_state);

  for (bool running = true;;)
  {
    for (MSG msg; PeekMessageW(&msg, 0, 0, 0, PM_REMOVE); )
    {
      if (msg.message == WM_QUIT)
      {
        exit_code = (int)msg.wParam;
				running = false;
				break;
      }
      else if (msg.message == WM_SYSKEYDOWN && msg.wParam == VK_RETURN && ((msg.wParam>>30)&1) == 0 ||
               msg.message == WM_KEYDOWN    && msg.wParam == VK_F11    && ((msg.wParam>>30)&1) == 0)
      {
        ToggleFullscreen();
      }
      else Wndproc(PlatformState.window, msg.message, msg.wParam, msg.lParam);
    }
		if (!running) break;

		for (umm i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; ++i) PlatformState.backbuffer[i] = 0;

		Tick(&game_state);

    HDC dc = GetDC(PlatformState.window);

    BITMAPINFO bitmap_info = {
      .bmiHeader = {
        .biSize        = sizeof(BITMAPINFOHEADER),
        .biWidth       = SCREEN_WIDTH,
        .biHeight      = -SCREEN_HEIGHT,
        .biPlanes      = 1,
        .biBitCount    = 32,
        .biCompression = BI_RGB,
      },
    };

    StretchDIBits(dc, PlatformState.scaled_screen_origin.x, PlatformState.scaled_screen_origin.y,
				          PlatformState.scaled_screen_dim.x, PlatformState.scaled_screen_dim.y,
				          0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, PlatformState.backbuffer, &bitmap_info, DIB_RGB_COLORS, SRCCOPY);

    ReleaseDC(PlatformState.window, dc);

		LARGE_INTEGER end_time;
		QueryPerformanceCounter(&end_time);

		smm decimillis = ((end_time.QuadPart - flip_time.QuadPart)*10000)/perf_freq.QuadPart;
		smm rem_time = 166 - decimillis;

		if (rem_time > 10) Sleep((DWORD)rem_time/10);

		QueryPerformanceCounter(&end_time);
		decimillis = ((end_time.QuadPart - flip_time.QuadPart)*10000)/perf_freq.QuadPart;

		WCHAR window_text[] = L"TET - frame time: xx.x ms";
		umm skip = sizeof("TET - frame time: ")-1;
		window_text[skip+0] = L'0' + (decimillis/100)%10;
		window_text[skip+1] = L'0' + (decimillis/10)%10;
		window_text[skip+3] = L'0' + (decimillis/1)%10;

		SetWindowTextW(PlatformState.window, &window_text[0]);

		flip_time = end_time;
  }

	timeEndPeriod(1);

  return exit_code;
}
