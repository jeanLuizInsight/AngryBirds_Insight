#include <GL/glew.h>
#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent) {
    this->setMouseTracking(true);
    /* armazenando posições de lançamento */
    _x_lanc = 200.0;
    _y_lanc = 472.0;
    _angulo_lancamento = 0.0;
    _clicked = false;
    _slingshot = false;
    _bird_da_vez = 'R';
    _is_repouso = true;
    _finish = false;
    _play_r = false;
    _play_b = false;
    _play_b2 = false;
    _play_b3 = false;
    _play_y = false;
    _blue_espec = false;
    _yellow_espec = false;
    _count_remover = 1000;
    _count_atualizar_bird = 150;
    _count_restart = 100;
    _qtd_pigs = 3;
    _play_vit = false;
    _play_der = false;

    _music_ambient = new QMediaPlayer(this);
    _music_ambient->setMedia(QUrl("qrc:/pasta/AngryBirdsSpace.mp3"));
    _music_ambient->setVolume(50);
    _music_ambient->play();

    _music_sling = new QMediaPlayer(this);
    _music_sling->setMedia(QUrl("qrc:/pasta/slingshotStreched.mp3"));
    _music_sling->setVolume(100);

    _music_lanc_red = new QMediaPlayer(this);
    _music_lanc_red->setMedia(QUrl("qrc:/pasta/birdFlyingRed.mp3"));
    _music_lanc_red->setVolume(100);

    _music_lanc_blue = new QMediaPlayer(this);
    _music_lanc_blue->setMedia(QUrl("qrc:/pasta/birdFlyingBlue.mp3"));
    _music_lanc_blue->setVolume(100);

    _music_lanc_yellow = new QMediaPlayer(this);
    _music_lanc_yellow->setMedia(QUrl("qrc:/pasta/birdFlyingYellow.mp3"));
    _music_lanc_yellow->setVolume(100);

    _music_pig_destroy = new QMediaPlayer(this);
    _music_pig_destroy->setMedia(QUrl("qrc:/pasta/pigletteDestroyed2.mp3"));
    _music_pig_destroy->setVolume(100);

    _music_bird_colision = new QMediaPlayer(this);
    _music_bird_colision->setMedia(QUrl("qrc:/pasta/birdColision.mp3"));
    _music_bird_colision->setVolume(100);

    _music_vitoria = new QMediaPlayer(this);
    _music_vitoria->setMedia(QUrl("qrc:/pasta/gameComplete.mp3"));
    _music_vitoria->setVolume(100);

    _music_derrota = new QMediaPlayer(this);
    _music_derrota->setMedia(QUrl("qrc:/pasta/levelFailed.mp3"));
    _music_derrota->setVolume(100);

    // plataformas...
    Planet base_P(_tex_name[1], 60.0, 'M');
    _base = new Body(&base_P, 100.0, 300.0, 0.0);
    _base->SetStatic();
    _base->SetOrient(0);

    Planet base_PI(_tex_name[1], 150.0, 'I');
    _base_inimiga = this->Add(&base_PI, 500.0, 180.0, 0.0, 333);
    _base_inimiga->SetStatic();
    _base_inimiga->SetOrient(0);

    /* catapulta */
    ModeledObject sling_P('S');
    _sling = new Body(&sling_P, 164.0, 510.0, 0.0);
    _sling->SetStatic();

    /* birds */
    ModeledObject red('R');
    _red = new Body(&red, _x_lanc, _y_lanc, 0.0);
    _red->_num_remove = 1;
    ModeledObject blue('B');
    _blue = new Body(&blue, 228.0, 537.0, 0.0);
    _blue->_num_remove = 2;
    _blue_2 = new Body(&blue, 228.0, 537.0, 0.0);
    _blue_3 = new Body(&blue, 228.0, 537.0, 0.0);
    ModeledObject yellow('Y');
    _yellow = new Body(&yellow, 165.0, 540.0, 0.0);
    _yellow->_num_remove = 3;

    /* pigs */
    ModeledObject pig1('P');                // cima
    this->Add(&pig1, 1000.0, 149.0, 0.0, 11);
    ModeledObject pig2('P');                // lado
    this->Add(&pig2, 1211.0, 369.0, 0.0, 22);
    ModeledObject pig3('P');                // baixo
