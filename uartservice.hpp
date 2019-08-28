#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QObject>
#include <QLoggingCategory>
#include <QLowEnergyService>

class BLEDevice;

class UARTService : public QObject
{
    Q_OBJECT
public:
    explicit UARTService(QObject *parent = nullptr);
    UARTService(BLEDevice *device, QObject *parent = nullptr);

    Q_INVOKABLE void setDevice(QObject *device);

signals:
    void rx(const QString &s);
    void rxbin(const QByteArray &bytes);

public slots:
    void tx(const QString &s);
    void txbin(const QByteArray &bytes);

private slots:
    void onDeviceStateChanged();
    void serviceStateChanged(const QLowEnergyService::ServiceState state);
    void serviceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void serviceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
private:
    BLEDevice *m_device;
    QLowEnergyService *m_uartService;
    QLowEnergyCharacteristic m_txCharacteristic; // incoming data from device
    QLowEnergyCharacteristic m_rxCharacteristic;
};

Q_DECLARE_LOGGING_CATEGORY(uartService)

#endif // UARTSERVICE_H
