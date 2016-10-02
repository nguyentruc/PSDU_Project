/*
 * test_story0014.cpp
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

	string user = "admin";

	ClientSimulator client;
	client.connectToPSDU();
	client.sessionInitiation_Normal(user);
	client.receive();
	return 0;
}
