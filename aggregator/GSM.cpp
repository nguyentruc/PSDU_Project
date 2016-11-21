/*
 * GSM.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "GSM.h"

GSM::GSM(Aggregator* aAggregator, const char *aDevice, int aBaudrate)
{
	mAggregator = aAggregator;
	mRcvCharBuf = boost::circular_buffer<uint8_t>(BUFFER_SIZE);
	cout << "Serial port: " << aDevice << endl;
	cout << "Baudrate = " << aBaudrate << endl;
	uartSetup(aDevice, aBaudrate);
}

GSM::~GSM()
{
	close(mUartFd);
}

/**
 * Uart configuration
 * @param[in]:
 * 	const char *aDevice: device file of serial port
 */
void GSM::uartSetup(const char *aDevice, int aBaudrate)
{
	//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
	mUartFd = -1;
	//OPEN THE UART
	//The flags (defined in fcntl.h):
	//	Access modes (use 1 of these):
	//		O_RDONLY - Open for reading only.
	//		O_RDWR - Open for reading and writing.
	//		O_WRONLY - Open for writing only.
	//
	//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
	//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
	//											immediately with a failure status if the output can't be written immediately.
	//
	//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
	mUartFd = open(aDevice, O_RDWR | O_NOCTTY);		//Open in non blocking read/write mode
	if (mUartFd == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
		return;
	}

	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(mUartFd, &options);
	options.c_cflag = uartBaudrate(aBaudrate) | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = 0;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcsetattr(mUartFd, TCSANOW, &options);

	//TODO: Flush input buffer before starting
	//sleep(2);
	tcflush(mUartFd, TCIFLUSH);
}

void GSM::start()
{
	/* Wait for incoming bytes from serial port */
	boost::thread(&GSM::GSMMonitor, this);

	/* Handle received message */
	boost::thread(&GSM::GSMHandler, this);
}

void GSM::GSMHandler()
{
	GSMInit();

	uint8_t state = 0;
	string phoneNum, rcvMsg;
	while (true)
	{
		rcvMsg = GSMGet();

		switch (state)
		{
		case 0:
			if (rcvMsg.find("+CMT:") != string::npos) // receive sms header
			{//TODO: use ::compare to optimize time
				state = 1;
				int firstQuote = rcvMsg.find('\"');
				int secondQuote = rcvMsg.find('\"', firstQuote + 1);
				phoneNum = rcvMsg.substr(firstQuote + 1, secondQuote - firstQuote - 1);
			}
			else if (rcvMsg.find("OK") != string::npos)
			{
				boost::unique_lock<boost::mutex> uniLck(mMtx_IsOk);
				mIsOk = 1;
				mCv_IsOk.notify_all();
			}
			else if (rcvMsg.find("ERROR") != string::npos)
			{
				boost::unique_lock<boost::mutex> uniLck(mMtx_IsOk);
				mIsOk = 0;
				mCv_IsOk.notify_all();
			}
			else
			{
				boost::unique_lock<boost::mutex> uniLck(mMtx_SharedMsg);
				mSharedMsg = rcvMsg;
				mCv_SharedMsg.notify_all();
			}
			break;
		case 1: // get sms body
			cout << "From phone number: " << phoneNum << endl;
			cout << "Received sms: " << rcvMsg << endl;

			ClientGSM *client = new ClientGSM(mAggregator, rcvMsg, phoneNum, this);
			client->start();

			state = 0;
			break;
		}

	}
}

/**
 * For testing uart
 */
