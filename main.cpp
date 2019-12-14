#define _CRT_SECURE_NO_WARNINGS

#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <stdio.h>


void drawScene(GLvoid);
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void initBuffer();
void setshader();

float tank_barrel_vertex_list[] = {
	0.133, 0.132, -0.033,	// 0
	0.133, 0.132, 0.033,	// 1
	0.733, 0.132, 0.033,	// 2
	0.733, 0.132, -0.033,	// 3
	0.166, 0.066, -0.033,	// 4
	0.166, 0.066, 0.033,	// 5
	0.733, 0.066, 0.033,	// 6
	0.733, 0.066, -0.033	// 7
};
float tank_head_vertex_list[] = {
	-0.1, 0.2, -0.066,
	-0.1, 0.2, 0.066,
	0.1, 0.2, 0.066,
	0.1, 0.2, -0.066,
	-0.2, 0.0, -0.132,
	-0.2, 0.0, 0.132,
	0.2, 0.0, 0.132,
	0.2, 0.0, -0.132,
};
float tank_body_vertex_list[] = {
	-0.4, 0.0, -0.266,
	-0.4, 0.0, 0.266,//
	0.4, 0.0, 0.266,//
	0.4, 0.0, -0.266,
	-0.5, -0.3, -0.266,
	-0.5, -0.3, 0.266,//
	0.5, -0.3, 0.266,//
	0.5, -0.3, -0.266,
	-0.466, -0.2, -0.18,
	-0.466, -0.2, 0.18,
	0.466, -0.2, 0.18,
	0.466, -0.2, -0.18,
	-0.5, -0.3, -0.18,
	-0.5, -0.3, 0.18,
	0.5, -0.3, 0.18,
	0.5, -0.3, -0.18,
	//------------------
	-0.4, 0.0, -0.18,
	-0.4, 0.0, 0.18,
	0.4, 0.0, 0.18,
	0.4, 0.0, -0.18,
};
float tank_barrel_color_list[] = {
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0
};
float tank_head_color_list[] = {
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0,
	0,1,0
};
float tank_body_color_list[] = {
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0,
	0,0.8,0
};
int tank_head_index_list[] = {
	0,1,3,
	3,1,2,
	//-------------위
	5,2,1,
	5,6,2,
	//-------------앞
	4,6,5,
	6,4,7,
	//-------------아래
	0,3,7,
	0,7,4,
	//-------------뒤
	0,4,5,
	0,5,1,
	//-------------왼
	6,3,2,
	6,7,3
	//-------------오른
};

int tank_body_index_list[] = {
	1,5,2,
	2,5,6,
	3,1,2,
	0,1,3,
	3,7,0,
	0,7,4,
	2,6,18,
	18,6,14,
	18,10,19,
	19,10,11,
	19,15,3,
	3,15,7,
	0,4,16,
	16,4,12,
	16,8,17,
	17,8,9,
	17,13,1,
	1,13,5,
	8,12,11,
	11,12,15,
	9,8,10,
	10,8,11,
	10,14,9,
	9,14,13,
	7,15,4,
	4,15,12,
	14,6,13,
	13,6,5
};

int main(int argc, char** argv) {
	//윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("tan vs k");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";
	setshader();
	initBuffer();
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutReshapeFunc(Reshape);
	glutMainLoop();
}
char* filetobuf(const char* file) {
	FILE* fptr;
	long length;
	char* buf;

	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;

	return buf;
}
GLuint complie_shaders() {
	const char* vertexsource = filetobuf("vertex.glsl");

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);

	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR : vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return false;
	}

	const char* fragmentsource = filetobuf("fragment.glsl");
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR:fragment shader 컴파일 실패\n" << PlatformRoleSOHOServer << std::endl;
		return false;
	}
	GLuint ShaderProgramID = glCreateProgram();
	glAttachShader(ShaderProgramID, vertexShader);
	glAttachShader(ShaderProgramID, fragmentShader);

	glLinkProgram(ShaderProgramID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	glUseProgram(ShaderProgramID);

	return ShaderProgramID;
}


