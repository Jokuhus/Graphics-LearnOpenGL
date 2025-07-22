#include "utils.hpp"

// 4x4 행렬을 단위 행렬로 초기화
void loadIdentity(float* m) 
{
    std::memset(m, 0, sizeof(float) * 16);
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

// 행렬 곱 (result = a * b)
void multiplyMatrix(float* result, const float* a, const float* b) 
{
    float temp[16];
    for (int row = 0; row < 4; ++row) 
    {
        for (int col = 0; col < 4; ++col) 
        {
            temp[col * 4 + row] = 0.0f;
            for (int i = 0; i < 4; ++i) 
            {
                temp[col * 4 + row] += a[i * 4 + row] * b[col * 4 + i];
            }
        }
    }
    std::memcpy(result, temp, sizeof(float) * 16);
}

// 원근 투영 행렬
void makePerspective(float* m, float fov, float aspect, float nearZ, float farZ) 
{
    float f = 1.0f / tanf(fov * 0.5f * 3.1415926f / 180.0f);
    std::memset(m, 0, sizeof(float) * 16);
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (farZ + nearZ) / (nearZ - farZ);
    m[11] = -1.0f;
    m[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
}

// 카메라 View 행렬 (LookAt)
void makeLookAt(float* m, float eyeX, float eyeY, float eyeZ,
                float centerX, float centerY, float centerZ,
                float upX, float upY, float upZ) 
{
    float f[3] = { centerX - eyeX, centerY - eyeY, centerZ - eyeZ };
    float up[3] = { upX, upY, upZ };

    // Normalize f
    float flen = sqrtf(f[0]*f[0] + f[1]*f[1] + f[2]*f[2]);
    for (int i = 0; i < 3; ++i) f[i] /= flen;

    // s = f x up
    float s[3] = {
        f[1]*up[2] - f[2]*up[1],
        f[2]*up[0] - f[0]*up[2],
        f[0]*up[1] - f[1]*up[0]
    };
    // Normalize s
    float slen = sqrtf(s[0]*s[0] + s[1]*s[1] + s[2]*s[2]);
    for (int i = 0; i < 3; ++i) s[i] /= slen;

    // u = s x f
    float u[3] = {
        s[1]*f[2] - s[2]*f[1],
        s[2]*f[0] - s[0]*f[2],
        s[0]*f[1] - s[1]*f[0]
    };

    float t[16];
    loadIdentity(t);
    t[0] = s[0]; t[4] = s[1]; t[8]  = s[2];
    t[1] = u[0]; t[5] = u[1]; t[9]  = u[2];
    t[2] = -f[0]; t[6] = -f[1]; t[10] = -f[2];

    float trans[16];
    loadIdentity(trans);
    trans[12] = -eyeX;
    trans[13] = -eyeY;
    trans[14] = -eyeZ;

    multiplyMatrix(m, t, trans);
}

void makeTranslation(float* m, float x, float y, float z)
{
    loadIdentity(m);
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

void makeScale(float* m, float sx, float sy, float sz)
{
    loadIdentity(m);
    m[0] = sx;
    m[5] = sy;
    m[10] = sz;
}

void makeRotationX(float* m, float angleDeg)
{
    float angleRad = angleDeg * (3.1415926f / 180.0f);
    float c = cosf(angleRad);
    float s = sinf(angleRad);

    loadIdentity(m);
    m[5] = c; m[9] = -s;
    m[6] = s; m[10] = c;
}

void makeRotationY(float* m, float angleDeg) 
{
    float angleRad = angleDeg * 3.1415926f / 180.0f;
    float c = cosf(angleRad);
    float s = sinf(angleRad);
    loadIdentity(m);
    m[0] = c;  m[8] = s;
    m[2] = -s; m[10] = c;
}

void makeRotationZ(float* m, float angleDeg)
{
    float angleRad = angleDeg * (3.1415926f / 180.0f);
    float c = cosf(angleRad);
    float s = sinf(angleRad);

    loadIdentity(m);
    m[0] = c; m[4] = -s;
    m[1] = s; m[5] = c;
}

void makeRotation(float* m, float angleDegX, float angleDegY, float angleDegZ)
{
    float   angleRadX = angleDegX * (3.1415926f / 180.0f);
    float   angleRadY = angleDegY * (3.1415926f / 180.0f);
    float   angleRadZ = angleDegZ * (3.1415926f / 180.0f);

    float   cx = cosf(angleRadX), cy = cosf(angleRadY), cz = cosf(angleRadZ);
    float   sx = sinf(angleRadX), sy = sinf(angleRadY), sz = sinf(angleRadZ);

    loadIdentity(m);
    m[0] = cz * cy;  m[4] = sx * sy * cz + cx * sz;  m[8] = -cz * sy * cx + sx * sz;
    m[1] = -cy * sz; m[5] = -sx * sy * sz + cx * cz; m[9] = cx * sy * sz + sx * cz;
    m[2] = sy;       m[6] = -sx * cy;                m[10] = cx * cy;
}