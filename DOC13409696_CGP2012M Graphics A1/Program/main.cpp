#include <iostream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <SDL.h>

#define GLM_FORCE_RADIANS //force glm to use radians //must do **before** including GLM headers
//NOTE: GLSL uses radians, so will do the same, for consistency

#include <glm/glm.hpp> //include the main glm header
#include <glm/gtc/matrix_transform.hpp> //include functions to ease the calculation of the view and projection matrices
#include <glm/gtc/type_ptr.hpp> //include functionality for converting a matrix object into a float array for usage in OpenGL


#include "cubeWithColorAndTextureCoordinates.h"

using namespace std;

/////////////////////
// global variables

std::string exeName;
SDL_Window *win; //pointer to the SDL_Window
SDL_GLContext context; //the SDL_GLContext

//string holding the **source** of our vertex shader, to save loading from a file
const std::string strVertexShader(
#ifdef OPENGL_VERSION_3_1
	"#version 140\n"
#endif
#ifdef OPENGL_VERSION_3_3
	"#version 140\n"
#endif
	"in vec4 position;\n"
	"in vec4 color;\n"
	"in vec2 vertexUV;\n"
	"uniform mat4 modelMatrix;\n"
	"uniform mat4 projectionMatrix;\n"
	"uniform mat4 viewMatrix;\n"
	"smooth out vec4 theColor;\n"
	"smooth out vec2 UV; \n"
	"void main()\n"
	"{\n"
	"   gl_Position = projectionMatrix * viewMatrix * modelMatrix * position;\n" //multiple the position by the transformation matrix (rotate)
	"   theColor = color;\n" //just pass on the color. It's a **smooth**, so will be interpolated
	"   UV = vertexUV; \n"
	"}\n"
	);

//string holding the **source** of our fragment shader, to save loading from a file
const std::string strFragmentShader(
#ifdef OPENGL_VERSION_3_1
	"#version 140\n"
#endif
#ifdef OPENGL_VERSION_3_3
	"#version 140\n"
#endif
	"smooth in vec4 theColor;\n"
	"smooth in vec2 UV;\n"
	"uniform sampler2D textureSampler;\n"
	"out vec4 outputColor;\n"
	"void main()\n"
	"{\n"
	"   outputColor = texture(textureSampler, UV);\n"
	"   outputColor.a = 1.0; \n"
	"}\n"
	);


//our variables
bool done = false;

const float vertexData[] = {

	//positions
	-0.1f, -0.1f, -0.1f, 1.0f,
	-0.1f, -0.1f, 0.1f, 1.0f,
	-0.1f, 0.1f, 0.1f, 1.0f,

	-0.1f, -0.1f, -0.1f, 1.0f,
	-0.1f, 0.1f, 0.1f, 1.0f,
	-0.1f, 0.1f, -0.1f, 1.0f,

	0.1f, 0.1f, -0.1f, 1.0f,
	-0.1f, -0.1f, -0.1f, 1.0f,
	-0.1f, 0.1f, -0.1f, 1.0f,

	0.1f, 0.1f, -0.1f, 1.0f,
	0.1f, -0.1f, -0.1f, 1.0f,
	-0.1f, -0.1f, -0.1f, 1.0f,

	0.1f, -0.1f, 0.1f, 1.0f,
	-0.1f, -0.1f, -0.1f, 1.0f,
	0.1f, -0.1f, -0.1f, 1.0f,

	0.1f, -0.1f, 0.1f, 1.0f,
	-0.1f, -0.1f, 0.1f, 1.0f,
	-0.1f, -0.1f, -0.1f, 1.0f,

	-0.1f, 0.1f, 0.1f, 1.0f,
	-0.1f, -0.1f, 0.1f, 1.0f,
	0.1f, -0.1f, 0.1f, 1.0f,

	0.1f, 0.1f, 0.1f, 1.0f,
	-0.1f, 0.1f, 0.1f, 1.0f,
	0.1f, -0.1f, 0.1f, 1.0f,

	0.1f, 0.1f, 0.1f, 1.0f,
	0.1f, -0.1f, -0.1f, 1.0f,
	0.1f, 0.1f, -0.1f, 1.0f,

	0.1f, -0.1f, -0.1f, 1.0f,
	0.1f, 0.1f, 0.1f, 1.0f,
	0.1f, -0.1f, 0.1f, 1.0f,

	0.1f, 0.1f, 0.1f, 1.0f,
	0.1f, 0.1f, -0.1f, 1.0f,
	-0.1f, 0.1f, -0.1f, 1.0f,

	0.1f, 0.1f, 0.1f, 1.0f,
	-0.1f, 0.1f, -0.1f, 1.0f,
	-0.1f, 0.1f, 0.1f, 1.0f,


	//colors

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,
	0.8f, 0.8f, 0.8f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

};

