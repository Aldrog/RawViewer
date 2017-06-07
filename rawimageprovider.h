#ifndef RAWIMAGEPROVIDER_H
#define RAWIMAGEPROVIDER_H

#include <QQuickImageProvider>

class RawImageProvider : public QQuickImageProvider
{
public:
    RawImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
    void loadImage(const QString &path, bool savePgm);
private:
    QImage img;
};

#endif // RAWIMAGEPROVIDER_H
