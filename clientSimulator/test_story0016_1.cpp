/*
 * test_story0016_1.cpp
 *
 *  Created on: Oct 12, 2016
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

	ClientSimulator subscriber0;
	subscriber0.connectToPSDU();
	subscriber0.sessionInitiation_Normal("subscriber");
	subscriber0.receive();
	subscriber0.~ClientSimulator();

	sleep(2);

	ClientSimulator client;
	client.connectToPSDU();
	client.sessionInitiation_Normal("admin");
	client.receive();
	client.changeAccPwd_Normal("abc");
	client.receive();
	client.~ClientSimulator();

	sleep(2);

	ClientSimulator subscriber1;
	subscriber1.connectToPSDU();
	subscriber1.sessionInitiation_Normal("subscriber");
	subscriber1.receive();
	return 0;
}