//the rotate we'll pass to the GLSL

glm::mat4 projectionMatrix; // the projectionMatrix for our "camera"
glm::mat4 viewMatrix; // the viewMatrix for our "camera" - which is the identity matrix by default

glm::vec3 eyePoint = glm::vec3(-3, 0, 0);
glm::vec3 lookAtPoint = glm::vec3(0, 0, 0);
glm::vec3 upVector = glm::vec3(0, 1, 0);


//CUBE 1 PARAMS
glm::mat4 modelMatrix; // the modelMatrix for our object - which is the identity matrix by default
glm::mat4 rotationMatrix; // the rotationMatrix for our object - which is the identity matrix by default
glm::mat4 translationMatrix; // the translationMatrix for our object - which is the identity matrix by default
glm::vec3 translateSpeed = glm::vec3(0.0f, 0.0f, 0.0f);
float rotateSpeed = 0.2f; //rate of change of the rotate - in radians per second

//cube2 params
glm::mat4 modelMatrix2;
glm::mat4 rotationMatrix2;
glm::mat4 translationMatrix2;
glm::vec3 translateSpeed2 = glm::vec3(0.0f, 0.0f, 0.0f);
float rotateSpeed2 = 0.03f;

//cube 3 params
glm::mat4 modelMatrix3;
glm::mat4 rotationMatrix3;
glm::mat4 translationMatrix3;
glm::vec3 translateSpeed3 = glm::vec3(0.00f, 0.0f, 0.0f);
float rotateSpeed3 = 0.9f;

//floor params
glm::mat4 modelMatrix4;
glm::vec3 translateAcceleration = glm::vec3(0.1f, 0.1f, 0.1f);

//our GL and GLSL variables

GLuint theProgram; //GLuint that we'll fill in to refer to the GLSL program (only have 1 at this point)
GLint positionLocation; //GLint that we'll fill in with the location of the `position` attribute in the GLSL
GLint colorLocation; //GLint that we'll fill in with the location of the `color` attribute in the GLSL
GLint vertexUVLocation;
GLint modelMatrixLocation; //GLint that we'll fill in with the location of the `modelMatrix` uniform in the GLSL
GLint projectionMatrixLocation; //GLint that we'll fill in with the location of the `projectionMatrix` uniform in the GLSL
GLint viewMatrixLocation;
GLint textureSamplerLocation;

GLuint vertexBufferObject;
GLuint vao;
GLuint textureID;
GLuint textureID2;
GLuint textureID3; 

// end Global Variables
/////////////////////////


void initialise()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	cout << "SDL initialised OK!\n";
}

void createWindow()
{
	//get executable name, and use as window title
	int beginIdxWindows = exeName.rfind("\\"); //find last occurrence of a backslash
	int beginIdxLinux = exeName.rfind("/"); //find last occurrence of a forward slash
	int beginIdx = max(beginIdxWindows, beginIdxLinux);
	std::string exeNameEnd = exeName.substr(beginIdx + 1);
	const char *exeNameCStr = exeNameEnd.c_str();

	//create window
	win = SDL_CreateWindow(exeNameCStr, 100, 100, 800, 800, SDL_WINDOW_OPENGL); //same height and width makes the window square ...

	//error handling
	if (win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "SDL CreatedWindow OK!\n";
}

void setGLAttributes()
{
#ifdef OPENGL_VERSION_3_1
	cout << "Built for OpenGL Version 3.1" << endl;
	// set the opengl context version
	int major = 3;
	int minor = 1;
#endif
#ifdef OPENGL_VERSION_3_3
	cout << "Built for OpenGL Version 3.3" << endl;
	// set the opengl context version
	int major = 3;
	int minor = 3;
#endif
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); //core profile
	cout << "Set OpenGL context to version " << major << "." << minor << " OK!\n";
}

