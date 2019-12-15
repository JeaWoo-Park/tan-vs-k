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
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"


#define SPEED 0.1


void drawScene(GLvoid);
void Keyboard(unsigned char key, int x, int y);
void setshader();
void Timer(int);

GLuint complie_shaders();
GLuint complie_texture();



bool line = false;
GLuint nomal_shader_ID;
GLuint texture_shader_ID;
glm::vec3 cameraPos;
glm::vec3 cameraDirection;
glm::vec3 cameraUP;

struct bb {
	float left, top, right, bottom;
};

bb interbb;

bool collide(bb* interbb, bb a, bb b) {
	if (a.top <= b.bottom &&
		a.right >= b.left &&
		a.left <= b.right &&
		a.bottom >= b.top)
	{
		// top
		if (a.top > b.top)
			interbb->top = a.top;
		else
			interbb->top = b.top;
		// bottom
		if (a.bottom < b.bottom)
			interbb->bottom = a.bottom;
		else
			interbb->bottom = b.bottom;
		// left
		if (a.left > b.left)
			interbb->left = a.left;
		else
			interbb->left = b.left;
		if (a.right < b.right)
			interbb->right = a.right;
		else
			interbb->right = b.right;
		return true;
	}
	else
		return false;
}


void Set_bb(bb* rect, float left, float top, float right, float bottom) {
	rect->right = right;
	rect->top = top;
	rect->left = left;
	rect->bottom = bottom;

}



class Map {
private:
	unsigned int transformLocation;
	unsigned int viewLocation;
	unsigned int projectionLocation;
	unsigned int texture;
	glm::mat4 Matrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	GLuint VAO[2], VBOpos, VBOcolor, VBOtexture, EBO, VBO[2];
	GLuint ID = texture_shader_ID;
	float floor_data_list1[24] = {
	   -10, 0 ,-10,1,0,1,   0,1,
	   -10, 0, 10,   1,0,1,   0,0,
	   10, 0, -10,   1,0,1,   1,1
	};
	float floor_data_list2[24] = {
	   10, 0, 10,   1,0,1,   1,0,
	   10, 0, -10,   1,0,1,   1,1,
	   -10, 0, 10,   1,0,1,   0,0
	};

public:
	void initBuffer() {
		glUseProgram(ID);

		glGenVertexArrays(2, VAO);
		glGenBuffers(2, VBO);
		glBindVertexArray(VAO[0]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_data_list1), floor_data_list1, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(VAO[1]);

		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_data_list2), floor_data_list2, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	void inittexture() {
		int width, height, channel;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned char* data1 = stbi_load("texture3.jpg", &width, &height, &channel, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);


	}
	void Draw() {
		glUseProgram(complie_texture());


		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1280.0 / (float)800.0, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUP);
		Matrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		transformLocation = glGetUniformLocation(complie_texture(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(Matrix));

		viewLocation = glGetUniformLocation(texture_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(texture_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


		glBindVertexArray(VAO[0]);

		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(VAO[1]);

		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
};
class Obstacle {
private:
	glm::mat4 Matrix = glm::mat4(1.0f);

	glm::mat4 transMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	unsigned int transformLocation;
	unsigned int viewLocation;
	unsigned int projectionLocation;
	GLuint ID = nomal_shader_ID;
	GLuint VBOpos, VBOcolor, VAO;
	GLuint EBO;
	float obstacle_color_list[24] = {
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5,
	   0.5,0.5,0.5
	};
	float obstacle_vertex_list[24] = {
	   -1, 0.5, -1,
	   -1, 0.5, 1,
	   1, 0.5, 1,
	   1, 0.5, -1,
	   -1, 0, -1,
	   -1, 0, 1,
	   1, 0, 1,
	   1, 0, -1,
	};
	int obstacle_index_list[36] = {
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
public:
	float trans_X = 0;
	float trans_Y = 0;
	float trans_Z = 0;
	Obstacle(float x, float z) : trans_X(x), trans_Z(z) {
		Matrix = glm::translate(Matrix, glm::vec3(trans_X, trans_Y, trans_Z));
	}
	void initBuffer() {
		glUseProgram(ID);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBOcolor);
		glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_color_list), obstacle_color_list, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &VBOpos);
		glBindBuffer(GL_ARRAY_BUFFER, VBOpos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_vertex_list), obstacle_vertex_list, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obstacle_index_list), obstacle_index_list, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	}
	void Draw() {

		glUseProgram(ID);
		glBindVertexArray(VAO);

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1280.0 / (float)800.0, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUP);

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(Matrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	bb get_bb() const {
		return bb{ float(trans_X - 1), float(trans_Z - 1),float(trans_X + 1),float(trans_Z + 1) };
	}
};
class BrokenObstacle {
private:
	glm::mat4 Matrix = glm::mat4(1.0f);

