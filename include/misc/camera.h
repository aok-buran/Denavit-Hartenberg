#pragma once

#include <Eigen/Dense>
#include <utility>

#include "glHelper.h"

using namespace Eigen;

// камера определяется тремя векторами:
// pos - положение камеры
// dir - направление камеры
// up - вектор "вверх"
class Camera {
    Vector3d pos; // положение камеры
    Vector3d dir; // направление камеры
    Vector3d up; // вектор "вверх"

    // получить точку, на которую смотрит камера
    Vector3d getEye() { return dir + pos; }

public:
    Camera();

    Camera(Vector3d pos, Vector3d dir, Vector3d up) :
            pos(std::move(pos)), dir(std::move(dir)), up(std::move(up)) {};

    // поворот камеры относительно оси OX экрана
    void rotateX(double alpha);

    // поворот камеры относительно оси OY экрана
    void rotateY(double alpha);

    // переместить камеру влево
    void moveLeft(double d);

    // переместить камеру вправо
    void moveRight(double d);

    // переместить камеру вперёд
    void moveForward(double d) { pos = pos - dir * d; }

    // переместить камеру назад
    void moveBack(double d) { pos = pos + dir * d; }

    // переместить камеру вниз
    void moveDown(double d) { pos(2) -= d; }

    // переместить камеру вверх
    void moveUp(double d) { pos(2) += d; }

    // применить параметры камеры к OpenGL
    void applyLookAt();
};


