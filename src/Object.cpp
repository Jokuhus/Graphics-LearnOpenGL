#include "Object.hpp"

Object::Object(const char* path) : _path(path), _TextureRatio(0.0f), _TextureMode(false)
{
	for (int i = 0; i < 3; i++)
	{
		_pos[i] = 0;
		_rot[i] = 0;
		_scale[i] = 1;
	}
}

Object::~Object()
{
	// optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(1, &_VAO);
  glDeleteBuffers(1, &_VBO);
  glDeleteBuffers(1, &_EBO);
}

void	checkMove(float& pos, float distance)
{
	pos += distance;
	if (pos > 4)
		pos = 4;
	else if (pos < -4)
		pos = -4;
}

void	checkRotate(float& rot, float degree)
{
	rot += degree;
	if (rot > 360)
		rot = 0;
	else if (rot < -360)
		rot = 0;
}

void	Object::move(MoveObject direction)
{
	switch (direction)
	{
		case MOVE_RIGHT:
			checkMove(_pos[0], 0.01f);
			break;
		case MOVE_LEFT:
			checkMove(_pos[0], -0.01f);
			break;
		case MOVE_UP:
			checkMove(_pos[1], 0.01f);
			break;
		case MOVE_DOWN:
			checkMove(_pos[1], -0.01f);
			break;
		case MOVE_CLOSE:
			checkMove(_pos[2], -0.05f);
			break;
		case MOVE_FAR:
			checkMove(_pos[2], 0.05f);
			break;
		case MOVE_RESET:
			_pos[0] = 0; _pos[1] = 0; _pos[2] = 0;
			break;
		default:
			break;
	}
}

void	Object::rotate(RotateObject	direction)
{
	switch (direction)
	{
		case ROTATE_CLOCK_X:
			checkRotate(_rot[0], 1.0f);
			break;
		case ROTATE_ANTICLOCK_X:
			checkRotate(_rot[0], -1.0f);
			break;
		case ROTATE_CLOCK_Y:
			checkRotate(_rot[1], -1.0f);
			break;
		case ROTATE_ANTICLOCK_Y:
			checkRotate(_rot[1], 1.0f);
			break;
		case ROTATE_CLOCK_Z:
			checkRotate(_rot[2], 1.0f);
			break;
		case ROTATE_ANTICLOCK_Z:
			checkRotate(_rot[2], -1.0f);
			break;
		case ROTATE_RESET:
			_rot[0] = 0; _rot[1] = 0; _rot[2] = 0;
			break;
		default:
			break;
	}
}

void Object::getModelMatrix(float* out) const 
{
    float T[16], R[16], S[16], temp[16];

    makeTranslation(T, _pos[0], _pos[1], _pos[2]);

    makeScale(S, _scale[0], _scale[1], _scale[2]);

    // R = Rz * Ry * Rx
    makeRotation(R, _rot[0], _rot[1], _rot[2]);

    // RS = R * S
    multiplyMatrix(temp, R, S);

    // M = T * R * S
    multiplyMatrix(out, T, temp);
}

void	Object::drawObject(unsigned int bumpSamplerLoc, unsigned int diffuseSamplerLoc) const
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _BumpTextureID); // 유효한 텍스처 ID로 바인딩
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _DiffTextureID);
	glUniform1i(bumpSamplerLoc, 0);
  glUniform1i(diffuseSamplerLoc, 1);
	glBindVertexArray(_VAO);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
}

void Object::updateTextureBlendRatio()
{
	if (!_isTextureExist)
		return ;
	if (_TextureMode && _TextureRatio < 1.0f)
		_TextureRatio += 0.02f;
	else if (!_TextureMode && _TextureRatio > 0.0f)
		_TextureRatio -= 0.02f;
}

