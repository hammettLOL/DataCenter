#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <linphone/core.h>
#include "signal.h"
#include <Windows.h>
#include "factory.h"


int main(int argc, char *argv[])
{

    Factory fab;
    fab.basic_Call();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();


}
