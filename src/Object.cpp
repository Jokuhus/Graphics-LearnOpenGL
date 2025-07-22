#include "Object.hpp"

Object::Object(const char* path) : _path(path)
{
	for (int i = 0; i < 3; i++)
	{
		_pos[i] = 0;
		_rot[i] = 0;
		_scale[i] = 1;
		std::cerr << "pos[" << i << "]: " << _pos[i] << "\n";
		std::cerr << "rot[" << i << "]: " << _rot[i] << "\n";
		std::cerr << "scale[" << i << "]: " << _scale[i] << std::endl;
	}
}

Object::~Object()
{
	// optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(2, _VAO);
  glDeleteBuffers(2, _VBO);
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
		rot = -360;
	else if (rot < -360)
		rot = 360;
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
			checkMove(_pos[2], 0.05f);
			break;
		case MOVE_FAR:
			checkMove(_pos[2], -0.05f);
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
			checkRotate(_rot[0], -1.0f);
			break;
		case ROTATE_ANTICLOCK_X:
			checkRotate(_rot[0], 1.0f);
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

void Object::getModelMatrix(float* out) const {
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

void	Object::drawObject(int currentMode) const
{
	glBindVertexArray(_VAO[currentMode]);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
}

void	Object::setObject()
{
	if (!loadOBJ())
		throw OBJLOADFAIL();

	shiftToCentre();
	std::vector<float>	shiftedCentroidVertices = shiftToCentroid();

  glGenVertexArrays(2, _VAO);
  glGenBuffers(2, _VBO);
  glGenBuffers(1, &_EBO);

	std::vector<std::vector<float>>	allVertices = {
		_vertices,
		shiftedCentroidVertices
	};

	for (int i = 0; i < 2; i++)
	{
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(_VAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, _VBO[i]);
		glBufferData(GL_ARRAY_BUFFER, allVertices[i].size() * sizeof(float), allVertices[i].data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

		//vertex attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	}

  // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0); 

  // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
  // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
  glBindVertexArray(0);
}

bool Object::loadOBJ()
{
    std::ifstream file(_path);
    if (!file.is_open()) 
    {
        std::cerr << "Failed to open OBJ file: " << _path << std::endl;
        return false;
    }

    std::vector<float> temp_vertices;
    std::vector<unsigned int> temp_indices;

    std::string line;
    while (std::getline(file, line)) 
    {
      std::istringstream iss(line);

      std::string prefix;
      iss >> prefix;

      if (prefix == "v") 
      {
        float x, y, z;
        iss >> x >> y >> z;
        temp_vertices.push_back(x);
        temp_vertices.push_back(y);
        temp_vertices.push_back(z);
      }
      else if (prefix == "f") 
      {
        std::vector<unsigned int> face_indices;
        unsigned int idx;
				std::string	face;

        // f 다음에 나오는 모든 정점 인덱스 읽기
        while (std::getline(iss, face, ' '))
				{
					std::stringstream	faceStream(face);
					if (faceStream >> idx)
          	face_indices.push_back(idx - 1); // 1-based -> 0-based
				}

        // 팬 트라이앵글 방식으로 삼각형 분할
        for (size_t i = 1; i + 1 < face_indices.size(); ++i) 
        {
            temp_indices.push_back(face_indices[0]);
            temp_indices.push_back(face_indices[i]);
            temp_indices.push_back(face_indices[i + 1]);
        }
      }
      // ignore: vn, vt, comments, etc.
    }

    _vertices = temp_vertices;
    _indices = temp_indices;
    return true;
}

std::vector<float>	Object::shiftToCentroid() const
{
	float sumX = 0.0f, sumY = 0.0f, sumZ = 0.0f;
	int numVertices = _vertices.size() / 3;

	// 모든 정점의 합 계산
	// --------------
	for (size_t i = 0; i < _vertices.size(); i += 3) {
			sumX += _vertices[i];
			sumY += _vertices[i + 1];
			sumZ += _vertices[i + 2];
	}

	// 모든 정점의 평균 계산
	// ----------------
	float centroidX = sumX / numVertices;
	float centroidY = sumY / numVertices;
	float centroidZ = sumZ / numVertices;

	// 모든 정점을 중심 기준으로 이동
	// ----------------------
	std::vector<float>	result;
	for (size_t i = 0; i < _vertices.size(); i += 3) 
	{
    result.push_back(_vertices[i]     - centroidX);
    result.push_back(_vertices[i + 1] - centroidY);
    result.push_back(_vertices[i + 2] - centroidZ);
	}

	return result;
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