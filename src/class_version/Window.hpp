#ifndef __WINDOW_HPP__
# define __WINDOW_HPP__

# include <exception>
# include <string>
#include <iostream>
# include <glad/glad.h>
# include <GLFW/glfw3.h>

struct Viewport {
    int x = 0;
    int y = 0;
    int width = 800;
    int height = 600;

    void apply() const {
        glViewport(x, y, width, height);
    }
};

struct WindowState {
    int width;
    int height;
    bool resized = false;
};

class Window
{
	private:
		GLFWwindow*	_window;
		std::string	_title;
		WindowState _state;
		Viewport _mainViewport;
		unsigned int	_shaderProgram, _VAO, _VBO;

	public:
		void	update();
		void	render() const;
		void	initGlfw();
		void	makeContextCurrent() const;
		void	loadGlFunc() const;
		void	setViewport() const;
		void	resizeViewport(int, int, int, int);
		void	updateSize(int, int);
		bool	shouldClose() const;
		unsigned int	compileShaders(void);

		Window(int, int, const char*);
		~Window();

		static void	framebuffer_size_callback(GLFWwindow*, int, int);

	private:
		void	processInput();
		Window(const Window& obj);
		Window& operator=(const Window& obj);

	class FailGlfwInit : public std::exception
	{
		virtual const char* what() const throw();
	};
	class FailGlfwCreate : public std::exception
	{
		virtual const char* what() const throw();
	};
	class FailGladInit : public std::exception
	{
		virtual const char* what() const throw();
	};
};

#endif