//    this->Add(&pig3, 1000.0, 500.0, 0.0, 33);
    this->Add(&pig3, 1111.0, 420.0, 0.0, 33);

    /* blocks */
    Block b(_tex_name[3], 'M');    // pé e
    b.SetBox(3.0f, 20.0f);
    Body *bb = this->Add(&b, 970.0, 192.0, 0.0, 0);
    bb->SetOrient(0);
//    bb->_restitution = 0.2f;
//    bb->_dynamicFriction = 0.2f;
//    bb->_staticFriction = 0.4f;
    Block b2(_tex_name[3], 'M');   // pé d
    b2.SetBox(3.0f, 20.0f);
    Body *bb2 = this->Add(&b2, 1030.0, 192.0, 0.0, 0);
    bb2->SetOrient(0);
//    bb2->_restitution = 0.2f;
//    bb2->_dynamicFriction = 0.2f;
//    bb2->_staticFriction = 0.4f;
    Block b4(_tex_name[2], 'P'); // deitado
    b4.SetBox(40.0f, 3.0f);
    Body *bb4 = this->Add(&b4, 1000.0, 168.0, 0.0, 0);
    bb4->SetOrient(0);
//    bb4->_restitution = 0.2f;
//    bb4->_dynamicFriction = 0.2f;
//    bb4->_staticFriction = 0.4f;
    Block b3(_tex_name[2],'P');
    b3.SetBox(10.0f, 10.0f);
    Body *bb3 = this->Add(&b3, 950.0, 206.0, 0.0, 0);
    bb3->SetOrient(-2);
//    bb3->_restitution = 0.2f;
//    bb3->_dynamicFriction = 0.2f;
//    bb3->_staticFriction = 0.4f;
    // ---------------- cima
    Block b5(_tex_name[2],'P');
    b5.SetBox(8, 12);
    Body *bb5 = this->Add(&b5, 965.0, 518.0, 0.0, 0);
    bb5->SetOrient(0);
    Block b6(_tex_name[2],'P');
    b6.SetBox(8, 12);
    Body *bb6 = this->Add(&b6, 1035.0, 520.0, 0.0, 0);
    bb6->SetOrient(0);
    Block b7(_tex_name[3],'M');
    b7.SetBox(12, 8);
    Body *bb7 = this->Add(&b7, 965.0, 539.0, 0.0, 0);
    bb7->SetOrient(0);
    Block b8(_tex_name[3],'M');
    b8.SetBox(12, 8);
    Body *bb8 = this->Add(&b8, 1035.0, 539.0, 0.0, 0);
    bb8->SetOrient(0);
    Block b9(_tex_name[3], 'M');
    b9.SetBox(36, 4);
    Body *bb9 = this->Add(&b9, 999.9, 553.0, 0.0, 0);
    bb9->SetOrient(0);
    Block b10(_tex_name[2],'P');
    b10.SetBox(10, 10);
    Body *bb10 = this->Add(&b10, 999.9, 570.0, 0.0, 0);
    bb10->SetOrient(0);
    // ---------------- baixo
    Block b11(_tex_name[2], 'P');
    b11.SetBox(10, 10);
    Body *bb11 = this->Add(&b11, 1162.0, 369.9, 0.0, 0);
    bb11->SetOrient(0);
    Block b12(_tex_name[3], 'M');
    b12.SetBox(10, 10);
    Body *bb12 = this->Add(&b12, 1185.0, 369.9, 0.0, 0);
    bb12->SetOrient(0);
    // ---------------- direita
    Block b14(_tex_name[3], 'M');
    b14.SetBox(15, 15);
    Body *bb14 = this->Add(&b14, 830.0, 368.9, 0.0, 0);
    bb14->SetOrient(0);
    Block b15(_tex_name[3], 'M');
    b15.SetBox(5, 25);
    Body *bb15 = this->Add(&b15, 805.0, 371.0, 0.0, 0);
    bb15->SetOrient(0);
    Block b16(_tex_name[3], 'M');
    b16.SetBox(10, 10);
    Body *bb16 = this->Add(&b16, 785.0, 370.9, 0.0, 0);
    bb16->SetOrient(0);
    //---------------- esquerda
    Block b17(_tex_name[2], 'P');
    b17.SetBox(15, 15);
    Body *bb17 = this->Add(&b17, 1130.0, 270.0, 0.0, 0);
    bb17->SetOrient(-7);
    Block b18(_tex_name[3], 'M');
    b18.SetBox(5, 25);
    Body *bb18 = this->Add(&b18, 1150.0, 260.0, 0.0, 0);
    bb18->SetOrient(-7);
    Block b19(_tex_name[3], 'M');
    b19.SetBox(10, 10);
    Body *bb19 = this->Add(&b19, 1170.0, 250.0, 0.0, 0);
    bb19->SetOrient(-7);

    _time_acumulado = 0;
    _max_fps = 60;
    /* constante padrão ... ms */
    _constant_dt = 1000 / _max_fps;
    _time_final = QDateTime::currentMSecsSinceEpoch();

    /* start timer */
    QTimer::singleShot(1000 / _max_fps, this, SLOT(tick()));

    this->setFocus();
}

