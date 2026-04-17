#include "Renderer.h"

const char* LoadFile(const char* path)
{
    //printf("Load file: %s\n", path);
    FILE* file;
    file = fopen(path, "r");
    if (!file)
    {
        printf("Shader file not found!");
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    char* data = new char[size + 1];
    rewind(file);
    size = fread(data, 1, size, file);
    data[size] = '\0';
    fclose(file);
    return data;
}

Shader::Shader() : shaderProgram(glCreateProgram())
{

}

void Shader::SetVertexShader(const char* filePath)
{
    const char* buffer = LoadFile(filePath);
    //printf("%s\n", Buffer);
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &buffer, NULL);
    glCompileShader(vertexShader);

    int compileResult;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult)
    {
        char log[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", log);
    }
}

void Shader::SetFragmentShader(const char* filePath)
{
    const char* buffer = LoadFile(filePath);
    //printf("%s\n", Buffer);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &buffer, NULL);
    glCompileShader(fragmentShader);

    int compileResult;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compileResult);

    if (!compileResult)
    {
        char log[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, log);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", log);
    }
}

void Shader::Set()
{
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
}

void Shader::Use()
{
    glUseProgram(shaderProgram);
}

void Shader::SetUniform(const char* name, int value)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform1i(location, value);
}

void Shader::SetUniform(const char* name, float value)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform1f(location, value);
}

void Shader::SetUniform(const char* name, Vector3F value)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetUniform(const char* name, Matrix4F value)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(location, 1, GL_TRUE, value.Array());
}

void Shader::SetUniform(const char* name, bool value)
{
    int location = glGetUniformLocation(shaderProgram, name);
    glUniform1i(location, (int)value);
}

GLBinding::GLBinding(Mesh* mesh)
{
    this->mesh = mesh;

    float* verticesData = GetVerticesData();
    glGenBuffers(1, &vboLocations);
    glBindBuffer(GL_ARRAY_BUFFER, vboLocations);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->vertices.size(), verticesData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete verticesData;

    float* normalsData = GetNormalsData();
    glGenBuffers(1, &vboNormals);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->vertices.size(), normalsData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete normalsData;

    float* texturesData = GetTexturesData();
    glGenBuffers(1, &vboTextures);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->vertices.size(), texturesData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete texturesData;

    if (mesh->points.size())
    {
        unsigned int* indicesData = GetPointIndicesData();
        glGenBuffers(1, &vboPointIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboPointIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh->points.size(), indicesData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] indicesData;
    }

    if (mesh->lines.size())
    {
        unsigned int* indicesData = GetLineIndicesData();
        glGenBuffers(1, &vboLineIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboLineIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 2 * mesh->lines.size(), indicesData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] indicesData;
    }
    if (mesh->triangles.size())
    {
        unsigned int* indicesData = GetTriangleIndicesData();
        glGenBuffers(1, &vboTriangleIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboTriangleIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 3 * mesh->triangles.size(), indicesData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] indicesData;
    }
    if (mesh->quads.size())
    {
        unsigned int* indicesData = GetQuadIndicesData();
        glGenBuffers(1, &vboQuadIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboQuadIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 4 * mesh->quads.size(), indicesData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        delete[] indicesData;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, vboLocations);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
    glBindBuffer(GL_ARRAY_BUFFER, vboTextures);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);

    glBindVertexArray(0);
}

void GLBinding::Update()
{
    float* verticesData = GetVerticesData();
    glBindBuffer(GL_ARRAY_BUFFER, vboLocations);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->vertices.size(), verticesData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete verticesData;
}

float* GLBinding::GetVerticesData()
{
    float* array = new float[3 * mesh->vertices.size()];
    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        array[i * 3] = mesh->vertices[i].location.x;
        array[i * 3 + 1] = mesh->vertices[i].location.y;
        array[i * 3 + 2] = mesh->vertices[i].location.z;
        //printf("%f %f %f\n", array[i * 3], array[i * 3 + 1], array[i * 3 + 2]);
    }
    return array;
}

float* GLBinding::GetTexturesData()
{
    float* array = new float[2 * mesh->vertices.size()];
    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        array[i * 2] = mesh->vertices[i].texture.x;
        array[i * 2 + 1] = mesh->vertices[i].texture.y;
        //printf("%f %f\n", array[i * 2], array[i * 2 + 1]);
    }
    return array;
}

float* GLBinding::GetNormalsData()
{
    float* array = new float[3 * mesh->vertices.size()];
    for (int i = 0; i < mesh->vertices.size(); i++)
    {
        array[i * 3] = mesh->vertices[i].normal.x;
        array[i * 3 + 1] = mesh->vertices[i].normal.y;
        array[i * 3 + 2] = mesh->vertices[i].normal.z;
        //printf("%f %f %f\n", array[i * 3], array[i * 3 + 1], array[i * 3 + 2]);
    }
    return array;
}

unsigned int* GLBinding::GetPointIndicesData()
{
    unsigned int* array = new unsigned int[mesh->points.size()];
    for (int i = 0; i < mesh->points.size(); i++)
    {
        array[i] = mesh->points[i];
        //printf("%d %d %d\n", array[i * 3], array[i * 3 + 1], array[i * 3 + 2]);
    }
    return array;
}

