#ifndef RAWIMAGEPROVIDER_H
#define RAWIMAGEPROVIDER_H

#include <QQuickImageProvider>

class RawImageProvider : public QQuickImageProvider
{
public:
    RawImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    void loadImage(const QUrl &url, bool savePgm);
    void loadStation(const QUrl &url, bool savePgm);
private:
    QImage img;
    const int image_width = 2448;
    const int image_height = 2048;
};

#endif // RAWIMAGEPROVIDER_H
