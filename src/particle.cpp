#include "particle.h"

    Particle::Particle(QObject *parent) //:
        //QObject(parent)//, QGraphicsItem()
    {
        angle = 0;      // Задаём угол поворота графического объекта
        steps = 1;      // Задаём стартовое положение ножек мухи
        countForSteps = 0;      // Счётчик для отсчета тиков таймера, при которых мы нажимали на кнопки
       // setRotation(angle);     // Устанавливаем угол поворота графического объекта
    }

    Particle::~Particle()
    {

    }

    QRectF Particle::boundingRect() const
    {
        return QRectF(-40,-50,80,100);   /// Ограничиваем область, в которой лежит треугольник
    }

    void Particle::paint(QPainter *painter, QWidget *widget)
    {
        // Рисуем ножки, без ножек же муха не сможет ползать
        painter->setPen(QPen(Qt::black, 2));
        if(steps == 0){     // Первое положение ножек
            // Left 1
            painter->drawLine(-24,-37,-22,-25);
            painter->drawLine(-22,-25,-17,-15);
            painter->drawLine(-17,-15,-10,-5);
            // Right 1
            painter->drawLine(37,-28,28,-18);
            painter->drawLine(28,-18,24,-8);
            painter->drawLine(24,-8,10,-5);

            // Left 2
            painter->drawLine(-35,-20,-25,-11);
            painter->drawLine(-25,-11,-14,-5);
            painter->drawLine(-14,-5,0,5);
            // Right 2
            painter->drawLine(37,-12,32,-4);
            painter->drawLine(32,-4,24,2);
            painter->drawLine(24,2,0,5);

            // Left 3
            painter->drawLine(-35,35,-26,24);
            painter->drawLine(-26,24,-16,16);
            painter->drawLine(-16,16,0,0);
            // Right 3
            painter->drawLine(37,26,32,17);
            painter->drawLine(32,17,24,8);
            painter->drawLine(24,8,0,0);
        } else if (steps == 1){     // Второе положение ножек
            // Left 1
            painter->drawLine(-32,-32,-25,-22);
            painter->drawLine(-25,-22,-20,-12);
            painter->drawLine(-20,-12,-10,-5);
            // Right 1
            painter->drawLine(32,-32,25,-22);
            painter->drawLine(25,-22,20,-12);
            painter->drawLine(20,-12,10,-5);

            // Left 2
            painter->drawLine(-39,-15,-30,-8);
            painter->drawLine(-30,-8,-18,-2);
            painter->drawLine(-18,-2,0,5);
            // Right 2
            painter->drawLine(39,-15,30,-8);
            painter->drawLine(30,-8,18,-2);
            painter->drawLine(18,-2,0,5);

            // Left 3
            painter->drawLine(-39,30,-30,20);
            painter->drawLine(-30,20,-20,12);
            painter->drawLine(-20,12,0,0);
            // Right 3
            painter->drawLine(39,30,30,20);
            painter->drawLine(30,20,20,12);
            painter->drawLine(20,12,0,0);
        } else if (steps == 2){     // Третье положение ножек
            // Left 1
            painter->drawLine(-37,-28,-28,-18);
            painter->drawLine(-28,-18,-24,-8);
            painter->drawLine(-24,-8,-10,-5);
            // Right 1
            painter->drawLine(24,-37,22,-25);
            painter->drawLine(22,-25,17,-15);
            painter->drawLine(17,-15,10,-5);

            // Left 2
            painter->drawLine(-37,-12,-32,-4);
            painter->drawLine(-32,-4,-24,2);
            painter->drawLine(-24,2,0,5);
            // Right 2
            painter->drawLine(35,-20,25,-11);
            painter->drawLine(25,-11,14,-5);
            painter->drawLine(14,-5,0,5);

            // Left 3
            painter->drawLine(-37,26,-32,17);
            painter->drawLine(-32,17,-24,8);
            painter->drawLine(-24,8,0,0);
            // Right 3
            painter->drawLine(35,35,26,24);
            painter->drawLine(26,24,16,16);
            painter->drawLine(16,16,0,0);
        }
        // Усики
        QPainterPath path(QPointF(-5,-34));
        path.cubicTo(-5,-34, 0,-36,0,-30);
        path.cubicTo(0,-30, 0,-36,5,-34);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);

        painter->setPen(QPen(Qt::black, 1));
        // Тушка
        painter->setBrush(Qt::black);
        painter->drawEllipse(-15, -20, 30, 50);
        // Голова
        painter->drawEllipse(-15, -30, 30, 20);
        // Глазища
        painter->setBrush(Qt::green);
        painter->drawEllipse(-15, -27, 12, 15);
        painter->drawEllipse(3, -27, 12, 15);

        // Левое крылище
        QPainterPath path2(QPointF(-10, -10));
        path2.cubicTo(-18, -10, -30, 10, -25, 35);
        path2.cubicTo(-25,35,-20,50,-15,40);
        path2.cubicTo(-15,40,0,20,-3,5 );
        path2.cubicTo(-3,5, -8,8,-10,-10);
        painter->setBrush(Qt::white);
        painter->drawPath(path2);

        // Правое крылище
        QPainterPath path3(QPointF(10, -10));
        path3.cubicTo(18, -10, 30, 10, 25, 35);
        path3.cubicTo(25,35,20,50,15,40);
        path3.cubicTo(15,40,0,20,3,5 );
        path3.cubicTo(3,5, 8,8,10,-10);
        painter->setBrush(Qt::white);
        painter->drawPath(path3);

        //Q_UNUSED(option);
        Q_UNUSED(widget);
    }

    void Particle::slotGameTimer()
    {
        /* Проверяем, нажата ли была какая-либо из кнопок управления объектом.
         * Прежде чем считать шажки
         * */
//        if(GetAsyncKeyState(VK_LEFT) ||
//           GetAsyncKeyState(VK_RIGHT) ||
//           GetAsyncKeyState(VK_UP) ||
//           GetAsyncKeyState(VK_DOWN))
//        {
//            /* Поочерёдно выполняем проверку на нажатие клавиш
//             * с помощью функции асинхронного получения состояния клавиш,
//             * которая предоставляется WinAPI
//             * */
//            if(GetAsyncKeyState(VK_LEFT)){
//                angle -= 5;        // Задаём поворот на 5 градусов влево
//                setRotation(angle); // Поворачиваем объект
//            }
//            if(GetAsyncKeyState(VK_RIGHT)){
//                angle += 5;        // Задаём поворот на 5 градусов вправо
//                setRotation(angle); // Поворачиваем объект
//            }
//            if(GetAsyncKeyState(VK_UP)){
//                setPos(mapToParent(0, -2));     /* Продвигаем объект на 5 пискселей вперёд
//                                                 * перетранслировав их в координатную систему
//                                                 * графической сцены
//                                                 * */
//            }
//            if(GetAsyncKeyState(VK_DOWN)){
//                setPos(mapToParent(0, 2));      /* Продвигаем объект на 5 пискселей назад
//                                                 * перетранслировав их в координатную систему
//                                                 * графической сцены
//                                                 * */
//            }

//            // Двигаем ножками, Dance, dance, Baby !!!
//            countForSteps++;
//            if(countForSteps == 4){
//                steps = 2;
//                update(QRectF(-40,-50,80,100));
//            } else if (countForSteps == 8){
//                steps = 1;
//                update(QRectF(-40,-50,80,100));
//            } else if (countForSteps == 12){
//                steps = 0;
//                update(QRectF(-40,-50,80,100));
//            } else if (countForSteps == 16) {
//                steps = 1;
//                update(QRectF(-40,-50,80,100));
//                countForSteps = 0;
//            }
//        }

//        /* Проверка выхода за границы поля
//         * Если объект выходит за заданные границы, то возвращаем его назад
//         * */
//        if(this->x() - 10 < -250){
//            this->setX(-240);       // слева
//        }
//        if(this->x() + 10 > 250){
//            this->setX(240);        // справа
//        }

//        if(this->y() - 10 < -250){
//            this->setY(-240);       // сверху
//        }
//        if(this->y() + 10 > 250){
//            this->setY(240);        // снизу
//        }
    }
