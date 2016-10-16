/*
 * test_story0019_4.cpp
 *
 *  Created on: Oct 16, 2016
 *      Author: trucndt
 */

#include "ClientSimulator.h"

using namespace std;

int main(int argc, char **argv)
{
	if (processCommandLineArgument(argc, argv) < 0)
	{
		return 0;
	}

	ClientSimulator client;
	client.connectToPSDU();
	client.sessionInitiation_Normal("subscriber");
	client.receive();
	client.subscribe_Normal("01257780872", "Power");
	client.receive();
	client.subscribe_Normal("01257780872", "Power");
	client.receive();
	client.subscribe_Normal("01257780872", "Power");
	client.receive();
	client.subscribe_Normal("01257780872", "Power");
	client.receive();
	client.~ClientSimulator();

	sleep(2);

	ClientSimulator admin;
	client.connectToPSDU();
	client.sessionInitiation_Normal("admin");
	client.receive();
	client.getSubscriberList_Normal("Power");
	client.receive();

	return 0;
}


