#ifndef BODY_H
#define BODY_H

#pragma once

#include "vector.h"
#include "stdlib.h"

struct Shape;

class Body {

public:
    Body(Shape *shape = NULL, GLfloat x = 0.0f, GLfloat y = 0.0f, GLfloat z = 0.0f);
    void ApplyForce(Vector f);
    void ApplyImpulse(const Vector &impulse, const Vector &contactVector);
    void SetStatic(void);
    void SetOrient(float radians);
    bool IsStatic();

    /* linear */
    Vector _position;             // armazena posição para calculos
    Vector _position_repouso;     // utilizado para comparação com _position (se igual objetos em repouso)
    Vector _position_draw_planet; // armazena posição de desenho do planeta
    Vector _velocity;             // armazena velocidade
    Vector _force;                // armazena aceleração

    /* angular */
    float _angularVelocity; // armazena velocidade angular
    float _torque;          // armazena torque/força pro giro
    float _orient;          // armazena angulo de orientação

    /* valores que são atualizados na interface de criação dos objetos (Struct Shape) */
    float _I;  // armazena momento de inércia
    float _iI; // armazena inércia inversa (utilizado nos calculos de movimentação linear)
    float _m;  // armazena massa
    float _im; // armazena massa inversa (utilizado nos calculos de movimentação angular)

    float _staticFriction;  // armazena fricção estática
    float _dynamicFriction; // armazena fricção dinamica
    float _restitution;     //

    // Shape interface
    Shape *_shape;

    bool _isLancado;
    int _num_remove; // referencia para remover o objeto da lista quando ocorrer colisão
};

#endif // BODY_H

