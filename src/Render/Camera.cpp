#include "Camera.h"

Camera::Camera(float near, float far, float fovyX, float fovyY)
{
	modelTransform = Matrix4F::UNIT;
	viewTransform = Matrix4F::UNIT;
	//OrthProjection = Matrix4F();
	//PersProjection = Matrix4F();
	this->sightNear = near;
	this->sightFar = far;
	this->fovyX = fovyX;
	this->fovyY = fovyY;
	UpdateProjectionTransform();

	moveSpeed = 0.5;
	lookSpeed = 1.0;
}

void Camera::SetWorldLocation(Vector3F location)
{
	Object::SetWorldLocation(location);
	viewTransform = transform.TransformInverse();
}

void Camera::SetWorldRotation(Rotator rotation)
{
	Object::SetWorldRotation(rotation);
	viewTransform = transform.TransformInverse();
}

void Camera::AddLocalLocation(Vector3F location)
{
	Object::AddLocalLocation(location);
	viewTransform = transform.TransformInverse();
}

void Camera::AddLocalRotation(Rotator rotation)
{
	Object::AddLocalRotation(rotation);
	viewTransform = transform.TransformInverse();
}

void Camera::Move(Vector3F direction)
{
	AddLocalLocation(direction * deltaMoveSpeed);
}
void Camera::Look(Rotator rotation)
{
	AddLocalRotation(rotation);
}

void Camera::UpdateProjectionTransform()
{
	float left = -sightNear * tan(fovyX * PI / 180 / 2);
	float right = sightNear * tan(fovyX * PI / 180 / 2);
	float top = sightNear * tan(fovyY * PI / 180 / 2);
	float bottom = -sightNear * tan(fovyY * PI / 180 / 2);

	projectionTransform[0][0] = sightNear / right;
	projectionTransform[1][1] = sightNear / top;
	projectionTransform[2][2] = -(sightFar + sightNear) / (sightFar - sightNear);
	projectionTransform[2][3] = (-2 * sightFar * sightNear) / (sightFar - sightNear);
	projectionTransform[3][2] = -1;

	/*
	orthProjection[0][0] = 2 / (near * tan(fovyX * PI / 180 / 2) * 2);
	orthProjection[1][1] = 2 / (near * tan(fovyY * PI / 180 / 2) * 2);
	orthProjection[2][2] = 2 / (far - near);
	orthProjection[2][3] = 2 / (far - near) * -(near + far) / 2;
	orthProjection[3][3] = 1;

	persProjection[0][0] = near;
	persProjection[1][1] = near;
	persProjection[2][2] = near + far;
	persProjection[2][3] = -near * far;
	persProjection[3][2] = 1;

	projectionTransform = Matrix4F::Product(orthProjection, persProjection);
	*/

	//projectionTransform.Print();
}

Matrix4F Camera::GetViewTransform()
{
	return viewTransform;
}

Matrix4F Camera::GetProjectionTransform()
{
	return projectionTransform;
}

void Camera::Tick(float deltaTime)
{
	Object::Tick(deltaTime);

	Vector3F v(10.0, 10.0, -2.0);

	deltaMoveSpeed = moveSpeed * deltaTime;
	deltaLookSpeed = lookSpeed * deltaTime;
}