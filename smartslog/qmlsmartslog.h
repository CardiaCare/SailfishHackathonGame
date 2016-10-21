#ifndef QMLSMARTSLOG_H
#define QMLSMARTSLOG_H

#include <QQuickItem>

class SmartSpace : public QQuickItem
{
    Q_OBJECT
    Q_DISABLE_COPY(SmartSpace)
    Q_PROPERTY(QString address READ address WRITE setAddress)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(qint32 port READ port WRITE setPort)

public:
    SmartSpace(QQuickItem *parent = 0);

    QString name() const;
    void setName(const QString &name);

    QString address() const;
    void setAddress(const QString &address);

    qint32 port() const;
    void setPort(const qint32 &port);

    void connect();

    ~SmartSpace();
};

#endif // QMLSMARTSLOG_H

