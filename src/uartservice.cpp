#include "uartservice.hpp"
#include "bledevice.hpp"
#include <QLowEnergyController>

Q_LOGGING_CATEGORY(uartService, "ble.service.uart")

/*!
  \class UARTService
  \brief The UARTService class
  @brief The UARTService class
  Imlements Nordic UART service.
  Needs bleservices.json definitions to be loaded (@see BLEService::loadDefinitions)
*/

UARTService::UARTService(BLEServiceInfo uartServiceInfo, QObject *parent) : QObject(parent),
    m_uartServiceInfo(uartServiceInfo), m_device(nullptr), m_uartService(nullptr)
{
    m_state = State::DeviceWait;
}

UARTService::State UARTService::state() const
{
    return m_state;
}

void UARTService::setDevice(QObject *device)
{
    m_device = qobject_cast<BLEDevice *>(device);
    if (m_device == nullptr)
        return;
    m_state = State::Init;
    emit stateChanged();
    connect(m_device, &BLEDevice::stateChanged,
            this,     &UARTService::onDeviceStateChanged);
}

void UARTService::tx(const QString &str)
{
    if (m_uartService)
        m_uartService->writeCharacteristic(m_rxCharacteristic, str.toUtf8(), QLowEnergyService::WriteWithoutResponse);
}

void UARTService::txbin(const QByteArray &bytes)
{
    if (m_uartService) {
        m_uartService->writeCharacteristic(m_rxCharacteristic, bytes, QLowEnergyService::WriteWithoutResponse);
    }
}

void UARTService::onDeviceStateChanged()
{
    if (m_device->state() == BLEDevice::Connected) {
        if (m_uartService)
            return;
        foreach (const BLEServiceInfo &service, m_device->services()) {
            if (service.name().contains("uart", Qt::CaseInsensitive)) {
                qCInfo(uartService) << "found uart service, searching tx/rx characteristics...";
                m_uartService = m_device->controller()->createServiceObject(QBluetoothUuid(service.uuid()), this);
                if (!m_uartService) {
                    qCWarning(uartService) << "Failed to create service, even though it is found";
                    m_state = State::DeviceWait;
                    emit stateChanged();
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
    } else if (m_device->state() == BLEDevice::Disconnected ||
               m_device->state() == BLEDevice::Error) {
        if (m_uartService) {
            delete m_uartService;
            m_uartService = nullptr;
            m_state = State::DeviceWait;
            emit stateChanged();
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
                    m_uartServiceInfo.characteristic("rx") );
        m_txCharacteristic = m_uartService->characteristic(
                    m_uartServiceInfo.characteristic("tx") );
        if (!(m_rxCharacteristic.isValid() && m_txCharacteristic.isValid())) {
            qCWarning(uartService) << "rx or tx characteristic is invalid";
            m_device->controller()->disconnectFromDevice();
            return;
        }
        QLowEnergyDescriptor txDescriptor = m_txCharacteristic.descriptor(
                    QBluetoothUuid::ClientCharacteristicConfiguration);
        if (txDescriptor.isValid()) {
            qCInfo(uartService) << "Enabling notifications.";
            m_uartService->writeDescriptor(txDescriptor, QByteArray::fromHex("0100"));
            m_state = State::Ready;
            emit stateChanged();
        } else {
            qCWarning(uartService) << "Invalid tx descriptor";
            m_device->controller()->disconnectFromDevice();
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
    //qCInfo(uartService) << "Characteristic changed" << characteristic.uuid().toString() << value;
    if (characteristic == m_txCharacteristic) {
        emit rx(QString::fromUtf8(value));
        emit rxbin(value);
    }
}

void UARTService::serviceCharacteristicWritten(const QLowEnergyCharacteristic &characteristic, const QByteArray &value)
{
    qCInfo(uartService) << "Characteristic written" << characteristic.uuid().toString() << value;
}
