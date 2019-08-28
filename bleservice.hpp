#ifndef BLESERVICE_H
#define BLESERVICE_H

#include <QObject>
#include <QLoggingCategory>

class BLEService
{
    Q_GADGET
    Q_PROPERTY(QString name READ name);
    Q_PROPERTY(QString uuid READ uuid);
    Q_PROPERTY(QString icon READ icon);
public:
    BLEService();
    BLEService(const QString &uuid);

    QString name() const;
    QString uuid() const;
    QString icon() const;

    bool operator ==(const BLEService &other);
private:
    QString m_uuid;
};

Q_DECLARE_LOGGING_CATEGORY(bleService)

#endif // BLESERVICE_H
