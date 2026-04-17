#pragma once

#include <stdio.h>
#include <string>
#include <iostream>

template <typename T>
class _Vector3;
template <typename T>
class _Vector4;
template <typename T>
class _Rotator;

template <typename T>
class _Matrix3
{
public:
	T elements[9];

	static const _Matrix3 UNIT;

	_Matrix3<T>& operator=(const _Matrix3& m)
	{
		memcpy(elements, m.elements, sizeof(T) * 9);
		return *this;
	}

	_Matrix3 operator+(const _Matrix3& m)
	{
		_Matrix3 res;
		for (int i = 0; i < 9; i++)
		{
			res.elements[i] = this->elements[i] + m.elements[i];
		}
		return res;
	}

	T* operator[](const int& i)
	{
		return &elements[3 * i];
	}

	const T* operator[](const int& i) const
	{
		return &elements[3 * i];
	}

	_Matrix3() : elements{ 0 } {}
	_Matrix3(T* elements) { memcpy(this->elements, elements, sizeof(T) * 9); }
	_Matrix3(const _Matrix3& matrix) { memcpy(elements, matrix.elements, sizeof(T) * 9); }
	_Matrix3(T e00, T e01, T e02,
		T e10, T e11, T e12,
		T e20, T e21, T e22) :
		elements{ e00, e01, e02,
				  e10, e11, e12,
				  e20, e21, e22 } {
	}

	T* Array()
	{
		return elements;
	}

	_Matrix3 Transpose()
	{
		_Matrix3 res;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				res[i][j] = (*this)[j][i];
			}
		}
		return res;
	}

	_Rotator<T> Rotation()
	{
		T yaw, pitch, roll;

		pitch = asin(-(*this)[2][1]);
		yaw = atan2(-(*this)[2][0], (*this)[2][2]);
		roll = atan2((*this)[0][1], (*this)[1][1]);

		return _Rotator<T>(yaw / PI * 180.0, pitch / PI * 180.0, roll / PI * 180.0);
	}

	void Print()
	{
		std::cout << elements[0] << ', ' << elements[1] << ', ' << elements[2] << std::endl
			<< elements[3] << ', ' << elements[4] << ', ' << elements[5] << std::endl
			<< elements[6] << ', ' << elements[7] << ', ' << elements[8] << std::endl;
	}
};

template <typename T>
class _Matrix4
{
public:
	T elements[16];

	static const _Matrix4 UNIT;

	_Matrix4& operator=(const _Matrix4& m)
	{
		memcpy(elements, m.elements, sizeof(T) * 16);
		return *this;
	}

	_Matrix4 operator+(const _Matrix4& m)
	{
		_Matrix4 res;
		for (int i = 0; i < 16; i++)
		{
			res.elements[i] = this->elements[i] + m.elements[i];
		}
		return res;
	}

	T* operator[](int i)
	{
		return &elements[4 * i];
	}

	const T* operator[](int i) const
	{
		return &elements[4 * i];
	}

	_Matrix4() : elements{ 0 } {}
	_Matrix4(T* elements) { memcpy(this->elements, elements, sizeof(T) * 16); }
	_Matrix4(const _Matrix4& matrix) { memcpy(elements, matrix.elements, sizeof(T) * 16); }
	_Matrix4(T e00, T e01, T e02, T e03,
		T e10, T e11, T e12, T e13,
		T e20, T e21, T e22, T e23,
		T e30, T e31, T e32, T e33) :
		elements{ e00, e01, e02, e03,
				  e10, e11, e12, e13,
				  e20, e21, e22, e23,
				  e30, e31, e32, e33 } {
	}

	T* Array()
	{
		return elements;
	}

	_Matrix4 Transpose()
	{
		_Matrix4 res;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				res[i][j] = *(this)[j][i];
			}
		}
		return res;
	}

	_Matrix4 TransformInverse()
	{
		_Matrix4 rotate = RotateMatrixToTransform(TransformToRotateMatrix(*this).Transpose());
		_Matrix4 translate = _Matrix4(1, 0, 0, -(*this)[0][3],
			0, 1, 0, -(*this)[1][3],
			0, 0, 1, -(*this)[2][3],
			0, 0, 0, 1);

		return Matmul(rotate, translate);
	}

	void Print()
	{
		printf("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
			elements[0], elements[1], elements[2], elements[3],
			elements[4], elements[5], elements[6], elements[7],
			elements[8], elements[9], elements[10], elements[11],
			elements[12], elements[13], elements[14], elements[15]);
	}
};

using Matrix3F = _Matrix3<float>;
using Matrix4F = _Matrix4<float>;
using Matrix3D = _Matrix3<double>;
using Matrix4D = _Matrix4<double>;

const Matrix3F Matrix3F::UNIT = Matrix3F(1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0);

const Matrix4F Matrix4F::UNIT = Matrix4F(1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);
const Matrix3D Matrix3D::UNIT = Matrix3D(1.0, 0.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 0.0, 1.0);

const Matrix4D Matrix4D::UNIT = Matrix4D(1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0);

std::ostream& operator<<(std::ostream& os, const Matrix3D& v);
std::ostream& operator<<(std::ostream& os, const Matrix4D& v);

template <typename T>
_Matrix3<T> TransformToRotateMatrix(const _Matrix4<T>& m)
{
	return _Matrix3<T>(m[0][0], m[0][1], m[0][2],
		m[1][0], m[1][1], m[1][2],
		m[2][0], m[2][1], m[2][2]);
}

template <typename T>
_Matrix4<T> RotateMatrixToTransform(const _Matrix3<T>& m)
{
	return _Matrix4<T>(m[0][0], m[0][1], m[0][2], 0,
		m[1][0], m[1][1], m[1][2], 0,
		m[2][0], m[2][1], m[2][2], 0,
		0, 0, 0, 1);
}

template <typename T>
_Matrix4<T> Matmul(const _Matrix4<T>& l, const _Matrix4<T>& r)
{
	_Matrix4<T> result;
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