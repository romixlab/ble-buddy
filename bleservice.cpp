#include "bleservice.hpp"
#include <QHash>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Q_LOGGING_CATEGORY(bleService, "ble.service")

class BLEServiceConfig {
public:
    BLEServiceConfig();

    struct ble_config_entry {
        QString name;
        QString icon;
    };
    QHash<QString, ble_config_entry> entries;
};
Q_GLOBAL_STATIC(BLEServiceConfig, bleServiceConfigGlobal)

BLEServiceConfig::BLEServiceConfig()
{
    QFile f(":/3rdparty/ble-buddy/bleservices.json");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCInfo(bleService) << "No bleservices.json where found";
        return;
    }
    QString j = f.readAll();
    QJsonDocument d = QJsonDocument::fromJson(j.toUtf8());
    if (d.isNull()) {
        qCWarning(bleService) << "bleservices.json parse error";
        return;
    }
    QVariantList services = d.object().toVariantHash()["services"].toList();
    foreach(const QVariant &service, services) {
        QVariantMap serviceMap = service.toMap();
        QString name = serviceMap["name"].toString();
        QString uuid = serviceMap["uuid"].toString();
        if (name.isEmpty() || uuid.isEmpty()) {
            qCWarning(bleService) << "service" << name << "is invalid";
            continue;
        }
        QString icon = serviceMap["icon"].toString();
        entries.insert(uuid, ble_config_entry{name, icon});
    }
    qCInfo(bleService) << "bleservices.json parsed successfully";
}

BLEService::BLEService()
{
}

BLEService::BLEService(const QString &uuid)
{
    if (uuid.contains('{')) {
        m_uuid = uuid.mid(1, uuid.length() - 2);
    } else {
        m_uuid = uuid;
    }
}

QString BLEService::name() const
{
    BLEServiceConfig *config = bleServiceConfigGlobal;
    if (!config) {
        return QString();
    }
    if (!config->entries.contains(m_uuid))
        return "unknown";
    const BLEServiceConfig::ble_config_entry &entry = config->entries.value(m_uuid);
    return entry.name;
}

QString BLEService::uuid() const
{
    return m_uuid;
}

QString BLEService::icon() const
{
    BLEServiceConfig *config = bleServiceConfigGlobal;
    if (!config) {
        return QString();
    }
    if (!config->entries.contains(m_uuid))
        return "unknown";
    const BLEServiceConfig::ble_config_entry &entry = config->entries.value(m_uuid);
    return entry.icon;
}

bool BLEService::operator ==(const BLEService &other)
{
    return other.m_uuid == m_uuid;
}
