/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGWRITERSYSLOG_H
#define LOGWRITERSYSLOG_H

#include <QString>

#include "abslogwriter.h"


const QString CLogWriterSyslog("syslogwriter");


class LogWriterSyslog : public AbsLogWriter
{
public:
	LogWriterSyslog (QString alternateName = QString());
    ~LogWriterSyslog ();

    void write(LogEntry* pEntry);

    AbsLogWriter* getWriterByName(QString name);

protected:
    void formatEntry(LogEntry* pLogEntry, int& priority, QString& logEntry);

    QString m_name;

    bool m_opened;
    std::string m_serviceName;

private:
    // Prohibit copy and assignment.
    LogWriterSyslog & operator=(const LogWriterSyslog &right);
    LogWriterSyslog (const LogWriterSyslog &right);
};

#endif
