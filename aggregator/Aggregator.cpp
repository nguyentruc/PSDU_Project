/*
 * Aggregator.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

Aggregator::Aggregator()
{
	mSim900Hdl = new Sim900Handler(this, "/dev/ttyUSB0");
}

void Aggregator::start()
{
	mSim900Hdl->start();
	mSim900Hdl->waitForDone();
}

Aggregator::~Aggregator()
{
	delete mSim900Hdl;
}

