#ifndef SHAPE_H
#define SHAPE_H

#include <iostream>
#include <QString>
#include <QVector2D>
#include <QGLWidget>
#include <QImage>
#include <GL/glu.h>
#include <math.h>
#include <cfloat>
#include "vector.h"
#include "mat.h"
#include "glm.h"
#include "body.h"

#define MaxPolyVertexCount 4

/**
 * Struct responsável pelas definiçõs de todos os objetos da cena
 * (tipo, tamanho, corpo, textura, variaveis fisicas, etc.)
 * @brief The Shape struct
 */
struct Shape {

    /* enum que representa o respectivo tipo do shape (utilizado nas colisões) */
    enum Type {
        eCircle,   // - 0 (planetas, birds e pigs)
        ePolygon   // - 1 (blocos)
    };

    Shape() {}
    virtual Shape *Clone(void) const = 0;
    virtual void Initialize(void) = 0;
    virtual void ComputeMass(float density) = 0;
    virtual void SetOrient(float radians) = 0;
    virtual void Draw(void) const = 0;
    virtual Type GetType(void) const = 0;

    /* objetos blender */
    GLMmodel *_model;   // carrega o objeto desenhado no blender
    GLuint _tex_name;
    char *_file_name;   // diretorio objeto CPU
    bool _is_draw_GS;  // planet >> desenhar espaço gravitacional?
    char _type_block;  // block >> M - madeira, P - pedra
    char _type_planet; // Planet > M - minha, I - inimiga
    char _type_object; // ModeledObject >> R - red, B - blue, Y - yellow, P - pig, S - slingshot

    Body *_body;

    /* utilizado na representação dos circulos (planetas, red, blue, yellow, pig) */
    float _raio;     // raio do objeto
    float _raio_G;   // raio do espaço G (apenas planetas)

    /* utilizado na representação dos blocos */
    Mat u; // matrix de orientação para o modelo do mundo...

};

struct Planet : public Shape {
    Planet(GLuint tex_name, float r, char type_planet) {
        _tex_name = tex_name;
        _type_planet = type_planet;
        _type_block = 'N';
        _type_object = 'N';
        _raio = r;
        _raio_G = _raio * 2.3;
        if (_raio > 100.0) {
            _is_draw_GS = true;
        } else {
            _is_draw_GS = false;
        }
    }

    Shape *Clone(void) const {
        return new Planet(_tex_name, _raio, _type_planet);
    }

    void Initialize(void) {
        ComputeMass(1.0f);
    }

    void ComputeMass(float density) {
        // é objeto estático (esses valores devem ser todod zerados...)
        _body->_m = M_PI * _raio * _raio * density;
        _body->_im = (_body->_m) ? 1.0f / _body->_m : 0.0f;
        _body->_I = _body->_m * _raio * _raio;
        _body->_iI = (_body->_I) ? 1.0f / _body->_I : 0.0f;
    }

    void SetOrient(float radians) {
        _body->_orient = radians; // matriz de orientação é utilizada apenas para blocos...
    }

    void Draw(void) const {
        GLfloat angulo;
        GLfloat tx, ty; // armazena as coordenadas da textura
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, _tex_name);

        glPushMatrix();
          glTranslatef(_body->_position_draw_planet.x(), _body->_position_draw_planet.y(), _body->_position_draw_planet.z());
          glBegin(GL_POLYGON);
          // iteração para desenhar cada vértice que irá representar ligação entre as linhas do circulo
          for (int i = 0; i < 80;  i++) {
              angulo = i * 2 * M_PI / 80;
              tx = cos(angulo) * 0.5 + 0.5;
              ty = sin(angulo) * 0.5 + 0.5;
              glTexCoord2f(tx, ty);
              glVertex2f(_body->_position_draw_planet.x() + (cos(angulo) * _raio), (_body->_position_draw_planet.y() + sin(angulo) * _raio));
          }
          glEnd();
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);

        if (_is_draw_GS) {
          // draw gravitational space
          GLfloat angulo;
          glPushMatrix();
              //glScalef(0.5, 0.5, 0.5);
              glTranslatef(_body->_position_draw_planet.x(), _body->_position_draw_planet.y(), _body->_position_draw_planet.z());
              glColor3ub(0, 127, 255);
              glBegin(GL_LINE_LOOP);
              // iteração para desenhar cada vértice que irá representar ligação entre as linhas do circulo
              for (int i = 0; i < 80;  i++) {
                  angulo = i * 2 * M_PI / 80;
                  glVertex2f(_body->_position_draw_planet.x() + (cos(angulo) * _raio_G), (_body->_position_draw_planet.y() + sin(angulo) * _raio_G));
              }
              glEnd();
          glPopMatrix();
        }
    }

    Type GetType(void) const {
        return eCircle;
    }
};

