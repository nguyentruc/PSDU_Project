/*
 * GPIO.cpp
 *
 *  Created on: Sep 22, 2016
 *      Author: trucndt
 */

#include "GPIO.h"

#define MAX_BUF 64
#define SYSFS_GPIO_DIR "/sys/class/gpio"

GPIO::GPIO(int aPinNum, uint8_t isOut)
{
	mPinNum = aPinNum;
	mFd = -1;
	mCurValue = -1;

	gpioExport();
	setDirection(isOut);
}

GPIO::~GPIO()
{
	if (mFd >= 0) close(mFd);
}

int GPIO::setDirection(uint8_t aWayOut)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", mPinNum);

	fd = open(buf, O_WRONLY);
	if(fd < 0)
	{
		printf("ERR: setDirection %d\n", fd);
		return fd;
	}

	if(aWayOut == 1)
	{
		write(fd, "out", 4);
	}
	else if (aWayOut == 0)
	{
		write(fd, "in", 3);
	}
	else if (aWayOut == 2)
	{
		write(fd, "high", 5);
	}

	mIsOut = aWayOut;

	close(fd);

	return 0;
}

int GPIO::getValue()
{
	int8_t value = -1;

	if(mIsOut == false)
	{
		if(mFd < 0) openFd();
		if(mFd < 0) return mFd;

		read(mFd, &value, 1);
		value = value - 48;

		mCurValue = value;
	}

	return value;
}

int GPIO::setEdge(const char* anEdge)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", mPinNum);

	fd = open(buf, O_WRONLY);
	if( fd < 0 )
	{
		printf("setEdge %d\n", fd);
		return fd;
	}

	write(fd, anEdge, strlen(anEdge) + 1);

	close(fd);

	return 0;
}

int GPIO::setValue(int aValue)
{
	if( mIsOut && mCurValue != aValue )
	{
		if(mFd < 0) openFd();
		if(mFd < 0) return mFd;

		if(aValue)
		{
			write(mFd, "1", 2);
		}
		else
		{
			write(mFd, "0", 2);
		}

		mCurValue = aValue;
	}

	return 0;
}

int GPIO::gpioExport()
{
	int fd, len;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);

	if( fd < 0 )
	{
		printf("exportPin %d\n", fd);
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", mPinNum);

	write(fd, buf, len);

	close(fd);

	return 0;
}

void GPIO::openFd()
{
	int len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", mPinNum);

	if(mFd >= 0) close(mFd);

	if( mIsOut )
		mFd = open(buf, O_RDWR);
	else
		mFd = open(buf, O_RDONLY);

	if(mFd < 0)
	{
		printf("openIoFd %d\n", mFd);
	}
}
