#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QImage>
#include <QCommandLineParser>
#include <QQuickStyle>
#include <QTranslator>
#include <QLibraryInfo>
#include <QDebug>
#include "rawimageprovider.h"
#include "viewsettings.h"

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
    QCommandLineOption stationModeOption(QStringLiteral("station"), app.translate("main", "Split view of all cameras of a station (assumes LPS directory structure)."));
    parser.addOption(stationModeOption);
    parser.addPositionalArgument(QStringLiteral("FILE"), app.translate("main", "Path to a raw image file."));

    parser.process(app);
    QStringList args = parser.positionalArguments();
    bool pgm = parser.isSet(savePgmOption);
    ViewSettings *settings = new ViewSettings();
    settings->viewMode = parser.isSet(stationModeOption) ? ViewSettings::Station : ViewSettings::Single;
    if(args.length() != 1) {
        parser.showHelp();
        return 0;
    }

    QQmlApplicationEngine engine;
    RawImageProvider *provider = new RawImageProvider();
    QUrl imageUrl = QUrl::fromLocalFile(args[0]);
    qDebug() << imageUrl << parser.positionalArguments();
    if(settings->viewMode == ViewSettings::Station)
        provider->loadStation(imageUrl, pgm);
    else
        provider->loadImage(imageUrl, pgm);
    engine.addImageProvider("raw", provider);
    engine.rootContext()->setContextProperty("fileName", imageUrl.fileName());
    qmlRegisterType<ViewSettings>("cvs.rawviewer", 1, 0, "ViewSettings");
    engine.rootContext()->setContextProperty("settings", settings);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