/* red - deslocamento normal, blue - vira 3, yellow - dobra velocidade */
struct ModeledObject : public Shape {

    /* dependendo do tipo eu carrego o modelo correto */
    ModeledObject(char tipo_objeto) {
        _type_object = tipo_objeto;
        _type_block = 'N';
        _type_planet = 'N';
        if (_type_object == 'R') {
            _model = glmReadOBJ((char*)"data/red.obj");
            _raio = 9.5;
        } else if (_type_object == 'B') {
            _model = glmReadOBJ((char*)"data/blue.obj");
            _raio = 8.0;
        } else if (_type_object == 'Y') {
            _model = glmReadOBJ((char*)"data/yellow.obj");
            _raio = 9.5;
        } else if (_type_object == 'P') {
            _model = glmReadOBJ((char*)"data/pig.obj");
            _raio = 13.0;
        } else if (_type_object == 'S') {
            _model = glmReadOBJ((char*)"data/slingshot.obj");
        }

        if (!_model) {
            std::cout << "GLWidget::initializeGL: !!! failed loading model from file...MODELEDOBJECT" << std::endl;
            exit(0);
            return;
        }

        glmUnitize(_model);
        glmFacetNormals(_model);
        glmVertexNormals(_model, 180.0);
    }

    Shape *Clone(void) const {
        return new ModeledObject(_type_object);
    }

    void Initialize(void) {
        float density = 2.5f;
        if (_type_object == 'P') {
            density = 1.0f;
        }
        ComputeMass(density);
    }

    void ComputeMass(float density) {
        _body->_m = M_PI * _raio * _raio * density;
        _body->_im = (_body->_m) ? 1.0f / _body->_m : 0.0f;
        _body->_I = _body->_m * _raio * _raio;
        _body->_iI = (_body->_I) ? 1.0f / _body->_I : 0.0f;
    }

    void SetOrient(float radians) {
        _body->_orient = radians; // matriz de orientação é utilizada apenas para blocos...
    }

    void Draw(void) const {
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHTING);

        glPushMatrix();
            glTranslatef(_body->_position.x(), _body->_position.y(), _body->_position.z());
            if (_type_object == 'S') {
                glScalef(40.0, 40.0, 0.0);
            } else if (_type_object == 'B') {
                glScalef(12.0, 12.0, 0.0);
            } else if (_type_object == 'P') {
                glScalef(18.0, 18.0, 0.0);
            } else {
                glScalef(15.0, 15.0, 0.0);
            }
            glColor3ub(255, 255, 0);
            glmDraw(_model, GLM_SMOOTH | GLM_MATERIAL);
        glPopMatrix();

        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
    }

    Type GetType(void) const {
        return eCircle;
    }
};

struct Block : public Shape {

    Block(GLuint tex_name, char type_block) {
        _tex_name = tex_name;
        _type_block = type_block;
    }

    Shape *Clone(void) const {
        Block *block = new Block(_tex_name, _type_block);
        block->u = u;
        for(unsigned int i = 0; i < m_vertexCount; ++i) {
            block->m_vertices[i] = m_vertices[i];
            block->m_normals[i] = m_normals[i];
            block->m_normals_textures[i] = m_normals_textures[i];
        }
        block->m_vertexCount = m_vertexCount;
        return block;
    }

