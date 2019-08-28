#ifndef BLEDEVICE_H
#define BLEDEVICE_H

#include <QObject>
#include <QVariantList>
#include <QBluetoothDeviceInfo>
#include <QLoggingCategory>
#include "bleservice.h"

class QLowEnergyController;
class BLEDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString address READ address)
    //Q_PROPERTY(QVariantList services READ services NOTIFY servicesChanged)
    Q_PROPERTY(bool servicesComplete READ servicesComplete NOTIFY servicesCompleteChanged)
    //icon
    //companyname etc
public:
    //BLEDevice(QObject *parent = nullptr);
    BLEDevice(const QBluetoothDeviceInfo &device, QObject *parent = nullptr);
    ~BLEDevice();

    enum State {
        Discovered,
        Connecting,
        Connected,
        Disconnected,
        Error
    };
    Q_ENUM(State)

    State state() const;

    QString name() const;
    QString address() const;
    QList<BLEService> services() const;
    bool servicesComplete() const;
    QBluetoothDeviceInfo device() const;
    QLowEnergyController *controller() const;

public slots:
    void establishConnection();

private slots:
    void serviceDiscovered(const QBluetoothUuid &service);
    void serviceScanDone();

signals:
    void nameChanged();
    void servicesChanged();
    void servicesCompleteChanged();
    void stateChanged();

private:
    State m_state;
    QBluetoothDeviceInfo m_device;
    QList<BLEService> m_services;
    bool m_servicesComplete;
    QLowEnergyController *m_controller;
};

Q_DECLARE_LOGGING_CATEGORY(bleDevice)

#endif // BLEDEVICE_H