GLWidget::~GLWidget() {
    glDeleteTextures(4, _tex_name);
    delete _base;
    delete _base_inimiga;
    delete _sling;
    delete _red;
    delete _blue;
    delete _blue_2;
    delete _blue_3;
    delete _yellow;
    _contacts.clear();
    _bodies.clear();
}

void GLWidget::tick() {
    /* mantenho o audio ambiente */
    if (_music_ambient->state() != QMediaPlayer::PlayingState && !_finish) {
        _music_ambient->play();
    }
    /* armazeno o tempo em que o método foi invocado */
    qint64 time_inicial = QDateTime::currentMSecsSinceEpoch();

    /* armazeno o tempo que passou (atual subtraido da ultima atualização) */
    qint64 time_gasto = time_inicial - _time_final;

    /* acumulo o tempo que passou */
    _time_acumulado += time_gasto;

    /* aguardo o tempo de atualização necessário para atualizar as variáveis */
    while (_time_acumulado >= _constant_dt) {
        /* atualizo as informações de colisões (todas as variaveis dos objetos que são utilizadas
         * para calcular a resolução de impulso) ... */
        _contacts.clear();
        // preciso verificar TODOS COM TODOS...
        for (int i = 0; i < _bodies.size(); ++i) {
            Body *A = _bodies[i];
            for (int j = i + 1; j < _bodies.size(); ++j) {
                Body *B = _bodies[j];
                // se não existir massa inversa pra ao menos 1 objeto nem preciso calcular..
                if (A->_im == 0 && B->_im == 0)
                    continue;
                Manifold m(A, B);
                m.Solve();
                // houve contato? armazeno para resolver em seguida...
                if (m._contact_count)
                    _contacts.push_back(m);
            }
        }

        /* já verifico se um bird colidiu com algum pig */
        for(int i = 0; i < _contacts.size(); ++i) {
            if ((_contacts[i]._A->_shape->_type_object == 'R' || _contacts[i]._A->_shape->_type_object == 'B' || _contacts[i]._A->_shape->_type_object == 'Y')
                    && _contacts[i]._B->_shape->_type_object == 'P' && _contacts[i]._B->_shape->_type_planet != 'I'  && _contacts[i]._A->_shape->_type_planet != 'I') {
                Body *B = _contacts[i]._B;
                _bodies_remove.push_back(B);
            }
            if ((_contacts[i]._B->_shape->_type_object == 'R' || _contacts[i]._B->_shape->_type_object == 'B' || _contacts[i]._B->_shape->_type_object == 'Y')
                    && _contacts[i]._A->_shape->_type_object == 'P' && _contacts[i]._A->_shape->_type_planet != 'I' && _contacts[i]._B->_shape->_type_planet != 'I') {
                Body *A = _contacts[i]._A;
                _bodies_remove.push_back(A);
            }
            //Body *bird = _contacts[i]._B;
            /* som ao colidir com planeta a primeira vez */
            if (_contacts[i]._B->_shape->_type_object == 'R' && !_play_r ) {
                _play_r = true;
                //_bodies_remove.push_back(bird);
                _music_bird_colision->play();
            }
            if (_contacts[i]._B->_shape->_type_object == 'B' && !_play_b) {
                _play_b = true;
                //_bodies_remove.push_back(bird);
                _music_bird_colision->play();
            }
            if (_contacts[i]._B->_shape->_type_object == 'Y' && !_play_y) {
                _play_y = true;
                //_bodies_remove.push_back(bird);
                _music_bird_colision->play();
            }
        }

        /* integro as forças */
        for (int i = 0; i < _bodies.size(); ++i) {
            this->IntegrateForces(_bodies[i]);
        }

        /* inicializo as variaveis para calculo das colisões */
        for (int i = 0; i < _contacts.size(); ++i) {
            _contacts[i].Initialize();
        }

        /* resolvo as colisões
           OBS: ajuste adicional para aumentar credibilidade nas simulações fisicas (resolver impulsos
           sobre os contatos de 5 a 20 vezes, sendo que na equação tem velocidade relativa, utilizando dt))*/
        for(int j = 0; j < 10; ++j) {
            for(int i = 0; i < _contacts.size(); ++i) {
              _contacts[i].ApplyImpulse();
            }
        }

        /* integro as velocidades */
        for (int i = 0; i < _bodies.size(); ++i) {
            this->IntegrateVelocity(_bodies[i]);
        }

        /* corrijo as posições dos objetos quando colidem */
        for(int i = 0; i < _contacts.size(); ++i) {
            _contacts[i].PositionalCorrection();
        }

        /* reseto as forças aplicadas */
        for (int i = 0; i < _bodies.size(); ++i) {
            Body *B = _bodies[i];
            B->_force.Set(0.0, 0.0, 0.0);
            B->_torque = 0.0;

            /* preciso verificar o posicionamento dos birds para atualizar jogada */
            if (B->_shape->_type_object == 'R' || B->_shape->_type_object == 'B' || B->_shape->_type_object == 'Y') {
                /* somente posso atualizar depois que */
                if (B->IsStatic()) {
                    _count_atualizar_bird--;
                    if (_count_atualizar_bird == 0) {
                        _count_atualizar_bird = 150;
                        if (_bird_da_vez == 'B' && !_clicked) {
                            _blue->_position.Set(_x_lanc, _y_lanc, 0.0);
                            _is_repouso = true;
                        }
                        if (_bird_da_vez == 'Y' && !_yellow->_isLancado && !_clicked && !_finish) {
                            _yellow->_position.Set(_x_lanc, _y_lanc, 0.0);
                            _is_repouso = true;
                        }
                    }
                }
            }
        }

        /* tambem verifico se é pra destruir o objeto */
        for (int i = 0; i < _bodies.size(); ++i) {
            Body *B = _bodies[i];
            for (int c = 0; c < _bodies_remove.size(); c++) {
                Body *BB = _bodies_remove[c];
                if (B->_num_remove == BB->_num_remove) {
                    _count_remover--;
                    if (_count_remover == 0) {
                        _count_remover = 1000;
                        _bodies.removeOne(BB);
                        _bodies_remove.removeOne(BB);
                        _music_pig_destroy->play();
                        _qtd_pigs--;
                    }
                }
            }
        }

        /* após todas as atualizações de valores sempre diminuo do acumulado */
        _time_acumulado -= _constant_dt;
    }

    /* atualizo variavel que armazena tempo da ultima invocação do método */
    _time_final = time_inicial;

    /* desenho o cenário */
    this->update();

    if (_bird_da_vez == 'Y' && _yellow->_isLancado && _finish) {
        _count_restart--;
        if (_count_restart == 0) {
            _count_restart = 100;
            _is_repouso = true;
        }
    }

    if (_finish && _is_repouso && _qtd_pigs > 0) {
        // pigs venceram
        _music_ambient->stop();
        if (!_play_der) {
            _play_der = true;
            _music_derrota->play();
        }
        if (_music_derrota->state() != QMediaPlayer::PlayingState) {
            this->Reset();
        }
    } else if (_qtd_pigs <= 0) {
        // jogador venceu
        _music_ambient->stop();
        if (!_play_vit) {
            _play_vit = true;
            _music_vitoria->play();
        }
        if (_music_vitoria->state() != QMediaPlayer::PlayingState) {
            this->Reset();
        }
    }

    /* reseto temporizador */
    QTimer::singleShot(1000 / _max_fps, this, SLOT(tick()));
}

