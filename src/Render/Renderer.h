#pragma once

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Math/MathS.h"
#include "Mesh.h"
#include "Camera.h"

// 一级宏：将参数转为字符串
#define TO_STRING(x) #x

// 二级宏：确保参数完全展开后再转为字符串
#define EXPAND_AND_STRINGIFY(x) TO_STRING(x)

static const char* LoadFile(const char* path);

class Shader
{
public:
    enum VariableType
    {
        INT,
        FLOAT,
        VEC3,
        MAT4
    };
    unsigned int vertexShader;
    unsigned int fragmentShader;

    unsigned int shaderProgram;

    Shader();

    void SetVertexShader(const char* filePath);
    void SetFragmentShader(const char* filePath);

    void Set();
    void Use();

    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, Vector3F value);
    void SetUniform(const char* name, Matrix4F value);
    void SetUniform(const char* name, bool value);
};

class GLBinding
{
public:
    Mesh* mesh;
    unsigned int vao;
    unsigned int vboLocations;
    unsigned int vboTextures;
    unsigned int vboNormals;
    unsigned int vboPointIndices;
    unsigned int vboLineIndices;
    unsigned int vboTriangleIndices;
    unsigned int vboQuadIndices;

    GLBinding(Mesh* mesh);
    float* GetVerticesData();
    float* GetTexturesData();
    float* GetNormalsData();
    unsigned int* GetPointIndicesData();
    unsigned int* GetLineIndicesData();
    unsigned int* GetTriangleIndicesData();
    unsigned int* GetQuadIndicesData();
    void Update();
};

class Renderer
{
public:
	int width = 1200;
	int height = 600;

    Shader* shader;
    unsigned int polygonMode;

    std::vector<Sphere*> sourceMeshes;
	std::vector<Mesh*> meshes;
    std::vector<GLBinding> bindings;

    Camera* camera;

    Renderer();
    void InitBindings();

    Sphere* CreateSourceMesh(Vector3F location, float radius, int segments);
    void RenderMesh(GLBinding* binding);
    void Tick(float deltaTime);
};