#ifndef UARTSERVICE_H
#define UARTSERVICE_H

#include <QObject>
#include <QLoggingCategory>
#include <QLowEnergyService>
#include "bleserviceinfo.hpp"

class BLEDevice;

class UARTService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    UARTService(BLEServiceInfo uartService, QObject *parent = nullptr);

    /// UART service state
    enum State {
        DeviceWait, ///< Waiting for a valid BLE device object
        Init,        ///< Initializing device
        Ready        ///< Working normally
    };
    Q_ENUM(State)
    State state() const;

    Q_INVOKABLE void setDevice(QObject *device);

signals:
    void stateChanged();
    void rx(const QString &s);
    void rxbin(const QByteArray &bytes);

public slots:
    void tx(const QString &str);
    void txbin(const QByteArray &bytes);

private slots:
    void onDeviceStateChanged();
    void serviceStateChanged(const QLowEnergyService::ServiceState state);
    void serviceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
    void serviceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value);
private:
    State m_state;
    BLEServiceInfo m_uartServiceInfo;
    BLEDevice *m_device;
    QLowEnergyService *m_uartService;
    QLowEnergyCharacteristic m_txCharacteristic; // incoming data from device
    QLowEnergyCharacteristic m_rxCharacteristic;
};

Q_DECLARE_LOGGING_CATEGORY(uartService)

#endif // UARTSERVICE_H
