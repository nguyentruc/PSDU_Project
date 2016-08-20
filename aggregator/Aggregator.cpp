/*
 * Aggregator.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

Aggregator::Aggregator()
{
	mSim900Hdl = new Sim900Handler(this, gPROG_ARGUMENT["serialDevice"].as<string>().c_str(),
			gPROG_ARGUMENT["baudrate"].as<int>());
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

