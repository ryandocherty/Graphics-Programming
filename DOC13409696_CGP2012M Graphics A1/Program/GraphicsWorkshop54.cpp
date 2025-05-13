#include <iostream>
#include <GL/glew.h>
#include <SDL.h>

using namespace std;

const GLchar* strVertexShader(
	"#version 330\n"
	"in vec2 position;\n"
	"void main()\n"
	"{\n"
	"   gl_Position = vec4(position.xy, 0.0f, 1.0f);\n"
	"}\n"
	);

const GLchar* strFragmentShader(
	"#version 330\n"
	"out vec4 outputColor;\n"
	"uniform vec3 uniColor;\n"
	"void main()\n"
	"{\n"
	"float lerpValue = gl_FragCoord.y / 500.0f;\n"
	"outputColor = mix(vec4(1.0f, 1.0f, 1.0f, 1.0f),\n"
	"vec4(0.2f, 0.2f, 0.2f, 1.0f), lerpValue);\n"
	//"   outputColor = vec4(uniColor, 1.0f);\n"
	"}\n"
	);

int main(int argc, char* args[])
{

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return 1;
	}


	std::cout << "SDL initialised" <<std::endl;

	SDL_Window* Window;

	Window = SDL_CreateWindow("Hey", 100, 100, 600, 600, SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GLContext context = SDL_GL_CreateContext(Window);
	std::cout << "SDL context initialised" << std::endl;

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
		return 1;

	float vertices[] = {
        -0.9f, -0.9f, // bottom left
        -0.9f, 0.9f, // bottom right
        -0.3f, 0.0f, // top

		0.9f, 0.9f, //top left
		0.9f, -0.9f, //top right
		0.3f, -0.0f, //bottom right

		-0.9f, 0.9f, //top left
		0.f, 0.1f, //top right
		0.9f,  0.9f, //bottom right

		0.9f, -0.9f, //top left
		-0.0f, -0.1f, //top right
		-0.9f,  -0.9f //bottom right

    };

	///Create a vertex buffer object
	GLuint vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Compile shader program
	GLuint vertexshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexshader, 1, &strVertexShader, NULL);
	glCompileShader(vertexshader);

	GLuint fragmemtshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmemtshader, 1, &strFragmentShader, NULL);
	glCompileShader(fragmemtshader);

	GLint vertextStatus;
	glGetShaderiv(vertexshader, GL_COMPILE_STATUS, &vertextStatus);
	std::cout<<"Vertex shader: "<< vertextStatus << std::endl;

	GLint fragStatus;
	glGetShaderiv(fragmemtshader, GL_COMPILE_STATUS, &fragStatus);
	std::cout<<"Frag shader: "<< fragStatus << std::endl;

	GLuint shaderprogram = glCreateProgram();
	glAttachShader(shaderprogram, vertexshader);
	glAttachShader(shaderprogram, fragmemtshader);
	glBindFragDataLocation(shaderprogram, 0, "outputColour");
	glLinkProgram(shaderprogram);
	glUseProgram(shaderprogram);

	std::cout<<"Shader program created"<<std::endl;

	///create vertex array object

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLint posAttrib = glGetAttribLocation(shaderprogram, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);


	GLint uniColor = glGetUniformLocation(shaderprogram, "uniColor");
	

	glUniform3f(uniColor, 0.0f, 2.0f, 0.0f); //triangle 1 colour
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUniform3f(uniColor, 0.0f, 0.0f, 2.0f); //triangle 2 colour
	glDrawArrays(GL_TRIANGLES, 3, 6);

	glUniform3f(uniColor, 2.0f, 0.0f, 0.0f); //triangle 3 colour
	glDrawArrays(GL_TRIANGLES, 6, 9);

	glUniform3f(uniColor, 2.0f, 2.0f, 2.0f); //triangle 4 colour
	glDrawArrays(GL_TRIANGLES, 9, 12);

	SDL_GL_SwapWindow(Window);



	while (getchar() != '\n') {}//Prevents application from closing without user input
	return 0;
}