unsigned int* GLBinding::GetLineIndicesData()
{
    unsigned int* array = new unsigned int[2 * mesh->lines.size()];
    for (int i = 0; i < mesh->lines.size(); i++)
    {
        array[i * 2] = mesh->lines[i][0];
        array[i * 2 + 1] = mesh->lines[i][1];
        //printf("%d %d %d\n", array[i * 3], array[i * 3 + 1], array[i * 3 + 2]);
    }
    return array;
}

unsigned int* GLBinding::GetTriangleIndicesData()
{
    unsigned int* array = new unsigned int[3 * mesh->triangles.size()];
    for (int i = 0; i < mesh->triangles.size(); i++)
    {
        array[i * 3] = mesh->triangles[i][0];
        array[i * 3 + 1] = mesh->triangles[i][1];
        array[i * 3 + 2] = mesh->triangles[i][2];
        //printf("%d %d %d\n", array[i * 3], array[i * 3 + 1], array[i * 3 + 2]);
    }
    return array;
}

unsigned int* GLBinding::GetQuadIndicesData()
{
    unsigned int* array = new unsigned int[4 * mesh->quads.size()];
    for (int i = 0; i < mesh->quads.size(); i++)
    {
        array[i * 4] = mesh->quads[i][0];
        array[i * 4 + 1] = mesh->quads[i][1];
        array[i * 4 + 2] = mesh->quads[i][2];
        array[i * 4 + 3] = mesh->quads[i][3];
    }
    return array;
}

Renderer::Renderer()
{
    /*
    if (!glfwInit())
    {
        printf("glfwInit failed\n");
    }

    window = glfwCreateWindow(900, 900, "GL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        printf("glfwCreateWindow failed\n");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    const GLubyte* rendererer = glGetString(GL_RENDERER); // get rendererer string
    const GLubyte* version = glGetString(GL_VERSION);   // version as a string
    printf("Renderer: %s\n", rendererer);
    printf("OpenGL version supported %s\n", version);

    glewInit();

    //glEnable(GL_ALPHA_TEST);
    //glAlphaFunc(GL_GREATER, 0.5);
    glEnable(GL_DEPTH_TEST);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_BLEND);  
    */
    
    shader = new Shader;
    std::string vsPath;
    std::string fsPath;
    #ifdef SHADER_DIR
    char shaderDir[] = EXPAND_AND_STRINGIFY(SHADER_DIR);
    vsPath.assign(shaderDir);
    fsPath.assign(shaderDir);
    #else
    //vsPath.assign("C:\\Project2\\Audio\\src\\Shader");
    //fsPath.assign("C:\\Project2\\Audio\\src\\Shader");
    #endif
    vsPath.append("/src/Shader/vs.glsl");
    fsPath.append("/src/Shader/fs.glsl");
    std::cout << "Vertex shader path: " << vsPath << std::endl;
    std::cout << "Fragment shader path: " << fsPath << std::endl;
    //system("pause");
    shader->SetVertexShader(vsPath.c_str());
    shader->SetFragmentShader(fsPath.c_str());
    shader->Set();

    camera = new Camera(0.01, 1000, 90, 90);
    camera->SetWorldLocation(Vector3F(0.0, 0.0, 0.0));
    //camera->AddLocalRotation(Vector3F(0.0, 0.0, 0.0));    
}

void Renderer::InitBindings()
{
    for (auto& mesh : meshes)
    {
        bindings.push_back(GLBinding(mesh));
    }
}

Sphere* Renderer::CreateSourceMesh(Vector3F location, float radius, int segments)
{
    Sphere* mesh = new Sphere;
    mesh->SetWorldLocation(location);
    mesh->radius = radius;
    mesh->xSegments = segments;
    mesh->ySegments = segments;
    mesh->Init();
    sourceMeshes.push_back(mesh);
	bindings.push_back(GLBinding(mesh));
    return mesh;
}

void Renderer::RenderMesh(GLBinding* binding)
{    
    shader->Use();

    shader->SetUniform("model_transform", binding->mesh->transform);
    shader->SetUniform("view_transform", camera->viewTransform);
    shader->SetUniform("projection_transform", camera->projectionTransform);      

    glBindVertexArray(binding->vao);    

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPointSize(1.0f);
    if (binding->mesh->points.size())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, binding->vboPointIndices);
        glDrawElements(GL_POINTS, binding->mesh->points.size(), GL_UNSIGNED_INT, 0);
    }
    if (binding->mesh->lines.size())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, binding->vboLineIndices);
        glDrawElements(GL_LINE_STRIP, 2 * binding->mesh->lines.size(), GL_UNSIGNED_INT, 0);
    }
    if (binding->mesh->triangles.size())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, binding->vboTriangleIndices);
        glDrawElements(GL_TRIANGLES, 3 * binding->mesh->triangles.size(), GL_UNSIGNED_INT, 0);
    }
    if (binding->mesh->quads.size())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, binding->vboQuadIndices);
        glDrawElements(GL_QUADS, 4 * binding->mesh->quads.size(), GL_UNSIGNED_INT, 0);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    
}

void Renderer::Tick(float deltaTime)
{
    for (auto& mesh : meshes)
    {
        mesh->Tick(deltaTime);
    }
    camera->Tick(deltaTime);

    //glDisable(GL_POLYGON_OFFSET_FILL);
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glDrawBuffer(GL_BACK);
    //glActiveTexture(GL_TEXTURE0);

    for (auto& binding : bindings)
    {
        binding.Update();
        RenderMesh(&binding);
    }
    //glBindTexture(GL_TEXTURE_2D, 0);
}