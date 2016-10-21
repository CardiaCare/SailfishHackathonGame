#include "qmlsmartslog.h"
//#include <qglobal.h>
extern "C"{
#include "vendor/SmartSlog/smartslog.h"
}
SmartSpace::SmartSpace(QQuickItem *parent):
    QQuickItem(parent)
{
    qDebug() << "It works!";
}

SmartSpace::~SmartSpace()
{
}

void SmartSpace::connect()
{

}

