#pragma once
#include <windows.h>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <list>
#include "Vector2.h"

using namespace std;

#define PIXEL_PER_METER 100 //100px == 1m
#define GRAVITY_DEFAULT -10.0f //slow default gravity(10px/frame)
#define GRAVITY_EARTH -9.80665f //normal earth gravity(9.80665m/s^2)

//발사되는 공 클래스.
class Ball
{
public:
	int radius;

	Vector2 position;// pixel, pixel
	Vector2 velocity;// pixel/frame, pixel/frame
	Vector2 curvelocity;// pixel/frame, pixel/frame
	Vector2 accel;// pixel/frame, pixel/frame
	float elasticity;// Bouncyness, Range of 0~1

	clock_t startTime;//Δti
	clock_t curTime;//Δtf

	Ball()
	{
		Set(6, Vector2(0,0), Vector2(0,0), Vector2(0, GRAVITY_EARTH), 0.5f);
	}

	Ball(int r, Vector2 pos, Vector2 vel, Vector2 acc, float ela)
	{
		Set(r, pos, vel, acc, ela);
	}

	//Set ball's radius, position, velocity, accel rate, elasticity.
	void Set(int r, Vector2 pos, Vector2 vel, Vector2 acc, float ela)
	{
		radius = r;
		position = pos;
		velocity = vel;
		curvelocity = velocity;
		accel = acc;
		elasticity = ela;

		startTime = clock();
		curTime = clock();
	}

	void Move(bool gravity)
	{
		if (gravity) curvelocity = velocity + (accel * (((float)curTime - startTime) / CLOCKS_PER_SEC));
		else { startTime = clock(); velocity = curvelocity; }

		position = position + curvelocity;
		curTime = clock();
	}

	void Flip()
	{
		velocity.Set(curvelocity.x * elasticity, curvelocity.y * (elasticity * -1.0f));
		startTime = clock();
		curTime = clock();
	}
};

class BallLauncher
{
public:
	bool gravity;//Is gravity active?
	Vector2 groundCenter;//Center x point of ground width.
	int groundWidth;//Same as window width.
	int groundHeight;//How far from bottom window border.

	COLORREF skyCol = RGB(148, 186, 227);
	COLORREF groundCol = RGB(115, 86, 83);

	Vector2 mouse;//Current Mouse Position
	Vector2 velocity;//Calculated start velocity
	const float forceDivider = 5.0f;//Value for dividing start velocity
	Vector2 preview[128];//Predicted Movement Path
	list<Ball*> activeBalls;//Active(Moving) Balls

	RECT inforect;//Rect for displaying info bar on top

	BallLauncher();
	~BallLauncher();

	//Create and push new ball instance
	void Create();

	//Update Mouse Position and Calculate Velocity from hypotenuse
	void Mouse(int x, int y);

	//Calculates Predicted Movement Path
	void CalculatePath();

	//Update Center of the world
	void Size(int winRight, int winBottom)
	{
		groundCenter.Set(winRight / 2, winBottom - groundHeight);
		groundWidth = winRight;
		inforect.left = 0;
		inforect.top = 0;
		inforect.right = winRight;
		inforect.bottom = 20;
	}

	//Iterate through ball list and move all of them
	void Update();

	void ClearField();

	void Draw(HDC& backMemDC, HDC& memHdc, HBITMAP& oldBitmap, RECT& winRect);
};

