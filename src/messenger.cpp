/*
 * Copyright 2021 grheard@gmail.com
 */

#include "messenger.h"

#include "mcThermostat.h"
#include "mcWpaIf.h"


Messenger::Messenger(QQmlContext* context, QObject* parent)
    : QObject(parent)
{
    MCThermostat* mct = new MCThermostat(this);
    context->setContextProperty(mct->propertyName,mct);
    m_clients.append(mct);

    MCWpaIf* mcw = new MCWpaIf(this);
    context->setContextProperty(mcw->propertyName,mcw);
    m_clients.append(mcw);
}


Messenger::~Messenger()
{
}
