#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"
#include "../shader_lib/shader.h"
#include "glm/glm.h"
#include "Image.hpp"
#include <iostream>

//number of textures desired, you may want to change it to get bonus point
#define TEXTURE_NUM 14
//directories of image files
char* texture_name[TEXTURE_NUM] = {
	"../Resources/wallTexture.bmp", // 0
	"../Resources/cabinetTexture.bmp", // 1
	"../Resources/pmucha.bmp", // 2
	"../Resources/pkiss.bmp", // 3
	"../Resources/ppiano.bmp", // 4
	"../Resources/pstar.bmp", // 5
	"../Resources/wall9.bmp", // 6
	"../Resources/wall12.bmp", // 7
	"../Resources/face24.bmp", // 8   //better: face6, face7, face24
	//"../Resources/teapotTexture.bmp", // 8
	"../Resources/pmucha_word_r.bmp", // 9
	"../Resources/pkiss_word_r.bmp", // 10
	"../Resources/ppiano_word_r.bmp", // 11
	"../Resources/pstar_word_r.bmp", // 12
	"../Resources/door.bmp", // 13
};
//texture id array
GLuint texture[TEXTURE_NUM];


void cabinet(float a, float b);
void skybox(float a, float b);
void painting();
void painting_words();

extern "C"
{
	#include "glm_helper.h"
}

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;
GLuint program;

//final
GLuint programPhong;
GLuint programDissolve;
GLuint programRamp;
GLuint programBonus;
int count = 0;
void cameraRangeDetect();

GLuint vaoHandle;
GLuint vbo_ids[3];
Vertex *vertices;
void renderFirst();
void renderSecond();
void renderThird(float a, float b, float c);
GLfloat mycolor = 0;
GLfloat frequency = 2;
bool renderBool = true;

//light1(只能關燈)
bool lamp = false;
float LightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Light position
float LightAmb[] = { 0.5f, 0.5f, 0.5f, 1.0f };//Ambient Light Values
float LightDif[] = { 0.0f, 0.0f, 0.0f, 1.0f };//Diffuse Light Values
float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values
//float LightDif[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Diffuse Light Values
//float LightSpc[] = { 1.0f, 1.0f, 1.0f, 1.0f };//Specular Light Values

//no need to modify the following function declarations and gloabal variables
void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	//char *obj_file_dir = "../Resources/Ball.obj";
	char *obj_file_dir = "../Resources/bunny.obj";
	char *bunny_file_dir = "../Resources/bunny.obj";
	char *teapot_file_dir = "../Resources/teapot.obj";
	char *main_tex_dir = "../Resources/honey_comb_master.ppm";
	//char *main_tex_dir = "../Resources/Stone.ppm";
	char *noise_tex_dir = "../Resources/Noise.ppm";
	char *ramp_tex_dir = "../Resources/Ramp.ppm";
	
	
	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = -5.28;//theta in degree
	float eyep = 286.8;//phi in degree
	//float eyet = 0.0;//theta in degree
	//float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

const float speed = 0.03; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed

GLuint mainTextureID; // TA has already loaded this texture for you
GLuint noiseTextureID; // TA has already loaded this texture for you
GLuint rampTextureID; // TA has already loaded this texture for you
GLMmodel *model, *bunnyModel, *teapotModel; //TA has already loaded the model for you(!but you still need to convert it to VBO(s)!)
GLuint depthFrameBuffer;
GLuint depthTextureID;
GLuint renderedTexture;
GLuint honeyTextureID;

float eyex = -1.64;
float eyey = 0.8;//0.94;
float eyez = -11.5;
GLfloat eye[] = { 0, 0, 0 };

GLfloat light_pos[] = { 7.16923, 0.252734, -12.9211 };//{ 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

#define deltaTime (10) // in ms (1e-3 second)
float time;

void Tick(int id)
{
	double d = deltaTime / 1000.0;
	time += d;

	glutPostRedisplay();
	glutTimerFunc(deltaTime, Tick, 0); // 100ms for passTime step size
}

int main(int argc, char *argv[])
{

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// remember to replace "YourStudentID" with your own student ID
	glutCreateWindow("CG_FinalProject");
	glutReshapeWindow(512, 512);

	glewInit();
	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutTimerFunc(deltaTime, Tick, 0); //pass Timer function
	glutMainLoop();

	glmDelete(model);
	return 0;
}

