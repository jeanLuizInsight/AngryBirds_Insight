#include "manifold.h"

Manifold::Manifold(Body *a, Body *b)
    : _A(a), _B(b) {
}

/**
 * Resolve as colisões dependendo do tipo do objeto
 * @brief Manifold::Solve
 */
void Manifold::Solve(void) {
    if (_A->_shape->GetType() == Shape::eCircle && _B->_shape->GetType() == Shape::eCircle) {
        this->CircleToCircle(_A, _B);
    }
    if (_A->_shape->GetType() == Shape::eCircle && _B->_shape->GetType() == Shape::ePolygon) {
        this->CircleToPolygon(_A, _B);
    }
    if (_A->_shape->GetType() == Shape::ePolygon && _B->_shape->GetType() == Shape::eCircle) {
        this->PolygonToCircle(_A, _B);
    }
    if (_A->_shape->GetType() == Shape::ePolygon && _B->_shape->GetType() == Shape::ePolygon) {
        this->PolygonToPolygon(_A, _B);
    }
}

/**
 * Responsável por atualizar as variáveis de colisão quando houver
 * @brief Manifold::Initialize
 */
void Manifold::Initialize() {
  // calcula a restituição entre os objetos
  _e = std::min(_A->_restitution, _B->_restitution);
  // calcula a fricção estatica e dinamica entre os objetos
  _sf = std::sqrt(_A->_staticFriction * _B->_staticFriction);
  _df = std::sqrt(_A->_dynamicFriction * _B->_dynamicFriction);
}

/**
 * Responsável por aplicar impulso nos objetos quando houver colisão
 * @brief Manifold::ApplyImpulse
 */
void Manifold::ApplyImpulse(void) {
  // para massa infinita atualiza posição...parado!!
  if (Vector::Equal(_A->_im + _B->_im, 0.0f)) {
    this->InfiniteMassCorrection();
    return;
  }

  for (unsigned int i = 0; i < _contact_count; ++i) {
    // calcula radii da colisão...
    Vector ra = _contacts[i] - _A->_position;
    Vector rb = _contacts[i] - _B->_position;

    // velocidade relativa entre A e B...
    Vector rv = _B->_velocity + Vector::Cross(_B->_angularVelocity, rb) - _A->_velocity - Vector::Cross(_A->_angularVelocity, ra);

    // velocidade relativa ao longo do normal da colisão
    float contactVel = Vector::Dot(rv, _normal);

    // caso as velocidades estejam se distanciando retorna sem calcular impulsos..
    if(contactVel > 0)
      return;

    float raCrossN = Vector::Cross(ra, _normal);
    float rbCrossN = Vector::Cross(rb, _normal);
    float invMassSum = _A->_im + _B->_im + Vector::Sqr(raCrossN) * _A->_iI + Vector::Sqr(rbCrossN) * _B->_iI;

    // calcula o impulso escalar
    float j = -(1.0f + _e) * contactVel;
    j /= invMassSum;
    j /= (float)_contact_count;

    // aplicando o impulso nos dois objetos com o normal, escalar calculados e vetores de contato..
    Vector impulse = _normal * j;
    _A->ApplyImpulse(-impulse, ra);
    _B->ApplyImpulse(impulse, rb);

    // impulso da fricção...
    rv = _B->_velocity + Vector::Cross(_B->_angularVelocity, rb) - _A->_velocity - Vector::Cross(_A->_angularVelocity, ra);

    Vector t = rv - (_normal * Vector::Dot(rv, _normal));
    t.Normalize();

    float jt = -Vector::Dot(rv, t);
    jt /= invMassSum;
    jt /= (float)_contact_count;

    // se o impulso de fricção for minimo não aplico nos objetos
    if(Vector::Equal(jt, 0.0f))
      return;

    // aplica a lei de coulumb...
    Vector tangentImpulse;
    if(std::abs(jt) < j * _sf) {
      tangentImpulse = t * jt;
    } else {
      tangentImpulse = t * -j * _df;
    }
    // aplicando o impulso da fricção od dois objetos
    _A->ApplyImpulse(-tangentImpulse, ra);
    _B->ApplyImpulse(tangentImpulse, rb);
  }
}

