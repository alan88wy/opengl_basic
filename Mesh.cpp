#include "Mesh.h"

Mesh::Mesh() {
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int noOfVertices, unsigned int noOfIndices) {

    indexCount = noOfIndices;

    glGenVertexArrays(1, &VAO);  // glGenVertexArrays(GLsizei n, GLuint* arrays); generate vertex array object names
    glBindVertexArray(VAO);      // bind a vertex array object

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * noOfIndices, indices, GL_STATIC_DRAW);


    glGenBuffers(1, &VBO);       // glGenBuffers(GLsizei n, GLuint* buffers); generate buffer object names
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // glBindBuffer(GLenum target, GLuint buffer); bind a named buffer object

    // glBufferData, glNamedBufferData — creates and initializes a buffer object's data store 
    // void glBufferData(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * noOfVertices, vertices, GL_STATIC_DRAW);

    // void glVertexAttribPointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride,
    //                            const void* pointer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); // define an array of generic vertex attribute data
    glEnableVertexAttribArray(0); // void glEnableVertexAttribArray(GLuint index); Enable or disable generic vertex attribute array

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind the buffer because we set it to 0

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);  // Remove after use. Unbind the buffer because we set it to 0
};

void Mesh::RenderMesh() {

    //if (VAO |= 0 && VBO != 0 && IBO != 0) {

    glBindVertexArray(VAO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    //}
};

void Mesh::ClearMesh() {

    if (IBO != 0) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }

    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        IBO = 0;
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        IBO = 0;
    }

    indexCount = 0;
};

Mesh::~Mesh() {
    ClearMesh();
};