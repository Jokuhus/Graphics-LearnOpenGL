#ifndef __UTILS_HPP__
# define __UTILS_HPP__

# include <vector>
# include <cmath>

void loadIdentity(float*);
void multiplyMatrix(float*, const float*, const float*);
void makePerspective(float*, float, float, float, float);
void makeLookAt(float*, float, float, float, float, float, float, float, float, float);
void makeRotationY(float*, float);

#endif