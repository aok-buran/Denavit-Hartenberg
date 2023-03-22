#include "misc/camera.h"

// конструктор по умолчанию
Camera::Camera() {
    dir << 1, 0, 0;
    pos << 0, 0, 0;
    up << 0, 0, 1;
}

// поворот камеры относительно оси OX экрана
void Camera::rotateX(double alpha) {
    // получаем матрицу поворота относительно вектора up
    Matrix3d R = GLHelper::getRMatrix(alpha, up);
    // поворачиваем вектор dir
    dir = R * dir;

    // получаем матрицу поворота относительно оси z
    Matrix3d Rup = GLHelper::getRMatrix(alpha, Vector3d(0, 0, 1));
    // поворачиваем вектор up
    up = Rup * up;
}

// поворот камеры относительно оси OY экрана
void Camera::rotateY(double alpha) {
    // получаем угол между осью z и направлением камеры
    double cDir = dir.dot(Vector3d(0, 0, 1));
    // при малых углах начинается синуглярность, поэтому
    // не даём в таких случаях посмотреть камерой вверх и вниз
    if ((cDir < 0.7 && alpha < 0) || (cDir > -0.7 && alpha > 0)) {
        // если угол позволяет сделать поворот,
        // то получаем единичный вектор, перпендикулярный dir и up
        Vector3d r = up.cross(dir).normalized();
        // получаем из него и угла alpha матрицу поворота
        Matrix3d R = GLHelper::getRMatrix(alpha, r);
        // получаем новые значения векторов
        up = R * up;
        dir = R * dir;
    }
}

// переместить камеру вправо
void Camera::moveRight(double d) {
    Vector3d r = up.cross(dir).normalized();
    pos += r * d;
}

// переместить камеру влево
void Camera::moveLeft(double d) {
    Vector3d r = up.cross(dir).normalized();
    pos -= r * d;
}

// применить параметры камеры к OpenGL
void Camera::applyLookAt() {
    // получаем точку, на которую смотрит камера
    Vector3d eye = getEye();
    // задаём параметры камеры OpenGL
    gluLookAt(eye(0), eye(1), eye(2),
              pos(0), pos(1), pos(2),
              up(0), up(1), up(2));
}
