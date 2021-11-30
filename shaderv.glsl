attribute vec2 position;
uniform mat4 MVP;

void main(void) {
	gl_Position = MVP * vec4(position.x, position.y, 0, 1);
}
          
          