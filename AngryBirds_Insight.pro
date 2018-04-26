QT += widgets opengl multimedia
LIBS += -lopengl32 -lglu32 -lglut32

SOURCES += \
    main.cpp \
    glm.cpp \
    glwidget.cpp \
    body.cpp \
    manifold.cpp

HEADERS += \
    glm.h \
    glwidget.h \
    vector.h \
    shape.h \
    mat.h \
    body.h \
    manifold.h

RESOURCES += \
    resources.qrc


