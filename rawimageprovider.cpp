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

void RawImageProvider::loadImage(const QUrl &url, bool savePgm)
{
    const int image_width = 2448;
    const int image_height = 2048;
    std::ifstream inStream(url.path().toStdString(), std::ios::binary);
    if (!inStream.is_open()) {
        qDebug() << "Error opening file";
        return;
    }
    uint16_t *image_buffer = (uint16_t *)malloc(image_width * image_height * 2);
    img = QImage(image_width, image_height, QImage::Format_Grayscale8);
    for (int i = 0; i < image_height; ++i) {
        char *imgLine = (char *)img.scanLine(i);
        for (int j = 0; j < image_width; j += 2) {
            char tmpBuf[3];
            inStream.read(tmpBuf, 3);
            image_buffer[i * image_width + j] = ((0x0FF0 & uint16_t(tmpBuf[0]) << 4) |
                                                 (0x000F & (uint16_t(uint8_t(tmpBuf[1] << 4)) >> 4)));
            image_buffer[i * image_width + j + 1] = ((0x0FF0 & uint16_t(tmpBuf[2] << 4)) |
                                                     (0x000F & uint16_t(tmpBuf[1] >> 4)));
            imgLine[j] = image_buffer[i * image_width + j] >> 4;
            imgLine[j + 1] = image_buffer[i * image_width + j + 1] >> 4;
        }
    }
    inStream.close();
    /*
     * Write PGM
     */
    if(savePgm) {
        int fileNameLength = url.path().lastIndexOf(".");
        if(fileNameLength <= 0)
            fileNameLength = url.path().length();
        std::string outPath = url.path().left(fileNameLength + 1).toStdString() + "pgm";
        std::ofstream outStream(outPath, std::ios::binary);
        if (!outStream.is_open()) {
            qDebug() << "Error opening output file";
            free(image_buffer);
            return;
        }
        outStream << "P5\n";
        outStream << image_width  << "\n";
        outStream << image_height << "\n";
        outStream << 4095 << "\n";
        //outStream.write((char*)image_buffer, image_width * image_height * 2);
        /*
         * First byte is most significant
         * which conflicts with little-endianness so write byte-by-byte
         */
        uint16_t *iter = image_buffer;
        for (int i = 0; i < image_height; ++i) {
            for (int j = 0; j < image_width; ++j) {
                char c = *iter >> 8;
                outStream << c;
                c = *iter >> 0;
                outStream << c;
                iter++;
            }
        }
        outStream.close();
    }
    free(image_buffer);
}
