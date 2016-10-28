/*
 * test_story0023_2.cpp
 *
 *  Created on: Oct 28, 2016
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
	client.unsubscribe_MissingPara();
	client.receive();

	return 0;
}


