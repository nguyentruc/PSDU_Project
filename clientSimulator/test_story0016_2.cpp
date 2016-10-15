/*
 * test_story0016_2.cpp
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

	ClientSimulator client;
	client.connectToPSDU();
	client.sessionInitiation_Normal("admin");
	client.receive();
	client.changeAccPwd_MissingPara();
	client.receive();
	return 0;
}


