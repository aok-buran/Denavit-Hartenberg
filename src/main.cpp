#include <GL/gl.h>
#include <GL/glut.h>
#include <cstdio>
#include "misc/glHelper.h"
#include "robot.h"


// задержка таймера в миллисекундах
static const int TIMER_DELAY = 100;
// ширина окна
static const int CLIENT_WIDTH = 1600;
// высота окна
static const int CLIENT_HEIGHT = 1080;
// путь к файлу параметров Денавита-Хартенберга
static const std::string DH_DEFAULT_PATH = "../data/kr10.json";
// скорость перемещения камеры
static const double CAMERA_VELOCITY = 1.0;
// скорость поворота камеры
static const double CAMERA_ROTATION_VELOCITY = 0.002;
// скорость поворота звена робота
static const double LINK_VELOCITY = 0.05;

// камера
Camera camera = Camera(
        Vector3d(150, 110.0, 30.0), // положение
        Vector3d(0.3, 0.5, 0).normalized(), // направление
        Vector3d(0, 0, 1) // вектор "вверх"
);
// робот, который будет визуализироваться
Robot robot;

// предыдущие положения курсора мыши
static const int VERY_BIG_POS = 10000;
int prevPosX = VERY_BIG_POS;
int prevPosY = VERY_BIG_POS;

// рисование сцены
static void Draw() {
    // очищаем буферы
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // делаем единичной модельную матрицу
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // применяем камеру к состояниям OpenGL (gluLookAt)
    camera.applyLookAt();
    // рисуем базис
    GLHelper::drawBasis(100);
    // сохраняем матрицу преобразования
    glPushMatrix();
    // смещаем сцену на [30,30,0]
    glTranslated(30, 30, 0);
    // рисуем робота
    robot.render();
    // восстанавливаем матрицу преобразования
    glPopMatrix();

    // выводим в заголовок информацию о роботе (положение и углы поворота)
    Vector3d pos = robot.getEndEffectorPos();
    // получаем параметры DH, они представляют из себя вектор векторов
    // каждый из вложенных векторов имеет длину 5, в первых
    // четырёх элементах хранятся параметры ДХ, в пятом - текущий угол поворота звена
    auto dh = robot.getDHParams();
    char title[256];
    sprintf(
            title,
            "p=[%10.3f, %10.3f, %10.3f ],       q=[%6.2f, %6.2f, %6.2f %6.2f, %6.2f, %6.2f ]",
            pos(0) * 10, pos(1) * 10, pos(2) * 10,
            dh[0][4] / M_PI * 180, dh[1][4] / M_PI * 180, dh[2][4] / M_PI * 180,
            dh[3][4] / M_PI * 180, dh[4][4] / M_PI * 180, dh[5][4] / M_PI * 180
    );
    glutSetWindowTitle(title);

    // меняем местами буфферы экрана
    glutSwapBuffers();
}

// обработчик перемещения мыши
void Motion(int x, int y) {
    // если не было предыдущих обработок
    if (prevPosX == VERY_BIG_POS) {
        // сохраняем текущее положение в качестве предыдущего
        prevPosX = x;
        prevPosY = y;
        // завершаем обработку
        return;
    }

    // получаем перемещение мыши относительно предыдущей обработки
    double dX = x - prevPosX;
    double dY = y - prevPosY;
    // сохраняем текущее положение в качестве предыдущего
    prevPosX = x;
    prevPosY = y;
    // поворачиваем камеру
    camera.rotateX(-dX * CAMERA_ROTATION_VELOCITY);
    camera.rotateY(-dY * CAMERA_ROTATION_VELOCITY);
    // перерисовываем сцену
    glutPostRedisplay();
}

// инициализация OpenGL
static void Init() {
    // задаём цвет фона
    glClearColor(0.17, 0.18, 0.19, 1.0);
    // разрешаем цвет материалов
    glEnable(GL_COLOR_MATERIAL);

    // задаём параметры смешивания
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_SMOOTH);

    // разрешаем проверку глубины
    glEnable(GL_DEPTH_TEST);

    // делаем матрицу проекции единичной
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // задаём параметры перспективы
    gluPerspective(60.0, 1.0, 0.1, 1000.0);

    // помещаем указатель мыши в центр экрана
    glutWarpPointer(CLIENT_WIDTH / 2, CLIENT_HEIGHT / 2);

}