	glm::mat4 transMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	unsigned int transformLocation;
	unsigned int viewLocation;
	unsigned int projectionLocation;
	GLuint ID = nomal_shader_ID;
	GLuint VBOpos, VBOcolor, VAO;
	GLuint EBO;
	float obstacle_color_list[24] = {
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8,
	   0.8,0.8,0.8
	};
	float obstacle_vertex_list[24] = {
	   -1, 0.5, -1,
	   -1, 0.5, 1,
	   1, 0.5, 1,
	   1, 0.5, -1,
	   -1, 0, -1,
	   -1, 0, 1,
	   1, 0, 1,
	   1, 0, -1,
	};
	int obstacle_index_list[36] = {
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
public:
	float trans_X = 0;
	float trans_Y = 0;
	float trans_Z = 0;
	BrokenObstacle(float x, float z) : trans_X(x), trans_Z(z) {
		Matrix = glm::translate(Matrix, glm::vec3(trans_X, trans_Y, trans_Z));
	}
	void initBuffer() {
		glUseProgram(ID);
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBOcolor);
		glBindBuffer(GL_ARRAY_BUFFER, VBOcolor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_color_list), obstacle_color_list, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &VBOpos);
		glBindBuffer(GL_ARRAY_BUFFER, VBOpos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(obstacle_vertex_list), obstacle_vertex_list, GL_STATIC_DRAW);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(obstacle_index_list), obstacle_index_list, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
	}
	void Draw() {

		glUseProgram(ID);
		glBindVertexArray(VAO);

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1280.0 / (float)800.0, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUP);

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(Matrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	bb get_bb() const {
		return bb{ float(trans_X - 1), float(trans_Z - 1),float(trans_X + 1),float(trans_Z + 1) };
	}
};
class Tank1 {
private:
	glm::mat4 HeadMatrix = glm::mat4(1.0f);
	glm::mat4 HeadMatrix_rotate = glm::mat4(1.0f);


	glm::mat4 BodyMatrix = glm::mat4(1.0f);
	glm::mat4 BodyMatrix_rotate = glm::mat4(1.0f);

	glm::mat4 transMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);

	unsigned int transformLocation;
	unsigned int viewLocation;
	unsigned int projectionLocation;
	GLuint ID = nomal_shader_ID;
	GLuint Barrel_VAO, Head_VAO, Body_VAO, Barrel_VBOpos, Barrel_VBOcolor, Head_VBOpos, Head_VBOcolor, Body_VBOpos, Body_VBOcolor;
	GLuint Barrel_EBO, Head_EBO, Body_EBO;

