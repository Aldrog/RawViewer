#include "rawimageprovider.h"

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <QDebug>

RawImageProvider::RawImageProvider() : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage RawImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    return img;
}

void RawImageProvider::loadImage(const QString &path)
{
    int image_width = 2448;
    int image_height = 2048;
    uint16_t *image_buffer = (uint16_t *)malloc(image_width * image_height * 2);
    img = QImage(image_width, image_height, QImage::Format_Grayscale8);
    std::ifstream inStream(path.toStdString(), std::ios::binary);
    if (!inStream.is_open()) {
        qDebug() << "Error opening file";
        return;
    }
    for (int i = 0; i < image_height; ++i) {
        char *imgLine = (char *)img.scanLine(i);
        for (int j = 0; j < image_width; j += 2) {
            char tmpBuf[3];
            inStream.read(tmpBuf, 3);
            image_buffer[i * image_width + j] = ((uint16_t(tmpBuf[0]) << 4) |
                                                 (uint16_t(uint8_t(tmpBuf[1] << 4)) >> 4));
            image_buffer[i * image_width + j + 1] = (uint16_t(tmpBuf[2] << 4) |
                                                     uint16_t(tmpBuf[1] >> 4));
            imgLine[j] = tmpBuf[0];
            imgLine[j + 1] = tmpBuf[2];
        }
    }
    free(image_buffer);
}