void GLWidget::initializeGL() {
    // aqui passa antes do construtor...
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    /* configurando iluminação */
    GLfloat pos0[4] = { 683.0, 384.0, -500.0, 1.0 };
    GLfloat ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat difuse[4] = { 0.7, 0.7, 0.7, 1.0 };   // cor
    GLfloat specular[4] = { 1.0, 1.0, 1.0, 1.0 }; // brilho
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    //glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 90);         // abertura do holofote
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);  // realiza o calculo do normal em ambos os lados dos objetos
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, difuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    GLfloat especularidade[4]={ 1.0, 1.0, 1.0, 1.0 };
    GLint especMaterial = 60;
    glMaterialfv(GL_FRONT,GL_SPECULAR, especularidade);  // refletancia
    glMateriali(GL_FRONT,GL_SHININESS, especMaterial);   // concentração do brilho

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

    glEnable(GL_NORMALIZE);

    /* Enable textures */
    this->initTextures();
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    this->drawScene();

    glFlush();
}

void GLWidget::drawScene() {
    /* planeta origem */
    _base->_shape->Draw();
    /* slingshot */
    _sling->_shape->Draw();
    if (_is_repouso && !_finish)
        this->drawSling();

    /* planeta inimigo, birds, blocos, pigs */
    for (int i = 0; i < _bodies.size(); ++i) {
        Body *b = _bodies[i];
        b->_shape->Draw();
    }
    if (!_red->_isLancado)
        _red->_shape->Draw();
    if (!_blue->_isLancado)
        _blue->_shape->Draw();
    if (!_yellow->_isLancado)
        _yellow->_shape->Draw();

    this->drawBackgound();
}

