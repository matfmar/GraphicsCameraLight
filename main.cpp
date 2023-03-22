#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "CommonValues.h"
#include "PointLight.h"

using namespace std;

//deklaracje tekstur, materiałów, świateł i dodatków
const float toRadians = 3.14 / 180.0f;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture metalTexture;
Texture woodTexture;

Material shinyMaterial;
Material dullMaterial;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];

//potrzebne do przeliczeń, aby nie było tak, że na szybszym procesorze kamera szybciej się poruszała
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// VERTEX SHADER
static const char* vShader = "Shaders/shader.vert.txt";     //tutaj jest kod vertex shadera
//FRAGMENT SHADER  
static const char* fShader = "Shaders/shader.frag.txt";     //tutaj jest kod fragment shadera

//wylicza wektory normalne do płaszczyzn - potrzebne do obsługi odbicia światła. Stosuję tu Phong shading, czyli uśrednienie normalnych na wierzchołkach.
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticesCount, unsigned int vLength, unsigned int normalOffset) {
    //TROJKATY NALEZY RYSOWAC ZGODNIE Z KIERUNKIEM WSKAZOWEK ZEGARA !!!!!!
    for (int i = 0; i < indiceCount; i += 3) {
        unsigned int in0 = indices[i] * vLength;
        unsigned int in1 = indices[i + 1] * vLength;
        unsigned int in2 = indices[i + 2] * vLength;
        glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
        glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
        glm::vec3 normal = glm::cross(v1, v2);
        normal = glm::normalize(normal);
        in0 += normalOffset;
        in1 += normalOffset;
        in2 += normalOffset;
        vertices[in0] += normal.x;
        vertices[in0 + 1] += normal.y;
        vertices[in0 + 2] += normal.z;
        vertices[in1] += normal.x;
        vertices[in1 + 1] += normal.y;
        vertices[in1 + 2] += normal.z;
        vertices[in2] += normal.x;
        vertices[in2 + 1] += normal.y;
        vertices[in2 + 2] += normal.z;
    }
    for (int i = 0; i < verticesCount / vLength; ++i) {
        unsigned int nOffset = i * vLength + normalOffset;
        glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
        vec = glm::normalize(vec);
        vertices[nOffset] = vec.x;
        vertices[nOffset + 1] = vec.y;
        vertices[nOffset + 2] = vec.z;
    }
}

//tworzy końcową ścianę, działanie analogiczne do CreateObjects()
void CreateGameOver() {
    unsigned int indicesGO[] = {
        0, 2, 1,
        0, 1, 3
    };
    GLfloat verticesGO[] = {
        -10.0f, -10.0f, 0.0f,    -10.0f, -10.0f,     0.0f, 0.0f, 0.0f,
        10.0f, 10.0f, 0.0f,     10.0f, 10.0f,     0.0f, 0.0f, 0.0f,
        -10.0f, 10.0f, 0.0f,    -10.0f, 10.0f,     0.0f, 0.0f, 0.0f,
        10.0f, -10.0f, 0.0f,     10.0f, -10.0f,     0.0f, 0.0f, 0.0f
    };
    calcAverageNormals(indicesGO, 6, verticesGO, 32, 8, 5);
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(verticesGO, indicesGO, 32, 6);
    meshList.push_back(obj1);
}

//tworzy graniastosłup , działanie analogiczne do CreateObjects()
void CreateGraniastoslup() {
    unsigned int  indicesGran[] = {
        0, 4, 3,
        1, 5, 4,
        2, 3, 5,
        0, 1, 4,
        1, 2, 5,
        2, 0, 3,
        0, 2, 1,
        3, 5, 4
    };
    GLfloat verticesGran[] = {
        -1.0f, 1.0f, 1.0f,      -1.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,       1.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        0.0, 1.0f, -1.0f,       0.0f, 1.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     -1.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f,      1.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        0.0, -1.0f, -1.0f,      0.0f, 1.0f,     0.0f, 0.0f, 0.0f
    };
    calcAverageNormals(indicesGran, 24, verticesGran, 48, 8, 5);
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(verticesGran, indicesGran, 48, 24);
    meshList.push_back(obj1);
}