/**
 * Responsável por realizar a correção no posicionamento do objeto respeitando um limite de penetração entre os mesmos
 * @brief Manifold::PositionalCorrection
 */
void Manifold::PositionalCorrection(void) {
  const float k_slop = 0.05f; // tolerancia de penetração entre os objetos
  const float percent = 0.4f; // percentagem de penetração para correção das posições...deve ser entre 0.2 e 0.8
  Vector correction = (std::max(_penetration - k_slop, 0.0f) / (_A->_im + _B->_im)) * _normal * percent;
  _A->_position.Sub(correction * _A->_im);
  _B->_position.Add(correction * _B->_im);
}

void Manifold::InfiniteMassCorrection(void) {
  _A->_velocity.Set(0.0f, 0.0f, 0.0f);
  _B->_velocity.Set(0.0f, 0.0f, 0.0f);
}

/**
 * Responsável por encontrar a melhor distancia de penetração entre dois blocos...
 * @brief Manifold::FindAxisLeastPenetration
 * @param faceIndex
 * @param A
 * @param B
 * @return
 */
float Manifold::FindAxisLeastPenetration(unsigned int *faceIndex, Block *A, Block *B) {
    float bestDistance = -FLT_MAX; // float máximo -, preciso garantir que irá armazenar a primeira vez...
    unsigned int bestIndex;

    for (unsigned int i = 0; i < A->m_vertexCount; ++i) {
        // recupera o normal face a partir de A
        Vector n = A->m_normals[i];
        Vector nw = A->u * n;

        // transforma normal face para o espaço de B
        Mat buT = B->u.Transpose();
        n = buT * nw;

        // Recupera ponto de apoio de B ao longo de -n...
        Vector s = B->GetSupport(-n);

        // recupera vértice na face de A e transformo para o espaço de B
        Vector v = A->m_vertices[i];
        v = A->u * v + A->_body->_position;
        v.Sub(B->_body->_position);
        v = buT * v;

        // calcula a distancia de penetração (no espaço de B)
        float d = Vector::Dot(n, s - v);

        // armazeno a maior distancia...
        if(d > bestDistance) {
            bestDistance = d;
            bestIndex = i;
        }
    }

    *faceIndex = bestIndex;
    return bestDistance;
}

/**
 * Responsável por encontrar a face de contato pertencente ao bloco incidente...
 * @brief Manifold::FindIncidentFace
 * @param v
 * @param RefPoly
 * @param IncPoly
 * @param referenceIndex
 */
void Manifold::FindIncidentFace(Vector *v, Block *RefPoly, Block *IncPoly, unsigned int referenceIndex) {
    Vector referenceNormal = RefPoly->m_normals[referenceIndex];

    // calcula normal no frame de incidente de referencia
    referenceNormal = RefPoly->u * referenceNormal;             // para espaço do mundo..
    referenceNormal = IncPoly->u.Transpose() * referenceNormal; // para espaço do modelo incidente..

    // buscar face + anti-normal com bloco incidente
    signed int incidentFace = 0;
    float minDot = FLT_MAX;
    for(unsigned int i = 0; i < IncPoly->m_vertexCount; ++i) {
        float dot = Vector::Dot(referenceNormal, IncPoly->m_normals[i]);
        if(dot < minDot) {
            minDot = dot;
            incidentFace = i;
        }
    }

    // atribui vértices/face para bloco incidente
    v[0] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->_body->_position;
    incidentFace = incidentFace + 1 >= (signed int)IncPoly->m_vertexCount ? 0 : incidentFace + 1;
    v[1] = IncPoly->u * IncPoly->m_vertices[incidentFace] + IncPoly->_body->_position;
}

