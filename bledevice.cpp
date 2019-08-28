#include "bledevice.hpp"
#include <QBluetoothUuid>
#include <QLowEnergyController>
#include "bleservice.h"

Q_LOGGING_CATEGORY(bleDevice, "ble.device")

//BLEDevice::BLEDevice(QObject *parent) : QObject(parent)
//{
//}

BLEDevice::BLEDevice(const QBluetoothDeviceInfo &device, QObject *parent) :
    m_device(device), QObject(parent)
{
    QBluetoothDeviceInfo::DataCompleteness completeness;
    QList<QBluetoothUuid> uuids = m_device.serviceUuids(&completeness);

    m_controller = nullptr;

    foreach (const QBluetoothUuid &uuid, uuids) {
        BLEService service(uuid.toString());
        m_services.append(service);
    }
    emit servicesChanged();

    if (completeness == QBluetoothDeviceInfo::DataIncomplete) {
        m_servicesComplete = false;
    } else {
        m_servicesComplete = true;
    }
    emit servicesCompleteChanged();

    qCInfo(bleDevice) << "Device found" << name();
    m_state = Discovered;
}

BLEDevice::~BLEDevice()
{
    if (m_controller) {
        qCInfo(bleDevice) << "Disconnecting from device" << name();
        m_controller->disconnectFromDevice();
        delete m_controller;
    }
}

BLEDevice::State BLEDevice::state() const
{
    return m_state;
}

QString BLEDevice::name() const
{
    return m_device.name();
}

QString BLEDevice::address() const
{
#if defined Q_OS_DARWIN
    // workaround for Core Bluetooth:
    return m_device.deviceUuid().toString();
#else
    return m_device.address().toString();
#endif
}

QList<BLEService> BLEDevice::services() const
{
    return m_services;
}

bool BLEDevice::servicesComplete() const
{
    return m_servicesComplete;
}

QBluetoothDeviceInfo BLEDevice::device() const
{
    return m_device;
}

QLowEnergyController *BLEDevice::controller() const
{
    return m_controller;
}

void BLEDevice::establishConnection()
{
    if (m_controller) {
        m_controller->disconnectFromDevice();
        delete m_controller;
    }

    qCInfo(bleDevice) << "Connecting to" << name();

    m_controller = QLowEnergyController::createCentral(m_device, this);
    //m_controller->setRemoteAddressType(m_addressType);

    connect(m_controller, &QLowEnergyController::serviceDiscovered,
            this, &BLEDevice::serviceDiscovered);
    connect(m_controller, &QLowEnergyController::discoveryFinished,
            this, &BLEDevice::serviceScanDone);

    connect(m_controller, static_cast<void (QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),
            this, [this](QLowEnergyController::Error error) {
        Q_UNUSED(error);
        qCWarning(bleDevice) << "Cannot connect to remote device.";
        m_state = Error;
        emit stateChanged();
    });
    connect(m_controller, &QLowEnergyController::connected, this, [this]() {
        qCInfo(bleDevice) << "Controller connected. Search services...";
        m_controller->discoverServices();
    });
    connect(m_controller, &QLowEnergyController::disconnected, this, [this]() {
        qCInfo(bleDevice) << "LowEnergy controller disconnected";
        m_state = Disconnected;
        emit stateChanged();
    });

    m_state = Connecting;
    emit stateChanged();
    m_controller->connectToDevice();
}

void BLEDevice::serviceDiscovered(const QBluetoothUuid &service)
{
    BLEService bleService(service.toString());
    qCInfo(bleDevice) << "Service discovered" << bleService.uuid() << "(" << bleService.name() << ")";
    if (!m_services.contains(bleService)) {
        m_services.append(bleService);
        emit servicesChanged();
    }
}

void BLEDevice::serviceScanDone()
{
    qCInfo(bleDevice) << "Service scan done";
    if (m_servicesComplete == false) {
        m_servicesComplete = true;
        emit servicesCompleteChanged();
    }

    m_state = Connected;
    emit stateChanged();
}
