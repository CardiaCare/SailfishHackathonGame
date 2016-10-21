#ifndef SMARTSLOG_PLUGIN_H
#define SMARTSLOG_PLUGIN_H

#include <QQmlExtensionPlugin>

class SmartslogPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    void registerTypes(const char *uri);
};

#endif // SMARTSLOG_PLUGIN_H

