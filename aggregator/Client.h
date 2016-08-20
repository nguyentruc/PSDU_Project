/*
 * Client.h
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_CLIENT_H_
#define AGGREGATOR_CLIENT_H_

#include "Aggregator.h"

using namespace std;

class Client
{
public:
	Client();
	virtual ~Client();

	void start();
	void waitForDone();
	virtual void clientHdl() {};

protected:
	void subscribe();
	void unSubscribe();
};

class ClientSim900: public Client
{
public:
	void clientHdl();
private:
	string mMessage;
	string mPhoneNum;
};

#endif /* AGGREGATOR_CLIENT_H_ */
