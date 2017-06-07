#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImage>
#include <QCommandLineParser>
#include "rawimageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Parse command line options
    QCommandLineParser parser;
    parser.setApplicationDescription(app.translate("main", "Raw G12 image viewer. At the moment only supports 2448x2048 resolution."));
    parser.addHelpOption();
    QCommandLineOption savePgmOption(QStringLiteral("pgm"), app.translate("main", "Save loaded image into PGM."));
    parser.addOption(savePgmOption);
    parser.addPositionalArgument(QStringLiteral("FILE"), app.translate("main", "Path to a raw image file."));

    parser.process(app);
    QStringList args = parser.positionalArguments();
    bool pgm = parser.isSet(savePgmOption);
    if(args.length() != 1) {
        parser.showHelp();
        return 0;
    }

    QQmlApplicationEngine engine;
    RawImageProvider provider;
    provider.loadImage(args[0], pgm);
    engine.addImageProvider("raw", &provider);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
