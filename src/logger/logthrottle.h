/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGTHROTTLE_H
#define LOGTHROTTLE_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QString>

#include "abslogwriter.h"


const QString CLogThrottle("throttle");

const int CRepeatingLogNotificationThresholdMilliseconds(1000);


class LogThrottle : public QObject, public AbsLogWriter
{
	Q_OBJECT

public:
    LogThrottle (QString alternateName = QString());
    ~LogThrottle ();

    void setTime(int milliseconds);

    void write(LogEntry* pEntry);

    void setLogLevel(LogEntry::LogLevel level) {/*Level cannot be changed in a throttle*/}

    bool setWriter(AbsLogWriter* pWriter);
    AbsLogWriter* getWriterByName(QString name);
    AbsLogWriter* getWriter();

signals:
	void startTimer();

private slots:
	void timeout();

protected:
    void NotifyRepeats ();

    QString m_name;

    AbsLogWriter* m_pWriter;

    QMutex m_logMutex;
    qint32 m_count;
    LogEntry m_logMessage;

    QTimer m_timer;

private:
	// Prohibit copy and assignment.
    LogThrottle & operator=(const LogThrottle &right);
    LogThrottle (const LogThrottle &right);
};

#endif
