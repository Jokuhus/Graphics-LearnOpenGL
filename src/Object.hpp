#ifndef __OBJECT_HPP__
# define __OBJECT_HPP__

#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <GLFW/glfw3.h>

# include <vector>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstring>
# include <exception>

class Object
{
	private:
		std::string								_path;
		std::vector<float>				_vertices;
		std::vector<unsigned int>	_indices;
		unsigned int							_VBO[3], _VAO[3], _EBO;

		bool loadOBJ();
		std::vector<float>	shiftToCentroid();
		std::vector<float>	shiftToCentre();

	public:
		Object(const char*);
		~Object();
		void	setObject();
		void	drawObject(int);

	class OBJLOADFAIL : public std::exception
	{
		virtual const char*	what();
	};
};

#endif