void	Object::setObject()
{
	loadOBJ();
	shiftToCentre();

	std::vector<float>	vertexData;
	unsigned int				idx = 0;
	int									vertSize = _vertices.size();
	int									normSize = _normals.size();
	int									textSize = _textures.size();

	for (std::vector<FaceData>::const_iterator it = _faceData.begin(); it != _faceData.end(); ++it)
	{
		_indices.push_back(idx++);

		// vertex coordinate
		if (it->vertex * 3 + 2 < vertSize)
		{
			vertexData.push_back(_vertices[it->vertex * 3 + 0]);
			vertexData.push_back(_vertices[it->vertex * 3 + 1]);
			vertexData.push_back(_vertices[it->vertex * 3 + 2]);
		}
		else
			throw std::runtime_error("ERROR::SETTER::OBJ::DATA_ERROR\nvertex index is out of data.");

		// normal coordinate
		if (it->normal > -1 && it->normal * 3 + 2 < normSize)
		{
			vertexData.push_back(_normals[it->normal * 3 + 0]);
			vertexData.push_back(_normals[it->normal * 3 + 1]);
			vertexData.push_back(_normals[it->normal * 3 + 2]);
		}
		else
			throw std::runtime_error("ERROR::SETTER::OBJ::DATA_ERROR\nnoraml index is out of data.");

		// texture coordinate
		if (it->texture > -1 && it->texture * 2 + 1 < textSize)
		{
			vertexData.push_back(_textures[it->texture * 2 + 0]);
			vertexData.push_back(_textures[it->texture * 2 + 1]);
		}
		else
		{
			vertexData.push_back(0);
			vertexData.push_back(0);
		}
	}

  glGenVertexArrays(1, &_VAO);
  glGenBuffers(1, &_VBO);
  glGenBuffers(1, &_EBO);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

	//vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // 노멀
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // texture
	glEnableVertexAttribArray(2);

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);
}

void Object::loadOBJ()
{
	std::string::size_type	extention = _path.find_last_of(".");
	if (extention == std::string::npos || _path.substr(extention + 1) != "obj")
		throw std::runtime_error("ERROR::LOADER::OBJ::WRONG_EXTENSION\ninvalid file extension.");

  std::ifstream file(_path);
  if (!file.is_open())
		throw std::runtime_error("ERROR::LOADER::OBJ::PATH_ERROR\nfailed to open OBJ file.");

  std::vector<float>		temp_vertices;
	std::vector<float>		temp_normals;
	std::vector<float>		temp_textures;
  std::vector<FaceData>	temp_faceData;
	char									vt_flag = 0, vn_flag = 0, mtl_flag = 0;

  std::string line;
  while (std::getline(file, line)) 
  {
    std::istringstream iss(line);

  	std::string prefix;
  	iss >> prefix;
		char	rest;

    if (prefix == "v") 
    {
      float x, y, z;
      if (!(iss >> x >> y >> z) || iss >> rest)
				throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nvertex data is in wrong format.");
      temp_vertices.push_back(x);
      temp_vertices.push_back(y);
      temp_vertices.push_back(z);
    }
		else if (prefix == "vn")
		{
			float x, y, z;
      if (!(iss >> x >> y >> z) || iss >> rest)
				throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nnormal data is in wrong format.");
      temp_normals.push_back(x);
      temp_normals.push_back(y);
      temp_normals.push_back(z);
		}
		else if (prefix == "vt")
		{
			float x, y, z;
      if (!(iss >> x >> y >> z) || iss >> rest)
				throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\ntexture data is in wrong format.");
      temp_textures.push_back(x);
      temp_textures.push_back(y);
		}
		else if (prefix == "mtllib")
		{
			std::string	path;
			iss >> path;
			loadMTL(path);
			mtl_flag = 1;
		}
    else if (prefix == "f") 
    {
      std::vector<FaceData> face_indices;
      int					vIdx, vtIdx, vnIdx;
			std::string	face;
			char				delim;

      // f 다음에 나오는 모든 정점 인덱스 읽기
      while (std::getline(iss, face, ' '))
			{
				if (face.empty()) continue;

				vIdx = 0; vtIdx = 0; vnIdx = 0;
				std::stringstream	faceStream(face);

				if (faceStream >> vIdx) 
				{
					if (faceStream >> delim) 
					{
						if (faceStream.peek() == '/') 
						{
							faceStream.get(); // skip second '/'
							if (vt_flag == 1)
								throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\ntexture indeces are missing.");
							else
								vt_flag = 2;
							if (!(faceStream >> vnIdx)) 
							{
								if (vn_flag == 1)
									throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nnormal indeces are missing.");
								else
									vn_flag = 2;
								vnIdx = 0;
							}
							else if (vn_flag == 2)
									throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nnormal indeces are missing.");
							else
								vn_flag = 1;
						} 
						else 
						{
							if (!(faceStream >> vtIdx)) 
							{
								if (vt_flag == 1)
								throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\ntexture indeces are missing.");
								else
									vt_flag = 2;
								vtIdx = 0;
							}
							else if (vt_flag == 2)
								throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\ntexture indeces are missing.");
							else
								vt_flag = 1;
							if (faceStream >> delim && !(faceStream >> vnIdx)) 
							{
								if (vn_flag == 1)
									throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nnormal indeces are missing.");
								else
									vn_flag = 2;
								vnIdx = 0;
							}
							else if (vn_flag == 2)
									throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nnormal indeces are missing.");
							else
								vn_flag = 1;
						}
					}
					face_indices.push_back({vIdx - 1, vtIdx - 1, vnIdx - 1});
				}
				else
					throw std::runtime_error("ERROR::LOADER::OBJ::FORMAT_ERROR\nvertex indeces are missing.");
			}

     	// 팬 트라이앵글 방식으로 삼각형 분할
     	for (size_t i = 1; i + 1 < face_indices.size(); ++i) 
     	{
     	  temp_faceData.push_back(face_indices[0]);
     	  temp_faceData.push_back(face_indices[i]);
     	  temp_faceData.push_back(face_indices[i + 1]);
     	}
    }
  }

	if (!vn_flag || !temp_normals.size())
	{
		temp_normals.clear();
		int	normalIndex = 0;
		for (size_t i = 0; i + 2 < temp_faceData.size(); i += 3)
		{
			float	A[3] = {
				temp_vertices[temp_faceData[i].vertex * 3 + 0],
				temp_vertices[temp_faceData[i].vertex * 3 + 1],
				temp_vertices[temp_faceData[i].vertex * 3 + 2]
			};
			float	B[3] = {
				temp_vertices[temp_faceData[i + 1].vertex * 3 + 0],
				temp_vertices[temp_faceData[i + 1].vertex * 3 + 1],
				temp_vertices[temp_faceData[i + 1].vertex * 3 + 2]
			};
			float	C[3] = {
				temp_vertices[temp_faceData[i + 2].vertex * 3 + 0],
				temp_vertices[temp_faceData[i + 2].vertex * 3 + 1],
				temp_vertices[temp_faceData[i + 2].vertex * 3 + 2]
			};
			std::vector<float>	normal = findNormal(A, B, C);
			temp_normals.insert(temp_normals.end(), normal.begin(), normal.end());
			temp_faceData[i].normal = normalIndex;
			temp_faceData[i + 1].normal = normalIndex;
			temp_faceData[i + 2].normal = normalIndex;
			++normalIndex;
		}
	}

  _vertices = temp_vertices;
	_normals  = temp_normals;
	_textures = temp_textures;
  _faceData = temp_faceData;

	if (!mtl_flag)
	{
		_BumpTextureID = generateDummyTexture(0);
		_DiffTextureID = generateDummyTexture(1);
	}

	checkFileData();
}

