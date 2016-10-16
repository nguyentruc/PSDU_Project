/*
 * test_story0019_1.cpp
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
	client.~ClientSimulator();

	sleep(2);

	ClientSimulator client0;
	client0.connectToPSDU();
	client0.sessionInitiation_Normal("subscriber");
	client0.receive();
	client0.subscribe_Normal("01257780871", "Power");
	client0.receive();
	client0.~ClientSimulator();

	sleep(2);

	ClientSimulator client1;
	client1.connectToPSDU();
	client1.sessionInitiation_Normal("subscriber");
	client1.receive();
	client1.subscribe_Normal("01257780873", "Power");
	client1.receive();
	client1.~ClientSimulator();

	sleep(2);

	ClientSimulator admin;
	client.connectToPSDU();
	client.sessionInitiation_Normal("admin");
	client.receive();
	client.getSubscriberList_Normal("Power");
	client.receive();

	return 0;
}


