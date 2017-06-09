#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImage>
#include <QCommandLineParser>
#include <QQuickStyle>
#include <QTranslator>
#include <QLibraryInfo>
#include "rawimageprovider.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QQuickStyle::setStyle(QStringLiteral("Material"));
    QGuiApplication app(argc, argv);

    // Load Translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);
    QTranslator translator;
    if(translator.load(":/translations/" + QLocale::system().name()))
        app.installTranslator(&translator);

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
    RawImageProvider *provider = new RawImageProvider();
    QUrl imageUrl = QUrl::fromUserInput(args[0]);
    provider->loadImage(imageUrl, pgm);
    engine.addImageProvider("raw", provider);
    engine.rootContext()->setContextProperty("fileName", imageUrl.fileName());
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
