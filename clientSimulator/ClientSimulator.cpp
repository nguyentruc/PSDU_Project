/*
 * ClientSimulator.cpp
 *
 *  Created on: Sep 30, 2016
 *      Author: trucndt
 */

#include "ClientSimulator.h"

po::variables_map gPROG_ARGUMENT;

ClientSimulator::ClientSimulator()
{
	mSock = -1;
}

ClientSimulator::~ClientSimulator()
{
	close(mSock);
}

void ClientSimulator::connectToServer()
{
    struct sockaddr_rc servAddr;

    // allocate socket
    if ((mSock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
    {
		dieWithError("socket() failed");
    }

    // set the connection parameters (who to connect to)
    str2ba(gPROG_ARGUMENT["address"].as<string>().c_str(), &servAddr.rc_bdaddr);
    servAddr.rc_family = AF_BLUETOOTH;
    servAddr.rc_channel = (uint8_t) gPROG_ARGUMENT["channel"].as<int>();

    if (connect(mSock, (struct sockaddr *)&servAddr, sizeof servAddr) < 0)
    {
    	dieWithError("connect() failed");
    }

}

void ClientSimulator::sessionInitiation_Normal(string& anUser)
{
	Json::Value root;

	printf("Command: Session Initiation\n");

	root["action"] = "SessionInitiation";

	if (anUser == "admin")
	{
		root["user"] = "admin";
		root["pass"] = "admin";
	}
	else
	{
		root["user"] = "subscriber";
		root["pass"] = "";
	}

	Json::FastWriter writer;
	string outMsg = writer.write(root);

	cout << "outMsg's size = " << outMsg.size() << endl;
	cout << "outMsg = " << outMsg << endl;

	if (send(mSock, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
	}

	Json::StyledWriter writerStyle;
	string rcvMsg = writerStyle.write(root);

	printf("JSON: %s\n", rcvMsg.c_str());
}

void ClientSimulator::receive()
{
	int rcvMsgSize;
	char rcvBuffer[1024];

	if ((rcvMsgSize = recv(mSock, rcvBuffer, 1024, 0)) <= 0)
	{
		cout << "recv() failed from: " << __FILE__ << ":" << __LINE__ << endl;
		cout << "rcvMsgSize = " << rcvMsgSize << endl;

		return;
	}
	rcvBuffer[rcvMsgSize] = NULL;

	printf("Received %d: %s\n", rcvMsgSize, rcvBuffer);

	Json::Reader reader;
	Json::Value root;

	if (reader.parse(rcvBuffer, rcvBuffer + rcvMsgSize, root) == 0)
	{
		printf("Error: Parse JSON failed\n");
		return;
	}

	Json::StyledWriter writer;
	string rcvMsg = writer.write(root);

	printf("JSON: %s\n", rcvMsg.c_str());
}

void ClientSimulator::addSubscriber_Normal()
{
}

int processCommandLineArgument(int argc, char **argv)
{
	po::options_description usage("Usage");

	usage.add_options()
			("help,h", "Print help messages")
			("address,a", po::value<string>()->required() , "Bluetooth address of server")
			("channel,c", po::value<int>()->default_value(1) , "Channel of the bluetooth device");
	try
	{
		po::store(po::command_line_parser(argc, argv).options(usage).run(), gPROG_ARGUMENT);

		if (gPROG_ARGUMENT.count("help"))
		{
			cout << usage << endl;
			return -1;
		}

		po::notify(gPROG_ARGUMENT);
	}
	catch (po::required_option& e)
	{
		cout << "ERROR: " << e.what() << endl;
		cout << usage << endl;
		return -1;
	}
	catch (po::error& e)
	{
		cout << "ERROR: " << e.what() << endl;
		return -1;
	}

	return 0;
}