signed int Manifold::Clip(Vector n, float c, Vector *face) {
  unsigned int sp = 0;
  Vector out[2] = {
    face[0],
    face[1]
  };

  // recuperar distancias de cada... extremidade >> linha
  // d = ax + by - c
  float d1 = Vector::Dot(n, face[0]) - c;
  float d2 = Vector::Dot(n, face[1]) - c;

  // se negativo (plano oculto) clip
  if(d1 <= 0.0f)
      out[sp++] = face[0];
  if(d2 <= 0.0f)
      out[sp++] = face[1];

  // pontos em diferentes lados do plano?
  if(d1 * d2 < 0.0f) {
    // armazena o ponto de intersecção
    float alpha = d1 / (d1 - d2);
    out[sp] = face[0] + alpha * (face[1] - face[0]);
    ++sp;
  }

  // atribui novos valors convertidos...
  face[0] = out[0];
  face[1] = out[1];

  assert(sp != 3);

  return sp;
}

/**
 * CircletoCircle
 * @brief ObjectToObject
 * @param m
 * @param a
 * @param b
 */
void Manifold::CircleToCircle(Body *a, Body *b) {
    // pegando vetor de translação: normal
    Vector normal = b->_position - a->_position;

    float dist_sqr = normal.MagSqr();
    float radius = a->_shape->_raio + b->_shape->_raio;

    // translação maior ou igual que raio ao quadrado, sem colisao...
    if (dist_sqr >= radius * radius){
        _contact_count = 0;
        return;
    }

    float distance = std::sqrt(dist_sqr);

    _contact_count = 1;

    if (distance == 0.0f) {
        _penetration = a->_shape->_raio;
        _normal = Vector(1.0f, 0.0f, 0.0f);
        _contacts[0] = a->_position;
    } else {
        _penetration = radius - distance;
        // já foi executado sqrt apenas divido...
        _normal = normal / distance;
        _contacts[0] = _normal * a->_shape->_raio + a->_position;
    }
}

/**
 * CircleToPolygon
 * @brief ObjectToBlock
 * @param m
 * @param a
 * @param b
 */
void Manifold::CircleToPolygon(Body *a, Body *b) {
    // casting para a struct do objeto corrente, para blocos sempre preciso realizar pois variaveis são acessadas
    // somente dentro da struct dele (Block) ...
    Block *B = reinterpret_cast<Block *>(b->_shape);

    _contact_count = 0;

    // Transform circle center to Polygon model space
    Vector center = a->_position;
    center = B->u.Transpose() * (center - b->_position);

    // encontra a borda com penetração mínima...conceito exato como utilização de pontos de suporte em bloco x bloco
    float separation = -FLT_MAX;
    unsigned int faceNormal = 0;
    for(unsigned int i = 0; i < B->m_vertexCount; ++i) {
      float s = Vector::Dot(B->m_normals[i], center - B->m_vertices[i]);
      if(s > a->_shape->_raio)
        return;

      if(s > separation) {
        separation = s;
        faceNormal = i;
      }
    }

    // pega os vértices para as faces
    Vector v1 = B->m_vertices[faceNormal];
    unsigned int i2 = faceNormal + 1 < B->m_vertexCount ? faceNormal + 1 : 0;
    Vector v2 = B->m_vertices[i2];

    // verifica se o centro está dentro do bloco..
    if(separation < EPSILON) {
      _contact_count = 1;
      _normal = -(B->u * B->m_normals[faceNormal]);
      _contacts[0] = _normal * a->_shape->_raio + a->_position;
      _penetration = a->_shape->_raio;
      return;
    }

    // determina qual região do centro da borda do circulo está dentro...
    float dot1 = Vector::Dot(center - v1, v2 - v1);
    float dot2 = Vector::Dot(center - v2, v1 - v2);
    _penetration = a->_shape->_raio - separation;


    if(dot1 <= 0.0f) {
      // mais próximo de v1
      if(Vector::DistSqr(center, v1) > a->_shape->_raio * a->_shape->_raio)
        return;

      _contact_count = 1;
      Vector n = v1 - center;
      n = B->u * n;
      n.Normalize();
      _normal = n;
      v1 = B->u * v1 + b->_position;
      _contacts[0] = v1;
    } else if(dot2 <= 0.0f) {
      // mais próximo de v2
      if(Vector::DistSqr(center, v2) > a->_shape->_raio * a->_shape->_raio)
        return;

      _contact_count = 1;
      Vector n = v2 - center;
      v2 = B->u * v2 + b->_position;
      _contacts[0] = v2;
      n = B->u * n;
      n.Normalize();
      _normal = n;
    } else {
      // mais próximo da face
      Vector n = B->m_normals[faceNormal];
      if(Vector::Dot(center - v1, n) > a->_shape->_raio)
        return;

      n = B->u * n;
      _normal = -n;
      _contacts[0] = _normal * a->_shape->_raio + a->_position;
      _contact_count = 1;
    }
}

