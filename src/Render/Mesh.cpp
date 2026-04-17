#include "Mesh.h"

void Sphere::Init()
{
	const float HalfPI = float(PI / 2.0);
	float dYaw = 2.0f * PI / float(xSegments);
	float dPitch = PI / float(ySegments);
	for (int i = 0; i <= xSegments; ++i)
	{
		float yaw = i * dYaw;
		float U = (float)i / (float)xSegments;
		for (int j = 0; j <= ySegments; ++j)
		{
			float pitch = -HalfPI - j * dPitch;
			float V = (float)j / (float)ySegments;
			float x = radius * cos(pitch) * cos(yaw);
			float y = radius * cos(pitch) * sin(yaw);
			float z = radius * sin(pitch);

			Vertex vertex(Vector3F(x, y, z), Vector3F(x, y, z).GetNormalized(), Vector3F(U, V));
			vertices.push_back(vertex);
		}
	}

	for (int i = 0; i < xSegments; ++i)
	{
		for (int j = 0; j < ySegments; ++j)
		{
			triangles.push_back(Vector3I(i * (ySegments + 1) + j, (i + 1) * (ySegments + 1) + j, i * (ySegments + 1) + j + 1));
			triangles.push_back(Vector3I(i * (ySegments + 1) + j + 1, (i + 1) * (ySegments + 1) + j, (i + 1) * (ySegments + 1) + j + 1));
		}
	}
}

WaveMesh::WaveMesh(int peakSize) : peakSize(peakSize)
{
	vertices.resize(peakSize * 3);

    for (int i = 0; i < peakSize; i++)
    {
        float x = (i - peakSize / 2) / (peakSize / 5.0);
        //printf("x: %f, y: %f\n", x, y);
        vertices[i].location = Vector3F(x, 0.0, -5.0);
        vertices[i + peakSize].location = Vector3F(x, 1.0, -5.0);
        vertices[i + peakSize * 2].location = Vector3F(x, 2.0, -5.0);
        if (i == peakSize - 1)
        {
            break;
        }
        quads.push_back(Vector4I(i, i + 1, i + peakSize + 1, i + peakSize));
		points.push_back(i + peakSize * 2);
    }
    peakVelocity.resize(peakSize);
}

void WaveMesh::SetPeak(int index, float value)
{
	vertices[index + peakSize].location.y = value;
    //vertices[index + peakSize * 2].location.y = value + 1.0;
}

void WaveMesh::Tick(float deltaTime)
{
    for (int i = 0; i < peakSize; i++)
    {                
        float h = vertices[i + peakSize * 2].location.y;
        if (h <= vertices[i + peakSize].location.y)
        {
            peakVelocity[i] = 10.0;
        }
        else
        {
            peakVelocity[i] += -9.81 * deltaTime;
        }
        h = vertices[i + peakSize * 2].location.y + peakVelocity[i] * deltaTime;
        vertices[i + peakSize * 2].location.y = h;
    }
}