//tworzy walec, działanie analogiczne do CreateObjects()
void CreateCyllinder(GLfloat d_angle) {
    int noOfVerticesOnCircle = (int)(360.0f / d_angle);
    int totalNoOfVertices = 2 * noOfVerticesOnCircle + 2;
    int totalNoOfTriangles = 4 * noOfVerticesOnCircle;

    GLfloat** verticesCyllinder = (GLfloat**)malloc(totalNoOfVertices * sizeof(GLfloat*));
    for (int i = 0; i < totalNoOfVertices; ++i)
        verticesCyllinder[i] = (GLfloat*)malloc(3 * sizeof(GLfloat));

    int** indicesCyllinder = (int**)malloc(totalNoOfTriangles * sizeof(int*));
    for (int i = 0; i < totalNoOfTriangles; ++i)
        indicesCyllinder[i] = (int*)malloc(3 * sizeof(GLfloat));

    GLfloat r = 1.0f;
    GLfloat upx = 0.0f, upy = 1.0f, upz = 0.0f;
    GLfloat downx = 0.0f, downy = -1.0f, downz = 0.0f;

    GLfloat angle = 0.0f;
    GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
    int newI = 0;
    for (int i = 0; i < noOfVerticesOnCircle; ++i) {
        z = r * cos(glm::radians(angle));
        x = r * sin(glm::radians(angle));
        verticesCyllinder[i][0] = x;
        verticesCyllinder[i][1] = 1.0f;
        verticesCyllinder[i][2] = z;
        angle += d_angle;
        newI = i;
    }
    newI++;

    angle = 0.0f;
    x = 0.0f; y = 0.0f; z = 0.0f;
    int vNewI = 0.0;
    for (int i = newI; i < (newI + noOfVerticesOnCircle); ++i) {
        z = r * cos(glm::radians(angle));
        x = r * sin(glm::radians(angle));
        verticesCyllinder[i][0] = x;
        verticesCyllinder[i][1] = -1.0f;
        verticesCyllinder[i][2] = z;
        angle += d_angle;
        vNewI = i;
    }
    vNewI++;
    int indexUp = vNewI;
    verticesCyllinder[vNewI][0] = upx;
    verticesCyllinder[vNewI][1] = upy;
    verticesCyllinder[vNewI][2] = upz;
    vNewI++;
    int indexDown = vNewI;
    verticesCyllinder[vNewI][0] = downx;
    verticesCyllinder[vNewI][1] = downy;
    verticesCyllinder[vNewI][2] = downz;

    int startGora = 0, contGora = 0;
    for (int i = 0; i < noOfVerticesOnCircle; ++i) {
        indicesCyllinder[i][0] = indexUp;
        indicesCyllinder[i][2] = i;
        if (i == (noOfVerticesOnCircle - 1)) {
            indicesCyllinder[i][1] = 0;
        }
        else {
            indicesCyllinder[i][1] = i + 1;
        }
        newI = i;
    }
    newI++;

    int startDol = newI, contDol = startDol;
    for (int i = newI; i < (newI + noOfVerticesOnCircle); ++i) {
        indicesCyllinder[i][0] = indexDown;
        indicesCyllinder[i][2] = i;
        if (i == (newI + noOfVerticesOnCircle - 1)) {
            indicesCyllinder[i][1] = newI;
        }
        else {
            indicesCyllinder[i][1] = i + 1;
        }
        vNewI = i;
    }
    vNewI++;

    int vvNewI = 0;
    for (int i = vNewI; i < (vNewI + noOfVerticesOnCircle); ++i) {
        indicesCyllinder[i][0] = contGora;
        indicesCyllinder[i][2] = contDol;
        if (i == (vNewI + noOfVerticesOnCircle) - 1) {
            indicesCyllinder[i][1] = startDol;
        }
        else {
            indicesCyllinder[i][1] = contDol + 1;
        }
        contGora++;
        contDol++;
        vvNewI = i;
    }
    vvNewI++;

    contGora = startGora;
    contDol = startDol;
    for (int i = vvNewI; i < (vvNewI + noOfVerticesOnCircle); ++i) {
        indicesCyllinder[i][0] = contGora;
        if (i == (vvNewI + noOfVerticesOnCircle) - 1) {
            indicesCyllinder[i][1] = startGora;
        }
        else {
            indicesCyllinder[i][1] = contGora + 1;
        }
        if (i == (vvNewI + noOfVerticesOnCircle) - 1) {
            indicesCyllinder[i][2] = startDol;
        }
        else {
            indicesCyllinder[i][2] = contDol + 1;
        }
        contGora++;
        contDol++;
    }

    GLfloat* verticesCyllinder2 = (GLfloat*)malloc(8*totalNoOfVertices * sizeof(GLfloat));
    unsigned int* indicesCyllinder2 = (unsigned int*)malloc(3*totalNoOfTriangles * sizeof(unsigned int));

    int run = 0, getter = 0, mark = 0;

    for (int i = 0; i < (totalNoOfVertices*8); ++i) {
        if (mark >= 0 && mark <= 2) {
            verticesCyllinder2[i] = verticesCyllinder[getter][run];
            run++;
            mark++;
            if (run == 3) {
                run = 0;
                getter++;
            }
        }
        else if (mark == 3) {
            verticesCyllinder2[i] = -1.0f;
            mark++;
        }
        else if (mark == 4) {
            verticesCyllinder2[i] = 1.0f;
            mark++;
        }
        else {
            verticesCyllinder2[i] = 0.0f;
            mark++;
            if (mark == 8) {
                mark = 0;
            }
        }
    }
    run = 0; getter = 0; mark = 0;
    for (int i = 0; i < (totalNoOfTriangles*3); ++i) {
        indicesCyllinder2[i] = indicesCyllinder[getter][run];
        run++;
        if (run == 3) {
            run = 0;
            getter++;
        }
    }

    calcAverageNormals(indicesCyllinder2, 3*totalNoOfTriangles, verticesCyllinder2, 8*totalNoOfVertices, 8, 5);
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(verticesCyllinder2, indicesCyllinder2, 8*totalNoOfVertices, 3*totalNoOfTriangles);
    meshList.push_back(obj1);

    free(verticesCyllinder2);
    free(indicesCyllinder2);
    for (int i = 0; i < totalNoOfVertices; ++i)
        free(verticesCyllinder[i]);
    free(verticesCyllinder);

    for (int i = 0; i < totalNoOfTriangles; ++i)
        free(indicesCyllinder[i]);
    free(indicesCyllinder);
}

