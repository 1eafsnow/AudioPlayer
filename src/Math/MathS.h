#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include <random>

#define PI 3.1415926535

#include "Vector.h"
#include "Rotator.h"
#include "Matrix.h"
#include "Quaternion.h"

float Random();

float RandomLeftOpen();

float RandomRightOpen();

float RandomOpen();

float Random(const float& min, const float& max, const bool& lInterval, const bool& rInterval);

int Max(const int& a, const int& b);

int Min(const int& a, const int& b);

float Max(const float& a, const float& b);

float Min(const float& a, const float& b);

float Clamp(const float& x, const float& min, const float& max);

void Normalize(Vector3D& v);

float Dot(const Vector2D& v1, const Vector2D& v2);

template <typename T>
float Dot(const _Vector3<T>& v1, const _Vector3<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

template <typename T>
_Vector3<T> Cross(const _Vector3<T>& v1, const _Vector3<T>& v2)
{
	return _Vector3<T>(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x);
}

Quaternion Cross(const Quaternion& q1, const Quaternion& q2);

template <typename T>
_Vector3<T> Matmul(const _Matrix3<T>& m, const _Vector3<T>& v)
{
	return _Vector3<T>(m.elements[0] * v.x + m.elements[1] * v.y + m.elements[2] * v.z,
		m.elements[3] * v.x + m.elements[4] * v.y + m.elements[5] * v.z,
		m.elements[6] * v.x + m.elements[7] * v.y + m.elements[8] * v.z);
}

Vector4D Matmul(const Matrix4D& m, const Vector4D& v);

Vector3D Matmul(const Matrix4D& m, const Vector3D& v);

Matrix3D Matmul(const Matrix3D l, const Matrix3D r);

template <typename T>
_Matrix3<T> RotationToRotateMatrix(const _Vector3<T>& r)
{
	float yaw = r.x / 180.0 * PI;
	float pitch = r.y / 180.0 * PI;
	float roll = r.z / 180.0 * PI;

	return _Matrix3<T>(cos(roll) * cos(yaw) - sin(roll) * -sin(pitch) * -sin(yaw), sin(roll) * cos(pitch), cos(roll) * sin(yaw) - sin(roll) * -sin(pitch) * cos(yaw),
		-sin(roll) * cos(yaw) + cos(roll) * sin(pitch) * -sin(yaw), cos(roll) * cos(pitch), -sin(roll) * sin(yaw) + cos(roll) * sin(pitch) * cos(yaw),
		cos(pitch) * -sin(yaw), -sin(pitch), cos(pitch) * cos(yaw));
}

float Angle(const Vector2D& v1, const Vector2D& v2);

float Angle(const Vector3D& v1, const Vector3D& v2);

Vector3D Reflect(const Vector3D& n, const Vector3D& i);

Vector2D Reflect(const Vector2D& n, const Vector2D& i);