void GLWidget::drawSling() {
    glPushMatrix();
      glColor3ub(0, 0, 0);
      glBegin(GL_LINES);
      // iteração para desenhar cada vértice que irá representar ligação entre as linhas do circulo
      float pos_x, pos_y;
      if (_bird_da_vez == 'R') {
          pos_x = _red->_position.x();
          pos_y = _red->_position.y() + 3;
      }
      if (_bird_da_vez == 'B') {
          pos_x = _blue->_position.x();
          pos_y = _blue->_position.y() + 3;
      }
      if (_bird_da_vez == 'Y') {
          pos_x = _yellow->_position.x();
          pos_y = _yellow->_position.y() + 3;
      }
      glVertex2f(_x_lanc, _y_lanc + 2);
      glVertex2f(pos_x, pos_y);
      glEnd();
    glPopMatrix();
}

Body *GLWidget::Add(Shape *shape, GLfloat x, GLfloat y, GLfloat z, int num) {
    Body *b = new Body(shape, x, y, z);
    b->_num_remove = num;
    _bodies.push_back(b);
    return b;
}

void GLWidget::IntegrateForces(Body *b) {
  if(b->_im == 0.0f)
    return;
  /* preciso verificar a proximidade com o planeta..para aplicar força gravitacional do mesmo no objeto */
  float distance = Vector::Distance(b->_position, _base_inimiga->_position);
  if (distance <= _base_inimiga->_shape->_raio_G) {
      float angle = Vector::AngleBetween(b->_position, _base_inimiga->_position);
      Vector v_force(gravityForce * cos(angle) * forceReducer, gravityForce * sin(angle) * forceReducer, 0.0);
      b->ApplyForce(v_force);
  }
  b->_velocity.Add((b->_force * b->_im) * (_constant_dt / 2.0f));
  b->_angularVelocity += b->_torque * b->_iI * (_constant_dt / 2.0f);
}

void GLWidget::IntegrateVelocity(Body *b) {
  if(b->_im == 0.0f)
    return;
  b->_position.Add(b->_velocity * _constant_dt);
  b->_orient += b->_angularVelocity * _constant_dt;
  b->SetOrient(b->_orient);
  this->IntegrateForces(b);
}

