#pragma once
#include "../Math/MathS.h"
#include "Object.h"

class Camera : public Object
{
public:
	Matrix4F modelTransform;
	Matrix4F viewTransform;
	Matrix4F projectionTransform;
	float focus;
	float sightNear;
	float sightFar;
	float fovyX;
	float fovyY;
	Matrix4F orthProjection;
	Matrix4F persProjection;

	float moveSpeed;
	float lookSpeed;

	float deltaMoveSpeed;
	float deltaLookSpeed;

	Camera(float near, float far, float fovyX, float fovyY);

	void SetWorldLocation(Vector3F location) override;
	void SetWorldRotation(Rotator rotation) override;
	void AddLocalLocation(Vector3F location) override;
	void AddLocalRotation(Rotator rotation) override;

	void Move(Vector3F direction);
	void Look(Rotator rotation);

	void UpdateProjectionTransform();
	Matrix4F GetViewTransform();
	Matrix4F GetProjectionTransform();

	virtual void Tick(float deltaTime) override;
};