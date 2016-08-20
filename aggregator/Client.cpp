/*
 * Client.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Client.h"

Client::Client()
{
	// TODO Auto-generated constructor stub

}

Client::~Client()
{
	// TODO Auto-generated destructor stub
}

void Client::start()
{
	boost::thread a(&Client::clientHdl, this);
	a.join();
}

void Client::waitForDone()
{
}

void Client::subscribe()
{
}

void Client::unSubscribe()
{
}

void ClientSim900::clientHdl()
{
	printf("ClientSim900\n");
}
