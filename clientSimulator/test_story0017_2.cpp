/*
 * test_story0017_2.cpp
 *
 *  Created on: Sep 30, 2016
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
	client.sessionInitiation_Normal("admin");
	client.receive();
	client.getSubscriberList_MissingPara();
	client.receive();

	return 0;
}
