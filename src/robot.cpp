#include "robot.h"

// конструктор робота
Robot::Robot() {
    // создаём модель звена
    joint = new Model3D();
    // кол-во звеньев равно нулю
    jointCnt = 0;
    // в качестве положения рабочего инструмента
    // принимается нулевой вектор
    endEffectorPos = Vector3d(0, 0, 0);
}

// деструктор робота
Robot::~Robot() {
    // освобождаем память модели извена
    delete joint;
}

// инициализация робота
void Robot::init(const std::string &dhFilePath) {
    // загружаем модель звена
    joint->load("../data/joint.stl");
    // загружаем ДХ параметры
    loadDH(dhFilePath);
}

// повернуть j-е звено робота на угол alpha
void Robot::rotateLink(int j, double alpha) {
    if (j < jointCnt)
        dhParams.at(nonVirtualJoints.at(j)).at(4) += alpha;
}

// загрузить параметры Денавита-Хартенберга из json-файла
void Robot::loadDH(const std::string &dhFilePath) {
    // подготавливаем json-данные
    Json::Reader reader;
    Json::Value obj;
    std::ifstream ifs(dhFilePath.c_str(), std::ios_base::binary);
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        (std::istreambuf_iterator<char>()));
    reader.parse(content, obj);
    std::cout << "Robot name: " << obj["name"].asString() << std::endl;

    // обнуляем количество звеньев
    jointCnt = 0;
    // перебираем звенья из json и заполняем ДХ-параметры
    for (int i = 0; i < (int)obj["links"].size(); i++) {
        double d = obj["links"][i]["d"].asDouble() / 10;
        double a = obj["links"][i]["a"].asDouble() / 10;
        double alpha = obj["links"][i]["alpha"].asDouble();
        double theta = obj["links"][i]["theta"].asDouble();
        bool isVirtual = obj["links"][i]["virtual"].asBool();
        double theta_0 = 0;
        // если звено не виртуальное
        if (!isVirtual) {
            // добавляем индекс этого звена в список не вирутальных
            nonVirtualJoints.push_back(i);
            // если для нового невиртуального звена есть начальное значение
            if (nonVirtualJoints.size() <= obj["jointInitValues"][i].size())
                // сохраняем его
                theta_0 = obj["jointInitValues"][i].asDouble();
        }
        // создаём новый набор параметров ДХ
        vector<double> dhParam{theta, d, a, alpha, theta_0 / 180 * M_PI, isVirtual ? 0.0 : 1.0};
        // добавляем его в список
        dhParams.push_back(dhParam);
        // увеличиваем количество звеньев на 1
        jointCnt++;
    }
}

// получить матрицу перехода из параметров Денавита-Хартенберга
Matrix4d getDHMatrix(vector<double> dh) {
    // заполняем параметры
    double theta = dh.at(0) + dh.at(4);
    double d = dh.at(1);
    double a = dh.at(2);
    double alpha = dh.at(3);

    // заполняем матрицу по формуле ДХ параметров
    Matrix4d m;
    m << cos(theta), -sin(theta) * cos(alpha), sin(theta) * sin(alpha), a * cos(theta),
            sin(theta), cos(theta) * cos(alpha), -cos(theta) * sin(alpha), a * sin(theta),
            0, sin(alpha), cos(alpha), d,
            0, 0, 0, 1;
    return m;
}

// рисование робота: для каждого звена, начиная с базы
// высчитывается матрица перехода от базовой системы
// координат к системе координат соответствующего звена
// после чего запускается рисование с применением
// полученной матрицы к координатам всех полигонов модели
void Robot::render() {
    // изначально матрица перехода единичная
    Matrix4d transformMatrix = Matrix<double, 4, 4>::Identity();
    // рисуем базис для текущей матрицы преобразования
    GLHelper::drawBasis(transformMatrix);
    // перебираем параметры Денавита-Хартенберга
    // по звеньям от базы до энд-эффектора
    for (auto &dhParam: dhParams) {
        // получаем матрицу перехода между предыдущим
        // звеном и текущим по параметрам ДХ
        Matrix4d DH = getDHMatrix(dhParam);
        // домножаем матрицу перехода от СК базы
        // к СК предыдущего звена на матрицу перехода
        // из СК предыдущего звена в СК текущего
        transformMatrix = transformMatrix * DH;
        // рисуем базис для полученной матрицы преобразования
        GLHelper::drawBasis(transformMatrix);
        // задаём цвет рисования звена
        glColor3d(0.55, 0.57, 0.59);
        // рисуем очередное звено
        joint->render(transformMatrix);
    }
    // сохраняем положение рабочего инструмента
    endEffectorPos = Vector3d(
            transformMatrix(0, 3),
            transformMatrix(1, 3),
            transformMatrix(2, 3)
    );
}
