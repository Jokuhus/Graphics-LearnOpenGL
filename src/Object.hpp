#ifndef __OBJECT_HPP__
# define __OBJECT_HPP__

#include <glad/glad.h> // include glad to get the required OpenGL headers
#include <GLFW/glfw3.h>

# include <vector>
# include <string>
# include	<map>
# include <algorithm>
# include <iostream>
# include <fstream>
# include <sstream>
# include <cstring>
# include <exception>
# include <stdexcept>


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
	int	vertex;
	int	texture;
	int	normal;
};

class Object
{
	private:
		std::string								_path;
		std::vector<float>				_vertices;
		std::vector<float>				_textures;
		std::vector<float>				_normals;
		std::vector<FaceData>			_faceData;
		std::vector<unsigned int> _indices;
		unsigned int							_VBO, _VAO, _EBO, _DiffTextureID, _BumpTextureID;
		float											_pos[3], _rot[3], _scale[3], _TextureRatio;
		bool											_TextureMode, _isTextureExist;

		bool								loadOBJ();
		void								loadMTL(std::string path);
		void								checkFileData() const;
		void								shiftToCentre();
		unsigned int				setTextureData(std::string& base_dir, std::string fileName, unsigned int slot);
		std::vector<float>	findNormal(const float *A, const float *B, const float *C) const;

	public:
		Object(const char* path);
		~Object();
		void	setObject();
		void	move(MoveObject direction);
		void	rotate(RotateObject direction);
		void	getModelMatrix(float* out) const;
		void	drawObject(unsigned int bumpSamplerLoc, unsigned int diffuseSamplerLoc);
		void	toggleTexureMode();
		float	getTextureRatio() const;

	class OBJLOADFAIL : public std::exception
	{
		virtual const char*	what() const throw()
		{
			return "ERROR::LOADER::OBJ::LOAD_FAILED";
		}
	};

	class OBJFORMATERROR : public std::exception
	{
		virtual const char*	what() const throw()
		{
			return "ERROR::LOADER::OBJ::FORMAT_ERROR";
		}
	};

	class	MTLLOADFAIL : public std::exception
	{
		virtual const char*	what() const throw()
		{
			return "ERROR::LOADER::MTL::LOAD_FAILED";
		}
	};
};

#endif