//tworzy ulicę, działanie analogiczne do CreateObjects()
void CreateStreet() {
    unsigned int  indicesStreet[] = {
        0, 2, 1,
        0, 1, 3
    };
    GLfloat verticesStreet[] = {
        -0.5f, 0.0f, 0.0f,      -1.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, -20.0f,      1.0f, 20.0f,     0.0f, 0.0f, 0.0f,
        -0.5f, 0.0f, -20.0f,     -1.0f, 20.0f,     0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,       1.0f, -1.0f,     0.0f, 0.0f, 0.0f
    };
    calcAverageNormals(indicesStreet, 6, verticesStreet, 32, 8, 5);
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(verticesStreet, indicesStreet, 32, 6);
    meshList.push_back(obj1);
}

//tworzy 2 obiekty z linii, działanie analogiczne do CreateObjects()
void CreateLineObjects() {
    unsigned int indicesOstroslup[] = {
        0, 1,
        1, 2,
        2, 0,
        0, 3,
        1, 3,
        2, 3
    };
    GLfloat verticesOstroslup[] = {
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,        0.0f, 0.0f,     0.0f, 0.0f, 0.0f
    };

    unsigned int indicesProstopadloscian[] = {
        0, 1,
        1, 2,
        2, 3,
        0, 3,
        4, 5,
        5, 6,
        6, 7,
        4, 7,
        0, 4,
        1, 5,
        2, 6,
        3, 7
    };
    GLfloat verticesProstopadloscian[] = {
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, 1.0f,     0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,      0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f,       0.0f, 0.0f,     0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,       0.0f, 0.0f,     0.0f, 0.0f, 0.0f
    };

    calcAverageNormals(indicesOstroslup, 12, verticesOstroslup, 32, 8, 5);
    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(verticesOstroslup, indicesOstroslup, 32, 12);
    meshList.push_back(obj1);

    calcAverageNormals(indicesProstopadloscian, 24, verticesProstopadloscian, 64, 8, 5);
    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(verticesProstopadloscian, indicesProstopadloscian, 64, 24);
    meshList.push_back(obj2);
}

