#include "ledservice.h"
#include "uartservice.hpp"

LEDService::LEDService(QObject *parent) : QObject(parent)
{
    m_brightness = 10; // 0-255
    m_enabled = false;
}

void LEDService::setUartService(QObject *service)
{
    m_uart = qobject_cast<UARTService *>(service);
    if (m_uart) {
        connect(m_uart, &UARTService::rxbin,
                this,      &LEDService::onRx);
        connect(m_uart, &UARTService::stateChanged,
                this,      &LEDService::onUartStateChanged);
    }
}

void LEDService::setColor(const QColor &color)
{
    if (!m_uart)
        return;
    if (m_uart->state() != UARTService::Ready)
        return;

    QColor dimmed;
    if (color.isValid()) {
        dimmed.setHsv(color.hue(), color.saturation(), m_brightness);
        m_lastColor = color;
    }

    QByteArray bytes(4, 0);
    quint8 *p = (quint8 *)bytes.data();
    p[0] = 'h';
    p[1] = (quint8)dimmed.red();
    p[2] = (quint8)dimmed.green();
    p[3] = (quint8)dimmed.blue();
    qDebug() << "tx:" << bytes.toHex();
    m_uart->txbin(bytes);
}

void LEDService::onUartStateChanged()
{
    //m_state = Idle;
    if (m_uart->state() == UARTService::State::Ready) {
        // do smth, when ble device is ready
    }
}

bool LEDService::enabled() const
{
    return m_enabled;
}

void LEDService::setEnabled(bool enabled)
{
    m_enabled = enabled;
    if (enabled) {
        if (m_lastColor.isValid())
            setColor(m_lastColor);
    } else {
        setColor(QColor());
    }
}

quint8 LEDService::brightness() const
{
    return m_brightness;
}

void LEDService::setBrightness(const quint8 &brightness)
{
    m_brightness = brightness;
    if (m_lastColor.isValid() && m_enabled) {
        setColor(m_lastColor);
    }
}

void LEDService::onRx(const QByteArray &data)
{
    //Receive data from device
}
