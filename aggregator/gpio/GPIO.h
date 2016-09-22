/*
 * GPIO.h
 *
 *  Created on: Sep 22, 2016
 *      Author: trucndt
 */

#ifndef AGGREGATOR_GPIO_GPIO_H_
#define AGGREGATOR_GPIO_GPIO_H_

class GPIO
{
public:
	GPIO(int aPinNum, bool isOut = true);
	virtual ~GPIO();

	int setDirection(bool aWayOut);
	int getValue();
	int setEdge(const char* anEdge);
	int setValue(int aValue);

	int getFd()
	{
		if(mFd < 0) openFd();
		return mFd;
	}

private:
	int mFd;
	int mPinNum;
	bool mIsOut;
	int mCurValue;

	int gpioExport();
	void openFd();
};

#endif /* AGGREGATOR_GPIO_GPIO_H_ */