//tworzy 2 pełne ostrosłupy
void CreateObjects() {

    unsigned int indices[] = {          //trójkąty wg wierzchołków. wazne: zgodnie z kierunkiem wskazówek zegara, żeby dobrze normalne się policzyły
        0, 3, 1,
        1, 3, 2,
        2, 3, 0,
        0, 1, 2
    };

    GLfloat vertices[] = {              //wierzchołki. odp: x, y, z (współrzędne punktu), u, v (do mapowania tekstury), a, b, c (wektor normalny - uzupełniane przez funkcję)
    //x, y, z, u, v //u,v = tex coord.
        -1.0f, -1.0f, 0.0f,      0.0f, 0.0f,    0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f,       0.5f, 0.0f,    0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f,       1.0f, 0.0f,    0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,        0.5f, 1.0f,    0.0f, 0.0f, 0.0f
    };
    
    calcAverageNormals(indices, 12, vertices, 32, 8, 5);        //przelicza wektory normalne

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12); 
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12); 
    meshList.push_back(obj2);
}

//inicjalizuje shadery - rozpoczyna ich kompilację i konsolidację
void CreateShaders() {
    Shader* shader1 = new Shader();
    shader1->CreateFromFiles(vShader, fShader);
    shaderList.push_back(shader1);
}

//moja funkcja, która definiuje macierz translacji i przemnaża dotychczasową macierz modelu o nią, stworzona w oparciu o wzory z literatury
glm::mat4 translate(glm::mat4 model, glm::vec3 vector) {
    glm::mat4 result = glm::mat4(1.0f);
    result = {
        1, 0, 0, vector.x,
        0, 1, 0, vector.y,
        0, 0, 1, vector.z,
        0, 0, 0, 1
    };
    result = glm::transpose(result);
    glm::mat4 result2 = result * model;
    return result2;
}

//jw. tylko że macierz skalującą, stworzona w oparciu o wzory z literatury
glm::mat4 scale(glm::mat4 model, glm::vec3 vector) {
    glm::mat4 result = glm::mat4(1.0f);
    result = {
        vector.x, 0, 0, 0,
        0, vector.y, 0, 0,
        0, 0, vector.z, 0,
        0, 0, 0, 1
    };
    result = glm::transpose(result);
    glm::mat4 result2 = result * model;
    return result2;
}

//funkcja do inputu danych
void setParameter(float* par, const char* text) {
    std::cout << text << ": " << std::endl;
    std::string s("");
    std::cin >> s;
    if (s != "x") {
        *par = atof(s.c_str());
        std::cout << "Nowa wartosc: " << *par << std::endl;
    }
    return;
}

