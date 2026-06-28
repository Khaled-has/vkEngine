#version 460 core

struct VertexData
{
	float x, y;
};

layout (binding = 0) readonly buffer VertexBuffer { VertexData data[]; } vertices;

void main()
{
	VertexData vtx = vertices.data[gl_VertexIndex];

	gl_Position = vec4(vtx.x, vtx.y, 0.0, 1.0);
}