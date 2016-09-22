/*
 * main.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Aggregator.h"

using namespace std;

po::variables_map gPROG_ARGUMENT;

int processCommandLineArgument(int argc, char **argv);

int main(int argc, char **argv)
{
	if (processCommandLineArgument(argc, argv) < 0)
	{
		return 0;
	}

	Aggregator aggregator;

	if (gPROG_ARGUMENT.count("subscribe")) //initial subscription
	{
		vector<string> listNum = gPROG_ARGUMENT["subscribe"].as<vector<string> >();
		for (int i = 0; i < listNum.size(); i++)
		{
			aggregator.addSubscriber(POWER_STATUS, listNum[i]);
		}
	}

	aggregator.start();

	return 0;
}

int processCommandLineArgument(int argc, char **argv)
{
	po::options_description usage("Usage");

	usage.add_options()
			("help,h", "Print help messages")
			("version,v", "Display the version number")
			("serialDevice,s", po::value<string>()->required() , "Path to serial port")
			("baudrate,b", po::value<int>()->default_value(115200), "Baudrate of the serial port")
			("noGSM", "Disable GSM module")
			("subscribe",  po::value<vector<string> >(), "Subscribe a phone number to power status");

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
