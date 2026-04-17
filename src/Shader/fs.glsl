#version 400

in vec3 Vertex;
in vec3 Normal;
in vec2 Texture;

out vec4 FragColor;

void main()
{
	vec3 normal = normalize(Normal);
	float c = Vertex.y / 10;
	FragColor = vec4(1.0, 1.0, 1.0, 0.0);
}