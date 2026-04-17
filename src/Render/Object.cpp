#include "Object.h"

Object::Object() : transform(Matrix4F::UNIT)
{

}

Vector3F Object::GetWorldLoation()
{
	return worldLocation;
}

Rotator Object::GetWorldRotation()
{
	return worldRotation;
}

void Object::SetWorldLocation(Vector3F location)
{
	transform[0][3] = location.x;
	transform[1][3] = location.y;
	transform[2][3] = location.z;

	worldLocation = location;

	for (Object*& child : children)
	{
		child->SetWorldLocation(location);
	}
}

void Object::SetWorldRotation(Rotator rotation)
{
	Matrix3F rotateMatrix = rotation.RotateMatrix();
	transform[0][0] = rotateMatrix[0][0];
	transform[0][1] = rotateMatrix[0][1];
	transform[0][2] = rotateMatrix[0][2];

	transform[1][0] = rotateMatrix[1][0];
	transform[1][1] = rotateMatrix[1][1];
	transform[1][2] = rotateMatrix[1][2];

	transform[2][0] = rotateMatrix[2][0];
	transform[2][1] = rotateMatrix[2][1];
	transform[2][2] = rotateMatrix[2][2];

	worldRotation = rotation;

	for (Object*& child : children)
	{
		child->SetWorldRotation(rotation);
	}
}

void Object::AddWorldLocation(Vector3F location)
{
	SetWorldLocation(worldLocation + location);

	for (Object*& child : children)
	{
		child->AddWorldLocation(location);
	}
}

void Object::AddWorldRotation(Rotator rotation)
{
	SetWorldRotation(worldRotation + rotation);

	for (Object*& child : children)
	{
		child->AddWorldRotation(rotation);
	}
}

void Object::SetWorldScale(float scale)
{
	transform[3][3] = 1 / scale;

	for (Object*& child : children)
	{
		child->SetWorldScale(scale);
	}
}

void Object::SetLocalScale(float scale)
{
	transform[3][3] *= 1 / scale;

	for (Object*& child : children)
	{
		child->SetLocalScale(scale);
	}
}

void Object::AddLocalLocation(Vector3F location)
{
	location = Matmul(TransformToRotateMatrix(transform), location);
	transform[0][3] += location.x;
	transform[1][3] += location.y;
	transform[2][3] += location.z;

	worldLocation = Vector3F(transform[0][3], transform[1][3], transform[2][3]);

	for (Object*& child : children)
	{
		child->AddLocalLocation(location);
	}
}

void Object::AddLocalRotation(Rotator rotation)
{
	Matrix4F rotateMatrix = RotateMatrixToTransform(rotation.RotateMatrix());
	transform = Matmul(transform, rotateMatrix);
	worldRotation = TransformToRotateMatrix(transform).Rotation();

	for (Object*& child : children)
	{
		child->AddLocalRotation(rotation);
	}
}

void Object::Tick(float deltaTime)
{

}