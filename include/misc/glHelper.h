#pragma once

#include <GL/gl.h>
#include <GL/glut.h>
#include <Eigen/Dense>
#include <iostream>

#include "camera.h"

using namespace Eigen;

// вспомогательный класс для работы с OpenGL
class GLHelper{
    // по двум точкам получаем три ортогональных базисных вектора,
    // первый сонаправлен с AB
    static Vector3d * ortho3(Vector3d A, Vector3d B);
    // получить набор координат, образующий окружность (9 точек) в
    // базисе векторов n1 и n2, сохраняет результат в массив coords
    static Vector3d * getCircleCoords(const Vector3d& n1, const Vector3d& n2);
    // метод для упрощения вызова glVertex3d от вектора
    static void vectorVertex(Vector3d v);
public:
    // Нарисовать координатную ось от A к B заданного цвета color
    // и размера R, цвет задаётся с помощью первых трёх элементов массива colors
    static void drawArrow3D(const Vector3d& A, const Vector3d& B, double *color, double R);
    // нарисовать базис заданного размера
    static void drawBasis(double  size);
    // нарисовать базис по матрице трансформации
    static void drawBasis(Matrix4d transformMatrix);
    // получаем матрицу поворота на угол alpha вдоль вектора v
    static Matrix3d getRMatrix(double alpha, Vector3d v);
};