void Manifold::PolygonToCircle(Body *a, Body *b) {
    CircleToPolygon(b, a);
    _normal = -_normal;
}

void Manifold::PolygonToPolygon(Body *a, Body *b) {
    Block *A = reinterpret_cast<Block *>(a->_shape);
    Block *B = reinterpret_cast<Block *>(b->_shape);
    _contact_count = 0;

    // verifica se existe um eixo de separação com o plano/face de A
    unsigned int faceA;
    float penetrationA = this->FindAxisLeastPenetration(&faceA, A, B);
    if(penetrationA >= 0.0f)
      return;

    // verifica se existe um eixo de separação com o plano/face de B
    unsigned int faceB;
    float penetrationB = this->FindAxisLeastPenetration(&faceB, B, A);
    if(penetrationB >= 0.0f)
      return;

    unsigned int referenceIndex;
    bool flip; // ponto sempre de A pra B...

    Block *RefPoly; // Referente
    Block *IncPoly; // Incidente

    // determina qual das 2 formas tem o plano/face de referencia...
    if(Vector::BiasGreaterThan(penetrationA, penetrationB)) {
      RefPoly = A;
      IncPoly = B;
      referenceIndex = faceA;
      flip = false;
    } else {
      RefPoly = B;
      IncPoly = A;
      referenceIndex = faceB;
      flip = true;
    }

    // face incidente no espaço do mundo...
    Vector incidentFace[2];
    this->FindIncidentFace(incidentFace, RefPoly, IncPoly, referenceIndex);

    // estabelece os vértices da face referente
    Vector v1 = RefPoly->m_vertices[referenceIndex];
    referenceIndex = referenceIndex + 1 == RefPoly->m_vertexCount ? 0 : referenceIndex + 1;
    Vector v2 = RefPoly->m_vertices[referenceIndex];

    // transforma os vértices para espaço do mundo
    v1 = RefPoly->u * v1 + RefPoly->_body->_position;
    v2 = RefPoly->u * v2 + RefPoly->_body->_position;

    // calcula face referente lateral normal no espaço do mundo
    Vector sidePlaneNormal = (v2 - v1);
    sidePlaneNormal.Normalize();

    Vector refFaceNormal(sidePlaneNormal.y(), -sidePlaneNormal.x(), 0.0);

    // ax + by = c
    // c é a distancia desde a origem
    float refC = Vector::Dot(refFaceNormal, v1);
    float negSide = -Vector::Dot(sidePlaneNormal, v1);
    float posSide =  Vector::Dot(sidePlaneNormal, v2);

    // Clip...face incidente para fazer referencia a face de planos laterais
    if(this->Clip(-sidePlaneNormal, negSide, incidentFace) < 2)
      return; // pode não haver pontos necessários..

    if(this->Clip(sidePlaneNormal, posSide, incidentFace) < 2)
      return;

    // Flip
    _normal = flip ? -refFaceNormal : refFaceNormal;

    // mantem pontos atrás da face referente
    unsigned int cp = 0;
    float separation = Vector::Dot(refFaceNormal, incidentFace[0]) - refC;
    if(separation <= 0.0f) {
      _contacts[cp] = incidentFace[0];
      _penetration = -separation;
      ++cp;
    } else {
      _penetration = 0;
    }
    separation = Vector::Dot(refFaceNormal, incidentFace[1]) - refC;
    if(separation <= 0.0f) {
      _contacts[cp] = incidentFace[1];

      _penetration += -separation;
      ++cp;

      // armazena a penetração média...
      _penetration /= (float)cp;
    }

    _contact_count = cp;
}
