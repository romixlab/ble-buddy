#include "uartservice.hpp"
#include "bledevice.h"
#include <QLowEnergyController>

Q_LOGGING_CATEGORY(uartService, "ble.service.uart")

UARTService::UARTService(QObject *parent) : QObject(parent),
    m_device(nullptr)
{
}

UARTService::UARTService(BLEDevice *device, QObject *parent) : QObject(parent),
    m_device(device)
{
    m_uartService = nullptr;
    setDevice(device);
}

void UARTService::setDevice(QObject *device)
{
    m_device = qobject_cast<BLEDevice *>(device);
    if (m_device == nullptr)
        return;
    connect(m_device, &BLEDevice::stateChanged,
            this,     &UARTService::onDeviceStateChanged);
}

void UARTService::tx(const QString &s)
{
    qCInfo(uartService) << "tx" << s;
    m_uartService->writeCharacteristic(m_rxCharacteristic, s.toUtf8());
}

void UARTService::txbin(const QByteArray &bytes)
{
    m_uartService->writeCharacteristic(m_rxCharacteristic, bytes);
}

void UARTService::onDeviceStateChanged()
{
    if (m_device->state() == BLEDevice::Connected) {
        foreach (const BLEService &service, m_device->services()) {
            if (service.name().contains("uart", Qt::CaseInsensitive)) {
                qCInfo(uartService) << "found uart service, searching tx/rx characteristics...";
                m_uartService = m_device->controller()->createServiceObject(QBluetoothUuid(service.uuid()), this);
                if (!m_uartService) {
                    qCWarning(uartService) << "Failed to create service, even though it is found";
                    return;
                }

                connect(m_uartService, &QLowEnergyService::stateChanged,
                        this,          &UARTService::serviceStateChanged);
                connect(m_uartService, &QLowEnergyService::characteristicChanged,
                        this,          &UARTService::serviceCharacteristicChanged);
                connect(m_uartService, &QLowEnergyService::characteristicWritten,
                        this,          &UARTService::serviceCharacteristicWritten);
                m_uartService->discoverDetails();

                qCInfo(uartService) << "Service created, discovering details";
            }
        }
    } else if (m_device->state() == BLEDevice::Disconnected) {
        if (m_uartService) {
            delete m_uartService;
            m_uartService = nullptr;
        }
    }
}

void UARTService::serviceStateChanged(const QLowEnergyService::ServiceState state)
{
    switch (state) {
    case QLowEnergyService::ServiceDiscovered:
    {
        qCInfo(uartService) << "Characteristics discovered";
        m_rxCharacteristic = m_uartService->characteristic(
                    QBluetoothUuid(QStringLiteral("{6e400002-b5a3-f393-e0a9-e50e24dcca9e}")));
        m_txCharacteristic = m_uartService->characteristic(
                    QBluetoothUuid(QStringLiteral("{6e400003-b5a3-f393-e0a9-e50e24dcca9e}")));
        if (!m_rxCharacteristic.isValid()) {
            qCWarning(uartService) << "rx characteristic is invalid";
            return;
        }
        if (!m_txCharacteristic.isValid()) {
            qCWarning(uartService) << "tx characteristic is invalid";
            return;
        }
        QLowEnergyDescriptor txDescriptor = m_txCharacteristic.descriptor(
                    QBluetoothUuid::ClientCharacteristicConfiguration);
        if (txDescriptor.isValid()) {
            qCInfo(uartService) << "Enabling notifications.";
            m_uartService->writeDescriptor(txDescriptor, QByteArray::fromHex("0100"));
        } else {
            qCWarning(uartService) << "Invalid tx descriptor";
        }

        break;
    }
    default:
        //nothing for now
        break;
    }
}

void UARTService::serviceCharacteristicChanged(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCInfo(uartService) << "Characteristic changed" << characteristic.uuid().toString() << value;
    if (characteristic == m_txCharacteristic) {
        emit rx(QString::fromUtf8(value));
        emit rxbin(value);
    }
}

void UARTService::serviceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCInfo(uartService) << "Characteristic written" << characteristic.uuid().toString() << value;
}
