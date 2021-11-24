/*
 * Copyright 2021 grheard@gmail.com
 */

#ifndef MESSENGER_H
#define MESSENGER_H

#include <QObject>
#include <QQmlContext>
#include <QList>


class Messenger : public QObject
{
    Q_OBJECT

public:
    Messenger(QQmlContext* context, QObject* parent = 0);
    ~Messenger();

protected:
    typedef QList<QObject*> Clients;
    Clients m_clients;
};

#endif
