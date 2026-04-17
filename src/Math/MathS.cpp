#include "MathS.h"

float Random()
{
	float r = (float)rand() / RAND_MAX;
	return r;
}

float RandomLeftOpen()
{
	float r = (float)(rand() + 1) / (RAND_MAX + 1);
	return r;
}

float RandomRightOpen()
{
	float r = (float)rand() / (RAND_MAX + 1);
	return r;
}

float RandomOpen()
{
	float r = (float)(rand() + 1) / (RAND_MAX + 2);
	return r;
}

float Random(const float& min, const float& max, const bool& lInterval, const bool& rInterval)
{
	float r = ((float)rand() / RAND_MAX) / (max - min) + min;
	return r;
}

int Max(const int& a, const int& b)
{
	return (a > b) ? a : b;
}

int Min(const int& a, const int& b)
{
	return (a < b) ? a : b;
}

float Max(const float& a, const float& b)
{
	return (a > b) ? a : b;
}

float Min(const float& a, const float& b)
{
	return (a < b) ? a : b;
}

float Clamp(const float& x, const float& min, const float& max)
{
	if (x < min)
	{
		return min;
	}
	else if (x > max)
	{
		return max;
	}
	return x;
}

void Normalize(Vector3D& v)
{
	float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x = v.x / length;
	v.y = v.y / length;
	v.z = v.z / length;
}

float Dot(const Vector2D& v1, const Vector2D& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

Quaternion Cross(const Quaternion& q1, const Quaternion& q2)
{
	Vector3F q1u(q1.x, q1.y, q1.z);
	Vector3F q2u(q2.x, q2.y, q2.z);
	Vector3F u = q2u * q1.w + q1u * q2.w + Cross(q1u, q2u);
	float w = q1.w * q2.w - Dot(q1u, q2u);
	return Quaternion(u, w);
}

Vector4D Matmul(const Matrix4D& m, const Vector4D& v)
{
	return Vector4D(m.elements[0] * v.x + m.elements[1] * v.y + m.elements[2] * v.z + m.elements[3] * v.w,
		m.elements[4] * v.x + m.elements[5] * v.y + m.elements[6] * v.z + m.elements[7] * v.w,
		m.elements[8] * v.x + m.elements[9] * v.y + m.elements[10] * v.z + m.elements[11] * v.w,
		m.elements[12] * v.x + m.elements[13] * v.y + m.elements[14] * v.z + m.elements[15] * v.w);
}

Vector3D Matmul(const Matrix4D& m, const Vector3D& v)
{
	float w = m.elements[12] * v.x + m.elements[13] * v.y + m.elements[14] * v.z + m.elements[15];
	return Vector3D(m.elements[0] * v.x + m.elements[1] * v.y + m.elements[2] * v.z + m.elements[3],
		m.elements[4] * v.x + m.elements[5] * v.y + m.elements[6] * v.z + m.elements[7],
		m.elements[8] * v.x + m.elements[9] * v.y + m.elements[10] * v.z + m.elements[11]) / w;
}

Matrix3D Matmul(const Matrix3D l, const Matrix3D r)
{
	Matrix3D result;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				result[i][j] += l[i][k] * r[k][j];
			}
		}
	}
	return result;
}

Matrix4D Matmul(const Matrix4D l, const Matrix4D r)
{
	Matrix4D result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				result[i][j] += l[i][k] * r[k][j];
			}
		}
	}
	return result;
}

float Angle(const Vector3D& v1, const Vector3D& v2)
{
	float angle = acos(Dot(v1, v2) / (v1.Length() * v2.Length()));
	return angle < PI ? angle : PI * 2 - angle;
}

float Angle(const Vector2D& v1, const Vector2D& v2)
{
	float angle = acos(Dot(v1, v2) / (v1.Length() * v2.Length()));
	return angle < PI ? angle : PI * 2 - angle;
}

Vector3D Reflect(const Vector3D& n, const Vector3D& i)
{
	return (n * (Dot(n, i)) * 2 - i).GetNormalized();
}

Vector2D Reflect(const Vector2D& n, const Vector2D& i)
{
	return n * (Dot(n, i)) * 2 - i;
}