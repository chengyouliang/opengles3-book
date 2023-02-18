#include <stdlib.h>
#include <math.h>
#include "esUtil.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0 //shader layout loc
#define VERTEX_COLOR_INDX     1 //shader layout loc

#define VERTEX_STRIDE         (sizeof(GLfloat)*(VERTEX_POS_SIZE+VERTEX_COLOR_SIZE))
typedef struct vao_sample
{
     // Handle to a program object
    GLuint programObject;
    GLuint  m_VaoId;
	GLuint  m_VboIds[2];
}UserData;

int  Init(ESContext *esContext)
{
     UserData *userData = esContext->userData;
     printf("%s %d\n",__FUNCTION__,__LINE__);
	const char vShaderStr[] =
			"#version 300 es                            \n"
			"layout(location = 0) in vec4 a_position;   \n"
			"layout(location = 1) in vec4 a_color;      \n"
			"out vec4 v_color;                          \n"
			"out vec4 v_position;                       \n"
			"void main()                                \n"
			"{                                          \n"
			"    v_color = a_color;                     \n"
			"    gl_Position = a_position;              \n"
			"    v_position = a_position;               \n"
			"}";


	const char fShaderStr[] =
			"#version 300 es\n"
			"precision mediump float;\n"
			"in vec4 v_color;\n"
			"in vec4 v_position;\n"
			"out vec4 o_fragColor;\n"
			"void main()\n"
			"{\n"
			"    float n = 10.0;\n"
			"    float span = 1.0 / n;\n"
			"    int i = int((v_position.x + 0.5)/span);\n"
			"    int j = int((v_position.y + 0.5)/span);\n"
			"\n"
			"    int grayColor = int(mod(float(i+j), 2.0));\n"
			"    if(grayColor == 1)\n"
			"    {\n"
			"        float luminance = v_color.r*0.299 + v_color.g*0.587 + v_color.b*0.114;\n"
			"        o_fragColor = vec4(vec3(luminance), v_color.a);\n"
			"    }\n"
			"    else\n"
			"    {\n"
			"        o_fragColor = v_color;\n"
			"    }\n"
			"}";

	// 4 vertices, with(x,y,z) ,(r, g, b, a) per-vertex
	GLfloat vertices[4 *(VERTEX_POS_SIZE + VERTEX_COLOR_SIZE )] =
			{
					-0.5f,  0.5f, 0.0f,       // v0
					1.0f,  0.0f, 0.0f, 1.0f,  // c0
					-0.5f, -0.5f, 0.0f,       // v1
					0.0f,  1.0f, 0.0f, 1.0f,  // c1
					0.5f, -0.5f, 0.0f,        // v2
					0.0f,  0.0f, 1.0f, 1.0f,  // c2
					0.5f,  0.5f, 0.0f,        // v3
					0.5f,  1.0f, 1.0f, 1.0f,  // c3
			};
	// Index buffer data
	GLushort indices[6] = { 0, 1, 2, 0, 2, 3};
    printf("%s %d\n",__FUNCTION__,__LINE__);
	userData->programObject =esLoadProgram(vShaderStr, fShaderStr);

	// Generate VBO Ids and load the VBOs with data
	glGenBuffers(2, userData->m_VboIds);
	glBindBuffer(GL_ARRAY_BUFFER, userData->m_VboIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    printf("%s %d\n",__FUNCTION__,__LINE__);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->m_VboIds[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Generate VAO Id
	glGenVertexArrays(1, &userData->m_VaoId);
	glBindVertexArray(userData->m_VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, userData->m_VboIds[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->m_VboIds[1]);

	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glEnableVertexAttribArray(VERTEX_COLOR_INDX);

	glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)0);
	glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *)(VERTEX_POS_SIZE *sizeof(GLfloat)));
    printf("%s %d\n",__FUNCTION__,__LINE__);
	glBindVertexArray(GL_NONE);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    printf("%s %d\n",__FUNCTION__,__LINE__);
	if(userData->programObject == 0) return;
     printf("%s %d\n",__FUNCTION__,__LINE__);
	glUseProgram(userData->programObject);
	glBindVertexArray(userData->m_VaoId);
     printf("%s %d\n",__FUNCTION__,__LINE__);
	// Draw with the VAO settings
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *)0);
     printf("%s %d\n",__FUNCTION__,__LINE__);
	// Return to the default VAO
	glBindVertexArray(GL_NONE);
}

///
// Cleanup
//
void Shutdown ( ESContext *esContext )
{
    UserData *userData = esContext->userData;
    glDeleteProgram(userData->programObject);
    glDeleteBuffers(2, userData->m_VboIds);
    glDeleteVertexArrays(1, &userData->m_VaoId);
}

int esMain ( ESContext *esContext )
{   printf("%s %d\n",__FUNCTION__,__LINE__);
   esContext->userData = malloc ( sizeof ( UserData ) );
    printf("%s %d\n",__FUNCTION__,__LINE__);
   esCreateWindow ( esContext, "vao", 640, 480, ES_WINDOW_RGB);
     printf("%s %d\n",__FUNCTION__,__LINE__);
   if ( !Init ( esContext ) )
   {
      return GL_FALSE;
   }
     printf("%s %d\n",__FUNCTION__,__LINE__);
   esRegisterShutdownFunc ( esContext, Shutdown );
   //esRegisterUpdateFunc ( esContext, Update );
   esRegisterDrawFunc ( esContext, Draw );
   return GL_TRUE;
}
