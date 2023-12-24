#pragma once

#include <GL/glew.h>

class Mesh
{
	public:
		Mesh();

		void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int noOfVertices, unsigned int noOfIndices);
		void RenderMesh();
		void ClearMesh();

		~Mesh();

	private:
		// VAO - Vertex array object
		//       A vertex array object (also known as VAO) can be bound just like a vertex buffer 
		//       object and any subsequent 
		//       vertex attribute calls from that point on will be stored inside the VAO.
		// VBO - Vertex Buffer Object (VBO) to store vertex data in GPU memory

		GLuint VAO, VBO, IBO;
		GLuint indexCount;
};