void GLWidget::Reset() {
    _contacts.clear();
    _bodies_remove.clear();
    _bodies.clear();
    _bodies.push_back(_base_inimiga);

    _x_lanc = 200.0;
    _y_lanc = 472.0;
    _angulo_lancamento = 0.0;
    _clicked = false;
    _slingshot = false;
    _bird_da_vez = 'R';
    _is_repouso = true;
    _finish = false;
    _play_r = false;
    _play_b = false;
    _play_b2 = false;
    _play_b3 = false;
    _play_y = false;
    _blue_espec = false;
    _yellow_espec = false;
    _count_remover = 1000;
    _count_atualizar_bird = 150;
    _count_restart = 100;
    _qtd_pigs = 3;
    _play_vit = false;
    _play_der = false;

    /* birds */
    _red->_position.Set(_x_lanc, _y_lanc, 0.0);
    _red->_velocity.Mult(0);
    _red->_torque = 0.0;
    _red->_isLancado = false;
    _blue->_position.Set(228.0, 537.0, 0.0);
    _blue->_velocity.Mult(0);
    _blue->_torque = 0.0;
    _blue->_isLancado = false;
    _blue_2->_velocity.Mult(0);
    _blue_2->_torque = 0.0;
    _blue_3->_velocity.Mult(0);
    _blue_3->_torque = 0.0;
    _yellow->_position.Set(165.0, 540.0, 0.0);
    _yellow->_velocity.Mult(0);
    _yellow->_torque = 0.0;
    _yellow->_isLancado = false;

    /* pigs */
    ModeledObject pig1('P');                // cima
    this->Add(&pig1, 1000.0, 149.0, 0.0, 11);
    ModeledObject pig2('P');                // lado
    this->Add(&pig2, 1211.0, 369.0, 0.0, 22);
    ModeledObject pig3('P');                // baixo
//    this->Add(&pig3, 1000.0, 500.0, 0.0, 33);
    this->Add(&pig3, 1111.0, 420.0, 0.0, 33);

    /* blocks */
    Block b(_tex_name[3], 'M');    // pé e
    b.SetBox(3.0f, 20.0f);
    Body *bb = this->Add(&b, 970.0, 192.0, 0.0, 0);
    bb->SetOrient(0);
    Block b2(_tex_name[3], 'M');   // pé d
    b2.SetBox(3.0f, 20.0f);
    Body *bb2 = this->Add(&b2, 1030.0, 192.0, 0.0, 0);
    bb2->SetOrient(0);
    Block b4(_tex_name[2], 'P'); // deitado
    b4.SetBox(40.0f, 3.0f);
    Body *bb4 = this->Add(&b4, 1000.0, 168.0, 0.0, 0);
    bb4->SetOrient(0);
    Block b3(_tex_name[2],'P');
    b3.SetBox(10.0f, 10.0f);
    Body *bb3 = this->Add(&b3, 950.0, 206.0, 0.0, 0);
    bb3->SetOrient(-2);
    // ---------------- cima
    Block b5(_tex_name[2],'P');
    b5.SetBox(8, 12);
    Body *bb5 = this->Add(&b5, 965.0, 518.0, 0.0, 0);
    bb5->SetOrient(0);
    Block b6(_tex_name[2],'P');
    b6.SetBox(8, 12);
    Body *bb6 = this->Add(&b6, 1035.0, 520.0, 0.0, 0);
    bb6->SetOrient(0);
    Block b7(_tex_name[3],'M');
    b7.SetBox(12, 8);
    Body *bb7 = this->Add(&b7, 965.0, 539.0, 0.0, 0);
    bb7->SetOrient(0);
    Block b8(_tex_name[3],'M');
    b8.SetBox(12, 8);
    Body *bb8 = this->Add(&b8, 1035.0, 539.0, 0.0, 0);
    bb8->SetOrient(0);
    Block b9(_tex_name[3], 'M');
    b9.SetBox(36, 4);
    Body *bb9 = this->Add(&b9, 999.9, 553.0, 0.0, 0);
    bb9->SetOrient(0);
    Block b10(_tex_name[2],'P');
    b10.SetBox(10, 10);
    Body *bb10 = this->Add(&b10, 999.9, 570.0, 0.0, 0);
    bb10->SetOrient(0);
    // ---------------- baixo
    Block b11(_tex_name[2], 'P');
    b11.SetBox(10, 10);
    Body *bb11 = this->Add(&b11, 1162.0, 369.9, 0.0, 0);
    bb11->SetOrient(0);
    Block b12(_tex_name[3], 'M');
    b12.SetBox(10, 10);
    Body *bb12 = this->Add(&b12, 1185.0, 369.9, 0.0, 0);
    bb12->SetOrient(0);
    // ---------------- lado
    Block b14(_tex_name[3], 'M');
    b14.SetBox(15, 15);
    Body *bb14 = this->Add(&b14, 830.0, 368.9, 0.0, 0);
    bb14->SetOrient(0);
    Block b15(_tex_name[3], 'M');
    b15.SetBox(5, 25);
    Body *bb15 = this->Add(&b15, 805.0, 371.0, 0.0, 0);
    bb15->SetOrient(0);
    Block b16(_tex_name[3], 'M');
    b16.SetBox(10, 10);
    Body *bb16 = this->Add(&b16, 785.0, 370.9, 0.0, 0);
    bb16->SetOrient(0);
    Block b17(_tex_name[2], 'P');
    b17.SetBox(15, 15);
    Body *bb17 = this->Add(&b17, 1130.0, 270.0, 0.0, 0);
    bb17->SetOrient(-7);
    Block b18(_tex_name[3], 'M');
    b18.SetBox(5, 25);
    Body *bb18 = this->Add(&b18, 1150.0, 260.0, 0.0, 0);
    bb18->SetOrient(-7);
    Block b19(_tex_name[3], 'M');
    b19.SetBox(10, 10);
    Body *bb19 = this->Add(&b19, 1170.0, 250.0, 0.0, 0);
    bb19->SetOrient(-7);

}