void createContext()
{
	context = SDL_GL_CreateContext(win);
	if (context == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}
	cout << "Created OpenGL context OK!\n";
}

void initGlew()
{
	GLenum rev;
	glewExperimental = GL_TRUE; //GLEW isn't perfect - see https://www.opengl.org/wiki/OpenGL_Loading_Library#GLEW
	rev = glewInit();
	if (GLEW_OK != rev){
		std::cout << "GLEW Error: " << glewGetErrorString(rev) << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLEW Init OK!\n";
	}
}

GLuint createShader(GLenum eShaderType, const std::string &strShaderFile)
{
	GLuint shader = glCreateShader(eShaderType);
	const char *strFileData = strShaderFile.c_str();
	glShaderSource(shader, 1, &strFileData, NULL);

	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		const char *strShaderType = NULL;
		switch (eShaderType)
		{
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}

		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, strInfoLog);
		delete[] strInfoLog;
	}

	return shader;
}

GLuint createProgram(const std::vector<GLuint> &shaderList)
{
	GLuint program = glCreateProgram();

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glAttachShader(program, shaderList[iLoop]);

	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		fprintf(stderr, "Linker failure: %s\n", strInfoLog);
		delete[] strInfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++)
		glDetachShader(program, shaderList[iLoop]);

	return program;
}

void initializeProgram()
{
	std::vector<GLuint> shaderList;

	shaderList.push_back(createShader(GL_VERTEX_SHADER, strVertexShader));
	shaderList.push_back(createShader(GL_FRAGMENT_SHADER, strFragmentShader));

	theProgram = createProgram(shaderList);
	if (theProgram == 0)
	{
		cout << "GLSL program creation error." << std::endl;
		SDL_Quit();
		exit(1);
	}
	else {
		cout << "GLSL program creation OK! GLUint is: " << theProgram << std::endl;
	}

	positionLocation = glGetAttribLocation(theProgram, "position");
	colorLocation = glGetAttribLocation(theProgram, "color");
	modelMatrixLocation = glGetUniformLocation(theProgram, "modelMatrix");
	projectionMatrixLocation = glGetUniformLocation(theProgram, "projectionMatrix");
	viewMatrixLocation = glGetUniformLocation(theProgram, "viewMatrix");
	vertexUVLocation = glGetAttribLocation(theProgram, "vertexUV");

	//clean up shaders (we don't need them anymore as they are no in theProgram
	for_each(shaderList.begin(), shaderList.end(), glDeleteShader);
}

void initializeVertexBuffer()
{
	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeWithColorAndTexturesCoordinates), cubeWithColorAndTexturesCoordinates, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	cout << "positionBufferObject created OK! GLUint is: " << vertexBufferObject << std::endl;
}

