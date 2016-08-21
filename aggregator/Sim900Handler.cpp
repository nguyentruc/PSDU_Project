/*
 * Sim900Handler.cpp
 *
 *  Created on: Aug 18, 2016
 *      Author: trucndt
 */

#include "Sim900Handler.h"

Sim900Handler::Sim900Handler(Aggregator* aAggregator, const char *aDevice, int aBaudrate)
{
	mAggregator = aAggregator;
	cout << "Serial port: " << aDevice << endl;
	cout << "Baudrate = " << aBaudrate << endl;
	uartSetup(aDevice, aBaudrate);
}

Sim900Handler::~Sim900Handler()
{
	close(mUartFd);
}

/**
 * Uart configuration
 * @param[in]:
 * 	const char *aDevice: device file of serial port
 */
void Sim900Handler::uartSetup(const char *aDevice, int aBaudrate)
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
	sleep(2);
	tcflush(mUartFd, TCIFLUSH);
}

void Sim900Handler::start()
{
	/* Handle Sim900 */
	mHandleThead = boost::thread(&Sim900Handler::sim900Hdl, this);
}

void Sim900Handler::waitForDone()
{
	mHandleThead.join();
}


void Sim900Handler::sim900Hdl()
{
	sim900Init();

	uint8_t state = 0;
	string phoneNum, rcvMsg;
	while (true)
	{
		rcvMsg = sim900Get();

		switch (state)
		{
		case 0:
			if (rcvMsg.find("+CMT:") != string::npos) // receive sms header
			{
				state = 1;
				int firstQuote = rcvMsg.find('\"');
				int secondQuote = rcvMsg.find('\"', firstQuote + 1);
				phoneNum = rcvMsg.substr(firstQuote + 1, secondQuote - firstQuote - 1);
			}
			break;
		case 1: // get sms body
			cout << "From phone number: " << phoneNum << endl;
			cout << "Received sms: " << rcvMsg << endl;
			state = 0;
			break;
		}

	}
}

/**
 * For testing uart
 */
void Sim900Handler::uartTest()
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
 * Sim900 configuration
 */
void Sim900Handler::sim900Init()
{
	printf("Initializing Sim900...\n");
	sleep(1); //delay 1s

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
 * Wait until a message available then get one
 * @Return:	The received messsage
 */
string Sim900Handler::sim900Get()
{
	char rcvBuf[256];
	uint8_t rcvIndex = 0;

	while (1)
	{
		int length = read(mUartFd, &rcvBuf[rcvIndex], 1);
		if (length <= 0)
		{
			printf("Error, length = %d at %s:%d", length, __FILE__, __LINE__);
			return string("ERROR");
		}

		if (rcvIndex > 0)
		{
			/* Received \r\n */
			if (rcvBuf[rcvIndex - 1] == '\r' && rcvBuf[rcvIndex] == '\n')
			{
				string rcvMsg(rcvBuf, rcvIndex - 1);
				cout << "Received: " << rcvMsg << endl;
				return rcvMsg;
			}
			else rcvIndex++;
		}
		else rcvIndex++;
	}
}

/**
 * Wait until receiving OK from Sim900
 * Return:
 * 	1 if success
 * 	-1 in case of receiving ERROR
 */
int Sim900Handler::waitForOk()
{
	string rcvMsg;
	while (rcvMsg.find("OK") == string::npos)
	{
		rcvMsg = sim900Get();

		if (rcvMsg.find("ERROR") != string::npos)
			return -1;
	}

	return 0;
}

int Sim900Handler::uartBaudrate(int aBaudrate)
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
