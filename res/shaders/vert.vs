#version 460 core

layout (location = 0) out vec3 mColor;

struct VertexData
{
	float x, y;
};

vec3 colors[3] = {
	vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0)
};

layout (binding = 0) readonly buffer VertexBuffer { VertexData data[]; } vertices;

void main()
{
	VertexData vtx = vertices.data[gl_VertexIndex];
	mColor = colors[gl_VertexIndex];
	
	gl_Position = vec4(vtx.x, vtx.y, 0.0, 1.0);
}