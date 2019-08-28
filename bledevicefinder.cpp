#include "bledevicefinder.hpp"
#include <QDebug>
#include <QBluetoothUuid>
#include "bledevice.h"
#include <QQmlEngine>

BLEDeviceFinder::BLEDeviceFinder(QObject *parent) : QObject(parent)
{
    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    m_deviceDiscoveryAgent->setLowEnergyDiscoveryTimeout(5000);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this,                   &BLEDeviceFinder::deviceFound);
    connect(m_deviceDiscoveryAgent, QOverload<QBluetoothDeviceDiscoveryAgent::Error>::of(
                  &QBluetoothDeviceDiscoveryAgent::error),
            this, &BLEDeviceFinder::scanError);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            this,                   &BLEDeviceFinder::scanFinished);
    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
            this,                   &BLEDeviceFinder::scanFinished);

    m_state = IDLE;
}

BLEDeviceFinder::~BLEDeviceFinder()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

BLEDeviceFinder::State BLEDeviceFinder::state() const
{
    return m_state;
}

QString BLEDeviceFinder::errorString() const
{
    return m_errorString;
}

QVariant BLEDeviceFinder::devices() const
{
    return QVariant::fromValue(m_devices);
}

QObject *BLEDeviceFinder::findByName(const QString &name) const
{
    foreach(QObject *o, m_devices) {
        BLEDevice *d = qobject_cast<BLEDevice *>(o);
        QString devName = d->name().toLower();
        if (devName.contains(name))
            return o;
    }
    return nullptr;
}

void BLEDeviceFinder::startScan()
{
    qDeleteAll(m_devices);
    m_devices.clear();

    qDebug() << "Scan starting";
    m_deviceDiscoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
    m_state = Scanning;
    emit stateChanged();
}

void BLEDeviceFinder::stopScan()
{
    m_deviceDiscoveryAgent->stop();
    if (m_deviceDiscoveryAgent->error() == QBluetoothDeviceDiscoveryAgent::NoError) {
        m_state = ScanSucceeded;
    } else {
        m_state = Error;
    }
    emit stateChanged();
}

void BLEDeviceFinder::deviceFound(const QBluetoothDeviceInfo &info)
{
    BLEDevice *device = new BLEDevice(info);
    m_devices.append(device);
    emit devicesChanged();
}

void BLEDeviceFinder::scanError(QBluetoothDeviceDiscoveryAgent::Error error)
{
    if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
        m_errorString = tr("The Bluetooth adaptor is powered off.");
    else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
        m_errorString = tr("Writing or reading from the device resulted in an error.");
    else
        m_errorString = tr("An unknown error has occurred.");
qDebug() << m_errorString;
    m_state = Error;
    emit stateChanged();
}

void BLEDeviceFinder::scanFinished()
{
    m_state = ScanSucceeded;
    emit stateChanged();
    qDebug() << "Scan finished";
}