void init(void)
{
	
	//add honey texture
	honeyTextureID = loadTexture(main_tex_dir, 512, 256);

	//renderedTexture:https://www.khronos.org/opengl/wiki/Framebuffer_Object_Extension_Examples#Quick_example.2C_render_to_texture_.282D_Depth_texture_ONLY.29		
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	//depthTextureID:https://www.khronos.org/opengl/wiki/Framebuffer_Object_Extension_Examples#Quick_example.2C_render_to_texture_.282D_Depth_texture_ONLY.29		
	glGenTextures(1, &depthTextureID);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 512, 512, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	//renderFrameBuffer(創建FBO):https://www.itread01.com/articles/1476697216.html		
	glGenFramebuffers(1, &depthFrameBuffer);//產生一個我們自己的幀緩衝
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);//綁定自己的幀緩衝
	//texture綁到FBO上		
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureID, 0);
	//檢查幀緩衝:https://learnopengl-cn.readthedocs.io/zh/latest/04%20Advanced%20OpenGL/05%20Framebuffers/		
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);//綁定幀緩衝到0 = 激活"默認"幀緩衝，沒有這一個步驟，在自己的幀緩衝裡面做的渲染操作都會沒有辦法呈現在畫面上:http://blog.shenyuanluo.com/LearnOpenGLNote25.html

	//HW3↑

	//final project
	//ref:http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
	Image* wallTex = loadTexture(texture_name[0]);
	Image* cabinetTex = loadTexture(texture_name[1]);
	Image* pmucha = loadTexture(texture_name[2]);//mucha
	Image* pkiss = loadTexture(texture_name[3]);//kiss
	Image* ppiano = loadTexture(texture_name[4]);//piano
	Image* pstar = loadTexture(texture_name[5]);//star
	Image* upWallTex = loadTexture(texture_name[6]);//upWall
	Image* downWallTex = loadTexture(texture_name[7]);//downWall
	Image* teapotTex = loadTexture(texture_name[8]);//teapot
	Image* pmucha_word = loadTexture(texture_name[9]);//mucha_word
	Image* pkiss_word = loadTexture(texture_name[10]);//kiss_word
	Image* ppiano_word = loadTexture(texture_name[11]);//piano_word
	Image* pstar_word = loadTexture(texture_name[12]);//star_word
	Image* door = loadTexture(texture_name[13]);//door

	mainTextureID = loadTexture(main_tex_dir, 512, 256);
	noiseTextureID = loadTexture(noise_tex_dir, 360, 360);
	rampTextureID = loadTexture(ramp_tex_dir, 256, 256);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(14, texture);//ref:https://www.youtube.com/watch?v=N9MnV7GznQ8
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wallTex->sizeX, wallTex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, wallTex->data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cabinetTex->sizeX, cabinetTex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, cabinetTex->data);
	
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pmucha->sizeX, pmucha->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pmucha->data);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pkiss->sizeX, pkiss->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pkiss->data);

	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ppiano->sizeX, ppiano->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, ppiano->data);

	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pstar->sizeX, pstar->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pstar->data);

	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, upWallTex->sizeX, upWallTex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, upWallTex->data);

	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, downWallTex->sizeX, downWallTex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, downWallTex->data);

	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, teapotTex->sizeX, teapotTex->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, teapotTex->data);

	// words
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pmucha_word->sizeX, pmucha_word->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pmucha_word->data);

	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pkiss_word->sizeX, pkiss_word->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pkiss_word->data);

	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ppiano_word->sizeX, ppiano_word->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, ppiano_word->data);

	glBindTexture(GL_TEXTURE_2D, texture[12]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pstar_word->sizeX, pstar_word->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, pstar_word->data);

	glBindTexture(GL_TEXTURE_2D, texture[13]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, door->sizeX, door->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, door->data);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);

	bunnyModel = glmReadOBJ(bunny_file_dir);

	glmUnitize(bunnyModel);
	glmFacetNormals(bunnyModel);
	glmVertexNormals(bunnyModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(bunnyModel);

	teapotModel = glmReadOBJ(teapot_file_dir);

	glmUnitize(teapotModel);
	glmFacetNormals(teapotModel);
	glmVertexNormals(teapotModel, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(teapotModel);

	vertices = new Vertex[3 * model->numtriangles];
	for (int i = 0; i < model->numtriangles; ++i) {
		for (int k = 0; k < 3; ++k) {
			for (int j = 0; j < 3; ++j) {
				vertices[3 * i + k].position[j] = model->vertices[3 * model->triangles[i].vindices[k] + j];
				vertices[3 * i + k].normal[j] = model->normals[3 * model->triangles[i].nindices[k] + j];
			}
			vertices[3 * i + k].texcoord[0] = model->texcoords[2 * model->triangles[i].tindices[k] + 0];
			vertices[3 * i + k].texcoord[1] = model->texcoords[2 * model->triangles[i].tindices[k] + 1];
		}
	}

	// vao
	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);
	// vbos
	glGenBuffers(3, vbo_ids);
	// position
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (3 * model->numtriangles), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// normal
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (3 * model->numtriangles), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// texcoord
	glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * (3 * model->numtriangles), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
	glBindVertexArray(0);

	GLuint vert = createShader("Shaders/final.vert", "vertex");
	GLuint frag = createShader("Shaders/final.frag", "fragment");
	program = createProgram(vert, frag);

	GLuint vertPhong = createShader("Shaders/phong.vert", "vertex");
	GLuint fragPhong = createShader("Shaders/phong.frag", "fragment");
	programPhong = createProgram(vertPhong, fragPhong);

	GLuint vertDissolve = createShader("Shaders/dissolve.vert", "vertex");
	GLuint fragDissolve = createShader("Shaders/dissolve.frag", "fragment");
	programDissolve = createProgram(vertDissolve, fragDissolve);

	GLuint vertRamp = createShader("Shaders/ramp.vert", "vertex");
	GLuint fragRamp = createShader("Shaders/ramp.frag", "fragment");
	programRamp = createProgram(vertRamp, fragRamp);

	GLuint vertBonus = createShader("Shaders/bonus.vert", "vertex");
	GLuint fragBonus = createShader("Shaders/bonus.frag", "fragment");
	programBonus = createProgram(vertBonus, fragBonus);
}