    void Initialize(void) {
        if (_type_block == 'P') {
            ComputeMass(2.0f);
        } else if (_type_block == 'M') {
            ComputeMass(1.0f);
        }
    }

    void ComputeMass(float density) {
        // calcula o centro de massa do bloco e o momento de inércia...
        Vector c(0.0f, 0.0f, 0.0f); // centroid
        float area = 0.0f;
        float I = 0.0f;
        const float k_inv3 = 1.0f / 3.0f;

        // atribui a area do bloco
        for(unsigned int i1 = 0; i1 < m_vertexCount; ++i1) {
          Vector p1(m_vertices[i1]);
          unsigned int i2 = i1 + 1 < m_vertexCount ? i1 + 1 : 0;
          Vector p2(m_vertices[i2]);

          float D = Vector::Cross(p1, p2);
          float triangleArea = 0.5f * D;

          area += triangleArea;

          // Use area to weight the centroid average, not just vertex position
          c.Add((p1 + p2) * triangleArea * k_inv3);

          float intx2 = p1.x() * p1.x() + p2.x() * p1.x() + p2.x() * p2.x();
          float inty2 = p1.y() * p1.y() + p2.y() * p1.y() + p2.y() * p2.y();
          I += (0.25f * k_inv3 * D) * (intx2 + inty2);
        }

        c.Mult(1.0f / area);

        // tranlada os vértices de/para centroid de/para o bloco no space model...
        for(unsigned int i = 0; i < m_vertexCount; ++i)
          m_vertices[i].Sub(c);

        _body->_m = density * area;
        _body->_im = (_body->_m) ? 1.0f / _body->_m : 0.0f;
        _body->_I = I * density;
        _body->_iI = _body->_I ? 1.0f / _body->_I : 0.0f;
    }

    void SetOrient(float radians) {
        u.Set(radians);
    }

    void Draw(void) const {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glBindTexture(GL_TEXTURE_2D, _tex_name);

        glPushMatrix();
            glBegin(GL_QUADS);
            for(unsigned int i = 0; i < m_vertexCount; ++i) {
                Vector v_pos = _body->_position + u * m_vertices[i];
                glTexCoord2f(m_normals_textures[i].x(), m_normals_textures[i].y());
                glVertex2f(v_pos.x(), v_pos.y());
            }
            glEnd();
        glPopMatrix();

        glDisable(GL_TEXTURE_2D);
    }

    Type GetType(void) const {
        return ePolygon;
    }

    // atribui os valores para os vetores dos vertices, normais e texturas de um bloco...
    void SetBox(float hw, float hh) {
        m_vertexCount = 4;
        m_vertices[0].Set(-hw, -hh, 0.0);
        m_vertices[1].Set(hw, -hh, 0.0);
        m_vertices[2].Set(hw, hh, 0.0);
        m_vertices[3].Set(-hw, hh, 0.0);
        m_normals[0].Set(0.0, -1.0, 0.0);
        m_normals[1].Set(1.0, 0.0, 0.0);
        m_normals[2].Set(0.0, 1.0, 0.0);
        m_normals[3].Set(-1.0, 0.0, 0.0);
        m_normals_textures[0].Set(0.0, 0.0, 0.0);
        m_normals_textures[1].Set(0.0, 1.0, 0.0);
        m_normals_textures[2].Set(1.0, 1.0, 0.0);
        m_normals_textures[3].Set(1.0, 0.0, 0.0);
    }

    // Busca o ponto extremo ao longo de uma direção...
    Vector GetSupport(const Vector &dir) {
        float bestProjection = -FLT_MAX;
        Vector bestVertex;

        for(unsigned int i = 0; i < m_vertexCount; ++i) {
          Vector v = m_vertices[i];
          float projection = Vector::Dot(v, dir);

          if(projection > bestProjection) {
            bestVertex = v;
            bestProjection = projection;
          }
        }
        return bestVertex;
    }

    unsigned int m_vertexCount;
    Vector m_vertices[MaxPolyVertexCount];
    Vector m_normals[MaxPolyVertexCount];
    Vector m_normals_textures[MaxPolyVertexCount];
};

#endif // SHAPE_H