glm::mat4 rotMatrix = glm::mat4(1.0f);
unsigned int transformLocation;
GLuint Barrel_VAO, Head_VAO, Body_VAO, Barrel_VBOpos, Barrel_VBOcolor, Head_VBOpos, Head_VBOcolor, Body_VBOpos, Body_VBOcolor;
GLuint Barrel_EBO, Head_EBO, Body_EBO;
GLuint programID;
void setshader() {
	programID = complie_shaders();
}
void initBuffer() {
	glUseProgram(programID);
	glGenVertexArrays(1, &Barrel_VAO);
	glBindVertexArray(Barrel_VAO);

	glGenBuffers(1, &Barrel_VBOcolor);
	glBindBuffer(GL_ARRAY_BUFFER, Barrel_VBOcolor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_barrel_color_list), tank_barrel_color_list, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &Barrel_VBOpos);
	glBindBuffer(GL_ARRAY_BUFFER, Barrel_VBOpos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_barrel_vertex_list), tank_barrel_vertex_list, GL_STATIC_DRAW);

	glGenBuffers(1, &Barrel_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Barrel_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tank_head_index_list), tank_head_index_list, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	
	glEnableVertexAttribArray(0);


	//--------------------------------------------------
	glGenVertexArrays(1, &Head_VAO);
	glBindVertexArray(Head_VAO);

	glGenBuffers(1, &Head_VBOcolor);
	glBindBuffer(GL_ARRAY_BUFFER, Head_VBOcolor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_head_color_list), tank_head_color_list, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &Head_VBOpos);
	glBindBuffer(GL_ARRAY_BUFFER, Head_VBOpos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_head_vertex_list), tank_head_vertex_list, GL_STATIC_DRAW);

	glGenBuffers(1, &Head_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Head_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tank_head_index_list), tank_head_index_list, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	
	glEnableVertexAttribArray(0);
	//--------------------------------
	glGenVertexArrays(1, &Body_VAO);
	glBindVertexArray(Body_VAO);

	glGenBuffers(1, &Body_VBOcolor);
	glBindBuffer(GL_ARRAY_BUFFER, Body_VBOcolor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_body_color_list), tank_body_color_list, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(1);
	
	glGenBuffers(1, &Body_VBOpos);
	glBindBuffer(GL_ARRAY_BUFFER, Body_VBOpos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tank_body_vertex_list), tank_body_vertex_list, GL_STATIC_DRAW);
	
	glGenBuffers(1, &Body_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Body_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(tank_body_index_list), tank_body_index_list, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

	
	
	glEnableVertexAttribArray(0);

}
bool line = false;
float Y = 0;
float X = 0;
float Z = 0;
void Draw_Barrel() {
	glUseProgram(programID);
	glBindVertexArray(Barrel_VAO);
	rotMatrix = glm::mat4(1.0f);	// 단위행렬로초기화 
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(X), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Z), glm::vec3(0, 0, 1));
	transformLocation = glGetUniformLocation(complie_shaders(), "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(rotMatrix));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}
void Draw_Head() {
	glUseProgram(programID);
	glBindVertexArray(Head_VAO);
	rotMatrix = glm::mat4(1.0f);	// 단위행렬로초기화 
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(X), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Z), glm::vec3(0, 0, 1));

	transformLocation = glGetUniformLocation(complie_shaders(), "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(rotMatrix));
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void Draw_Body() {
	glUseProgram(programID);
	glBindVertexArray(Body_VAO);
	rotMatrix = glm::mat4(1.0f);	// 단위행렬로초기화 
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Y), glm::vec3(0, 1, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(X), glm::vec3(1, 0, 0));
	rotMatrix = glm::rotate(rotMatrix, glm::radians(Z), glm::vec3(0, 0, 1));

	transformLocation = glGetUniformLocation(complie_shaders(), "transform");
	glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(rotMatrix));
	glDrawElements(GL_TRIANGLES, 84, GL_UNSIGNED_INT, 0);
}


void drawScene()
{
	
	
	glClearColor(1.0, 1.0, 1.0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);

	Draw_Barrel();
	Draw_Head();
	Draw_Body();

	glutSwapBuffers();

}
void Reshape(int w, int h) {
	glViewport(0, 0, w, h);
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'l':
		if (line) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			line = !line;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			line = !line;
		}
		break;
		break;
	case 'y':
		Y += 5.0f;
		break;
	case 'Y':
		Y -= 5.0f;
		break;
	case 'x':
		X += 5.0f;
		break;
	case 'X':
		X -= 5.0f;
		break;
	case 'z':
		Z += 5.0f;
		break;
	case 'Z':
		Z -= 5.0f;
		break;
	}

	glutPostRedisplay();
}