void display(void)
{
	if (lamp)
	{
		glEnable(GL_LIGHTING);//Enable Lighting
	}
	else 
	{
		glDisable(GL_LIGHTING);//Enable Lighting
	}
	glEnable(GL_LIGHT1);//Enable Light1
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmb);//Set Light1 Ambience
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDif);//Set Light1 Diffuse
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpc);//Set Light1 Specular

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glRotatef(180.0f, 0, 1, 0);

	//final
	glPushMatrix();
		skybox(15, 10);
		cabinet(12, 8);
		cabinet(2, -2);
		cabinet(-8, -12);
		painting();
		painting_words();
	glPopMatrix();

	GLuint loc;
	GLfloat MV[16], P[16], V[16];

	glPushMatrix();
		glLoadIdentity();
		gluLookAt(
			eyex, 
			eyey, 
			eyez,
			eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180), 
			eyey + sin(eyet*M_PI / 180), 
			eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
			0.0, 
			1.0, 
			0.0);
		glGetFloatv(GL_MODELVIEW_MATRIX, V);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		eyex,
		eyey,
		eyez,
		eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180),
		eyey + sin(eyet*M_PI / 180),
		eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	//draw_light_bulb();

	glPushMatrix();
	glRotatef(180.0f, 0, 1, 0);
	glTranslatef(0, 0, 3);

	eye[0] = eyex;
	eye[1] = eyey;
	eye[2] = eyez;

	//no. 1
	glScalef(1, 1, 1);
	glTranslatef(-13, 0, 10);
	glRotatef(3.3 + (time * 100), 0, 1, 0);
	glScalef(2, 2, 2);

	glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	glGetFloatv(GL_PROJECTION_MATRIX, P);

	glUseProgram(programPhong);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model->textures[0].id);

	glUniform1i(glGetUniformLocation(programPhong, "Tex"), 0);

	loc = glGetUniformLocation(programPhong, "MV");
	glUniformMatrix4fv(loc, 1, GL_FALSE, MV);
	loc = glGetUniformLocation(programPhong, "P");
	glUniformMatrix4fv(loc, 1, GL_FALSE, P);
	loc = glGetUniformLocation(programPhong, "V");
	glUniformMatrix4fv(loc, 1, GL_FALSE, V);

	// pass material
	loc = glGetUniformLocation(programPhong, "ambient");
	glUniform4fv(loc, 1, model->materials[1].ambient);
	loc = glGetUniformLocation(programPhong, "diffuse");
	glUniform4fv(loc, 1, model->materials[1].diffuse);
	loc = glGetUniformLocation(programPhong, "specular");
	glUniform4fv(loc, 1, model->materials[1].specular);

	loc = glGetUniformLocation(programPhong, "shiness");
	glUniform1fv(loc, 1, &(model->materials[1].shininess));

	loc = glGetUniformLocation(programPhong, "Light_position");
	glUniform3fv(loc, 1, light_pos);
	loc = glGetUniformLocation(programPhong, "eye");
	glUniform3fv(loc, 1, eye);

	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 3 * model->numtriangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);

	//no. 2
	glScalef(0.5, 0.5, 0.5);
	glRotatef(3.3 + (time * 100), 0, -1, 0);
	glTranslatef(13, 0, -10);
	glTranslatef(-13, 0, 0);
	//glRotatef(90, 0, 1, 0);
	glRotatef(3.3 + (time * 200), 0, 1, 0);
	glScalef(2, 2, 2);

	glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	glGetFloatv(GL_PROJECTION_MATRIX, P);

	glUseProgram(programRamp);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model->textures[0].id);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, rampTextureID);

	glUniform1i(glGetUniformLocation(programRamp, "Tex"), 0);
	glUniform1i(glGetUniformLocation(programRamp, "rampTexture"), 1);

	loc = glGetUniformLocation(programRamp, "MV");
	glUniformMatrix4fv(loc, 1, GL_FALSE, MV);
	loc = glGetUniformLocation(programRamp, "P");
	glUniformMatrix4fv(loc, 1, GL_FALSE, P);
	loc = glGetUniformLocation(programRamp, "V");
	glUniformMatrix4fv(loc, 1, GL_FALSE, V);

	// pass material
	loc = glGetUniformLocation(programRamp, "ambient");
	glUniform4fv(loc, 1, model->materials[1].ambient);
	loc = glGetUniformLocation(programRamp, "diffuse");
	glUniform4fv(loc, 1, model->materials[1].diffuse);
	loc = glGetUniformLocation(programRamp, "specular");
	glUniform4fv(loc, 1, model->materials[1].specular);

	loc = glGetUniformLocation(programRamp, "shiness");
	glUniform1fv(loc, 1, &(model->materials[1].shininess));

	loc = glGetUniformLocation(programRamp, "Light_position");
	glUniform3fv(loc, 1, light_pos);
	loc = glGetUniformLocation(programRamp, "eye");
	glUniform3fv(loc, 1, eye);

	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 3 * model->numtriangles);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);
	
	//no. 3
	glScalef(0.5, 0.5, 0.5);
	glRotatef(3.3 + (time * 200), 0, -1, 0);
	glTranslatef(13, 0, 0);
	glTranslatef(-13, 0, -10);
	glRotatef(3.3 + (time * 300), 0, 1, 0);
	glScalef(2, 2, 2);

	glGetFloatv(GL_MODELVIEW_MATRIX, MV);
	glGetFloatv(GL_PROJECTION_MATRIX, P);

	count++;
	if (count >= 1000) {
		count = 0;
	}

	glUseProgram(programBonus);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, model->textures[0].id);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTextureID);

	glUniform1i(glGetUniformLocation(programDissolve, "Tex"), 0);
	glUniform1i(glGetUniformLocation(programDissolve, "rampTexture"), 1);

	//dissolve factor control
	loc = glGetUniformLocation(programDissolve, "dissolveFactor");
	glUniform1f(loc, count * 0.00075f);

	loc = glGetUniformLocation(programDissolve, "MV");
	glUniformMatrix4fv(loc, 1, GL_FALSE, MV);
	loc = glGetUniformLocation(programDissolve, "P");
	glUniformMatrix4fv(loc, 1, GL_FALSE, P);
	loc = glGetUniformLocation(programDissolve, "V");
	glUniformMatrix4fv(loc, 1, GL_FALSE, V);

	// pass material
	loc = glGetUniformLocation(programDissolve, "ambient");
	glUniform4fv(loc, 1, model->materials[1].ambient);
	loc = glGetUniformLocation(programDissolve, "diffuse");
	glUniform4fv(loc, 1, model->materials[1].diffuse);
	loc = glGetUniformLocation(programDissolve, "specular");
	glUniform4fv(loc, 1, model->materials[1].specular);

	loc = glGetUniformLocation(programDissolve, "shiness");
	glUniform1fv(loc, 1, &(model->materials[1].shininess));

	loc = glGetUniformLocation(programDissolve, "Light_position");
	glUniform3fv(loc, 1, light_pos);
	loc = glGetUniformLocation(programDissolve, "eye");
	glUniform3fv(loc, 1, eye);

	glBindVertexArray(vaoHandle);
	glDrawArrays(GL_TRIANGLES, 0, 3 * model->numtriangles);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, NULL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(NULL);

	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	///////////////////////////////////
	//try mirror 2
	//启用深度测试，并把场景的东西都画好
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glTranslatef(0, 0, -3);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);
	glutSolidTeapot(1);
	//glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, 0);

	//设置模板，当画镜面时把镜面范围的模板值换成1&0xff
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	//画镜面时必须设置深度值为只读，否则会更新深度值导致倒影画不上去
	glDepthMask(GL_FALSE);
	//如果只希望画反射范围就要设置不把镜面的实际颜色画上去
	glColorMask(0, 0, 0, 0);

	glPushMatrix();
	//mirror range
	//glBindTexture(GL_TEXTURE_2D, texture[7]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	/*glTexCoord2f(0.0f, 1.0f);*/ glVertex3f(-3, -3, 1);
	/*glTexCoord2f(0.0f, 0.0f);*/ glVertex3f(-3, 3, 1);
	/*glTexCoord2f(1.0f, 1.0f);*/ glVertex3f(3, -3, 1);
	/*glTexCoord2f(1.0f, 0.0f);*/ glVertex3f(3, 3, 1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	glColorMask(1, 1, 1, 1);
	glDepthMask(GL_TRUE);

	//继续设置模板测试，不等于镜面模板值的则不会显示
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilFunc(GL_KEEP, GL_KEEP, GL_KEEP);

	//这里为画水壶的倒影
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 5.0f);
		glScalef(1.0, 1.0, 1.0);
		glColor3f(1, 1, 1);
		//glColor3f(0.1, 0.15, 0.5);
		glDisable(GL_TEXTURE_2D);
		glutWireTeapot(1);
		glEnable(GL_TEXTURE_2D);
	glPopMatrix();
	
	glDisable(GL_STENCIL_TEST);

	//半透明鏡面
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glPushMatrix();
	glBegin(GL_TRIANGLE_STRIP);
	//glColor3f(0, 1, 0);
	glColor4f(0.1f, 0.1f, 0.1f, 1);
	/*glTexCoord2f(0.0f, 1.0f);*/ glVertex3f(-3, -3, 1);
	/*glTexCoord2f(0.0f, 0.0f);*/ glVertex3f(-3, 3, 1);
	/*glTexCoord2f(1.0f, 1.0f);*/ glVertex3f(3, -3, 1);
	/*glTexCoord2f(1.0f, 0.0f);*/ glVertex3f(3, 3, 1);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	glDisable(GL_BLEND);
	/////////////////////////////
	//鏡框
	glPushMatrix();
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(0, 0.224, 0.459);
	//glColor4f(0.1f, 0.1f, 0.1f, 1);
	glVertex3f(-3.3, -3.3, 1.2);
	glVertex3f(-3.3, 3.3, 1.2);
	glVertex3f(3.3, -3.3, 1.2);
	glVertex3f(3.3, 3.3, 1.2);
	glEnd();
	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();

	cameraRangeDetect();
}

