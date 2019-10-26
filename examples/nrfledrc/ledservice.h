#ifndef LEDSERVICE_H
#define LEDSERVICE_H

#include <QObject>
#include <QColor>

class UARTService;

class LEDService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)
    Q_PROPERTY(quint8 brightness READ brightness WRITE setBrightness)
public:
    explicit LEDService(QObject *parent = nullptr);
    Q_INVOKABLE void setUartService(QObject *service);

    quint8 brightness() const;
    void setBrightness(const quint8 &brightness);

    bool enabled() const;
    void setEnabled(bool enabled);

signals:

public slots:
    void setColor(const QColor &color);

private slots:
    void onRx(const QByteArray &data);
    void onUartStateChanged();

private:
    UARTService *m_uart;
    quint8 m_brightness;
    QColor m_lastColor;
    bool m_enabled;
};

#endif // LEDSERVICE_H
