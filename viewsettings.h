#ifndef VIEWSETTINGS_H
#define VIEWSETTINGS_H

#include <QObject>

class ViewSettings : public QObject
{
    Q_OBJECT
public:
    enum ViewMode {
        Single, Station
    } viewMode;
    Q_ENUM(ViewMode)

    explicit ViewSettings(QObject *parent = nullptr);

    Q_PROPERTY(ViewMode viewMode MEMBER viewMode CONSTANT)

signals:

public slots:
private:
};

#endif // VIEWSETTINGS_H
