#pragma once

#include <vector>
#include <cstdlib>
#include <Eigen/Dense>
#include <cmath>
#include <json/json.h>

#include "misc/models.h"
#include "misc/glHelper.h"

using namespace Eigen;

// класс робота
class Robot {
    // можель звена робота
    Model3D *joint;
    // Параметры DH, представляют из себя вектор векторов
    // каждый из вложенных векторов имеет длину 5: в первых
    // четырёх элементах хранятся параметры ДХ,
    // в пятом - текущий угол поворота звена
    std::vector<std::vector<double>> dhParams;
    // последнее положение рабочего инструмента
    Vector3d endEffectorPos;
    // список индексов невиртуальных звеньев
    std::vector<int> nonVirtualJoints;
    // кол-во звеньев
    int jointCnt;
    // загрузить параметры Денавита-Хартенберга из json-файла
    void loadDH(const std::string& dhFilePath);
public:
    Robot(); // конструктор робота
    virtual ~Robot(); // деструктор робота
    void init(const std::string& dhFilePath); // инициализация робота
    void render(); // рисование робота
    // повернуть j-е звено робота на угол alpha
    void rotateLink(int j, double alpha);
    Vector3d getEndEffectorPos(){ return endEffectorPos;}
    std::vector<std::vector<double>> getDHParams() { return dhParams; }
};
