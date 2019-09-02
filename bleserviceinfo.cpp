#include "bleserviceinfo.hpp"
#include <QHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Q_LOGGING_CATEGORY(bleServiceInfo, "ble.serviceinfo")

class BLEServiceInfoConfig {
public:
    BLEServiceInfoConfig() {};

    void loadDefinitions(const QString &filename);

    struct ble_config_entry {
        QString name;
        QString icon;
        QHash<QBluetoothUuid, QString> characteristics;
    };
    QHash<QBluetoothUuid, ble_config_entry> entries;
};
Q_GLOBAL_STATIC(BLEServiceInfoConfig, bleServiceConfigGlobal)

void BLEServiceInfoConfig::loadDefinitions(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCInfo(bleServiceInfo) << filename << "not found";
        return;
    }
    QString j = f.readAll();
    QJsonDocument d = QJsonDocument::fromJson(j.toUtf8());
    if (d.isNull()) {
        qCWarning(bleServiceInfo) << "bleservices.json parse error";
        return;
    }
    QVariantList services = d.object().toVariantHash()["services"].toList();
    foreach(const QVariant &service, services) {
        QVariantMap serviceMap = service.toMap();
        QString name = serviceMap["name"].toString();
        QBluetoothUuid uuid = QBluetoothUuid(serviceMap["uuid"].toString());
        if (name.isEmpty() || uuid.isNull()) {
            qCWarning(bleServiceInfo) << "service" << name << "is invalid";
            continue;
        }
        QString icon = serviceMap["icon"].toString();

        QHash<QBluetoothUuid, QString> characteristics;
        QVariantList characteristicsList = serviceMap["characteristics"].toList();
        foreach(const QVariant &c, characteristicsList) {
            QVariantMap cm = c.toMap();
            characteristics.insert(
                        QBluetoothUuid(cm["uuid"].toString()),
                        cm["name"].toString() );
        }
        qCDebug(bleServiceInfo) << name << characteristics;
        entries.insert(uuid, ble_config_entry{name, icon, characteristics});
    }
    qCInfo(bleServiceInfo) << filename << "parsed successfully";
}

BLEServiceInfo::BLEServiceInfo()
{
}

BLEServiceInfo::BLEServiceInfo(const QBluetoothUuid &uuid) : m_uuid(uuid)
{
}

BLEServiceInfo BLEServiceInfo::byname(const QString &servicename)
{
    BLEServiceInfoConfig *config = bleServiceConfigGlobal;
    if (!config) {
        return BLEServiceInfo();
    }
    QHashIterator<QBluetoothUuid, BLEServiceInfoConfig::ble_config_entry> i(config->entries);
    while (i.hasNext()) {
        i.next();
        if (i.value().name == servicename) {
            return BLEServiceInfo(QBluetoothUuid(i.key()));
        }
    }
    return BLEServiceInfo();
}

void BLEServiceInfo::loadDefinitions(const QString &filename)
{
    BLEServiceInfoConfig *config = bleServiceConfigGlobal;
    if (config) {
        config->loadDefinitions(filename);
    }
}

QString BLEServiceInfo::name() const
{
    BLEServiceInfoConfig *config = bleServiceConfigGlobal;
    if (!config) {
        return QString();
    }
    if (!config->entries.contains(m_uuid))
        return "unknown";
    const BLEServiceInfoConfig::ble_config_entry &entry = config->entries.value(m_uuid);
    return entry.name;
}

QBluetoothUuid BLEServiceInfo::uuid() const
{
    return m_uuid;
}

QString BLEServiceInfo::icon() const
{
    BLEServiceInfoConfig *config = bleServiceConfigGlobal;
    if (!config) {
        return QString();
    }
    if (!config->entries.contains(m_uuid))
        return "unknown";
    const BLEServiceInfoConfig::ble_config_entry &entry = config->entries.value(m_uuid);
    return entry.icon;
}

bool BLEServiceInfo::operator ==(const BLEServiceInfo &other)
{
    return other.m_uuid == m_uuid;
}

bool BLEServiceInfo::isValid() const
{
    return !m_uuid.isNull();
}
