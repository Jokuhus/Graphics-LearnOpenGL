#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <vector>
# include <cmath>
# include <cstring>

void loadIdentity(float* m);
void multiplyMatrix(float* result, const float* a, const float* b);
void makePerspective(float* m, float fov, float aspect, float nearZ, float farZ);
void makeLookAt(float* m, float eyeX, float eyeY, float eyeZ,
                float centerX, float centerY, float centerZ,
                float upX, float upY, float upZ);
void makeTranslation(float* m, float x, float y, float z);
void makeScale(float* m, float sx, float sy, float sz);
void makeRotationX(float* m, float angleDeg);
void makeRotationY(float* m, float angleDeg);
void makeRotationZ(float* m, float angleDeg);
void makeRotation(float* m, float angleDegX, float angleDegY, float angleDegZ);

#endif
