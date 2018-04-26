#ifndef MANIFOLD_H
#define MANIFOLD_H

#pragma once

#include <math.h>
#include <algorithm>
#include <cmath>
#include "vector.h"
#include "body.h"
#include "shape.h"

/**
 * Classe responsável por todos os cálculos da engine física do jogo..forçcas físicas, colisões, resolução de impulso
 * @brief The Manifold class
 */
class Manifold {

public:
  Manifold(Body *a = NULL, Body *b = NULL);
  void Solve(void);                 // gera informação de colisão
  void Initialize();                // gera precalculos para resolver impulso
  void ApplyImpulse(void);          // gera impulso e aplica
  void PositionalCorrection(void);  // resolve correção de penetração posicional
  void InfiniteMassCorrection(void);

  /* realizam os cálculos de colisão/contato */
  float FindAxisLeastPenetration(unsigned int *faceIndex, Block *A, Block *B);
  void FindIncidentFace(Vector *v, Block *RefPoly, Block *IncPoly, unsigned int referenceIndex);
  signed int Clip(Vector n, float c, Vector *face);
  void CircleToCircle(Body *a, Body *b);   // planetToObject, objectToObject
  void CircleToPolygon(Body *a, Body *b);  // planetToBlock, objectToBlock
  void PolygonToCircle(Body *a, Body *b);  // blockToPlanet, blocktoObject
  void PolygonToPolygon(Body *a, Body *b); // blockToBlock


  Body *_A;
  Body *_B;

  float _penetration;          // armazena profundidade da penetração de colisão
  Vector _normal;              // armazena normal de A pra B
  Vector _contacts[2];         // armazena os pontos de contato durante a colisão
  unsigned int _contact_count; // armazena a quantidade de contatos durante a colisão
  float _e;                    // restituição mista      <AxB>
  float _df;                   // fricção dimanica mista <AxB>
  float _sf;                   // fricção estática mista <AxB>
};

#endif // MANIFOLD_H

