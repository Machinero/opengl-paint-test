#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

struct Point {
  GLfloat x;
  GLfloat y;
};

GLuint ibo;
GLuint vbo;
GLuint program;
GLint position;
GLuint matrixID;
glm::mat4 mvp;
unsigned int pointsSize = 0;
unsigned int indiciesSize = 0;
static constexpr int VERTEX_BUFFER_SIZE = 4096 * sizeof(Point);
static constexpr int INDEX_BUFFER_SIZE = 4096 * sizeof(unsigned short);
Point lastPoint;
Point p;
static unsigned short pointIndicies[2];
bool mouseDownPressed = false;

std::vector<Point> points;
std::vector<unsigned int> indicies;

#define POINT_XY(point) \
  point.x, \
  point.y  \

std::string loadFile(std::string file) {
  std::ifstream input(file);

  if (!input.good()) {
    std::cout << "File not found\n";
  }

  return {std::istreambuf_iterator<char>(input),
          std::istreambuf_iterator<char>()};
}

void debugDraw()
{
  std::cout << "Points\n";
  for (auto x : points)
  {
    std::cout << x.x << " " << x.y << '\n';
  }

  std::cout << "Indicies\n";
  for (auto x : indicies)
  {
    std::cout << x << '\n';
  }
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

void initBuffers() {
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(position);
  glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_SIZE, nullptr,
               GL_DYNAMIC_DRAW);
}

void moveTo(int x, int y)
{
  p = {GLfloat(x), GLfloat(y)};
  glBufferSubData(GL_ARRAY_BUFFER, pointsSize * sizeof(Point), sizeof(Point), &p);
  points.push_back(p);

  pointIndicies[1] = pointsSize;
  indicies.push_back(pointIndicies[0]);
  indicies.push_back(pointIndicies[1]);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indiciesSize * sizeof(unsigned short), sizeof(unsigned short) * 2, pointIndicies);
  pointsSize++;
  indiciesSize += 2;
  pointIndicies[0] = pointIndicies[1];

  std::cout << "Points: " << pointsSize << " Indicies: " << indiciesSize << '\n';
  glutPostRedisplay();
}

void setPoint(int x, int y)
{
  p = {GLfloat(x), GLfloat(y)};
  points.push_back(p);
  glBufferSubData(GL_ARRAY_BUFFER, pointsSize * sizeof(Point), sizeof(Point), &p);
  pointIndicies[0] = pointsSize;
  pointsSize++;
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

  initBuffers();
  // setPoint(500, 500);
  // moveTo(600, 600);
  // moveTo(600, 500);
  // moveTo(700, 500);

  // setPoint(300, 300);
  // moveTo(300, 400);
  // moveTo(350, 550);
  // moveTo(0, 0);

  return true;
}

void mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      std::cout << "Button down " << x << " " << y << '\n';
      setPoint(x, y);
    }
  }
}

void mouseMotion(int mx, int my) {
  // if (mouseDownPressed)
  // {
    moveTo(mx, my);
  // }
}

void display() {
  glUseProgram(program);
  glClear(GL_COLOR_BUFFER_BIT);

  /* Draw using the vertices in our vertex buffer object */

  glUniformMatrix4fv(matrixID, 1, GL_FALSE, &mvp[0][0]);

  // glDrawArrays(GL_POINTS, 0, pointsSize);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(GL_LINES, indiciesSize, GL_UNSIGNED_SHORT, nullptr);

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