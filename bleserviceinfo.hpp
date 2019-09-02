#ifndef BLESERVICEINFO_H
#define BLESERVICEINFO_H

#include <QObject>
#include <QBluetoothUuid>
#include <QLoggingCategory>

class BLEServiceInfo
{
    Q_GADGET
    Q_PROPERTY(QString name READ name);
    //Q_PROPERTY(QString uuid READ uuid);
    Q_PROPERTY(QString icon READ icon);
public:
    BLEServiceInfo();
    BLEServiceInfo(const QBluetoothUuid &uuid);

    static void loadDefinitions(const QString &filename);

    BLEServiceInfo byname(const QString &servicename);
    QBluetoothUuid characteristic(const QString &name) const;

    QString name() const;
    QBluetoothUuid uuid() const;
    QString icon() const;

    bool operator ==(const BLEServiceInfo &other);
    bool isValid() const;
private:
    QBluetoothUuid m_uuid;
};

Q_DECLARE_LOGGING_CATEGORY(bleServiceInfo)

#endif // BLESERVICEINFO_H
