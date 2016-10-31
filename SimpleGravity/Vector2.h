#pragma once
class Vector2
{
public:
	float x;
	float y;

	Vector2()
	{
		Set(0,0);
	}

	Vector2(float _x, float _y)
	{
		Set(_x, _y);
	}

	~Vector2()
	{
	}
	
	void Set(float _x,  float _y)
	{
		x = _x;
		y = _y;
	}
};

Vector2 operator+ (Vector2 lhs, Vector2 rhs) { return Vector2(lhs.x + rhs.x, lhs.y + rhs.y); }
Vector2 operator+ (Vector2 lhs, float rhs) { return Vector2(lhs.x + rhs, lhs.y + rhs); }

Vector2 operator- (Vector2 lhs, Vector2 rhs) { return Vector2(lhs.x - rhs.x, lhs.y - rhs.y); }
Vector2 operator- (Vector2 lhs, float rhs) { return Vector2(lhs.x - rhs, lhs.y - rhs); }

Vector2 operator* (Vector2 lhs, Vector2 rhs) { return Vector2(lhs.x * rhs.x, lhs.y * rhs.y); }
Vector2 operator* (Vector2 lhs, float rhs) { return Vector2(lhs.x * rhs, lhs.y * rhs); }

Vector2 operator/ (Vector2 lhs, Vector2 rhs) { return Vector2(lhs.x / rhs.x, lhs.y / rhs.y); }
Vector2 operator/ (Vector2 lhs, float rhs) { return Vector2(lhs.x / rhs, lhs.y / rhs); }

inline bool operator==(const Vector2& lhs, const Vector2& rhs)
{
	if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
	else return false;
}