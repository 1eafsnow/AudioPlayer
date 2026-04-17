#include "Object.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Vertex
{
public:
    Vector3F location;
    Vector3F normal;
    Vector3F texture;

    Vertex(Vector3F location = Vector3F(0, 0, 0), Vector3F normal = Vector3F(0, 0, 0), Vector3F texture = Vector3F(0, 0, 0)) : location(location), normal(normal), texture(texture) {}
};

class Mesh : public Object
{
public:
    std::vector<Vertex> vertices;

	std::vector<GLuint> points;
	std::vector<Vector2I> lines;
    std::vector<Vector3I> triangles;
    std::vector<Vector4I> quads;
};

class Sphere : public Mesh
{
public:
    float radius;
    int xSegments;
    int ySegments;

    void Init();
};

class WaveMesh : public Mesh
{
public:
    int vertexSize;
    int peakSize;
    std::vector<float> peakVelocity;

    WaveMesh(int peakSize);
	void SetPeak(int index, float value);

    virtual void Tick(float deltaTime) override;
};