void	Object::loadMTL(std::string fileName)
{
	std::string::size_type	extention = fileName.find_last_of(".");
	if (extention == std::string::npos || fileName.substr(extention + 1) != "mtl")
		throw std::runtime_error("ERROR::LOADER::MTL::WRONG_EXTENSION\ninvalid file extension.");

	std::string base_dir = _path.substr(0, _path.find_last_of("/\\") + 1);
	std::ifstream MtlFile(base_dir + fileName);
  if (!MtlFile.is_open()) 
		throw std::runtime_error("ERROR::LOADER::MTL::PATH_ERROR\nfailed to open MTL file.");

	bool	bump = false, diffuse = false;

  std::string line;
  while (std::getline(MtlFile, line)) 
  {
    std::istringstream iss(line);

    std::string prefix;
    iss >> prefix;

    if (prefix == "bump") 
    {
			std::string	bumpFile;
			iss >> bumpFile;
			_BumpTextureID = setTextureData(base_dir, bumpFile, 0);
			bump = true;
    }
		else if (prefix == "map_Kd")
		{
			std::string diffFile;
			iss >> diffFile;
			_DiffTextureID = setTextureData(base_dir, diffFile, 1);
			diffuse = true;
		}
	}
	if (!bump)
		_BumpTextureID = generateDummyTexture(0);
	if (!diffuse)
		_DiffTextureID = generateDummyTexture(1);
}

unsigned int	Object::setTextureData(std::string& base_dir, std::string fileName, unsigned int slot)
{
  std::string::size_type	extention = fileName.find_last_of(".");
	if (extention == std::string::npos || fileName.substr(extention + 1) != "bmp")
		throw std::runtime_error("ERROR::LOADER::BMP::WRONG_EXTENSION\ninvalid file extension.");

	std::ifstream BmpFile(base_dir + fileName);
  if (!BmpFile.is_open()) 
		throw std::runtime_error("ERROR::LOADER::BMP::FILE_OPEN_FAIL\nfailed to open file.");

	unsigned char	header[54];
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3

			
	BmpFile.read(reinterpret_cast<char*>(header), 54);
	if (header[0]!='B' || header[1]!='M')
		throw std::runtime_error("ERROR::LOADER::BMP::WRONG_FORMAT\nwrong bmp file header format.");
	// 바이트 배열에서 int 변수를 읽습니다. 
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// 몇몇 BMP 파일들은 포맷이 잘못되었습니다. 정보가 누락됬는지 확인해봅니다. 
	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	std::vector<unsigned char>	temp_Texture(imageSize);
	BmpFile.seekg(dataPos);
	if (!BmpFile.read(reinterpret_cast<char*>(temp_Texture.data()), imageSize))
		throw std::runtime_error("ERROR::LOADER::BMP::WRONG_FORMAT\nimage data not found.");

	// OpenGL Texture를 생성합니다.
	unsigned int	textureID;
	glActiveTexture(GL_TEXTURE0 + slot);
	glGenTextures(1, &textureID);

	// 새 텍스처에 "Bind" 합니다 : 이제 모든 텍스처 함수들은 이 텍스처를 수정합니다. 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	// OpenGL에게 이미지를 넘겨줍니다. 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, temp_Texture.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	_isTextureExist = true;

	return textureID;
}

