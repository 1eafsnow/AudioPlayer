#version 400

layout(location = 0) in vec3 VertexLocation;
layout(location = 1) in vec3 NormalVector;
layout(location = 2) in vec2 TextureCoordinate;

out vec3 Vertex;
out vec3 Normal;
out vec2 Texture;

uniform mat4 model_transform;
uniform mat4 view_transform;
uniform mat4 projection_transform;

void main()
{
	vec4 vertex = model_transform * vec4(VertexLocation, 1.0);
	Vertex = vec3(vertex);
	gl_Position = projection_transform * view_transform * vertex;
	Normal = mat3(model_transform) * NormalVector;
	Texture = TextureCoordinate;
}