/*
 * Aggregator.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_AGGREGATOR_H_
#define AGGREGATOR_AGGREGATOR_H_

#include <stdio.h>
#include <iostream>
#include "utils.h"
#include "Sim900Handler.h"
#include "Client.h"

#include <list>
#include <map>

class Client;
class Sim900Handler;

class Aggregator {
public:
	Aggregator();
	virtual ~Aggregator();

	void start();

private:
	Sim900Handler *mSim900Hdl;
};

#endif /* AGGREGATOR_AGGREGATOR_H_ */
