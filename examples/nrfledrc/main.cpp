#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "bleserviceinfo.hpp"
#include "bledevicefinder.hpp"
#include "bledevice.hpp"
#include "uartservice.hpp"
#include "ledservice.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    //QIcon::setThemeName("material");

    // Following objects also be created and managed from QML, but apparently delays in UI thread
    // can cause remote device to disconnect, so moved them here for now
    BLEServiceInfo::loadDefinitions(":/bleservices.json");
    BLEDeviceFinder bleDeviceFinder;
    UARTService uartService(BLEServiceInfo::byname("Nordic UART"));
    LEDService ledService;
    ledService.setUartService(&uartService);
    QObject::connect(&bleDeviceFinder, &BLEDeviceFinder::devicesChanged, [&](){
        BLEDevice *dev = qobject_cast<BLEDevice *>(bleDeviceFinder.findByName("wooden"));
        if (dev) {
            qDebug() << "Target device found, connecting";
            bleDeviceFinder.stopScan();
            dev->establishConnection();
            uartService.setDevice(dev);
        }
    });


    qmlRegisterUncreatableType<BLEDeviceFinder>("tech.vhrd.ble", 1, 0, "BLEDeviceFinder",
                                                "Only for enums.");
    qmlRegisterUncreatableType<UARTService>("tech.vhrd.ble", 1, 0, "UARTService",
                                                "Only for enums.");

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("bleDeviceFinder", &bleDeviceFinder);
    engine.rootContext()->setContextProperty("uartService", &uartService);
    engine.rootContext()->setContextProperty("ledService", &ledService);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
