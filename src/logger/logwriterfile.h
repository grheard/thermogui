/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef LOGFILEWRITER_H
#define LOGFILEWRITER_H

#include <QString>

#include "abslogwriter.h"


const QString CLogWriterFile("filewriter");


class LogWriterFile : public AbsLogWriter
{
public:
    LogWriterFile (QString alternateName = QString());
    ~LogWriterFile ();

    void setFileName (QString fileName);

    void write(LogEntry* pEntry);

    AbsLogWriter* getWriterByName(QString name);

protected:
    QString m_name;
    QString m_logFileName;

private:
    // Prohibit copy and assignment.
    LogWriterFile & operator=(const LogWriterFile &right);
    LogWriterFile (const LogWriterFile &right);
};

#endif
