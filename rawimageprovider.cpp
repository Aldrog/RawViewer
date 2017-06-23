#include "rawimageprovider.h"

#include <QRegularExpression>
#include <QDir>
#include <QPainter>
#include <QDebug>

RawImageProvider::RawImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage RawImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return img;
}

void RawImageProvider::loadImage(const QUrl &url, bool savePgm)
{
    QFile inFile(url.path());
    if (!inFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Error opening file";
        return;
    }
    QDataStream inStream(&inFile);
    uint16_t *image_buffer = (uint16_t *)malloc(image_width * image_height * 2);
    img = QImage(image_width, image_height, QImage::Format_Grayscale8);
    for (int i = 0; i < image_height; ++i) {
        char *imgLine = (char *)img.scanLine(i);
        for (int j = 0; j < image_width; j += 2) {
            char tmpBuf[3];
            inStream.readRawData(tmpBuf, 3);
            image_buffer[i * image_width + j] = ((0x0FF0 & uint16_t(tmpBuf[0]) << 4) |
                                                 (0x000F & (uint16_t(uint8_t(tmpBuf[1] << 4)) >> 4)));
            image_buffer[i * image_width + j + 1] = ((0x0FF0 & uint16_t(tmpBuf[2] << 4)) |
                                                     (0x000F & uint16_t(tmpBuf[1] >> 4)));
            imgLine[j] = image_buffer[i * image_width + j] >> 4;
            imgLine[j + 1] = image_buffer[i * image_width + j + 1] >> 4;
        }
    }
    inFile.close();
    /*
     * Write PGM
     */
    if(savePgm) {
        int fileNameLength = url.path().lastIndexOf(".");
        if(fileNameLength <= 0)
            fileNameLength = url.path().length();
        QString outPath = url.path().left(fileNameLength + 1) + "pgm";
        QFile outFile(outPath);
        if (!outFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Error opening output file" << outPath << "for reason" << outFile.errorString();
            free(image_buffer);
            return;
        }
        QDataStream outStream(&outFile);
        outStream.writeRawData("P5\n", 3);
        outStream.writeRawData(QString::number(image_width).toStdString().c_str(), QString::number(image_width).length());
        outStream.writeRawData(" ", 1);
        outStream.writeRawData(QString::number(image_height).toStdString().c_str(), QString::number(image_height).length());
        outStream.writeRawData("\n", 1);
        outStream.writeRawData("4095\n", 5);
        //outStream.write((char*)image_buffer, image_width * image_height * 2);
        /*
         * First byte is most significant
         * which conflicts with little-endianness so write byte-by-byte
         */
        uint16_t *iter = image_buffer;
        for (int i = 0; i < image_height; ++i) {
            for (int j = 0; j < image_width; ++j) {
                char c = *iter >> 8;
                outStream.writeRawData(&c, 1);
                c = *iter >> 0;
                outStream.writeRawData(&c, 1);
                iter++;
            }
        }
        outFile.close();
    }
    free(image_buffer);
}

void RawImageProvider::loadStation(const QUrl &url, bool savePgm)
{
    QRegularExpression idRegex(QStringLiteral("c\\d_im(\\d+)_"));
    const auto &idMatch = idRegex.match(url.fileName());
    if(!idMatch.hasMatch())
        return;
    int id = idMatch.captured(1).toInt();
    QString loc = idMatch.captured(1);
    QDir imagesLoc = QFileInfo(url.path()).absoluteDir();
    imagesLoc.cdUp();
    imagesLoc.cdUp();
    const QStringList &camDirList = imagesLoc.entryList({ QStringLiteral("c?") }, QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
    QImage overview(image_width * 3, image_height * (camDirList.size() / 3), QImage::Format_Grayscale8);
    QPainter painter(&overview);
    int x = overview.width() - image_width;
    int y = overview.height() - image_height;
    for (QString camDirName : camDirList)
    {
        QDir currentDir = imagesLoc;
        currentDir.cd(camDirName);
        currentDir.cd(QString::number(id / 100 * 100));
        QFileInfo fileInfo = currentDir.entryInfoList({ QStringLiteral("c?_im%1_*.raw").arg(loc) })[0];
        loadImage(QUrl(fileInfo.filePath()), savePgm);
        painter.drawImage(x, y, img);
        x -= image_width;
        if(x < 0) {
            x = overview.width() - image_width;
            y -= image_height;
        }
    }
    img = overview;
}
