/*
 * ClientSimulator.h
 *
 *  Created on: Sep 30, 2016
 *      Author: trucndt
 */

#ifndef CLIENTSIMULATOR_CLIENTSIMULATOR_H_
#define CLIENTSIMULATOR_CLIENTSIMULATOR_H_

#include "utils.h"
#include "json/json.h"

using namespace std;

extern po::variables_map gPROG_ARGUMENT;

class ClientSimulator
{
public:
	ClientSimulator();
	virtual ~ClientSimulator();

	void connectToServer();
	void receive();
	void sessionInitiation_Normal(string& anUser);
	void addSubscriber_Normal();

private:
	int mSock;
};

int processCommandLineArgument(int argc, char **argv);

#endif /* CLIENTSIMULATOR_CLIENTSIMULATOR_H_ */
