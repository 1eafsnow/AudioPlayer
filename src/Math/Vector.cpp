#include "Vector.h"

std::ostream& operator<<(std::ostream& os, const Vector2D& v)
{
	os << '(' << v.x << ', ' << v.y << ')';
	return os;
}

std::ostream& operator<<(std::ostream& os, const Vector3D& v)
{
	os << '(' << v.x << ', ' << v.y << ', ' << v.y << ')';
	return os;
}

std::ostream& operator<<(std::ostream& os, const Vector4D& v)
{
	os << '(' << v.x << ', ' << v.y << ', ' << v.y << ', ' << v.w << ')';
	return os;
}

Vector2D operator *(const float& s, const Vector2D& v)
{
	return Vector2D(v.x * s, v.y * s);
}

Vector3D operator *(const float& s, const Vector3D& v)
{
	return Vector3D(v.x * s, v.y * s, v.z * s);
}

Vector4D operator *(const float& s, const Vector4D& v)
{
	return Vector4D(v.x * s, v.y * s, v.z * s, v.w * s);
}