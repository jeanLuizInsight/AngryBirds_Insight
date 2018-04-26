#ifndef MAT_H
#define MAT_H

#include <math.h>
#include <vector.h>


/**
 * Armazena valores para calculos dos blocos com posição obtetos, etc..
 * @brief The Mat class
 */
class Mat {

public:
    Mat() {}
    Mat(float radians) { float c = cos(radians); float s = sin(radians); m00 = c; m01 = -s; m10 = s; m11 =  c; }
    Mat(float a, float b, float c, float d) : m00( a ), m01( b ), m10( c ), m11( d ) {}

    void Set(float radians) {
        float c = cos(radians);
        float s = sin(radians);
        m00 = c; m01 = -s;
        m10 = s; m11 =  c;
    }

    Mat Abs(void) { return Mat(abs(m00), abs(m01), abs(m10), abs(m11)); }
    Vector AxisX(void) { return Vector(m00, m10, 0.0f); }
    Vector AxisY(void) { return Vector(m01, m11, 0.0f); }
    Mat Transpose(void) { return Mat(m00, m10, m01, m11); }
    // operação utilizada para rootacionar o vetor
    const Vector operator*(const Vector &rhs) const { return Vector(m00 * rhs.x() + m01 * rhs.y(), m10 * rhs.x() + m11 * rhs.y(), 0.0); }
    const Mat operator*(Mat rhs) const {
      // [00 01]  [00 01]
      // [10 11]  [10 11]
      return Mat(
        m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0],
        m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1],
        m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0],
        m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1]
      );
    }

    float m00, m01;
    float m10, m11;
    float m[2][2];
    float v[4];
};

#endif // MAT_H

