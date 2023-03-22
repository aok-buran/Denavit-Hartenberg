#pragma once

#include <fstream>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <Eigen/Dense>


using namespace std;
using namespace Eigen;

// вспомогательная структура треугольника
struct Triangle {
    Vector3d vertices[3];
};

// класс для работы с stl-моделями
class Model3D {
    // массив треугольников
    Triangle *triangleList{};
    // количество треугольников
    int triangleCnt;
public:
    // конструктор
    Model3D() : triangleCnt(0) {};

    // деструктор
    virtual ~Model3D() { delete triangleList; };

    // загрузка модели из файла
    void load(const string &modelPath);

    // рисование модели по матрице трансформации
    void render(const Matrix4d &transformMatrix) const;
};
