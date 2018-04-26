#include "body.h"
#include "shape.h"

Body::Body(Shape *shape, GLfloat x, GLfloat y, GLfloat z)
  : _shape(shape->Clone()) {
    _isLancado = false;
    _shape->_body = this;
    if (_shape->GetType() == Shape::eCircle && (_shape->_type_planet == 'I' || _shape->_type_planet == 'M')) {
        _position_draw_planet.Set(x, y, z);
        _position.Set(x + 500.0, y + 179.0, z);
    } else {
        _position.Set(x, y, z);
    }
    _position_repouso = _position;
    _velocity.Set(0.0f, 0.0f, 0.0f);
    _angularVelocity = 0.0f;
    _torque = 0.0f;
    _orient = 0.0f;
    _force.Set(0.0f, 0.0f, 0.0f);
    _staticFriction = 0.5f;
    _dynamicFriction = 0.3f;
    _restitution = 0.2f;
    _shape->Initialize();
}

void Body::ApplyForce(Vector f) {
    _force.Add(f);
}

void Body::ApplyImpulse(const Vector &impulse, const Vector &contactVector) {
    _velocity.Add(impulse * _im);
    _angularVelocity += _iI * Vector::Cross(contactVector, impulse);
}

void Body::SetStatic(void) {
    _I = 0.0f;
    _iI = 0.0f;
    _m = 0.0f;
    _im = 0.0f;
}

void Body::SetOrient(float radians) {
  _orient = radians;
  _shape->SetOrient(radians);
}

bool Body::IsStatic() {
    if (_isLancado
            && (_position_repouso.x() == _position.x() || (_position_repouso.x() <= _position.x() + 2 || _position_repouso.x() >= _position.x() - 2))
            && (_position_repouso.y() == _position.y() || (_position_repouso.y() <= _position.y() + 2 || _position_repouso.y() >= _position.y() - 2))) {
        return true;
    } else {
        // preciso verificar se esta entre os limites da tela
        if (_position.x() > 0 && _position.x() < 1366 && _position.y() > 0 && _position.y() < 768) {
            _position_repouso = _position;
            return false;
        } else {
            return true;
        }
    }
}
