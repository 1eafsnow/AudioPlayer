#pragma once

#include <stdio.h>
#include <string>
#include <iostream>

template <typename T>
class _Vector2
{
public:
	T x;
	T y;

	static const _Vector2 Zero;

	_Vector2 operator+(const _Vector2& v)
	{
		return _Vector2(x + v.x, y + v.y);
	}

	_Vector2 operator-(const _Vector2& v) const
	{
		return _Vector2(x - v.x, y - v.y);
	}

	_Vector2 operator*(const _Vector2& v)
	{
		return _Vector2(x * v.x, y * v.y);
	}

	_Vector2 operator/(const _Vector2& v)
	{
		return _Vector2(x / v.x, y / v.y);
	}

	_Vector2 operator*(const double& s)
	{
		return _Vector2(x * s, y * s);
	}

	_Vector2 operator*(const double& s) const
	{
		return _Vector2(x * s, y * s);
	}

	_Vector2 operator/(const double& s)
	{
		return _Vector2<T>(x / s, y / s);
	}

	_Vector2 operator-()
	{
		return _Vector2(-x, -y);
	}

	void operator=(const _Vector2& v)
	{
		memcpy(this, &v, sizeof(_Vector2));
	}

	bool operator==(const _Vector2& v)
	{
		return x == v.x && y == v.y ? true : false;
	}

	bool operator!=(const _Vector2& v)
	{
		return x == v.x && y == v.y ? false : true;
	}

	T& operator[](const int& i)
	{
		return x;
	}

	_Vector2(T x = 0, T y = 0) : x(x), y(y) {}
	_Vector2(const _Vector2& v) : x(v.x), y(v.y) {}

	double Length() const
	{
		return sqrt(x * x + y * y);
	}

	void Normalize()
	{
		double l = Length();
		x = x / l;
		y = y / l;
	}

	_Vector2 GetNormalized() const
	{
		_Vector2 res;
		double l = Length();
		res.x = x / l;
		res.y = y / l;

		return res;
	}

	void Print()
	{
		printf("(x: %f, y: %f)\n", x, y);
	}
};

template <typename T>
class _Vector3
{
public:
	T x;
	T y;
	T z;

	static const _Vector3 Zero;
	static const _Vector3 AxisX;
	static const _Vector3 AxisY;
	static const _Vector3 AxisZ;

	_Vector3 operator+(const _Vector3& v)
	{
		return _Vector3(x + v.x, y + v.y, z + v.z);
	}

	_Vector3 operator-(const _Vector3& v) const
	{
		return _Vector3(x - v.x, y - v.y, z - v.z);
	}

	_Vector3 operator*(const _Vector3& v)
	{
		return _Vector3(x * v.x, y * v.y, z * v.z);
	}

	_Vector3 operator/(const _Vector3& v)
	{
		return _Vector3(x / v.x, y / v.y, z / v.z);
	}

	_Vector3 operator*(const double& s)
	{
		return _Vector3(x * s, y * s, z * s);
	}

	_Vector3 operator*(const double& s) const
	{
		return _Vector3(x * s, y * s, z * s);
	}

	_Vector3 operator/(const double& s)
	{
		return _Vector3<T>(x / s, y / s, z / s);
	}

	_Vector3 operator-()
	{
		return _Vector3(-x, -y, -z);
	}

	void operator=(const _Vector3& v)
	{
		memcpy(this, &v, sizeof(_Vector3));
	}

	bool operator==(const _Vector3& v)
	{
		return x == v.x && y == v.y && z == v.z ? true : false;
	}

	bool operator!=(const _Vector3& v)
	{
		return x == v.x && y == v.y && z == v.z ? false : true;
	}

	T& operator[](const int& i)
	{
		switch (i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		default: throw std::out_of_range("Index out of range for _Vector3");
		}
	}

	_Vector3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
	_Vector3(const _Vector3& v) : x(v.x), y(v.y), z(v.z) {}

	double Length() const
	{
		return sqrt(x * x + y * y + z * z);
	}

	void Normalize()
	{
		double l = Length();
		x = x / l;
		y = y / l;
		z = z / l;
	}

	_Vector3 GetNormalized() const
	{
		_Vector3 res;
		double l = Length();
		res.x = x / l;
		res.y = y / l;
		res.z = z / l;

		return res;
	}

	void Print()
	{
		printf("(x: %f, y: %f, z: %f)\n", x, y, z);
	}
};

