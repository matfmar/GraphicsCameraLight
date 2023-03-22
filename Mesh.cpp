//bardzo wa�na klasa przekazuj�ca dane obiekt�w (np. z CreateObjects()) do potoku (do vertex shadera)
#include "Mesh.h"

Mesh::Mesh()
{
	VAO = 0;    //vertex array object - ca�a przestrze� do rysowania
	VBO = 0;    //vertex buffer object - podzbi�r VAO z konretnymi punktami i parametrami
	IBO = 0;    //index buffer object - wykorzystuj� do rysowania index draws, czyli identyfikacj� tr�jk�t�w po indeksowanych wierzcho�kach - podzbi�r tych indeks�w
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
    indexCount = numOfIndices;

    glGenVertexArrays(1, &VAO);         //tworzy VAO (w rzeczywisto�ci rezerwuje pami�� na dany obiekt na karcie graficznej)
    glBindVertexArray(VAO);

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0])*numOfIndices, indices, GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*numOfVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0])*8, 0);  //tworzy interfejs wierzcho�k�w (x,y,z) do shadera (layout 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0])*3));   //tworzy interfejs wierzcho�k�w tekstur (u,v) (layout 1)
    glEnableVertexAttribArray(1);   
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));   //tworzy interfejs normalnych (x,y,z) (layout 2)
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::RenderMesh(bool lines) {
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    if (lines) {
        glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);       //je�li obiekt zbudowany z linii, to rysuje linie
    }
    else {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);   //je�li obiekt zbudowany z tr�jk�t�w, to rysuje tr�jk�ty
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::ClearMesh() {
    if (IBO != 0) {
        glDeleteBuffers(1, &IBO);
        IBO = 0;
    }
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
        VBO = 0;
    }
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
    indexCount = 0;
}

Mesh::~Mesh() {
    ClearMesh();
}
