#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iterator>
#include <string>

struct Point {
  GLfloat x;
  GLfloat y;
};

GLuint vbo;
GLuint program;
GLint position;
GLuint matrixID;
glm::mat4 mvp;
int bufferSize = 8 * 4096;

int pointsSize = 0;
Point p;

std::string loadFile(std::string file) {
  std::ifstream input(file);

  if (!input.good()) {
    std::cout << "File not found\n";
  }

  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

GLuint compileShader(GLuint type, const char *source) {
  GLuint shaderID = glCreateShader(type);
  glShaderSource(shaderID, 1, &source, nullptr);
  glCompileShader(shaderID);

  GLint compileStatus;
  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
  if (compileStatus != GL_TRUE) {
    GLsizei logLength = 0;
    GLchar message[1024];
    glGetShaderInfoLog(shaderID, 1024, &logLength, message);
    std::cerr << message << "\n";
  }

  return shaderID;
}

GLuint createProgram(std::string vertexShaderPath,
                     std::string fragmentShaderPath) {
  std::string vertexShaderSource = loadFile(vertexShaderPath);
  std::string fragmentShaderSource = loadFile(fragmentShaderPath);

  GLuint vertexShader =
      compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
  GLuint fragmentShader =
      compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  GLint programLinkStatus;
  glGetProgramiv(program, GL_LINK_STATUS, &programLinkStatus);
  if (programLinkStatus != GL_TRUE) {
    GLsizei logLength = 0;
    GLchar message[1024];
    glGetProgramInfoLog(program, 1024, &logLength, message);
    std::cerr << message << "\n";
  }

  return program;
}

GLuint createShader(GLenum type, const GLchar *src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);
  return shader;
}

bool init() {
  program = createProgram("./../shaderv.glsl", "./../shaderf.glsl");

  if (program == 0) {
    return false;
  }
  /* Enable blending */
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  position = glGetAttribLocation(program, "position");
  glm::mat4 proj =
      glm::ortho(0.0f, 1280.0f, 720.0f, 0.0f); // right, left, bottom, top
  glm::mat4 view(1.0f);                        // camera
  glm::mat4 model(1.0f);                       // model
  mvp = proj * view;
  matrixID = glGetUniformLocation(program, "MVP");

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
  return true;
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
    }
  }
}

void mouseMotion(int mx, int my) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  p = {(GLfloat)mx, (GLfloat)my};
  glBufferSubData(GL_ARRAY_BUFFER, pointsSize * sizeof(Point), 8, &p);
  pointsSize++;
  glutPostRedisplay();
}

void display() {
  glUseProgram(program);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw using the vertices in our vertex buffer object */
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(position);
  glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

  glDrawArrays(GL_LINE_STRIP, 0, pointsSize);

  glutSwapBuffers();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(1280, 720);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Paint-Freeglut");
  GLenum glew_status = glewInit();

  if (GLEW_OK != glew_status) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }
  if (init()) {
    glutDisplayFunc(display);
    // Call the function mouse() on mouse button pressed and mouseMotion() on
    // mouse move with pressed button
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);
    glutMainLoop();
  }

  glDeleteProgram(program);
}