void GLWidget::resizeGL(int w, int h) {
    _width = w;
    _height = h;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluOrtho2D(0, w, h, 0);

    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::initTextures() {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &_tex_name[0]);
    // fundo...
    glBindTexture(GL_TEXTURE_2D, _tex_name[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _backgound.load(":/pasta/space_blue.jpg");
    if(_backgound.isNull())  {
        std::cout << "ERRO ao carregar imagem de background!";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 _backgound.width(),
                 _backgound.height(),
                 1,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 _backgound.bits());

    // planatas
    glBindTexture(GL_TEXTURE_2D, _tex_name[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _planets.load(":/pasta/planets.jpg");
    if(_planets.isNull())  {
        std::cout << "ERRO ao carregar imagem de planeta!";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 _planets.width(),
                 _planets.height(),
                 1,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 _planets.bits());

    // pedra
    glBindTexture(GL_TEXTURE_2D, _tex_name[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _pedra.load(":/pasta/bloc_pedra3.jpg");
    if(_pedra.isNull())  {
        std::cout << "ERRO ao carregar imagem de pedra!";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 _pedra.width(),
                 _pedra.height(),
                 1,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 _pedra.bits());

    // madeira
    glBindTexture(GL_TEXTURE_2D, _tex_name[3]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    _madeira.load(":/pasta/bloc_madeira3.jpg");
    if(_madeira.isNull())  {
        std::cout << "ERRO ao carregar imagem de madeira!";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 _madeira.width(),
                 _madeira.height(),
                 1,
                 GL_BGRA,
                 GL_UNSIGNED_BYTE,
                 _madeira.bits());
}

void GLWidget::drawBackgound() {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, _tex_name[0]);

    glPushMatrix();
        glTranslatef(0.0, 0.0, 0.0);
        glColor3ub(204, 204, 204);
        glBegin(GL_QUADS);
            // preenchendo toda tela...
            glTexCoord2f(0.0, 0.0);
            glVertex2f(0.0, 0.0);

            glTexCoord2f(1.0, 0.0);
            glVertex2f(_width, 0.0);

            glTexCoord2f(1.0, 1.0);
            glVertex2f(_width, _height);

            glTexCoord2f(0.0, 1.0);
            glVertex2f(0.0, _height);
        glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void GLWidget::keyPressEvent(QKeyEvent *e) {
    QGLWidget::keyPressEvent(e);
    switch (e->key()) {
        case Qt::Key_R:
            this->Reset();
        break;
        case Qt::Key_Escape:
            exit(0);
        break;

        default:
        break;
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *e) {
    QGLWidget::mouseMoveEvent(e);

    /* armazeno a posição do mouse para atualizar bird */
     QPoint pos = e->pos();
     if (_clicked) {
         _music_sling->play();
        // this->atualizaPosicaoMouse(pos.x(), pos.y());
         // verifico o limite maximo
         //std::cout << "CAIU MOVEEE" << std::endl;
         /* verifico qual é o bird da vez */
         if (_bird_da_vez == 'R') {
             _red->_position.Set(pos.x(), pos.y(), 0.0);
         } else if (_bird_da_vez == 'B') {
            _blue->_position.Set(pos.x(), pos.y(), 0.0);
         } else if (_bird_da_vez == 'Y') {
             _yellow->_position.Set(pos.x(), pos.y(), 0.0);
         }

     }
}

void GLWidget::mousePressEvent(QMouseEvent *e) {
    QGLWidget::mousePressEvent(e);
    /* verifico se o botão esquerdo foi pressionado armazeno a posição do mause */
    if (e->button() == Qt::LeftButton) {
        QPoint pos = e->pos();
        std::cout << "width: " <<  _width << " height: " << _height << std::endl;
        std::cout << "click X: " <<  pos.x() << " click Y: " << pos.y() << std::endl;

        //this->atualizaPosicaoMouse(pos.x(), pos.y());

        /* se o mouse foi pressionado na posição de lançamento atualizo variavel */
        if (((pos.x() > 188 && pos.x() < 208) && (pos.y() > 463 && pos.y() < 482))) {
            //std::cout << "CAIU PREEEES" << std::endl;
            _clicked = true;
        }
    }
    if (e->button() == Qt::RightButton) {
        /* aplicando especificidade de lançamento pros birds blue e yellow */
        if (_blue->_isLancado && !_blue_espec) {
            _blue_espec = true;
            // crio mais dois objetos blue a partir da posição da do blue no momento do click
            _blue_2->_position.Set(_blue->_position.x() + 30, _blue->_position.y() + 30, 0.0);
            _blue_2->_velocity.Set(_blue->_velocity.x(), _blue->_velocity.y(), 0.0);
            Vector force(cos(120 * M_PI / 180) * 10, sin(120 * M_PI / 180) * 10, 0.0);
            _blue_2->ApplyForce(force);
            /* momento de adicionar na lista */
            _bodies.push_back(_blue_2);

            _blue_3->_position.Set(_blue->_position.x() + 30, _blue->_position.y() - 30, 0.0);
            _blue_3->_velocity.Set(_blue->_velocity.x(), _blue->_velocity.y(), 0.0);
            Vector force2(cos(-120 * M_PI / 180) * 10, sin(-120 * M_PI / 180) * 10, 0.0);
            _blue_3->ApplyForce(force2);
            /* momento de adicionar na lista */
            _bodies.push_back(_blue_3);

        } else if (_yellow->_isLancado && !_yellow_espec) {
            _yellow_espec = true;
            // dobro a velocidade no momento do click
            _yellow->_velocity.Mult(2);
        }
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e) {
    QGLWidget::mouseReleaseEvent(e);
    /* verifico se o botão esquerdo foi solto */
    if (_clicked && e->button() == Qt::LeftButton) {
        QPointF pos = e->pos();
        Vector v_pos(pos.x(), pos.y(), 0.0);
        Vector v_lanc(_x_lanc, _y_lanc, 0.0);
        //std::cout << "CAIU RELEEEESE" << std::endl;
        _clicked = false;
        /* armazeno a distancia do deslocamento para calcular a aceleração a ser aplicada */
        // 71 é a distancia maior permitida
        float dist = Vector::Distance(v_pos, v_lanc);
        float acc = 0.0;
        if (dist >= 71)
            acc = 30;
        if (dist < 71 && dist >= 50)
            acc = 27;
        if (dist < 50 && dist >= 30)
            acc = 23;

        /* atualizo o angulo de lançamento */
        _angulo_lancamento = 0.0;
        _angulo_lancamento = Vector::AngleBetweenLanc(v_pos, v_lanc);

        /* ordem de lançamento */
        if (_bird_da_vez == 'R') { // red
            if (acc == 0.0) {
                _red->_position.Set(_x_lanc, _y_lanc, 0.0);
            } else {
                /* aplico a força da aceleração com base na distancia */
                Vector force(cos(_angulo_lancamento * M_PI / 180) * acc, sin(_angulo_lancamento * M_PI / 180) * acc, 0.0);
                _red->ApplyForce(force);
                /* momento de adicionar na lista */
                _red->_isLancado = true;
                _is_repouso = false;
                _bodies.push_back(_red);
                _bird_da_vez = 'B';
                _music_lanc_blue->play();
            }
        } else if (_bird_da_vez == 'B') { // blue
            if (acc == 0.0) {
                _blue->_position.Set(_x_lanc, _y_lanc, 0.0);
            } else {
                Vector force(cos(_angulo_lancamento * M_PI / 180) * acc, sin(_angulo_lancamento * M_PI / 180) * acc, 0.0);
                _blue->ApplyForce(force);
                _blue->_isLancado = true;
                _is_repouso = false;
                _bodies.push_back(_blue);
                _bird_da_vez = 'Y';
                _music_lanc_blue->play();
            }
        } else if (_bird_da_vez == 'Y') { // yellow
            if (acc == 0.0) {
                _yellow->_position.Set(_x_lanc, _y_lanc, 0.0);
            } else {
                Vector force(cos(_angulo_lancamento * M_PI / 180) * acc, sin(_angulo_lancamento * M_PI / 180) * acc, 0.0);
                _yellow->ApplyForce(force);
                /* momento de adicionar na lista */
                _yellow->_isLancado = true;
                _is_repouso = false;
                _bodies.push_back(_yellow);
                /* ultimo bird lançado, o jogo será resetado */
                _finish = true;
                _music_lanc_yellow->play();
            }
        }
    }
}