// обработчик клавиатуры: key - символ, введённый с клавиатуры (не код)
static void Key(unsigned char key, int x, int y) {
    switch (key) {
        case 27: // Esc
            exit(0);
        case 97 : // A
            camera.moveLeft(CAMERA_VELOCITY);
            break;
        case 100:// D
            camera.moveRight(CAMERA_VELOCITY);
            break;
        case 119: // W
            camera.moveForward(CAMERA_VELOCITY);
            break;
        case 115: // S
            camera.moveBack(CAMERA_VELOCITY);
            break;
        case 113: // Q
            camera.moveUp(CAMERA_VELOCITY);
            break;
        case 101: // E
            camera.moveDown(CAMERA_VELOCITY);
            break;
        case 49: // 1
            robot.rotateLink(0, LINK_VELOCITY);
            break;
        case 50: // 2
            robot.rotateLink(0, -LINK_VELOCITY);
            break;
        case 51: // 3
            robot.rotateLink(1, LINK_VELOCITY);
            break;
        case 52: // 4
            robot.rotateLink(1, -LINK_VELOCITY);
            break;
        case 53: // 5
            robot.rotateLink(2, LINK_VELOCITY);
            break;
        case 54: // 6
            robot.rotateLink(2, -LINK_VELOCITY);
            break;
        case 55: // 7
            robot.rotateLink(3, LINK_VELOCITY);
            break;
        case 56: // 8
            robot.rotateLink(3, -LINK_VELOCITY);
            break;
        case 57: // 9
            robot.rotateLink(4, LINK_VELOCITY);
            break;
        case 48: // 0
            robot.rotateLink(4, -LINK_VELOCITY);
            break;
        case 45: // -
            robot.rotateLink(5, LINK_VELOCITY);
            break;
        case 61: // =
            robot.rotateLink(5, -LINK_VELOCITY);
            break;
    }
    // перерисовываем окно
    glutPostRedisplay();
}

// таймер
void Timer(int value) {
    // вызываем таймер заново
    glutTimerFunc(100, Timer, value);
    // перерисовываем окно
    glutPostRedisplay();
}


// изменение размеров окна width и height - новые ширина и выота соответственно
void Reshape(int width, int height) {
    // задаём новые размеры для viewPort
    glViewport(0, 0, width, height);
    // делаем матрицу проекций единичной
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // задаём параметры перспективы
    gluPerspective(52.0f, (GLdouble) width / (GLdouble) height, 1.0f, 1000.0f);
    // делаем модельную матрицу единичной
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// главный метод
int main(int argc, char **argv) {
    // инициализируем движок OpenGL
    GLenum type;
    glutInit(&argc, argv);
    type = GLUT_RGB | GLUT_DOUBLE;
    glutInitDisplayMode(type);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(CLIENT_WIDTH, CLIENT_HEIGHT);
    glutCreateWindow("ABGR extension");
    if (!glutExtensionSupported("GL_EXT_abgr")) {
        printf("Couldn't find abgr extension.\n");
        exit(0);
    }
#if !GL_EXT_abgr
    printf("WARNING: client-side OpenGL has no ABGR extension support!\n");
    printf("         Drawing only RGBA (and not ABGR) images and textures.\n");
#endif
    // инициализируем состояния OpenGL
    Init();
    // инициализируем робота
    robot.init(argc > 1 ? argv[1] : DH_DEFAULT_PATH);
    // связываем с OpenGL соответствующие функции
    glutKeyboardFunc(Key);
    glutDisplayFunc(Draw);
    glutReshapeFunc(Reshape);
    glutPassiveMotionFunc(Motion);
    // запускаем таймер
    glutTimerFunc(TIMER_DELAY, Timer, 0);
    // запускаем главный цикл приложения
    glutMainLoop();
    return 0;
}




