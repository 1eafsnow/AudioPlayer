#pragma once
#include "../Math/MathS.h"

class Object
{
public:
	char* name;

	Vector3F worldLocation;
	Rotator worldRotation;

	Matrix4F transform;

	Object* parent;
	std::vector<Object*> children;

	Object();

	Vector3F GetWorldLoation();
	Rotator GetWorldRotation();

	virtual void SetWorldLocation(Vector3F location);
	virtual void SetWorldRotation(Rotator rotation);
	virtual void AddWorldLocation(Vector3F location);
	virtual void AddWorldRotation(Rotator rotation);
	virtual void SetWorldScale(float scale);

	virtual void SetLocalScale(float scale);
	virtual void AddLocalLocation(Vector3F location);
	virtual void AddLocalRotation(Rotator rotation);

	virtual void Tick(float deltaTime);
};