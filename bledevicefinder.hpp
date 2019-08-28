#ifndef BLEDEVICEFINDER_H
#define BLEDEVICEFINDER_H

#include <QObject>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLoggingCategory>

class BLEDeviceFinder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariant devices READ devices NOTIFY devicesChanged);
    Q_PROPERTY(State state READ state NOTIFY stateChanged);
public:
    explicit BLEDeviceFinder(QObject *parent = nullptr);
    ~BLEDeviceFinder();

    enum State {
        IDLE,
        Scanning,
        ScanSucceeded,
        Error
    };
    Q_ENUM(State)

    State state() const;
    Q_INVOKABLE QString errorString() const;

    QVariant devices() const;

    Q_INVOKABLE QObject* findByName(const QString &name) const;

public slots:
    void startScan();
    void stopScan();

signals:
    void devicesChanged();
    void stateChanged();

private slots:
    void deviceFound(const QBluetoothDeviceInfo &info);
    void scanError(QBluetoothDeviceDiscoveryAgent::Error error);
    void scanFinished();

private:
    State m_state;
    QString m_errorString;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QList<QObject*> m_devices;
};

#endif // BLEDEVICEFINDER_H
