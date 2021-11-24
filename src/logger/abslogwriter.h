/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef ABSLOGWRITER_H
#define ABSLOGWRITER_H

#include "logentry.h"


class AbsLogWriter
{
public:
	AbsLogWriter();
	virtual ~AbsLogWriter() {}

	virtual void write(LogEntry* pEntry) = 0;

	virtual void setLogLevel(LogEntry::LogLevel level);
	virtual LogEntry::LogLevel getLogLevel();

	virtual bool setWriter(AbsLogWriter* pWriter);

	virtual AbsLogWriter* getWriterByName(QString name);

	virtual AbsLogWriter* getWriter();

protected:
    LogEntry::LogLevel m_level;
};

#endif
