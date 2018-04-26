#ifndef GLWIDGET_H
#define GLWIDGET_H

#pragma once

#include <iostream>
#include <QGLWidget>
#include <math.h>
#include <time.h>
#include <QDateTime>
#include <QTimer>
#include <QImage>
#include <QList>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPointF>
#include <cassert>
#include <QMediaPlayer>
#include <GL/glut.h>
#include <body.h>
#include <shape.h>
#include <manifold.h>

class GLWidget : public QGLWidget {
    Q_OBJECT

public:
    explicit GLWidget(QWidget* parent = 0);
    virtual ~GLWidget();

    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void initTextures();

    void drawBackgound();
    void drawScene();
    void drawSling();
    Body *Add(Shape *shape, GLfloat x, GLfloat y, GLfloat z, int num);
    void IntegrateForces(Body *b);
    void IntegrateVelocity(Body *b);
    void Reset();

    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

protected slots:
    void tick();

private:
    qint64 _time_final;
    qint64 _time_acumulado;
    int _max_fps;
    float _constant_dt;

    int _width, _height;

    QImage _backgound;
    QImage _planets;
    QImage _pedra;
    QImage _madeira;
    GLuint _tex_name[4];

    /* posição inicial de lançamento */
    GLfloat _x_lanc, _y_lanc;
    bool _clicked;
    float _angulo_lancamento;
    bool _slingshot;
    char _bird_da_vez; // R- red, B - blue, Y - yellow
    bool _is_repouso, _finish;
    bool _play_r, _play_b, _play_b2, _play_b3, _play_y;
    bool _yellow_espec, _blue_espec;
    int _qtd_pigs;
    bool _play_vit, _play_der;

    // bases manipulo fora da lista
    Body *_base;
    Body *_base_inimiga;
    Body *_sling;

    // birds...preciso criar um body pra cada..pois vou armazena-lo na lista de contato apenas após lançá-lo..
    Body *_red;
    Body *_blue;
    Body *_blue_2; // desenho apenas dentro da lista
    Body *_blue_3; // desenho apenas dentro da lista
    Body *_yellow;

    /* armazena lista de objeto de contato.. */
    QList<Body *> _bodies;
    QList<Body *> _bodies_remove;
    QList<Manifold> _contacts;

    QMediaPlayer *_music_ambient;
    QMediaPlayer *_music_sling;
    QMediaPlayer *_music_lanc_red;
    QMediaPlayer *_music_lanc_blue;
    QMediaPlayer *_music_lanc_yellow;
    QMediaPlayer *_music_pig_destroy;
    QMediaPlayer *_music_bird_colision;
    QMediaPlayer *_music_vitoria;
    QMediaPlayer *_music_derrota;

    int _count_remover;
    int _count_atualizar_bird;
    int _count_restart;

};

#endif // GLWIDGET_H