void	Object::shiftToCentre()
{
	// 모든 정점의 최소 / 최대 좌표 계산
	// -------------------------
	float	FLT_MAX = std::numeric_limits<float>::max();
	float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
	float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;

	for (size_t i = 0; i < _vertices.size(); i += 3) 
	{
			float x = _vertices[i];
			float y = _vertices[i + 1];
			float z = _vertices[i + 2];

			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (z < minZ) minZ = z;
			if (x > maxX) maxX = x;
			if (y > maxY) maxY = y;
			if (z > maxZ) maxZ = z;
	}
	// 중심점(center) 계산
	// ----------------
	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;
	float centerZ = (minZ + maxZ) / 2.0f;
	float	maxLength = (maxX - minX) > (maxY - minY) ? (maxX - minX) : (maxY - minY);
	maxLength = ((maxZ - minZ) > maxLength ? (maxZ - minZ) : maxLength);

	// 모든 정점을 중심 기준으로 이동
	// ----------------------
	for (size_t i = 0; i < _vertices.size(); i += 3) 
	{
    _vertices[i] 		 = (_vertices[i]     - centerX) / maxLength;  // x
    _vertices[i + 1] = (_vertices[i + 1] - centerY) / maxLength;  // y
    _vertices[i + 2] = (_vertices[i + 2] - centerZ) / maxLength;  // z
	}
}

void	Object::checkFileData() const
{
	int	maxV = -1, maxVt = -1, maxVn = -1;

	for (std::vector<FaceData>::const_iterator it = _faceData.begin(); it != _faceData.end(); it++)
	{
		maxV = maxV > it->vertex ? maxV : it->vertex;
		maxVt = maxVt > it->texture ? maxVt : it->texture;
		maxVn = maxVn > it->normal ? maxVn : it->normal;
	}

	if (maxV * 3 + 2 >= static_cast<int>(_vertices.size()))
			throw std::runtime_error("ERROR::LOADER::OBJ::DATA_ERROR\nvertex index is out of data.");
	if (maxVt >= 0 && maxVt * 2 + 1 >= static_cast<int>(_textures.size()))
			throw std::runtime_error("ERROR::LOADER::OBJ::DATA_ERROR\ntexture index is out of data.");
	if (maxVn * 3 + 2 >= static_cast<int>(_normals.size()))
			throw std::runtime_error("ERROR::LOADER::OBJ::DATA_ERROR\nnormal index is out of data.");
}

float	Object::getTextureRatio() const
{
	return _TextureRatio;
}

void	Object::toggleTexureMode()
{
	if (_isTextureExist)
		_TextureMode = !_TextureMode;
}

std::vector<float>	Object::findNormal(const float *A, const float *B, const float *C) const
{
	float	firstVector[3] = {
		B[0] - A[0],
		B[1] - A[1],
		B[2] - A[2]
	};
	float	secondVector[3] = {
		C[0] - A[0],
		C[1] - A[1],
		C[2] - A[2]
	};
	std::vector<float>	normal = {
		firstVector[1] * secondVector[2] - firstVector[2] * secondVector[1],
		firstVector[2] * secondVector[0] - firstVector[0] * secondVector[2],
		firstVector[0] * secondVector[1] - firstVector[1] * secondVector[0]
	};
	return normal;
}

unsigned int	Object::generateDummyTexture(unsigned int slot) const
{
	unsigned int	dummyTextureID;
	glActiveTexture(GL_TEXTURE0 + slot);
	glGenTextures(1, &dummyTextureID);
	glBindTexture(GL_TEXTURE_2D, dummyTextureID);
	unsigned char grayPixel[] = {178, 178, 178, 255};
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, grayPixel);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return dummyTextureID;
}