int main() {

    std::cout << "Witam w programie modelujacym pewna scene, kamere i oswietlenie" << std::endl;
    std::cout << "Autor: Mateusz Marzec, nr indeksu 315551" << std::endl;
   
    //SETTING PARAMS
    float movespeed(5.0f), turnspeed(40.0f);    //CAMERA
    float shinyintensity(1.0f), shinyshininess(256.0f), dullintensity(0.3f), dullshininess(2.0f);   //MATERIALS
    float dirlightambient(0.1f), dirlightdiffuse(0.3f), dirlightx(0.0f), dirlighty(-1.0f), dirlightz(0.0f); //DIRECTIONAL LIGHT (SUN)
    float plightr(0.0f), plightg(1.0f), plightb(0.0f), plightambient(0.1f), plightdiffuse(1.0f), plightx(1.0f), plighty(2.0f), plightz(-5.0f), plightcon(0.3f), plightlin(0.2f), plightexp(0.01f);   //POINT LIGHT

    char input('y'), input2('y');
    std::cout << "Czy chcesz pozmieniac parametry modelowania/wyswietlania ? <y/n>" << std::endl;
    std::cin >> input;
    if (input == 'y') {
        std::cout << "Aby zachowac domyslne ustawienie danego parametru (podane w nawiasie) wystarczy wpisac 'x'" << std::endl << std::endl;
        setParameter(&movespeed, "Podaj szybkosc ruchow liniowych (5.0)");
        setParameter(&turnspeed, "Podaj szybkosc ruchow obrotowych (40.0)");
        setParameter(&shinyintensity, "Podaj wspolczynnik odbicia kierunkowy materialu blyszczacego (1.0)");
        setParameter(&shinyshininess, "Podaj gladkosc materialu blyszczacego (256.0)");
        setParameter(&dullintensity, "Podaj wspolczynnik odbicia kierunkowy materialu matowego (0.3)");
        setParameter(&dullshininess, "Podaj gladkosc materialu matowego (2.0)");
        setParameter(&dirlightambient, "Podaj wspolczynnik tla swiatla ze slonca (0.1)");
        setParameter(&dirlightdiffuse, "Podaj wspolczynnik kierunkowy swiatla ze slonca (0.3)");
        setParameter(&dirlightx, "Podaj kierunek x promieni slonecznych (0.0)");
        setParameter(&dirlighty, "Podaj kierunek y promieni slonecznych (-1.0)");
        setParameter(&dirlightz, "Podaj kierunek z promieni slonecznych (0.0)");
        setParameter(&plightr, "Podaj (0-1) natezenie barwy czerwonej blizszego swiatla punktowego (0.0)");
        setParameter(&plightg, "Podaj (0-1) natezenie barwy zielonej blizszego swiatla punktowego (1.0)");
        setParameter(&plightb, "Podaj (0-1) natezenie barwy niebieskiej blizszego swiatla punktowego (0.0)");
        setParameter(&plightambient, "Podaj wspolczynnik tla blizszego swiatla punktowego (0.1)");
        setParameter(&plightdiffuse, "Podaj wspolczynnik kierunkowy blizszego swiatla punktowego (1.0)");
        setParameter(&plightx, "Podaj lokalizacje x blizszego swiatla punktowego (1.0)");
        setParameter(&plighty, "Podaj lokalizacje y blizszego swiatla punktowego (2.0)");
        setParameter(&plightz, "Podaj lokalizacje z blizszego swiatla punktowego (-5.0)");
        setParameter(&plightcon, "Podaj staly wspolczynnik w funkcji kwadratowej oslabienia swiatla punktowego (0.3)");
        setParameter(&plightlin, "Podaj liniowy wspolczynnik w funkcji kwadratowej oslabienia swiatla punktowego (0.2)");
        setParameter(&plightexp, "Podaj kwadratowy wspolczynnik w funkcji kwadratowej oslabienia swiatla punktowego (0.1)");
        std::cout << "Czy chcesz uruchomic automatyczny algorytm eliminacji elementow zaslonietych ? (domylsny z-buffer z biblioteki openGL) - NIE PODLEGA OCENIE: <y/n>" << std::endl;
        std::cin >> input2;
        if (input2 != 'n' && input2 != 'y') {
            std::cout << "Nie taka miala byc odpowiedz" << std::endl;
            return -1;
        }
    }
    else if (input != 'n') {
        std::cout << "Nie taka miala byc odpowiedz." << std::endl;
        return -1;
    }
        //END OF SETTING PARAMS

    mainWindow = Window(1024, 768);     //par. ekranu
    mainWindow.Initialize(input2);
    
    //funkcje tworzace mesh-e (siatki) dla wyswietlanych obiektow - ktore to potem sa renderowane
    CreateObjects();        //tworzy 2 pelne ostroslupy na 1. planie
    CreateLineObjects();    //tworzy ostroslup z prostopadloscianem z linii na 2. planie
    CreateStreet();         //tworzy ulice wzdluz ktorej sa obiekty
    CreateCyllinder(1.0f);  //tworzy walec (na 3. planie)
    CreateGraniastoslup();  //tworzy graniastoslup o podstawie trojkata (na 3. planie)
    CreateGameOver();       //tworzy sciane na samym koncu ulicy
    CreateCyllinder(1.0f);  //tworzy drugi walec (na 4. planie)

    CreateShaders();        //tworzy (kompiluje z plikow tekstowych - patrz folder z programem - vertex shader i fragment shader do obsluzenia bezpośrednio przez GPU

    //obiekt kamery
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, movespeed, turnspeed);

    //inicjalizacja tekstur (cegły, kamienie, metal i drewno)
    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTexture();
    dirtTexture = Texture("Textures/stone.png");
    dirtTexture.LoadTexture();
    metalTexture = Texture("Textures/metal.png");
    metalTexture.LoadTexture();
    woodTexture = Texture("Textures/wood.png");
    woodTexture.LoadTexture();

    //inicjalizacja materialow - pod katem ich wlasciwosci odbiejajacych swiatlo: gladki i matowy.
    shinyMaterial = Material(shinyintensity, shinyshininess);
    dullMaterial = Material(dullintensity, dullshininess);

    //inicjalizacja swiatla slonecznego, jest to swiatlo biale
    mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
        dirlightambient, dirlightdiffuse,
        dirlightx, dirlighty, dirlightz);

    unsigned int pointLightCount = 0;
    //pierwsze punktowe zrodlo swiatła - użytkownik może zmieniać jego parametry
    pointLights[0] = PointLight(plightr, plightg, plightb,
        plightambient, plightdiffuse,
        plightx, plighty, plightz,
        plightcon, plightlin, plightexp);
    pointLightCount++;
    //drugie punktowe żródło światła - niebieskie, istnieje niezależnie od użytkownika
    pointLights[1] = PointLight(0.0f, 0.0f, 1.0f,
        0.1f, 1.0f,
        1.0f, 2.0f, -15.0f,
        0.1f, 0.1f, 0.1f);
    pointLightCount++;

    //identyfikatory zmiennych typu uniform zdefiniowanych w shaderach
    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformKolor = 0, uniformTextureOnPar = 0, uniformEyePosition = 0,
        uniformSpecularIntensity = 0, uniformShininess = 0;

    //GLOWNA PETLA PROGRAMU - AZ SIE GO NIE ZAMKNIE
    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        lastTime = now;

        //GET AND HANDLE USER INPUT EVENTS
        glfwPollEvents();

        camera.keyControl(mainWindow.getKeys(), deltaTime);     //sprawdzenie czy klawisz sie nie wcisnal

        //CLEAR WINDOW
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);           //tlem jest czerń
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //wyjecie z shaderów identyfikatorów zmiennych uniform - bedzie potrzebne do renderowania obiektow
        shaderList[0]->UseShader();
        uniformModel = shaderList[0]->GetModelLocation();
        uniformProjection = shaderList[0]->GetProjectionLocation();
        uniformView = shaderList[0]->GetViewLocation();
        uniformKolor = shaderList[0]->GetKolorLocation();
        uniformTextureOnPar = shaderList[0]->GetTextureOnParLocation();
        uniformEyePosition = shaderList[0]->GetEyePositionLocation();
        uniformSpecularIntensity = shaderList[0]->GetSpecularIntensityLocation();
        uniformShininess = shaderList[0]->GetShininessLocation();

        //ustawia swiatla: jedno sloneczne kierunkowej i dwa punktowe (ale program dopuszcza tak naprawdę nieograniczoną ich ilość)
        shaderList[0]->SetDirectionalLight(&mainLight);
        shaderList[0]->SetPointLight(pointLights, pointLightCount);

        //macierz perspektywy i funkcja ją przygotowująca - napisane przeze mnie
        glm::mat4 projection(1.0f);
        projection = camera.zoom((GLfloat)mainWindow.getBufferWidth(), (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);

        //macierz modelująca, przekształcająca współrzędne związane z obiektem na współrzędne związane ze światem
        glm::mat4 model(1.0f);
        //wektor opisujacy kolor danego obiektu
        glm::vec4 kolor(0.0f, 0.0f, 0.0f, 1.0f);
        //parametr okreslajacy czy do danego obiektu bedzie uzywana tekstura
        GLint textureOn(0);
        
        glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));     //zaladowanie macierzy perspektywy do shadera
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix())); //zaladowanie do shadera (ale wpierw obliczenie) macierzy widoku
        glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);  //zaladowanie do shadera pozycji kamery

        //DEFINIOWANIE OBIEKTOW
        
        //ostroslup pelny 1 (lewa strona), świecący, metalowy
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));      //napisana przeze mnie funkcja definiujaca macierz skalujacą
        model = translate(model, glm::vec3(-1.2f, 0.0f, -2.5f));    //napisana przeze mnie funkcja definiująca macierz translacji (i mnożąca ją od razu przez dotychczasowy stan)
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));       //zaladowanie macierzy modelu do shadera
        textureOn = 1;      //texture yes 
        glUniform1i(uniformTextureOnPar, textureOn);
        metalTexture.UseTexture();                                  //inicjalizuje uzycie tekstury
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);      //inicjalizuje uzycie materialu dla danego obiektu odnosnie typu odbicia swiatla
        meshList[0]->RenderMesh(false);                         //wyswietlenie obiektu na ekranie

        //ostroslup pelny 2 (prawa strona), matowy, drewniany
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        model = translate(model, glm::vec3(1.2f, 0.0f, -2.5f));
        //kolor = glm::vec4(0.7f, 0.2f, 0.2f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 1;
        glUniform1i(uniformTextureOnPar, textureOn);
        woodTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[1]->RenderMesh(false);

        
        //ostroslupZlinii 1 (lewa strona), zielony
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
        model = translate(model, glm::vec3(-1.2f, 0.0f, -6.0f));
        kolor = glm::vec4(0.2f, 0.7f, 0.2f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 0;      //no texture 
        glUniform1i(uniformTextureOnPar, textureOn);
        //brickTexture.UseTexture();
        meshList[2]->RenderMesh(true);

        //prostopadloscianZlinii 1 (prawa strona), niebieski
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(0.5f, 0.5f, 1.0f));
        model = translate(model, glm::vec3(1.2f, 0.0f, -6.0f));
        kolor = glm::vec4(0.2f, 0.2f, 0.7f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 0;      //no texture 
        glUniform1i(uniformTextureOnPar, textureOn);
        //dirtTexture.UseTexture();
        meshList[3]->RenderMesh(true);
        
        
        //cylinder1 (prawa strona), szary, błyszczący, okrągły
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
        model = translate(model, glm::vec3(1.2f, 0.0f, -10.0f));
        kolor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 0;
        glUniform1i(uniformTextureOnPar, textureOn);
        //metalTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[5]->RenderMesh(false);

       
        //graniastoslup (lewa strona), szary, matowy, kanciasty
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
        model = translate(model, glm::vec3(-1.2f, 0.0f, -10.0f));
        kolor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 0;
        glUniform1i(uniformTextureOnPar, textureOn);
        //woodTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[6]->RenderMesh(false);
        
        
        //ulica
        model = glm::mat4(1.0f);
        //model = scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
        model = translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        //kolor = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 1;
        glUniform1i(uniformTextureOnPar, textureOn);
        dirtTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[4]->RenderMesh(false);

        //game_over (środek), tekstura
        model = glm::mat4(1.0f);
        //model = scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
        model = translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
        //kolor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 1;
        glUniform1i(uniformTextureOnPar, textureOn);
        brickTexture.UseTexture();
        dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[7]->RenderMesh(false);

        //cylinder2 (prawa strona), szary, błyszczący, okrągły
        model = glm::mat4(1.0f);
        model = scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = translate(model, glm::vec3(-0.7f, -1.0f, -15.0f));
        kolor = glm::vec4(0.8f, 0.8f, 0.8f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform4fv(uniformKolor, 1, glm::value_ptr(kolor));
        textureOn = 0;
        glUniform1i(uniformTextureOnPar, textureOn);
        //metalTexture.UseTexture();
        shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShininess);
        meshList[5]->RenderMesh(false);


        glUseProgram(0);                        //polecenie wykonania kodu z shaderow

        mainWindow.SwapBuffers();               //polecenie wyswietlenie danych z bufora (patrz wyżej) na ekran.
    }

    return 0;
}
