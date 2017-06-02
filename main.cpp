#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImage>
#include "rawimageprovider.h"

int main(int argc, char *argv[])
{
    if(argc < 2)
        return -1;
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    RawImageProvider provider;
    provider.loadImage(argv[1]);
    engine.addImageProvider("raw", &provider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
