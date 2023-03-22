#include "misc/glHelper.h"

// получаем матрицу поворота на угол alpha вдоль вектора v
Matrix3d GLHelper::getRMatrix(double alpha, Vector3d v) {
    double x = v(0);
    double y = v(1);
    double z = v(2);

    Matrix3d R;
    R << cos(alpha) + (1 - cos(alpha)) * x * x,
            (1 - cos(alpha)) * x * y - (sin(alpha)) * z,
            (1 - cos(alpha)) * x * z + (sin(alpha)) * y,

            (1 - cos(alpha)) * y * x + (sin(alpha)) * z,
            cos(alpha) + (1 - cos(alpha)) * y * y,
            (1 - cos(alpha)) * y * z - (sin(alpha)) * x,

            (1 - cos(alpha)) * z * x - (sin(alpha)) * y,
            (1 - cos(alpha)) * z * y + (sin(alpha)) * x,
            cos(alpha) + (1 - cos(alpha)) * z * z;

    return R;
}

// по двум точкам получаем три ортогональных базисных вектора,
// первый сонаправлен с AB
// требуется удалить в конце использования данные
Vector3d *GLHelper::ortho3(Vector3d A, Vector3d B) {
    // проверяем, что векторы не совпадают
    if (A(0) == B(0) && A(1) == B(1) && A(2) == B(2))
        throw std::runtime_error(std::string("A and B must be different vectors"));

    Vector3d n1 = B - A;

    double x2;
    double y2;
    double z2;

    if (n1(0) != 0) {
        y2 = 0.5;
        z2 = 0.5;
        x2 = (-n1(2) * (z2 - A(2)) - n1(1) * (y2 - A(1))) / n1(0) - A(0);
    } else if (n1(1) != 0) {
        x2 = 0.5;
        z2 = 0.5;
        y2 = (-n1(0) * (x2 - A(0)) - n1(2) * (z2 - A(2))) / n1(1) - A(1);
    } else {
        x2 = 0.5;
        y2 = 0.5;
        z2 = (-n1(0) * (x2 - A(0)) - n1(1) * (y2 - A(1))) / n1(2) - A(2);
    }

    auto *n = new Vector3d[3];
    n[0] = n1.normalized();
    n[1] = Vector3d(x2, y2, z2).normalized();
    n[2] = n[0].cross(n[1]).normalized();

    return n;
}

// метод для упрощения вызова glVertex3d от вектора
void GLHelper::vectorVertex(Vector3d v) {
    glVertex3d(v(0), v(1), v(2));
}

// получить набор координат, образующий окружность (9 точек) в
// базисе векторов n1 и n2, сохраняет результат в массив coords
// требуется удалить в конце использования данные
Vector3d *GLHelper::getCircleCoords(const Vector3d &n1, const Vector3d &n2) {
    auto *coords = new Vector3d[9];
    coords[0] = n1;
    coords[1] = (n1 + n2) / sqrt(2);
    coords[2] = n2;
    coords[3] = (n2 - n1) / sqrt(2);
    coords[4] = -n1;
    coords[5] = (-n1 - n2) / sqrt(2);
    coords[6] = -n2;
    coords[7] = (-n2 + n1) / sqrt(2);
    coords[8] = n1;
    return coords;
}

// Нарисовать координатную ось от A к B заданного цвета color
// и размера R, цвет задаётся с помощью первых трёх элементов массива colors
void GLHelper::drawArrow3D(
        const Vector3d &A, const Vector3d &B, double *color, double R
) {
    // получаем три ортогональных базисных вектора,
    // первый сонаправлен с AB
    Vector3d *n = ortho3(A, B);
    // получаем по последним двум базисным векторам
    // координаты окружности, перпендикулярной AB
    Vector3d *circleCoords = getCircleCoords(n[1], n[2]);


    // задаём цвет рисования
    glColor4d(color[0], color[1], color[2], 0.8);

    // рисуем цилиндр прямоугольниками
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i < 9; i++) {
        vectorVertex(A + circleCoords[i] * R);
        vectorVertex(B + circleCoords[i] * R);
    }
    glEnd();

    // рисуем конус треугольниками, первая вершина
    // участвует во всех треугольниках
    glBegin(GL_TRIANGLE_FAN);
    // задаём её
    vectorVertex(B + n[0] * 4 * R);
    // в цикле добавляем оставшиеся вершины
    for (int i = 0; i < 9; i++) {
        vectorVertex(B + circleCoords[i] * 2 * R);
    }
    glEnd();
    delete n;
    delete circleCoords;
}

// нарисовать базис заданного размера
void GLHelper::drawBasis(double size) {
    GLHelper::drawArrow3D({0, 0, 0}, {size, 0, 0}, new double[3]{0.6, 0.15, 0.17}, 0.5);
    GLHelper::drawArrow3D({0, 0, 0}, {0, size, 0}, new double[3]{0.17, 0.6, 0.15}, 0.5);
    GLHelper::drawArrow3D({0, 0, 0}, {0, 0, size}, new double[3]{0.15, 0.17, 0.6}, 0.5);
}

// нарисовать базис по матрице трансформации
void GLHelper::drawBasis(Matrix4d transformMatrix) {
    Vector3d nx(transformMatrix(0, 0), transformMatrix(1, 0), transformMatrix(2, 0));
    Vector3d ny(transformMatrix(0, 1), transformMatrix(1, 1), transformMatrix(2, 1));
    Vector3d nz(transformMatrix(0, 2), transformMatrix(1, 2), transformMatrix(2, 2));
    Vector3d pos(transformMatrix(0, 3), transformMatrix(1, 3), transformMatrix(2, 3));

    GLHelper::drawArrow3D(pos, pos + 4 * nx, new double[3]{0.6, 0.15, 0.17}, 0.3);
    GLHelper::drawArrow3D(pos, pos + 4 * ny, new double[3]{0.17, 0.6, 0.15}, 0.3);
    GLHelper::drawArrow3D(pos, pos + 4 * nz, new double[3]{0.15, 0.17, 0.6}, 0.3);
}