void GSM::uartTest()
{
	//----- TX BYTES -----

	dprintf(mUartFd, "Hello World!\n");

	//----- CHECK FOR ANY RX BYTES -----
	if (mUartFd != -1)
	{
		// Read up to 255 characters from the port if they are there
		while (1)
		{
			unsigned char rx_buffer[256];
			int rx_length = read(mUartFd, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
			if (rx_length < 0)
			{
				//An error occured (will occur if there are no bytes)
				cout << rx_length;
			}
			else if (rx_length == 0)
			{
				//No data waiting
				cout << rx_length;
			}
			else
			{
				//Bytes received
				rx_buffer[rx_length] = '\0';
				printf("%i bytes read : %s\n", rx_length, rx_buffer);
			}
		}
	}
}

/**
 * GSM configuration
 */
void GSM::GSMInit()
{
	printf("Initializing GSM module...\n");
	sleep(1); //delay 1s

	mRcvCharBuf.clear();// This is temporary, flushing in OS is a better practice

	dprintf(mUartFd, "ATE0\r");
	if (waitForOk() < 0)
	{
		cout << "Error at " << __FILE__ << ":" << __LINE__ << endl;
	}

	dprintf(mUartFd, "AT+CMGF=1\r");
	if (waitForOk() < 0)
	{
		cout << "Error at " << __FILE__ << ":" << __LINE__ << endl;
	}

	dprintf(mUartFd, "AT+CNMI=1,2,0,0,0\r");
	if (waitForOk() < 0)
	{
		cout << "Error at " << __FILE__ << ":" << __LINE__ << endl;
	}

	dprintf(mUartFd, "AT+CLIP=1\r");
	if (waitForOk() < 0)
	{
		cout << "Error at " << __FILE__ << ":" << __LINE__ << endl;
	}

//	dprintf(mUartFd, "AT+CSMP=17,167,0,240\r");
	dprintf(mUartFd, "AT+CSMP=17,167,0,0\r"); // send normal 7 bit sms
	if (waitForOk() < 0)
	{
		cout << "Error at " << __FILE__ << ":" << __LINE__ << endl;
	}

	printf("Finished\n");
}

/**
 * Read as many bytes as possible and store in the circular buffer
 */
void GSM::GSMMonitor()
{
	uint8_t rcvBuf[BUFFER_SIZE];

	/* This thread should have higher priority */
	makeRealTimeThread();

	while (1)
	{
		int length = read(mUartFd, rcvBuf, BUFFER_SIZE);
		if (length <= 0)
		{
			printf("Error, length = %d at %s:%d", length, __FILE__, __LINE__);
			return;
		}

		boost::unique_lock<boost::mutex> uniLck(mMtx_RcvCharBuf);

		for (int i = 0; i < length; i++)
		{
			mRcvCharBuf.push_back(rcvBuf[i]);
		}
		mCv_RcvCharBuf.notify_all();
	}
}

/**
 * Wait until receiving \r\n
 * @Return:	The received message
 */
string GSM::GSMGet()
{
	uint8_t rcvStr[BUFFER_SIZE];
	uint16_t rcvIdx = 0;

	while (1)
	{
		boost::unique_lock<boost::mutex> uniLck(mMtx_RcvCharBuf);

		while (mRcvCharBuf.empty() == true)
		{
			mCv_RcvCharBuf.wait(uniLck);
		}

		/* read each byte from the buffer */
		rcvStr[rcvIdx] = mRcvCharBuf.front();
		mRcvCharBuf.pop_front();

		uniLck.unlock();

		/* Check for \r\n */
		if (rcvIdx > 0)
		{
			if (rcvStr[rcvIdx - 1] == '\r' && rcvStr[rcvIdx] == '\n')
			{
				string ret((char *)rcvStr, rcvIdx - 1);
				cout << "Received GSM: " << ret << endl;
				return ret;
			}
		}

		rcvIdx++;
	}


}

/**
 * Wait until receiving OK from GSM module
 * Return:
 * 	1 if success
 * 	-1 in case of receiving ERROR
 */
int GSM::waitForOk()
{
	string rcvMsg;
	while (rcvMsg.find("OK") == string::npos)
	{
		rcvMsg = GSMGet();

		if (rcvMsg.find("ERROR") != string::npos)
			return -1;
	}

	return 0;
}

int GSM::uartBaudrate(int aBaudrate)
{
	switch (aBaudrate)
	{
	case 9600:
		return B9600;
	case 38400:
		return B38400;
	case 115200:
		return B115200;
	default:
		cout << "ERROR: Baudrate " << aBaudrate << " is not supported";
	}
	return -1;
}

int GSM::sendSms(const string &aPhoneNum, const string &aMsg)
{
	boost::lock_guard<boost::mutex> lckGuard(mMtx_sendAT);
    char sendMsg[50];

    cout << "Sending SMS message to " << aPhoneNum << endl;

    dprintf(mUartFd, "AT+CMGS=\"%s\"\r", aPhoneNum.c_str());
    sleep(1);

    dprintf(mUartFd, "%s", aMsg.c_str());
    dprintf(mUartFd, "%c", 0x1A);

    /* Wait for OK */
	boost::unique_lock<boost::mutex> uniLck(mMtx_IsOk);
	mCv_IsOk.wait(uniLck);

	return mIsOk;
}

string GSM::checkAccMoney()
{
	boost::lock_guard<boost::mutex> lckGuard(mMtx_sendAT);
	dprintf(mUartFd, "AT+CUSD=1,\"*101#\"\r");

	{
		/* Wait for OK */
		boost::unique_lock<boost::mutex> uniLck(mMtx_IsOk);
		mCv_IsOk.wait(uniLck);

		if (mIsOk == 0) return "";
	}

	while (true)
	{
		/* received cusd message */
		boost::unique_lock<boost::mutex> uniLck(mMtx_SharedMsg);

		if (mCv_SharedMsg.timed_wait(uniLck, boost::posix_time::milliseconds(20000)))
		{
			if (mSharedMsg.compare(0, 5, "+CUSD") == 0) //compare first 5 chars
			{
				int firstQuote = mSharedMsg.find('\"');
				int secondQuote = mSharedMsg.find('\"', firstQuote + 1);
				return mSharedMsg.substr(firstQuote + 1, secondQuote - firstQuote - 1);
			}
		}
		else
		{
			cout << "Potential problems in GSM " << __FILE__ << ":" << __LINE__ << endl;
			return "";
		}
	}
}

string GSM::refillAccMoney(const char* aCode)
{
	boost::lock_guard<boost::mutex> lckGuard(mMtx_sendAT);
	dprintf(mUartFd, "AT+CUSD=1,\"*100*%s#\"\r", aCode);
	printf("\nAT+CUSD=1,\"*100*%s#\"\r\n", aCode);

	{
		/* Wait for OK */
		boost::unique_lock<boost::mutex> uniLck(mMtx_IsOk);
		mCv_IsOk.wait(uniLck);

		if (mIsOk == 0) return "";
	}

	while (true)
	{
		/* received cusd message */
		boost::unique_lock<boost::mutex> uniLck(mMtx_SharedMsg);

		if (mCv_SharedMsg.timed_wait(uniLck, boost::posix_time::milliseconds(20000)))
		{
			if (mSharedMsg.compare(0, 5, "+CUSD") == 0) //compare first 5 chars
			{
				int firstQuote = mSharedMsg.find('\"');
				int secondQuote = mSharedMsg.find('\"', firstQuote + 1);
				return mSharedMsg.substr(firstQuote + 1, secondQuote - firstQuote - 1);
			}
		}
		else
		{
			cout << "Potential problems in GSM " << __FILE__ << ":" << __LINE__ << endl;
			return "";
		}
	}
}
