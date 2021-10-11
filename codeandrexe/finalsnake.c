
/* 
 * the snake game for mini2440  
 * linux-2.6.32.2 miniGUI1.3.3  NEC 240*320 tft lcd
 */
 
#include <stdio.h>
#include <stdlib.h>
 
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
 
#define IDC_SLIDER 100
#define IDC_BUTTON 200
#define IDC_TIMER  300
 
enum {
  LEFT,	
  RIGHT,
  UP,	
  DOWN,
  
  LIVE,	
  DEAD,
  STOP
};
 
#define LEN  	50				//snake max length
 
static RECT rc_snake = {0, 0, 240, 240};
static volatile int speed = 30; //can write
static int score = 0;
static BITMAP bmp;
static struct
{
	int x[LEN];
	int y[LEN];
	int node;
	int direction;
	int life;
}snake;
 
static struct 
{
	int x;
	int y;
	int yes;
}food;
	
static void draw_snake(HDC hdc)
{
	int i;
 
	if(food.yes == 1) {				//create food
		food.x = rand()%210;
		food.y = rand()%200;
		
		while(food.x%10 != 0)
			food.x++;
		while(food.y%10 != 0)
			food.y++;
		food.yes = 0;
	}
 
	//draw snake
	//SetPenColor(hdc, PIXEL_blue);
	SetPenColor(hdc, PIXEL_yellow);
	for(i=0; i<snake.node; i++)
		Rectangle(hdc, snake.x[i], snake.y[i], snake.x[i]+10, snake.y[i]+10);
 
    if((snake.x[0] == food.x) && (snake.y[0] == food.y)) {
            SetPenColor(hdc, PIXEL_lightgray);                          //clean food
            Rectangle(hdc, food.x, food.y, food.x+10, food.y+10);
            snake.node++;
            food.yes =1;
			//speed-=1;
            score += 10;
    }
 
	if(food.yes == 0) {		//show food
		SetPenColor(hdc, PIXEL_blue);
		Rectangle(hdc, food.x, food.y, food.x+10, food.y+10);
	}
	
	if(snake.x[0]<1 || snake.x[0]>214 || snake.y[0]<1 || snake.y[0] > 200) {
		snake.life = DEAD;
		return;
	}
	
}
 
static void slider_proc(HWND hwnd, int id, int nc, DWORD add_data)
{
	if((id == IDC_SLIDER) && (nc == TBN_CHANGE)) {
		speed = SendMessage(hwnd, TBM_GETPOS, 0, 0);
		/*
		if(snake.life == LIVE) {
			KillTimer(GetParent(hwnd), IDC_TIMER);
			SetTimer(GetParent(hwnd), IDC_TIMER, speed);
			//speed*=10;
		}*/
		if(food.yes==0){
		  speed = SendMessage(hwnd, TBM_GETPOS, speed, speed);
		  SetTimer(GetParent(hwnd), IDC_TIMER, speed);
		}
		else if(food.yes==1){
		  speed-=1;
		  speed = SendMessage(hwnd, TBM_GETPOS, speed, speed);
		  SetTimer(GetParent(hwnd), IDC_TIMER, 1);
		}
	}
}
 
static void init_game(void)
{
    snake.node = 2;
    snake.life = LIVE;
    snake.direction = RIGHT;
    snake.x[0] = 50;
    snake.y[0] = 50;
    snake.x[1] = 50;
    snake.y[1] = 60;
	score = 0;
    food.yes = 1;
}
 
