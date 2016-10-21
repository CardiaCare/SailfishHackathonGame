#ifndef PARTICLE_H
#define PARTICLE_H

#include <QObject>

#include <QPainter>


class Particle
{
public:
    explicit Particle(QObject *parent = 0);
    ~Particle();

signals:

public slots:
    void slotGameTimer(); /// Слот, который отвечает за обработку перемещения треугольника

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, QWidget *widget);

private:
    qreal angle;        /// Угол поворота графического объекта
    int steps;          // Номер положения ножек мухи
    int countForSteps;  // Счётчик для отсчета тиков таймера, при которых мы нажимали на кнопки
};

#endif // PARTICLE_H
