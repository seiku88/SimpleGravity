#include "BallLauncher.h"

BallLauncher::BallLauncher()
{
	gravity = true;
}


BallLauncher::~BallLauncher()
{
	ClearField();
}

void BallLauncher::Create()
{
	Ball *newball = new Ball(rand()%10+10, Vector2(0,0), velocity, Vector2(0, GRAVITY_EARTH), (rand()%25+50)*0.01f);

	activeBalls.push_back(newball);
}

void BallLauncher::Mouse(int x, int y)
{
	mouse.Set(x, y);
	velocity.Set((x - groundCenter.x)*(forceDivider/100), (y - groundCenter.y)*(forceDivider/-100));
	CalculatePath();
}

void BallLauncher::CalculatePath()
{
	Vector2 tmp;
	//bool fliped = false;
	for (int i = 0; i < 128; i++)
	{
		if (gravity) tmp = velocity + (Vector2(0, (GRAVITY_EARTH*((i*((1.0f/30.0f)/(forceDivider/2375.0f))/1000.0f)))));//30fps -> 30fpms
		else tmp = velocity;

		if (i==0) preview[i] = tmp;//If it's a first element
		else if (preview[i - 1].y <= 0)
		{
			//+나중에 튕기는 path까지 그리는 코드 추가해보기.
			preview[i] = preview[i-1];
		}
		else preview[i] = preview[i-1] + tmp;//Add new velocity to previous position
	}
}

void BallLauncher::Update()
{
	Ball *tmp;
	for (list<Ball *>::iterator itr = activeBalls.begin(); itr != activeBalls.end();)
	{
		tmp = *itr;
		tmp->Move(gravity);

		if (tmp->position.x >= groundWidth - groundCenter.x || tmp->position.x <= groundCenter.x - groundWidth)//Delete ball if it touched the wall
		{
			activeBalls.erase(itr++);
			DeleteObject(tmp);
		}
		else if (tmp->position.y <= 0.5f && tmp->curvelocity.y < 0.4f && tmp->curvelocity.y > 0)//Delete ball if it's movement is too small
		{
			activeBalls.erase(itr++);
			DeleteObject(tmp);
		}
		else if (tmp->position.y <= 0 && tmp->curvelocity.y < 0)//Flip velocity of ball if it touched the ground
		{
			tmp->position.y = 0;
			tmp->Flip();
			++itr;
		}
		else ++itr;
	}
}

void BallLauncher::ClearField()
{
	Ball *tmp;
	for (list<Ball *>::iterator itr = activeBalls.begin(); itr != activeBalls.end();)
	{
		tmp = *itr;
		activeBalls.erase(itr++);
		DeleteObject(tmp);
	}
}

void BallLauncher::Draw(HDC& backMemDC, HDC& memHdc, HBITMAP& oldBitmap, RECT& winRect)
{
	HGDIOBJ original = NULL;

	//Saving the original object
	original = SelectObject(backMemDC, GetStockObject(DC_PEN));
	SelectObject(backMemDC, GetStockObject(DC_BRUSH));

	if (gravity)//Draw normal sky if gravity is active
	{
		SetDCBrushColor(backMemDC, skyCol);
		SetDCPenColor(backMemDC, skyCol);

		Rectangle(backMemDC, winRect.left, winRect.top, winRect.right, winRect.bottom);
	}

	//Draw Ground
	SetDCBrushColor(backMemDC, groundCol);
	SetDCPenColor(backMemDC, groundCol);

	Rectangle(backMemDC, winRect.left, winRect.bottom - groundHeight, winRect.right, winRect.bottom);

	//Draw Ball Preview
	SetDCBrushColor(backMemDC, RGB(255, 255, 255));
	SetDCPenColor(backMemDC, RGB(255, 0, 0));
	Ellipse(backMemDC, groundCenter.x - 6, groundCenter.y - 12, groundCenter.x + 6, groundCenter.y);

	for (int i = 0; i < 128; i++)
	{
		if (i == 0)//If it's a first element
		{
			//Draw a line from center to first position
			MoveToEx(backMemDC, groundCenter.x, groundCenter.y, NULL);
			LineTo(backMemDC, groundCenter.x + preview[i].x, groundCenter.y - preview[i].y);
		}
		else
		{
			//Draw a line from previous position to current index's position
			MoveToEx(backMemDC, groundCenter.x + preview[i-1].x, groundCenter.y - preview[i-1].y, NULL);
			LineTo(backMemDC, groundCenter.x + preview[i].x, groundCenter.y - preview[i].y);
		}
	}

	//Restoring the original object(restore original color)
	SelectObject(backMemDC, original);

	//Draw Launched Balls
	Ball* tmp;
	for (list<Ball*>::iterator itr = activeBalls.begin(); itr != activeBalls.end(); ++itr)
	{
		//left: (x-(radius/2))+center
		//right: (x+(radius/2))+center
		//top: center-(y+radius)
		//bottom: center-y
		tmp = *itr;
		Ellipse(backMemDC, (tmp->position.x - (tmp->radius / 2)) + groundCenter.x, groundCenter.y - (tmp->position.y + tmp->radius), (tmp->position.x + (tmp->radius / 2)) + groundCenter.x, groundCenter.y - tmp->position.y);
	}

	//Write current velocity info on screen
	WCHAR info[255];
	if (gravity) wsprintf(info, TEXT("[Gravity ON]  Velocity:(%d, %d)"), (int)velocity.x, (int)velocity.y);
	else wsprintf(info, TEXT("[Gravity OFF]  Velocity:(%d, %d)"), (int)velocity.x, (int)velocity.y);
	DrawText(backMemDC, info, -1, &inforect, DT_LEFT | DT_WORDBREAK);
}
