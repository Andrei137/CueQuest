#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h> 
#include <GL/freeglut.h>
#include "loadShaders.h"

// Identificatori OpenGL
GLuint
    VaoId,
    VboId,
    ColorBufferId,
    ProgramId,
    codColLocation;
GLfloat winWidth{ 750 }, winHeight{ 750 };
int codCol;     

void CreateVBO(void)
{
    // Varfuri
    GLfloat Vertices[] = {
        -0.8f, -0.8f, 0.0f, 1.0f,
         0.0f,  0.8f, 0.0f, 1.0f,
         0.8f, -0.8f, 0.0f, 1.0f
    };

    // Culori
    GLfloat Colors[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };

    // Creare VAO
    glGenVertexArrays(1, &VaoId);

    // Generare VAO si indexare catre VaoId
    glBindVertexArray(VaoId);

    // Creare buffer pentru varfuri
    glGenBuffers(1, &VboId);

    // Generare buffer si indexare catre VboId
    glBindBuffer(GL_ARRAY_BUFFER, VboId);

    // Setarea tipului de buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    // Se asociaza atributul pentru shader (0 = coordonate)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

    // Creare buffer pentru culoare
    glGenBuffers(1, &ColorBufferId);

    // Generare buffer si indexare catre ColorBufferId
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);

    // Setarea tipului de buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    // Se asociaza atributul pentru shader (1 = culoare)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void DestroyVBO(void)
{
    // Eliberarea atributelor din shadere
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    // Stergerea bufferelor pentru varfuri si culori
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &ColorBufferId);
    glDeleteBuffers(1, &VboId);

    // Eliberarea obiectelor de tip VAO
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
}

void CreateShaders(void)
{
    // Creare si compilarea obiectelor de tip shader
    // Vertex shader (.vert) afecteaza geometria scenei
    // Fragment shader (.frag) afecteaza culoarea pixelilor
    ProgramId = LoadShaders("shaders/example.vert", "shaders/example.frag");
    glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
    // Eliminarea obiectelor de tip shader
    glDeleteProgram(ProgramId);
}

void Initialize(void)
{
    // Culoarea de fond a ecranului
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    CreateVBO();
    CreateShaders();

    // Variabila uniforma pentru a "comunica" cu shaderele
    codColLocation = glGetUniformLocation(ProgramId, "codColShader");
}

void RenderFunction(void)
{
    glClear(GL_COLOR_BUFFER_BIT);

    codCol = 0;
    glUniform1i(codColLocation, codCol);
    glLineWidth(5.0);
    glDrawArrays(GL_LINE_LOOP, 0, 3);

    codCol = 1;
    glUniform1i(codColLocation, codCol);
    glEnable(GL_POINT_SMOOTH);
    glPointSize(20.0);
    glDrawArrays(GL_POINTS, 0, 3);
    glDisable(GL_POINT_SMOOTH);

    glFlush();
}

void Cleanup(void)
{
    DestroyShaders();
    DestroyVBO();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("CueQuest");

    glewInit(); 
    
    Initialize();
    glutDisplayFunc(RenderFunction);
    glutCloseFunc(Cleanup);

    glutMainLoop();

    return 0;
}
