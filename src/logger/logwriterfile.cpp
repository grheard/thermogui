/*
 * Copyright 2021 grheard@gmail.com
 */

#include <QCoreApplication>
#include <QDebug>
#include <QFile>

#include "logwriterfile.h"


LogWriterFile::LogWriterFile (QString alternateName)
    : m_name(alternateName.isEmpty() ? CLogWriterFile : alternateName)
{
}


LogWriterFile::~LogWriterFile ()
{
}


void LogWriterFile::setFileName (QString fileName)
{
    m_logFileName = fileName;
}


void LogWriterFile::write (LogEntry* pEntry)
{
	if (!pEntry) return;

	if (pEntry->GetLevel() < m_level) return;

    QFile file(m_logFileName);
    QString logstr;

    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        LogEntry entry(this,
                       QCoreApplication::instance()->applicationName().toStdString().c_str(),
                       LogEntry::Error,
                       __PRETTY_FUNCTION__,
                       "Could not open file '%s'",
                       m_logFileName.toStdString().c_str());
        entry.ToString(logstr);
        qDebug() << logstr.toUtf8();
    }

    pEntry->ToString(logstr);
    logstr += QString("\n");

    file.write(logstr.toUtf8());
    file.close();
}


AbsLogWriter* LogWriterFile::getWriterByName(QString name)
{
	if (name == m_name)
	{
		return this;
	}

	return 0;
}
