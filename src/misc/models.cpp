#include "misc/models.h"

// загрузка модели из файла
void Model3D::load(const std::string& modelPath) {
    // открываем поток данных из файла
    fstream modelFStream;
    modelFStream.open(modelPath, std::ios::in | std::ios::binary);
    // если получилось открыть
    if (modelFStream) {
        // читаем заголовок файла
        char head[80] = "";
        modelFStream.read(head, 80);

        // получаем количество полигонов
        int32_t size;
        modelFStream.read(reinterpret_cast<char *> (&size), sizeof(int32_t));
        triangleCnt = size;
        triangleList = new Triangle[size];

        // читаем координаты вершин
        Vector3d P0, P1, P2;
        Vector3d normal;
        float p0[3], p1[3], p2[3], n[3];
        char attribute[2] = "0";
        for (int i = 0; i < size; i++) {

            modelFStream.read(reinterpret_cast<char *> (&n[0]), 4);
            modelFStream.read(reinterpret_cast<char *> (&n[1]), 4);
            modelFStream.read(reinterpret_cast<char *> (&n[2]), 4);

            modelFStream.read(reinterpret_cast<char *> (&p0[0]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p0[1]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p0[2]), 4);
            triangleList[i].vertices[0] = {p0[0], p0[1], p0[2]};


            modelFStream.read(reinterpret_cast<char *> (&p1[0]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p1[1]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p1[2]), 4);
            triangleList[i].vertices[1] = {p1[0], p1[1], p1[2]};

            modelFStream.read(reinterpret_cast<char *> (&p2[0]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p2[1]), 4);
            modelFStream.read(reinterpret_cast<char *> (&p2[2]), 4);

            triangleList[i].vertices[2] = {p2[0], p2[1], p2[2]};
            modelFStream.read(attribute, 2);
        }
        // закрываем поток
        modelFStream.close();
    } else {
        triangleCnt = 0;
        std::cout<<"File not found"<<std::endl;
    }
}

// рисование модели по матрице трансформации
void Model3D::render(const Matrix4d& transformMatrix) const {

    // подготавливаем рисование треугольников
    glEnable(GL_BLEND);
    glBegin(GL_TRIANGLES);

    // перебираем полигоны модели
    for (int i = 0; i < triangleCnt; i++) {
        // пов вершинам получаем три четырёхмерных вектора от одной к другой
        Vector3d v1 = triangleList[i].vertices[0];
        Vector3d v2 = triangleList[i].vertices[1];
        Vector3d v3 = triangleList[i].vertices[2];
        Vector4d v14(v1(0), v1(1), v1(2), 1),
                v24(v2(0), v2(1), v2(2), 1),
                v34(v3(0), v3(1), v3(2), 1);

        // применяем к векторам трансформацию
        v14 = transformMatrix * v14;
        v24 = transformMatrix * v24;
        v34 = transformMatrix * v34;

        // рисуем соответствующий треугольник по трём полученным вершинам
        glVertex3d(v14(0), v14(1), v14(2));
        glVertex3d(v24(0), v24(1), v24(2));
        glVertex3d(v34(0), v34(1), v34(2));
    }

    // завершаем рисование треугольников
    glEnd();
    glDisable(GL_BLEND);
}

