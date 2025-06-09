#include <iostream>
#include "Window.hpp"

int main(void)
{
	Window *window = new Window(800, 600, "LearnOpenGL");
	try
	{
		window->initGlfw();
		window->makeContextCurrent();
		window->loadGlFunc();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return -1;
	}

	window->setViewport();

	while(!window->shouldClose())
	{
		window->update();
    window->render();
	}

	delete window;
	glfwTerminate();
	return 0;
}