static int snake_ctrl_proc(HWND hwnd, int message, WPARAM wparam, LPARAM lparam)
{
	char date[1024]="\n";//modify
	HDC hdc;
	HWND shwnd, bhwnd1, bhwnd2;
	char buf[100] = {0};
	int i, key;
	
	switch(message) {
		case MSG_CREATE:
			shwnd = CreateWindow(CTRL_TRACKBAR, "", WS_CHILD | WS_VISIBLE | TBS_NOTIFY | TBS_NOTICK,
				IDC_SLIDER, 20, 240, 200, 20, hwnd, 0);
			SendMessage(shwnd, TBM_SETRANGE, 10, 200); 	//slider value 10~200
			SendMessage(shwnd, TBM_SETLINESIZE, 1, 0);
			SendMessage(shwnd, TBM_SETPAGESIZE, 10, 0);	//page step
			SendMessage(shwnd, TBM_SETTICKFREQ, 10, 0);	//scale
			SendMessage(shwnd, TBM_SETPOS, speed, 0);
			SetNotificationCallback(shwnd, slider_proc);
 
			bhwnd1 = CreateWindow(CTRL_BUTTON, "start", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, IDC_BUTTON,
						40, 265, 60, 24, hwnd, 0);
			bhwnd2 = CreateWindow(CTRL_BUTTON, "stop", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, IDC_BUTTON+1,
						140, 265, 60, 24, hwnd, 0);	
						
			init_game();
			srand(time(NULL));		
			
			break;
			
		case MSG_COMMAND:
			if(wparam == IDC_BUTTON)
			{
				if(snake.life == DEAD){
					init_game();
					}
                else if((snake.x[0] == food.x) && (snake.y[0] == food.y)) {
					speed-=10;
					//SetTimer(hwnd, IDC_TIMER, 1);//modify
				}
                else if((snake.x[0] != food.x) && (snake.y[0] != food.y)) {
					snake.life = LIVE;
					//SetTimer(hwnd, IDC_TIMER, speed);
				}
				/*
				else if(food.yes==0){//modify
					snake.life = LIVE;
					SetTimer(hwnd, IDC_TIMER, speed);
				}*/
			    SetTimer(hwnd, IDC_TIMER, speed);//modify
			}
			else
			{
				snake.life = STOP;
				KillTimer(hwnd, IDC_TIMER);
			}
			break;
 
		case MSG_KEYDOWN:
			key = LOWORD(wparam);
			switch(key) {
				case SCANCODE_CURSORBLOCKUP:
					if(snake.direction != DOWN)
						snake.direction = UP;
					break;
				case SCANCODE_CURSORBLOCKDOWN:
					if(snake.direction != UP)
						snake.direction = DOWN;
					break;
				case SCANCODE_CURSORBLOCKLEFT:
					if(snake.direction != RIGHT)
						snake.direction = LEFT;
					break;
				case SCANCODE_CURSORBLOCKRIGHT:
					if(snake.direction != LEFT)
						snake.direction = RIGHT;
					break;
			}
			break;
			
		case MSG_PAINT:
			hdc = BeginPaint(hwnd);
			ClipRectIntersect(hdc, &rc_snake);
 
			SetPenColor(hdc, PIXEL_black); 	//draw board
			Rectangle(hdc, 1, 1, 234, 220);
			
			draw_snake(hdc);
			if(snake.life == DEAD) {
				sprintf(buf, "game over!");
				KillTimer(hwnd, IDC_TIMER);				
				sprintf(date,"score:%d",score);
				MessageBox(hwnd,date,"Game Over",MB_OK|MB_ICONINFORMATION);//modify
			} else {
				sprintf(buf, "score:%d,speed:%d", score,speed);
			}
			TextOut(hdc, 2, 221, buf);
			EndPaint(hwnd, hdc);
			break;		
			
		case MSG_TIMER:
			for( i=snake.node-1; i>0; i--) {
				snake.x[i] = snake.x[i-1];
				snake.y[i] = snake.y[i-1];
			}
 
			switch(snake.direction) {
				case LEFT:
					snake.x[0] -= 10;
					break;
				case RIGHT:
					snake.x[0] += 10;
					break;
				case UP:
					snake.y[0] -= 10;
					break;
				case DOWN:
					snake.y[0] += 10;
					break;
			}
			
			InvalidateRect(hwnd, &rc_snake, TRUE);
			break;
			
		case MSG_DESTROY:
			DestroyAllControls(hwnd);
			return 0;
 
		case MSG_CLOSE:
			DestroyMainWindow(hwnd);
			PostQuitMessage(hwnd);
			return 0;		
	}
 
	return DefaultMainWinProc(hwnd, message, wparam, lparam);
}
 
int MiniGUIMain(int argc, const char *argv[])
{
	MSG msg;
	HWND mhwnd;
	MAINWINCREATE createinfo;
 
	createinfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
	createinfo.dwExStyle = WS_EX_NONE;
	createinfo.spCaption = "snake";
	createinfo.hMenu = 0;
	createinfo.hCursor = GetSystemCursor(0);
	createinfo.hIcon = 0;
	createinfo.hHosting = HWND_DESKTOP;
	createinfo.MainWindowProc = snake_ctrl_proc;
	createinfo.lx = 190;
	createinfo.ty = 60;
	createinfo.rx = 430;
	createinfo.by = 380;
	createinfo.iBkColor = COLOR_lightgray;
	createinfo.dwAddData = 0;
 
	mhwnd = CreateMainWindow(&createinfo);
	//FillBoxWithBitmap(hdc,250,0,25,25,&bmp);
	if(mhwnd == HWND_INVALID) 
		return -1;
 
	ShowWindow(mhwnd, SW_SHOWNORMAL);
 
	while(GetMessage(&msg, mhwnd)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
 
	MainWindowThreadCleanup(mhwnd);
	
	return 0;
}
 
#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

