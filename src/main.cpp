
#include <QtGui/QGuiApplication>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlContext>

#include "logger/logger.h"
#include "config.h"
#include "mqtt.h"
#include "messenger.h"


int main(int argc, char *argv[])
{
    Logger::Instance()->setServiceName(QString::fromUtf8("gui"));

    LogWriterSyslog* psyslog(new LogWriterSyslog());
    psyslog->setLogLevel(LogEntry::Info);
    Logger::Instance()->addWriter(psyslog);

    Logger::Instance()->suspend();

    Logger::Instance()->write(0,LogEntry::Info,__PRETTY_FUNCTION__,"Starting.");

    QGuiApplication app(argc, argv);

    Config::createInstance(&app);

    if (!Config::instance()->getLogFilename().isEmpty())
    {
        LogWriterFile* pfilewriter(new LogWriterFile());
        pfilewriter->setLogLevel(LogEntry::Trace);
        pfilewriter->setFileName(Config::instance()->getLogFilename());
        Logger::Instance()->addWriter(pfilewriter);
    }

    Logger::Instance()->resume();

    Mqtt::createInstance(&app);
    Mqtt::instance()->initialize(QString::fromUtf8("gui"));
    Mqtt::instance()->setDebug(Config::instance()->getMqttDebug());

    QQmlApplicationEngine engine(&app);

    Messenger messenger(engine.rootContext(),&app);

    engine.load(QUrl("qrc:/qml/main.qml"));
    if (engine.rootObjects().isEmpty())
    {
        Logger::Instance()->write(0,LogEntry::Error,__PRETTY_FUNCTION__,"Missing QML root object.");
        return 1;
    }

    Mqtt::instance()->connect();

    Logger::Instance()->write(0,LogEntry::Info,__PRETTY_FUNCTION__,"Started.");

    int ret = app.exec();

    Mqtt::instance()->disconnect();

    Logger::Instance()->write(0,LogEntry::Info,__PRETTY_FUNCTION__,"Exiting.");

    Logger::RemoveInstance();

    return ret;
}
