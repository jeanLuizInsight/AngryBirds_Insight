#ifndef VECTOR_H
#define VECTOR_H

const float EPSILON = 0.0001f;
const float gravityForce = 280.555f;  // gravidade do planeta (verificar proporção correta..)
const float forceReducer = 0.005f;

#pragma once

#include <QGLWidget>
#include <math.h>
#include <cassert>

/**
 * Armazena valores para x, y e z, porém o jogo movimento os obketos apenas em x e y. Todos
 * os objetos terão o mesmo valor para z.
 * @brief The Vector class
 */
class Vector {

public:
    Vector(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0) { _x = x; _y = y; _z = z; }
    Vector(const Vector& obj) { _x = obj._x; _y = obj._y; _z = obj._z; }
    void Set(GLfloat x, GLfloat y, GLfloat z) { _x = x; _y = y; _z = z; }

    /* apenas com x e y */
    void Add(const Vector &v) { _x += v.x(); _y += v.y(); }
   // void Add(Vector& v) { _x += v.x(); _y += v.y(); }
    void Add(float s) { _x += s; _y += s; }
    void Sub(const Vector &v) { _x -= v.x(); _y -= v.y(); }
    void Mult(float s) { _x *= s; _y *= s; }
    void Div(float s) { _x /= s; _y /= s;}
    GLfloat MagSqr() { return _x * _x + _y * _y; }
    GLfloat Mag() { return sqrt(_x * _x + _y * _y); }
    void Normalize() { if (Mag() > EPSILON) Div(Mag()); }

    /* dot e cross x, y e z */
    GLfloat DotProduct(Vector v) {
        GLfloat scalar = _x * v.x() + _y * v.y() + _z * v.z();
        return scalar;
    }
    Vector CrossProduct(Vector v) {
        GLfloat x = _y * v.z() - _z * v.y();
        GLfloat y = _z * v.x() - _x * v.z();
        GLfloat z = _x * v.y() - _y * v.x();
        return Vector(x, y, z);
    }
    static GLfloat DotProduct(Vector v1, Vector v2) {
        GLfloat scalar = v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
        return scalar;
    }
    static Vector CrossProduct(Vector v1, Vector v2) {
        GLfloat x = v1.y() * v2.z() - v1.z() * v2.y();
        GLfloat y = v1.z() * v2.x() - v1.x() * v2.z();
        GLfloat z = v1.x() * v2.y() - v1.y() * v2.x();
        return Vector(x, y, z);
    }

    /* dot, cross, etc.  x e y */
    static Vector Min(Vector& a, Vector& b) { return Vector(a.x() < b.x() ? a.x() : b.x(), a.y() < b.y() ? a.y() : b.y(), 0.0f); }
    static Vector Max(Vector& a, Vector& b) { return Vector(a.x() > b.x() ? a.x() : b.x(), a.y() > b.y() ? a.y() : b.y(), 0.0f); }
    static float Dot(Vector a, Vector b){ return a.x() * b.x() + a.y() * b.y(); }
    static float DistSqr(const Vector& a, const Vector& b) { Vector c = a - b; return Dot(c, c); }
    static float Distance(Vector a, Vector b) { float dx = a.x() - b.x(); float dy = a.y() - b.y(); return sqrt(dx * dx + dy * dy); }
    static Vector Cross(const Vector& v, float a) { return Vector(a * v.y(), -a * v.x(), 0.0f); }
    static Vector Cross(float a, const Vector& v) { return Vector( -a * v.y(), a * v.x(), 0.0f); }
    static float Cross(const Vector& a, const Vector& b) { return a.x() * b.y() - a.y() * b.x(); }

    // comparação com a tolerância (valor minimo) EPSILON
    static bool Equal(float a, float b) { return abs(a - b) <= EPSILON; }
    static float Sqr(float a) { return a * a; }
    static float Clamp(float min, float max, float a) {
      if (a < min) return min;
      if (a > max) return max;
      return a;
    }

    // utilizado para evitar erros de ponto flutuante entre dois valores
    static bool BiasGreaterThan(float a, float b ) {
      const float k_biasRelative = 0.95f;
      const float k_biasAbsolute = 0.01f;
      return a >= b * k_biasRelative + a * k_biasAbsolute;
    }

    /* apenas x e y */
    void Rotate(float radians) {
        float c = cos(radians);
        float s = sin(radians);

        float xp = _x * c - _y * s;
        float yp = _x * s + _y * c;

        _x = xp;
        _y = yp;
    }

    static float AngleBetween(Vector a, Vector b) {
        // retorna em radianos
        float angle = atan2(b.y() - a.y(), b.x() - a.x());
        return angle;
    }

    static float AngleBetweenLanc(Vector a, Vector b) {
        // retorna em graus
        float angle = atan2(b.y() - a.y(), b.x() - a.x()) * 180 / M_PI;
        return angle;
    }

    // utilizado pra atualizar a orientação dos blocos ao criá-los
    static float Random(float l, float h ){
      float a = (float)rand( );
      a /= RAND_MAX;
      a = (h - l) * a + l;
      return a;
    }

    /* operadores de sinal, soma, sub, div, mult sem atribuição (não utilizado para z) */
    Vector operator-(const Vector &rhs) const { return Vector(_x - rhs.x(), _y - rhs.y(), _z); }
    Vector operator-(void) const { return Vector(-_x, -_y, _z); }
    Vector operator+(const Vector &rhs) const { return Vector(_x + rhs.x(), _y + rhs.y(), _z); }
    Vector operator+(float s) const { return Vector(_x + s, _y + s, _z); }
    Vector operator*(float s) const { return Vector(_x * s, _y * s, _z); }
    Vector operator/(float s) const { return Vector(_x / s, _y / s, _z); }

    GLfloat x() const { return _x; }
    GLfloat y() const { return _y; }
    GLfloat z() const { return _z; }
private:
    GLfloat _x, _y, _z;
};

inline Vector operator*(float s, const Vector& v) {
   return Vector(s * v.x(), s * v.y());
}


#endif // VECTOR_H