template <typename T>
class _Vector4
{
public:
	T x;
	T y;
	T z;
	T w;

	static const _Vector4 Zero;

	_Vector4 operator+(const _Vector4& v)
	{
		return _Vector4(x + v.x, y + v.y, z + v.z, w + v.w);
	}

	_Vector4 operator-(const _Vector4& v)
	{
		return _Vector4(x - v.x, y - v.y, z - v.z, w - v.w);
	}

	_Vector4 operator*(const _Vector4& v)
	{
		return _Vector4(x * v.x, y * v.y, z * v.z, w * v.w);
	}

	_Vector4 operator/(const _Vector4& v)
	{
		return _Vector4(x / v.x, y / v.y, z / v.z, w / v.w);
	}

	_Vector4 operator*(const double& s)
	{
		return _Vector4(x * s, y * s, z * s, w * s);
	}

	_Vector4 operator/(const double& s)
	{
		return _Vector4(x / s, y / s, z / s, w / s);
	}

	_Vector4 operator-()
	{
		return _Vector4(-x, -y, -z, -w);
	}

	void operator=(const _Vector4& v)
	{
		memcpy(this, &v, sizeof(_Vector4));
	}

	bool operator==(const _Vector4& v)
	{
		return x == v.x && y == v.y && z == v.z && w == v.w ? true : false;
	}

	bool operator!=(const _Vector4& v)
	{
		return x == v.x && y == v.y && z == v.z && w == v.w ? false : true;
	}

	T& operator[](const int& i)
	{
		switch (i)
		{
		case 0: return x;
		case 1: return y;
		case 2: return z;
		case 3: return w;
		default: throw std::out_of_range("Index out of range for _Vector4");
		}
	}

	_Vector4(T x = 0, T y = 0, T z = 0, T w = 0) : x(x), y(y), z(z), w(w) {}

	double Length()
	{
		return sqrt(x * x + y * y + z * z + w * w);
	}

	void Normalize()
	{
		double l = Length();
		x = x / l;
		y = y / l;
		z = z / l;
		w = w / l;
	}

	_Vector4 GetNormalized()
	{
		_Vector4 res;
		double l = Length();
		res.x = x / l;
		res.y = y / l;
		res.z = z / l;
		res.w = w / l;

		return res;
	}

	void Print()
	{
		printf("(x: %f, y: %f, z: %f, w: %f)\n", x, y, z, w);
	}
};

using Vector2I = _Vector2<int>;
using Vector3I = _Vector3<int>;
using Vector4I = _Vector4<int>;
using Vector2F = _Vector2<float>;
using Vector3F = _Vector3<float>;
using Vector4F = _Vector4<float>;
using Vector2D = _Vector2<double>;
using Vector3D = _Vector3<double>;
using Vector4D = _Vector4<double>;

const Vector2I Vector2I::Zero(0, 0);
const Vector3I Vector3I::Zero(0, 0, 0);
const Vector4I Vector4I::Zero(0, 0, 0, 0);
const Vector2F Vector2F::Zero(0.0f, 0.0f);
const Vector3F Vector3F::Zero(0.0f, 0.0f, 0.0f);
const Vector4F Vector4F::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const Vector3F Vector3F::AxisX(1.0f, 0.0f, 0.0f);
const Vector3F Vector3F::AxisY(0.0f, 1.0f, 0.0f);
const Vector3F Vector3F::AxisZ(0.0f, 0.0f, 1.0f);
const Vector2D Vector2D::Zero(0.0f, 0.0f);
const Vector3D Vector3D::Zero(0.0f, 0.0f, 0.0f);
const Vector4D Vector4D::Zero(0.0f, 0.0f, 0.0f, 0.0f);
const Vector3D Vector3D::AxisX(1.0f, 0.0f, 0.0f);
const Vector3D Vector3D::AxisY(0.0f, 1.0f, 0.0f);
const Vector3D Vector3D::AxisZ(0.0f, 0.0f, 1.0f);

std::ostream& operator<<(std::ostream& os, const Vector2D& v);
std::ostream& operator<<(std::ostream& os, const Vector3D& v);
std::ostream& operator<<(std::ostream& os, const Vector4D& v);

Vector2D operator *(const float& s, const Vector2D& v);
Vector3D operator *(const float& s, const Vector3D& v);
Vector4D operator *(const float& s, const Vector4D& v);