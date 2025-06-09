#include "Window.hpp"

Window::Window(int width, int height, const char* title) : _title(title)
{
	_state.width = width;
	_state.height = height;
	_mainViewport.width = width;
	_mainViewport.height = height;
}

Window::~Window()
{
	glDeleteVertexArrays(1, &_VAO);
	glDeleteProgram(_shaderProgram);
	glDeleteVertexArrays(1, &_VBO);
	glfwDestroyWindow(_window);
}

void	Window::initGlfw()
{
	//initialize GLFW
	if (!glfwInit())
		throw FailGlfwInit();

	//set OpenGL version to 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// use core-profile only
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//for Mac OS X
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//create a window object
	_window = glfwCreateWindow(_state.width, _state.height, _title.c_str(), NULL, NULL);
	if (_window == NULL)
	{
		glfwTerminate();
		throw FailGlfwCreate();
	}

	makeContextCurrent();
	loadGlFunc();
	//set user pointer
	glfwSetWindowUserPointer(_window, this);

	_shaderProgram = compileShaders();

	float vertices[] = {
     0.0f,  0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f
};

	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);

	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void	Window::makeContextCurrent() const
{
	//tell GLFW to make context of window as main context on current thread
	glfwMakeContextCurrent(_window);
}

void	Window::loadGlFunc() const
{
	//initialize GLAD to load address of OpenGL function poointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		glfwTerminate();
		throw FailGladInit();
	}
}

void	Window::setViewport() const
{
	//tell OpenGL size of redering window
	glViewport(0, 0, _mainViewport.width, _mainViewport.height);

	//callback every window resize
	glfwSetFramebufferSizeCallback(_window, Window::framebuffer_size_callback);
}

void	Window::resizeViewport(int posX, int posY, int width, int height)
{
	_mainViewport.x = posX;
	_mainViewport.y = posY;
	_mainViewport.width = width;
	_mainViewport.height = height;
	_mainViewport.apply();
}

void	Window::updateSize(int width, int height)
{
	_state.width = width;
	_state.height = height;
}

void	Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
  if (self)
	{
    self->updateSize(width, height);
    self->resizeViewport(0, 0, width, height);
  }
}

bool	Window::shouldClose() const
{
	return glfwWindowShouldClose(_window);
}

void	Window::update()
{
	processInput();
}

void	Window::render() const
{
	glfwMakeContextCurrent(_window);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(_shaderProgram);
	glBindVertexArray(_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glfwPollEvents();
	glfwSwapBuffers(_window);
}

unsigned int	Window::compileShaders(void)
{
	//vertex shader source code
	const char	*vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	//fragment shader source code
	const char	*fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\0";

		//create vertex shader and compile
		unsigned int	vertexShader;
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		int	success;
		char	infoLog[512];

		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		}

		//create fragment shader and compile
		unsigned int	fragmentShader;
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" <<
				infoLog << std::endl;
		}

		//create program, attatch shaders and link
		unsigned int	shaderProgram;
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vertexShader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);

		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" <<
				infoLog << std::endl;
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		return shaderProgram;
}

void	Window::processInput()
{
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(_window, true);
}

const char*	Window::FailGlfwInit::what() const throw()
{
	return "Failed to initialize GLFW";
}

const char*	Window::FailGlfwCreate::what() const throw()
{
	return "Failed to create GLFW window";
}

const char*	Window::FailGladInit::what() const throw()
{
	return "Failed to initialize GLAD";
}