	float tank_barrel_vertex_list[24] = {
	0.133, 0.132, -0.033,   // 0
	0.133, 0.132, 0.033,   // 1
	0.733, 0.132, 0.033,   // 2
	0.733, 0.132, -0.033,   // 3
	0.166, 0.066, -0.033,   // 4
	0.166, 0.066, 0.033,   // 5
	0.733, 0.066, 0.033,   // 6
	0.733, 0.066, -0.033   // 7
	};
	float tank_head_vertex_list[24] = {
	   -0.1, 0.2, -0.066,
	   -0.1, 0.2, 0.066,
	   0.1, 0.2, 0.066,
	   0.1, 0.2, -0.066,
	   -0.2, 0.0, -0.132,
	   -0.2, 0.0, 0.132,
	   0.2, 0.0, 0.132,
	   0.2, 0.0, -0.132
	};
	float tank_body_vertex_list[60] = {
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
	float tank_barrel_color_list[24] = {
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0
	};
	float tank_head_color_list[24] = {
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0,
	   1,1,0
	};
	float tank_body_color_list[84] = {
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0
	};
	int tank_head_index_list[36] = {
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
	int tank_body_index_list[84] = {
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


public:


	float Head_rot = 135;

	float trans_X = 8;
	float trans_Y = 0.3;
	float trans_Z = 8;

	float Body_rot = 0;

	void initBuffer();

	void Draw_Barrel() {
		glUseProgram(ID);
		glBindVertexArray(Barrel_VAO);

		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1280.0 / (float)800.0, 0.1f, 100.0f);
		view = glm::lookAt(cameraPos, cameraDirection, cameraUP);
		HeadMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		HeadMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		HeadMatrix_rotate = glm::rotate(HeadMatrix_rotate, glm::radians(Head_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));


		HeadMatrix = transMatrix * HeadMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(HeadMatrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	void Draw_Head() {
		glUseProgram(ID);
		glBindVertexArray(Head_VAO);
		HeadMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		HeadMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		HeadMatrix_rotate = glm::rotate(HeadMatrix_rotate, glm::radians(Head_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));

		HeadMatrix = transMatrix * HeadMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(HeadMatrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	void Draw_Body() {
		glUseProgram(ID);
		glBindVertexArray(Body_VAO);
		BodyMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		BodyMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		BodyMatrix_rotate = glm::rotate(BodyMatrix_rotate, glm::radians(Body_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));

		BodyMatrix = transMatrix * BodyMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(BodyMatrix));
		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
		glDrawElements(GL_TRIANGLES, 84, GL_UNSIGNED_INT, 0);
	}

	Tank1(GLuint pID) : ID(pID) {

	}
	bb get_bb() const {
		return bb{ float(trans_X - 0.5), float(trans_Z - 0.5), float(trans_X + 0.5), float(trans_Z + 0.5) };
	}
	void Draw() {
		Draw_Barrel();
		Draw_Head();
		Draw_Body();
	}
};
void Tank1::initBuffer() {
	glUseProgram(ID);
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
class Tank2 {
private:
	glm::mat4 HeadMatrix = glm::mat4(1.0f);
	glm::mat4 HeadMatrix_rotate = glm::mat4(1.0f);


	glm::mat4 BodyMatrix = glm::mat4(1.0f);
	glm::mat4 BodyMatrix_rotate = glm::mat4(1.0f);

	glm::mat4 transMatrix = glm::mat4(1.0f);
	glm::mat4 scaleMatrix = glm::mat4(1.0f);
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	unsigned int transformLocation;
	unsigned int viewLocation;
	unsigned int projectionLocation;
	GLuint ID = nomal_shader_ID;
	GLuint Barrel_VAO, Head_VAO, Body_VAO, Barrel_VBOpos, Barrel_VBOcolor, Head_VBOpos, Head_VBOcolor, Body_VBOpos, Body_VBOcolor;
	GLuint Barrel_EBO, Head_EBO, Body_EBO;

	float tank_barrel_vertex_list[24] = {
	0.133, 0.132, -0.033,   // 0
	0.133, 0.132, 0.033,   // 1
	0.733, 0.132, 0.033,   // 2
	0.733, 0.132, -0.033,   // 3
	0.166, 0.066, -0.033,   // 4
	0.166, 0.066, 0.033,   // 5
	0.733, 0.066, 0.033,   // 6
	0.733, 0.066, -0.033   // 7
	};
	float tank_head_vertex_list[24] = {
	   -0.1, 0.2, -0.066,
	   -0.1, 0.2, 0.066,
	   0.1, 0.2, 0.066,
	   0.1, 0.2, -0.066,
	   -0.2, 0.0, -0.132,
	   -0.2, 0.0, 0.132,
	   0.2, 0.0, 0.132,
	   0.2, 0.0, -0.132,
	};
	float tank_body_vertex_list[60] = {
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
	float tank_barrel_color_list[24] = {
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0
	};
	float tank_head_color_list[24] = {
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0,
	   0,1,0
	};
	float tank_body_color_list[84] = {
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,1,
	   0,0.8,1,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0,
	   0,0.8,0
	};
	int tank_head_index_list[36] = {
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
	int tank_body_index_list[84] = {
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


public:
	float Head_rot = -45;

	float trans_X = -8;
	float trans_Y = 0.3;
	float trans_Z = -8;

	float Body_rot = 0;

	void initBuffer();

	void Draw_Barrel() {
		glUseProgram(ID);
		glBindVertexArray(Barrel_VAO);


		projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)1280.0 / (float)800.0, 0.1f, 100.0f);

		view = glm::lookAt(cameraPos, cameraDirection, cameraUP);

		HeadMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		HeadMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		HeadMatrix_rotate = glm::rotate(HeadMatrix_rotate, glm::radians(Head_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));

		HeadMatrix = transMatrix * HeadMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(HeadMatrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	void Draw_Head() {
		glUseProgram(ID);
		glBindVertexArray(Head_VAO);
		HeadMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		HeadMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		HeadMatrix_rotate = glm::rotate(HeadMatrix_rotate, glm::radians(Head_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));

		HeadMatrix = transMatrix * HeadMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(HeadMatrix));

		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}

	void Draw_Body() {
		glUseProgram(ID);
		glBindVertexArray(Body_VAO);
		BodyMatrix = glm::mat4(1.0f);   // 단위행렬로초기화 
		BodyMatrix_rotate = glm::mat4(1.0f);
		transMatrix = glm::mat4(1.0f);
		scaleMatrix = glm::mat4(1.0f);
		BodyMatrix_rotate = glm::rotate(BodyMatrix_rotate, glm::radians(Body_rot), glm::vec3(0, 1, 0));
		transMatrix = glm::translate(transMatrix, glm::vec3(trans_X, trans_Y, trans_Z));
		scaleMatrix = glm::scale(scaleMatrix, glm::vec3(1, 1, 1));

		BodyMatrix = transMatrix * BodyMatrix_rotate * scaleMatrix;

		transformLocation = glGetUniformLocation(complie_shaders(), "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(BodyMatrix));
		viewLocation = glGetUniformLocation(nomal_shader_ID, "viewTransform");
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

		projectionLocation = glGetUniformLocation(nomal_shader_ID, "projectionTransform");
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
		glDrawElements(GL_TRIANGLES, 84, GL_UNSIGNED_INT, 0);
	}

	Tank2(GLuint pID) : ID(pID) {

	}

	void Draw() {
		Draw_Barrel();
		Draw_Head();
		Draw_Body();
	}
	bb get_bb() const {
		return bb{ float(trans_X - 0.5), float(trans_Z - 0.5), float(trans_X + 0.5), float(trans_Z + 0.5) };
	}
};
void Tank2::initBuffer() {
	glUseProgram(ID);
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



Tank2 player2(nomal_shader_ID);
Tank1 player1(nomal_shader_ID);
Map map;
Obstacle ob[6] = { {-5,-5}, {-7,-5}, {-5,-7}, {5,5}, {7,5}, {5,7} };
BrokenObstacle bob[3]{ {0,0},{5,-5}, {-5,5} };

int main(int argc, char** argv) {
	//윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(50, 0);
	glutInitWindowSize(1280, 800);
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
	player1.initBuffer();
	player2.initBuffer();
	map.inittexture();
	map.initBuffer();
	for (int i = 0; i < 6; ++i) {
		ob[i].initBuffer();
	}
	for (int i = 0; i < 3; ++i) {
		bob[i].initBuffer();
	}
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1, Timer, 1);
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
GLuint complie_texture() {
	const char* vertexsource = filetobuf("texvertex.glsl");

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

	const char* fragmentsource = filetobuf("texture.glsl");
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
void setshader() {
	nomal_shader_ID = complie_shaders();
	texture_shader_ID = complie_texture();
}
void drawScene()
{


	glClearColor(1.0, 1.0, 1.0, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 640, 800);
	float x;
	float z;
	x = cos(glm::radians(player1.Head_rot));
	z = sin(glm::radians(player1.Head_rot));
	cameraPos = glm::vec3(-(2 * cos(-glm::radians(player1.Head_rot))) + player1.trans_X, player1.trans_Y + 0.5, -(2 * sin(-glm::radians(player1.Head_rot))) + player1.trans_Z);
	cameraDirection = glm::vec3(player1.trans_X, player1.trans_Y, player1.trans_Z);
	cameraUP = glm::vec3(0.0f, 0.5f, 0.0f);
	player1.Draw();
	player2.Draw();
	map.Draw();
	for (int i = 0; i < 6; ++i) {
		ob[i].Draw();
	}
	for (int i = 0; i < 3; ++i) {
		bob[i].Draw();
	}
	glViewport(640, 0, 640, 800);
	cameraPos = glm::vec3(-(2 * cos(-glm::radians(player2.Head_rot))) + player2.trans_X, player2.trans_Y + 0.5, -(2 * sin(-glm::radians(player2.Head_rot))) + player2.trans_Z);
	cameraDirection = glm::vec3(player2.trans_X, player2.trans_Y, player2.trans_Z);
	cameraUP = glm::vec3(0.0f, 1.0f, 0.0f);
	player1.Draw();
	player2.Draw();
	map.Draw();
	for (int i = 0; i < 6; ++i) {
		ob[i].Draw();
	}
	for (int i = 0; i < 3; ++i) {
		bob[i].Draw();
	}


	glutSwapBuffers();

}

void Keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'L':
		if (line) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			line = !line;
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			line = !line;
		}
	}

	glutPostRedisplay();
}
int Zdirection_1 = 0;
int Zdirection_2 = 0;
int Xdirection_1 = 0;
int Xdirection_2 = 0;
void Timer(int value) {
	// 장애물과 플레이어의 충돌
	for (int i = 0; i < 6; ++i) {
		if (collide(&interbb, player1.get_bb(), ob[i].get_bb())) {
			Set_bb(&interbb, 0, 0, interbb.right - interbb.left, interbb.bottom - interbb.top);
			//위 아래 충돌
			if (interbb.right > interbb.bottom)
			{
				if (player1.trans_Z < ob[i].trans_Z)
					player1.trans_Z -= interbb.bottom;
				else
					player1.trans_Z += interbb.bottom;
				
			}
			//오른쪽 왼쪽 충돌
			else
			{
				if (player1.trans_X < ob[i].trans_X)
					player1.trans_X -= interbb.right;
				else
					player1.trans_X += interbb.right;
				
			}
		}
		if (collide(&interbb, player2.get_bb(), ob[i].get_bb())) {
			Set_bb(&interbb, 0, 0, interbb.right - interbb.left, interbb.bottom - interbb.top);
			//위 아래 충돌
			if (interbb.right > interbb.bottom)
			{
				if (player2.trans_Z < ob[i].trans_Z)
					player2.trans_Z -= interbb.bottom;
				else
					player2.trans_Z += interbb.bottom;

			}
			//오른쪽 왼쪽 충돌
			else
			{
				if (player2.trans_X < ob[i].trans_X)
					player2.trans_X -= interbb.right;
				else
					player2.trans_X += interbb.right;

			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		if (collide(&interbb, player1.get_bb(), bob[i].get_bb())) {
			Set_bb(&interbb, 0, 0, interbb.right - interbb.left, interbb.bottom - interbb.top);
			//위 아래 충돌
			if (interbb.right > interbb.bottom)
			{
				if (player1.trans_Z < bob[i].trans_Z)
					player1.trans_Z -= interbb.bottom;
				else
					player1.trans_Z += interbb.bottom;

			}
			//오른쪽 왼쪽 충돌
			else
			{
				if (player1.trans_X < bob[i].trans_X)
					player1.trans_X -= interbb.right;
				else
					player1.trans_X += interbb.right;

			}
		}
		if (collide(&interbb, player2.get_bb(), bob[i].get_bb())) {
			Set_bb(&interbb, 0, 0, interbb.right - interbb.left, interbb.bottom - interbb.top);
			//위 아래 충돌
			if (interbb.right > interbb.bottom)
			{
				if (player2.trans_Z < bob[i].trans_Z)
					player2.trans_Z -= interbb.bottom;
				else
					player2.trans_Z += interbb.bottom;

			}
			//오른쪽 왼쪽 충돌
			else
			{
				if (player2.trans_X < bob[i].trans_X)
					player2.trans_X -= interbb.right;
				else
					player2.trans_X += interbb.right;

			}
		}
	}
	if (GetAsyncKeyState('W') & 0x8000) {

		player1.trans_X += (SPEED * cos(glm::radians(player1.Head_rot)));
		player1.trans_Z += -(SPEED * sin(glm::radians(player1.Head_rot)));
		Zdirection_1 = 1;
	}
	else if (GetAsyncKeyState('S') & 0x8000) {
		player1.trans_X += -(SPEED * cos(glm::radians(player1.Head_rot)));
		player1.trans_Z += (SPEED * sin(glm::radians(player1.Head_rot)));
		Zdirection_1 = -1;
	}
	else {
		Zdirection_1 = 0;
	}

	if (GetAsyncKeyState('A') & 0x8000) {
		player1.trans_X += -(SPEED * cos(glm::radians(player1.Head_rot - 90)));
		player1.trans_Z += (SPEED * sin(glm::radians(player1.Head_rot - 90)));
		Xdirection_1 = -1;
	}
	else if (GetAsyncKeyState('D') & 0x8000) {
		player1.trans_X += -(SPEED * cos(glm::radians(player1.Head_rot + 90)));
		player1.trans_Z += (SPEED * sin(glm::radians(player1.Head_rot + 90)));
		Xdirection_1 = 1;
	}
	else {
		Xdirection_1 = 0;
	}

	if (GetAsyncKeyState('V') & 0x8000) {
		player1.Head_rot += 3.0;
	}
	else if (GetAsyncKeyState('B') & 0x8000) {
		player1.Head_rot -= 3.0;
	}

	if (Zdirection_1 == 1) {
		if (Xdirection_1 == 0)
			player1.Body_rot = player1.Head_rot;
		if (Xdirection_1 == 1)
			player1.Body_rot = -45 + player1.Head_rot;
		if (Xdirection_1 == -1)
			player1.Body_rot = +45 + player1.Head_rot;
	}
	else if (Zdirection_1 == 0) {
		if (Xdirection_1 == 0)
			;
		if (Xdirection_1 == 1)
			player1.Body_rot = -90 + player1.Head_rot;
		if (Xdirection_1 == -1)
			player1.Body_rot = +90 + player1.Head_rot;
	}
	else if (Zdirection_1 == -1) {
		if (Xdirection_1 == 0)
			player1.Body_rot = -180 + player1.Head_rot;
		if (Xdirection_1 == 1)
			player1.Body_rot = -135 + player1.Head_rot;
		if (Xdirection_1 == -1)
			player1.Body_rot = -225 + player1.Head_rot;
	}

	//----------------------------------------------------------------
	if (GetAsyncKeyState(VK_UP) & 0x8000) {

		player2.trans_X += (SPEED * cos(glm::radians(player2.Head_rot)));
		player2.trans_Z += -(SPEED * sin(glm::radians(player2.Head_rot)));
		Zdirection_2 = 1;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		player2.trans_X += -(SPEED * cos(glm::radians(player2.Head_rot)));
		player2.trans_Z += (SPEED * sin(glm::radians(player2.Head_rot)));
		Zdirection_2 = -1;
	}
	else {
		Zdirection_2 = 0;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		player2.trans_X += -(SPEED * cos(glm::radians(player2.Head_rot - 90)));
		player2.trans_Z += (SPEED * sin(glm::radians(player2.Head_rot - 90)));
		Xdirection_2 = -1;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		player2.trans_X += -(SPEED * cos(glm::radians(player2.Head_rot + 90)));
		player2.trans_Z += (SPEED * sin(glm::radians(player2.Head_rot + 90)));
		Xdirection_2 = 1;
	}
	else {
		Xdirection_2 = 0;
	}
	if (Zdirection_2 == 1) {
		if (Xdirection_2 == 0)
			player2.Body_rot = player2.Head_rot;
		if (Xdirection_2 == 1)
			player2.Body_rot = -45 + player2.Head_rot;
		if (Xdirection_2 == -1)
			player2.Body_rot = +45 + player2.Head_rot;
	}
	else if (Zdirection_2 == 0) {
		if (Xdirection_2 == 0)
			;
		if (Xdirection_2 == 1)
			player2.Body_rot = -90 + player2.Head_rot;
		if (Xdirection_2 == -1)
			player2.Body_rot = 90 + player2.Head_rot;
	}
	else if (Zdirection_2 == -1) {
		if (Xdirection_2 == 0)
			player2.Body_rot = -180 + player2.Head_rot;
		if (Xdirection_2 == 1)
			player2.Body_rot = -135 + player2.Head_rot;
		if (Xdirection_2 == -1)
			player2.Body_rot = -225 + player2.Head_rot;
	}
	if (GetAsyncKeyState(0xBC) & 0x8000) {
		player2.Head_rot += 3.0;
	}
	else if (GetAsyncKeyState(0xBE) & 0x8000) {
		player2.Head_rot -= 3.0;
	}
	glutPostRedisplay(); // 화면재출력
	glutTimerFunc(1, Timer, 1);
}