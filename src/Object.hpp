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

# include "utils.hpp"

enum	MoveObject {
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
	MOVE_FAR,
	MOVE_CLOSE,
	MOVE_RESET
};

enum	RotateObject {
	ROTATE_CLOCK_X,
	ROTATE_ANTICLOCK_X,
	ROTATE_CLOCK_Y,
	ROTATE_ANTICLOCK_Y,
	ROTATE_CLOCK_Z,
	ROTATE_ANTICLOCK_Z,
	ROTATE_RESET
};

struct	FaceData {
	unsigned int	vertex;
	unsigned int	texture;
	unsigned int	normal;
};

class Object
{
	private:
		std::string								_path;
		std::vector<float>				_vertices;
		std::vector<float>				_normals;
		std::vector<FaceData>			_faceData;
		std::vector<unsigned int> _indices;
		unsigned int							_VBO, _VAO, _EBO;
		float											_pos[3], _rot[3], _scale[3];

		bool loadOBJ();
		std::vector<float>	shiftToCentroid() const;
		void								shiftToCentre();

	public:
		Object(const char* path);
		~Object();
		void	setObject();
		void	move(MoveObject direction);
		void	rotate(RotateObject direction);
		void	getModelMatrix(float* out) const;
		void	drawObject() const;

	class OBJLOADFAIL : public std::exception
	{
		virtual const char*	what() const throw()
		{
			return "OBJ load failed.";
		}
	};
};

#endif