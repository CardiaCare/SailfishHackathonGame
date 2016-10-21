#include "smartslog_plugin.h"
#include "qmlsmartslog.h"

#include <qqml.h>

void SmartslogPlugin::registerTypes(const char *uri)
{
    // @uri SmartSlog
    qmlRegisterType<SmartSpace>(uri, 1, 0, "SmartSpace");
}