void cameraRangeDetect() 
{
	//std::cout << "(" << eyex << ", " << eyey << ", " << eyez << ")" << std::endl;
	if (eyex <= -14) { eyex = -14; }
	else if (eyex >= 14) { eyex = 14; }
	if (eyey <= -9.5) { eyey = -9.5; }
	else if (eyey >= 9.5) { eyey = 9.5; }
	if (eyez <= -11.5) { eyez = -11.5; }
	else if (eyez >= 11.5) { eyez = 11.5; }
}

void skybox(float a, float b)
{
	glTranslatef(0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);

	//up
	glBindTexture(GL_TEXTURE_2D, texture[6]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(a, b, -a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(a, b, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-a, b, -a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-a, b, a);
	glEnd();

	//down
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(a, -b, a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(a, -b, -a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-a, -b, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-a, -b, -a);
	glEnd();

	//far
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(a, b, a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(a, -b, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-a, b, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-a, -b, a);
	glEnd();

	//left
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(a, b, -a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(a, -b, -a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(a, b, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(a, -b, a);
	glEnd();

	//right
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-a, b, a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-a, -b, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-a, b, -a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-a, -b, -a);
	glEnd();

	//near
	glBindTexture(GL_TEXTURE_2D, texture[13]);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-a, b, -a);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-a, -b, -a);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(a, b, -a);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(a, -b, -a);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void cabinet(float a, float b)
{
	glTranslatef(0.0f, 0.0f, 0.0f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	//up
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-11, -2, a);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-11, -2, b);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-15, -2, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-15, -2, b);
	glEnd();

	//down
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-15, -10, b);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-15, -10, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-11, -10, b);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-11, -10, a);
	glEnd();

	//far
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-15, -2, a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-15, -10, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-11, -2, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-11, -10, a);
	glEnd();

	//left
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-11, -2, a);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-11, -10, a);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-11, -2, b);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-11, -10, b);
	glEnd();

	//right
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-15, -2, b);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-15, -10, b);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-15, -2, a);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-15, -10, a);
	glEnd();

	//near
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-11, -2, b);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-11, -10, b);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-15, -2, b);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-15, -10, b);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}
void painting() 
{
	//mucha
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, -6.9475f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, 2.5f, -6.9475f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, -10.6775f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, 2.5f, -10.6775f);
	glEnd();

	//kiss
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, 2.425f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, 2.5f, 2.425f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, -2.625f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, 2.5f, -2.625f);
	glEnd();

	//piano
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[4]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, 10.6775f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, 2.5f, 10.6775f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, 6.7475f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, 2.5f, 6.7475f);
	glEnd();

	//star
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[5]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-6.28f, -5, 14.8f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-6.28f, 5, 14.8f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(6.28f, -5, 14.8f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(6.28f, 5, 14.8f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void painting_words()
{
	//mucha word
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	//glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, -4.3f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, -1.5f, -4.3f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, -6.7f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, -1.5f, -6.7f); //-6.9475f
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//kiss word
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, 5.1f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, -1.5f, 5.1f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, 2.7f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, -1.5f, 2.7f); //2.425f
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//piano word
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[11]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(14.9, -2.5f, 13.4f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(14.9, -1.5f, 13.4f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(14.9, -2.5f, 11.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(14.9, -1.5f, 11.0f); //10.6775f
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);

	//star word
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[12]);
	glTranslatef(0.0f, 0.0f, 0.0f);
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(-8.9f, -2.5f, 14.8f);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-8.9f, -1.5f, 14.8f);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(-6.5f, -2.5f, 14.8f);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(-6.5f, -1.5f, 14.8f); //x = 6.28f ~ -6.28f
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}


void renderFirst()  //Render scene except target object(s)(in this case, the barrier) to the depth texture.
{
	//ref:https://tiankefeng0520.iteye.com/blog/2008008
	glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);//綁定我們自己建立的深度緩衝區
	//glEnable(GL_DEPTH_TEST);//開啟深度緩衝區
	//glDepthMask(GL_TRUE);//GL_TRUE代表深度緩衝區可讀寫

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//開始寫入圖像內容到我們自己的緩衝區，這個動作代表我們可以對寫到緩衝區裡面的所有內容直接加工(使用depthTex)：參照 http://blog.shenyuanluo.com/LearnOpenGLNote25.html 的第7點
	//glPushMatrix();
	//glScalef(0.3, 0.3, 0.3);
	//glTranslatef(-2.1, 3.3 - fmod(time, 3), 1.3);
	//glmDraw(teapotModel, GLM_TEXTURE);
	//glPopMatrix();

	//glPushMatrix();
	//glScalef(0.3, 0.3, 0.3);
	//glTranslatef(2, 3.3 - fmod(time, 3), 1.4);
	//glmDraw(bunnyModel, GLM_TEXTURE);
	//glPopMatrix();

	//// floor
	//glDisable(GL_CULL_FACE);
	//glPushMatrix();
	//glScalef(30, 1, 30);
	//glTranslatef(0.0f, 0.0f, 0.0f);
	//glBegin(GL_TRIANGLE_STRIP);

	//glColor3f(1, 1, 1);
	//glVertex3f(1, -0.032, 1);
	//glVertex3f(1, -0.031, -1);
	//glVertex3f(-1, -0.032, 1);
	//glVertex3f(-1, -0.031, -1);

	//glEnd();
	//glPopMatrix();
	//glEnable(GL_CULL_FACE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);//使深度緩衝區恢復為預設緩衝區，如果註解掉，螢幕會全黑
}