void initializeTexturesAndSamplers()
{

	//TEXTURE 1
	SDL_Surface* image = SDL_LoadBMP("assets/rubix.bmp");
	if (image == NULL)
	{
		cout << "image loading (for texture) failed." << std::endl;
		SDL_Quit();
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); //enable 2D texturing
	glGenTextures(1, &textureID); //generate a texture ID and store it
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, image->format->BytesPerPixel, image->w, image->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(image);

	//TEXTURE 2
	SDL_Surface* image2 = SDL_LoadBMP("assets/minecraftblock.bmp");
	if (image == NULL)
	{
		cout << "image loading (for texture) failed." << std::endl;
		SDL_Quit();
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); //enable 2D texturing
	glGenTextures(1, &textureID2); //generate a texture ID and store it
	glBindTexture(GL_TEXTURE_2D, textureID2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, image2->format->BytesPerPixel, image2->w, image2->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image2->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(image2);

	//TEXTURE 3
	SDL_Surface* image3 = SDL_LoadBMP("assets/dice.bmp");
	if (image == NULL)
	{
		cout << "image loading (for texture) failed." << std::endl;
		SDL_Quit();
		exit(1);
	}

	glEnable(GL_TEXTURE_2D); //enable 2D texturing
	glGenTextures(1, &textureID3); //generate a texture ID and store it
	glBindTexture(GL_TEXTURE_2D, textureID3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, image3->format->BytesPerPixel, image3->w, image3->h, 0, GL_BGR, GL_UNSIGNED_BYTE, image3->pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_FreeSurface(image3);

	cout << "texture 1 - Rubix created OK! GLUint is: " << textureID << std::endl;
	cout << "texture 2 - Block  created OK! GLUint is: " << textureID2 << std::endl;
	cout << "texture 3 - Dice  created OK! GLUint is: " << textureID3 << std::endl;
}

void loadAssets()
{
	initializeProgram(); //create GLSL Shaders, link into a GLSL program
	initializeVertexBuffer(); //load data into a vertex buffer
	initializeTexturesAndSamplers();

	glGenVertexArrays(1, &vao); //create a Vertex Array Object
	glBindVertexArray(vao); //make the VAO active
	cout << "Vertex Array Object created OK! GLUint is: " << vao << std::endl;

	//setup face culling details.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW); //front faces are counter-clockwise


	cout << "Loaded Assets OK!\n";
}

void updateSimulation(double simLength) //update simulation with an amount of time to simulate for (in seconds)
{
	//cube 1
	//calculate the amount of rotate for this timestep
	float rotate = (float)simLength * rotateSpeed; //simlength is a double for precision, but rotateSpeedVector in a vector of float, alternatively use glm::dvec3
	//modify the rotationMatrix with the rotate, as a rotate, around the z-axis
	const glm::vec3 unitX = glm::vec3(1, 0, 0);
	const glm::vec3 unitY = glm::vec3(0, 1, 0);
	const glm::vec3 unitZ = glm::vec3(0, 0, 1);
	const glm::vec3 unit45 = glm::normalize(glm::vec3(0, 1, 1));

	rotationMatrix = glm::rotate(rotationMatrix, rotate, unit45);
	glm::vec3 translate = float(simLength) * translateSpeed; //scale the translationSpeed by time to get the translation amount
	translationMatrix = glm::translate(translationMatrix, translate);
	modelMatrix = translationMatrix * rotationMatrix;

	//cube 2
	float rotate2 = (float)simLength * rotateSpeed2;
	rotationMatrix2 = glm::rotate(rotationMatrix2, rotate2, unit45);
	glm::vec3 translate2 = float(simLength) * translateSpeed2;
	translationMatrix2 = glm::translate(translationMatrix2, translate2);
	modelMatrix2 = translationMatrix2 * rotationMatrix2;

	//cube 3
	float rotate3 = (float)simLength * rotateSpeed3;
	rotationMatrix3 = glm::rotate(rotationMatrix3, rotate3, unit45);
	glm::vec3 translate3 = float(simLength) * translateSpeed3;
	translationMatrix3 = glm::translate(translationMatrix3, translate3);
	modelMatrix3 = translationMatrix3 * rotationMatrix3;

	//perspective projection looks from the origin, down the negative z - axis
	float fovyRadians = glm::degrees(90.0f);
	float aspectRatio = 1.0f;
	float nearClipPlane = 0.1f;
	float farClipPlane = 1000.0f;

	projectionMatrix = glm::perspective(fovyRadians, aspectRatio, nearClipPlane, farClipPlane);
	viewMatrix = glm::lookAt(eyePoint, lookAtPoint, upVector);

}

void render()
{
	glUseProgram(theProgram);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)); //uploaed the modelMatrix to the appropriate uniform location

	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix)); //uploaed the projectionMatrix to the appropriate uniform location
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glEnableVertexAttribArray(positionLocation);
	glEnableVertexAttribArray(vertexUVLocation);
	glEnableVertexAttribArray(colorLocation);

	int s = sizeof(cubeWithColorAndTexturesCoordinates);
	size_t colorData = sizeof(vertexData) / 2;
	size_t textureData = colorData + sizeof(GL_FLOAT)* 4 * 36; //colorData plus number of bytes for color
	size_t textureData2 = colorData + sizeof(GL_FLOAT)* 4 * 36;

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //bind positionBufferObject

	glVertexAttribPointer(positionLocation, 4, GL_FLOAT, GL_FALSE, 0, 0); //define **how** values are reader from positionBufferObject in Attrib 0
	glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, 0, (void*)textureData);
	glVertexAttribPointer(colorLocation, 4, GL_FLOAT, GL_FALSE, 0, (void*)colorData); //define **how** values are reader from positionBufferObject in Attrib 1

	glUniform1i(textureSamplerLocation, 0); //make texture unit 0 feed our textureSampler
	glActiveTexture(GL_TEXTURE0); //make texture unit 0 the active texture unit (which texture unit subsequent texture state calls will	affect)
	glBindTexture(GL_TEXTURE_2D, textureID); //make our texture object feed the active texture unit
	glDrawArrays(GL_TRIANGLES, 0, 36); //Draw something, using Triangles, and 3 vertices - i.e. one lonely triangle

	//cube2 modelMatrix
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix2));
	glUniform1i(textureSamplerLocation, 0); //make texture unit 0 feed our textureSampler
	glActiveTexture(GL_TEXTURE0); //make texture unit 0 the active texture unit (which texture unit subsequent texture state calls will	affect)
	glBindTexture(GL_TEXTURE_2D, textureID2); //make our texture object feed the active texture unit
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//cube3 modelMatrix
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix3));
	glUniform1i(textureSamplerLocation, 0); //make texture unit 0 feed our textureSampler
	glActiveTexture(GL_TEXTURE0); //make texture unit 0 the active texture unit (which texture unit subsequent texture state calls will	affect)
	glBindTexture(GL_TEXTURE_2D, textureID3); //make our texture object feed the active texture unit
	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisableVertexAttribArray(0); //cleanup
	glUseProgram(0); //clean up

}
void handleInput()
{
	SDL_Event event; //somewhere to store an event

	//NOTE: there may be multiple events per frame
	while (SDL_PollEvent(&event)) { //loop until SDL_PollEvent returns 0 (meaning no more events)
		switch (event.type)
		{
		case SDL_QUIT:
			done = true; //set done flag if SDL wants to quit (i.e. if the OS has triggered a close event,
			//  - such as window close, or SIGINT
			break;

			//keydown handling - we should to the opposite on key-up for direction controls (generally)
		case SDL_KEYDOWN:
			if (!event.key.repeat)
				switch (event.key.keysym.sym)
			{
				//hit escape to exit
				case SDLK_ESCAPE: done = true;
				case SDLK_LEFT:  translateSpeed.x -= translateAcceleration.x; break;
				case SDLK_RIGHT: translateSpeed.x += translateAcceleration.x; break;

				case SDLK_UP:    translateSpeed.y += translateAcceleration.y; break;
				case SDLK_DOWN:  translateSpeed.y -= translateAcceleration.y; break;

				case SDLK_a:     translateSpeed2.x -= translateAcceleration.x; break;
				case SDLK_d:     translateSpeed2.x += translateAcceleration.x; break;

				case SDLK_w:     translateSpeed2.y += translateAcceleration.y; break;
				case SDLK_s:     translateSpeed2.y -= translateAcceleration.y; break;
			}
			break;

			//keyup handling
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_LEFT:  translateSpeed.x += translateAcceleration.x; break;
			case SDLK_RIGHT: translateSpeed.x -= translateAcceleration.x; break;

			case SDLK_UP:    translateSpeed.y -= translateAcceleration.y; break;
			case SDLK_DOWN:  translateSpeed.y += translateAcceleration.y; break;

			case SDLK_a:     translateSpeed2.x += translateAcceleration.x; break;
			case SDLK_d:     translateSpeed2.x -= translateAcceleration.x; break;

			case SDLK_w:     translateSpeed2.y -= translateAcceleration.y; break;
			case SDLK_s:     translateSpeed2.y += translateAcceleration.y; break;

			}
			break;

		default:
			break;
		}
	}
}


void cleanUp()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(win);
	cout << "Cleaning up OK!\n";
}

int main(int argc, char* args[])
{
	exeName = args[0];
	//setup
	//- do just once
	initialise();
	createWindow();
	setGLAttributes();
	createContext();
	initGlew();


	//load stuff from files
	//- usually do just once
	loadAssets();


	while (!done)
	{
		handleInput(); //this should get input from the system (keyboard, mouse, network, whatever) and set control variables
		//- e.g. set the speed to go left if left is pressed, right if right is pressed, and neither if neither or both

		updateSimulation(0.01); //call update simulation with an amount of time to simulate for (in seconds)
		glClear(GL_COLOR_BUFFER_BIT);

		render(); //RENDER HERE - PLACEHOLDER

		SDL_GL_SwapWindow(win);; //present the frame buffer to the display (swapBuffers)

	} //LOOP TO HERE

	//cleanup and exit
	cleanUp();
	SDL_Quit();

	return 0;
}