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

void ClientSimulator::connectToPSDU()
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

void ClientSimulator::sessionInitiation_Normal(const string& anUser)
{
	Json::Value root;

	printf("COMMAND: Session Initiation\n");

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

	sendToPSDU(root);
}

void ClientSimulator::addSubscriber_Normal(const string& aNumber, const string& aStatus)
{
	Json::Value root;

	printf("COMMAND: Add Subscriber (Normal)\n");

	root["action"] = "AddSubscriber";
	root["status"] = aStatus;
	root["phone"] = aNumber;

	sendToPSDU(root);
}

void ClientSimulator::getSubscriberList_Normal(const string& aStatus)
{
	Json::Value root;

	printf("COMMAND: Get Subscriber List (Normal)\n");

	root["action"] = "GetSubscriberList";
	root["status"] = aStatus;

	sendToPSDU(root);
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

	printf("RECEIVED %d: %s\n", rcvMsgSize, rcvBuffer);

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

void ClientSimulator::addSubscriber_MissingPara()
{
	Json::Value root;

	printf("COMMAND: Add Subscriber (Missing parameters)\n");

	root["action"] = "AddSubscriber";

	sendToPSDU(root);
}

void ClientSimulator::sessionInitiation_WrongPass(const string& anUser)
{
	Json::Value root;

	printf("COMMAND: Session Initiation (Wrong password)\n");

	root["action"] = "SessionInitiation";

	if (anUser == "admin")
	{
		root["user"] = "admin";
		root["pass"] = "vcxvcxv";
	}
	else
	{
		root["user"] = "subscriber";
		root["pass"] = "fdhsfkjdfhdkj";
	}

	sendToPSDU(root);
}

void ClientSimulator::sessionInitiation_WrongUser()
{
	Json::Value root;

	printf("COMMAND: Session Initiation (Wrong password)\n");

	root["action"] = "SessionInitiation";
	root["user"] = "adm1n";
	root["pass"] = "vcxvcxv";

	sendToPSDU(root);
}

void ClientSimulator::sessionInitiation_MissingPara()
{
	Json::Value root;

	printf("COMMAND: Session Initiation (Wrong password)\n");

	root["action"] = "SessionInitiation";
	root["user"] = "admin";

	sendToPSDU(root);
}

void ClientSimulator::sessionInitiation_Skipped()
{
	addSubscriber_Normal("01257780872", "Power");
}

void ClientSimulator::sendToPSDU(const Json::Value& aRoot)
{
	Json::FastWriter writer;
	string outMsg = writer.write(aRoot);

	cout << "outMsg's size = " << outMsg.size() << endl;
	cout << "outMsg = " << outMsg << endl;

	if (send(mSock, outMsg.c_str(), outMsg.size() + 1, 0) != outMsg.size() + 1)
	{
		cout << "send() failed from: " << __FILE__ << ":" << __LINE__ << endl;
	}

	Json::StyledWriter writerStyle;
	string rcvMsg = writerStyle.write(aRoot);

	printf("JSON: %s\n", rcvMsg.c_str());
}