void renderSecond()
{
	// looping teapot and bunny
	glPushMatrix();
	glScalef(0.3, 0.3, 0.3);
	glTranslatef(-2.1, 3.3 - fmod(time, 3), 1.3);
	glmDraw(teapotModel, GLM_TEXTURE);
	glPopMatrix();

	glPushMatrix();
	glScalef(0.3, 0.3, 0.3);
	glTranslatef(2, 3.3 - fmod(time, 3), 1.4);
	glmDraw(bunnyModel, GLM_TEXTURE);
	glPopMatrix();

}

void renderThird(float a, float b, float c) //Render target object(s) using depth texture.
{


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, honeyTextureID);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthTextureID);


	glUseProgram(program);

		GLuint loc;
		GLfloat M[16], P[16], V[16];

		loc = glGetUniformLocation(program, "honeyTex");
		glUniform1i(loc, 0);
		loc = glGetUniformLocation(program, "depthTex");
		glUniform1i(loc, 1);

		glGetFloatv(GL_MODELVIEW_MATRIX, V);

		glLoadIdentity();
		glTranslatef(a, b, c);
		glRotatef(ball_rot[0], 1, 0, 0);
		glRotatef(ball_rot[1], 0, 1, 0);
		glRotatef(ball_rot[2], 0, 0, 1);

		glGetFloatv(GL_MODELVIEW_MATRIX, M);
		glGetFloatv(GL_PROJECTION_MATRIX, P);

		loc = glGetUniformLocation(program, "V");
		glUniformMatrix4fv(loc, 1, GL_FALSE, V);
		loc = glGetUniformLocation(program, "M");
		glUniformMatrix4fv(loc, 1, GL_FALSE, M);
		loc = glGetUniformLocation(program, "P");
		glUniformMatrix4fv(loc, 1, GL_FALSE, P);


		loc = glGetUniformLocation(program, "eye");
		eye[0] = eyex;
		eye[1] = eyey;
		eye[2] = eyez;
		glUniform3fv(loc, 1, eye);

		loc = glGetUniformLocation(program, "time");
	glUniform1f(loc, time);

	//Bonus:
	loc = glGetUniformLocation(program, "mycolor");
	glUniform1fv(loc, 1, &mycolor);
	loc = glGetUniformLocation(program, "frequency");
	glUniform1f(loc, frequency);


	glPopMatrix();

	////繪製透明圖片（地板）：https://blog.csdn.net/huutu/article/details/20872525
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_CULL_FACE);//關掉面剔除(把非正面朝向的面保留)
	//glEnable(GL_BLEND);
	////glBlendFunc(GL_SRC_ALPHA, GL_ONE);//不能用這個，地板會被透過去(目標色會覆蓋源色)
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//https://blog.csdn.net/alicehyxx/article/details/4263058

	glBindVertexArray(vaoHandle);
	glActiveTexture(GL_TEXTURE0);
	glScalef(0.3, 0.3, 0.3);
	glDrawArrays(GL_TRIANGLES, 0, 3 * model->numtriangles);
	glBindVertexArray(0);


	glUseProgram(NULL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	//glColor3f(0.4, 0.5, 0);
	//glColor3f(1, 1, 1);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glColor3f(1.0, 1.0, 1.0);
	glPopMatrix();
}

//please implement mode toggle(switch mode between phongShading/Dissolving/Ramp) in case 'b'(lowercase)
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mask color
	{
		lamp = !lamp;
		break;
	}

	case 'v':
	{
		frequency += 4.0f;
		if (frequency > 11)
		{
			frequency = 2.0f;
		}
		std::cout << "frequency: " << frequency << std::endl;

		break;
	}

	case 'n':
	{
		renderBool = !renderBool;
		std::cout << "renderSecond: " << frequency << std::endl;

		break;
	}

	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.64;
		eyez = 3.0;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

//no need to modify the following functions
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x-mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz=0;
	if(left|| right || forward || backward || up || down)
	{ 
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		eyey += dy*sin(eyet*M_PI / 180);
		eyez += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if(lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		light_pos[1] += dy*sin(eyet*M_PI / 180);
		light_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if(ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy*cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx*sin(eyep*M_PI / 180);
		ball_pos[1] += dy*sin(eyet*M_PI / 180);
		ball_pos[2] += dy*(-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx*cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if(bx||by||bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right =false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
