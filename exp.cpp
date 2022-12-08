#include "exp.h"
#include <nRF24L01.h>
#include <RF24.h>

extern uint8_t RF24_CE;
extern uint8_t RF24_CSN;

RF24 RF24radio(RF24_CE, RF24_CSN);

void ExPstorage::insertData(uint16_t mask, uint8_t dataIndex, uint8_t offset, uint8_t data)
{
	dataArray[dataIndex] &= mask;
	mask = data << offset;
	dataArray[dataIndex] |= mask;
}

uint8_t ExPstorage::extractData(uint16_t mask, uint8_t dataIndex, uint8_t offset)
{
	uint16_t temp = dataArray[dataIndex] & mask;
	return (temp >> offset);
}

uint8_t ExPstorage::calculateCRC()
{
	uint16_t lowMask = 0b0000000011111111;
	uint16_t highMask = 0b1111111100000000;
	uint8_t CRC = 0;
	for (uint8_t i = 0; i < 4; i++)
	{
		uint8_t value = dataArray[i] & lowMask;
		if (i) CRC += value;
		value = dataArray[i] & highMask;
		value = value >> 8;
		CRC += value;
	}
	return (CRC);
}

bool ExPstorage::checkCRC()
{
	uint16_t mask = 0b0000000011111111;
	uint8_t CRC = dataArray[0] & mask;
	return (CRC == calculateCRC());
}

void ExPstorage::writeCRC()
{
	uint16_t mask = 0b1111111100000000;
	dataArray[0] &= mask;
	dataArray[0] |= calculateCRC();
}

ExPstorage::ExPstorage()
{

}

void ExPstorage::begin()
{
	RF24radio.begin();
    RF24radio.setChannel(5);
    RF24radio.setDataRate     (RF24_250KBPS);
    RF24radio.setPALevel      (RF24_PA_MAX);
}

void ExPstorage::startTransit(uint8_t ID)
{
	RF24radio.openWritingPipe (pipe[ID]);
	insertData(0b0001111111111111, 0, 13, ID);
}

void ExPstorage::startListening()
{
	for (uint8_t i = 0; i <= 5; i++)
	{
        RF24radio.openReadingPipe (i, pipe[i]);
    }
    RF24radio.startListening();
}

uint8_t ExPstorage::getID()
{
	return(extractData(0b1110000000000000, 0, 13));
}

void ExPstorage::pushI2C(bool I2C)
{
	insertData(0b1110111111111111, 0, 12, I2C);
}

bool ExPstorage::getI2C()
{
	return(extractData(0b0001000000000000, 0, 12));
}

void ExPstorage::pushVersion(uint8_t version)
{
	insertData(0b1111000011111111, 0, 8, version);
}

uint8_t ExPstorage::getVersion()
{
	return(extractData(0b0000111100000000, 0, 8));
}

void ExPstorage::pushTemperature(float temperature)
{
	dataArray[1] = temperature * 100 + 27315;
}

float ExPstorage::getTemperature()
{
	return (static_cast<float>(dataArray[1] - 27315) / 100);
}

void ExPstorage::pushHumidity(uint8_t humidity)
{
	insertData(0b0000000011111111, 2, 8, humidity);
}

uint8_t ExPstorage::getHumidity()
{
	return(extractData(0b1111111100000000, 2, 8));
}

void ExPstorage::pushWater(bool water)
{
	insertData(0b1111111111111101, 2, 1, water);
}

bool ExPstorage::getWater()
{
	return(extractData(0b0000000000000010, 2, 1));
}

void ExPstorage::pushMovement(bool movement)
{
	insertData(0b1111111111111110, 2, 0, movement);
}

bool ExPstorage::getMovement()
{
	return(extractData(0b0000000000000001, 2, 0));
}

void ExPstorage::pushPressurePa(uint32_t pressurePa)
{
	dataArray[3] = pressurePa - 54464;
}

uint32_t ExPstorage::getPressurePa()
{
	return (static_cast<uint32_t>(dataArray[3]) + 54464);
}

void ExPstorage::pushPressureMM(float pressureMM)
{
	pushPressurePa(pressureMM * 133.3);
}

float ExPstorage::getPressureMM()
{
	return (getPressurePa() / 133.3);
}

void ExPstorage::pushGasCurrent(uint8_t gas)
{
	insertData(0b0000000011111111, 3, 8, gas);
}

uint8_t ExPstorage::getGasCurrent()
{
	return(extractData(0b1111111100000000, 3, 8));
}

void ExPstorage::pushGasAverage(uint8_t gas)
{
	insertData(0b1111111100000000, 3, 0, gas);
}

uint8_t ExPstorage::getGasAverage()
{
	return(extractData(0b0000000011111111, 3, 0));
}

void ExPstorage::pushData(uint16_t* array)
{
	for (uint8_t i = 0; i < 4; i++)
	{
		dataArray[i] = array[i];
	}
}

const uint16_t* ExPstorage::getData()
{
	return dataArray;
}

void ExPstorage::transmitData()
{
	writeCRC();
	RF24radio.write(&dataArray, sizeof(dataArray));
}

boolean ExPstorage::receiveData()
{
	if (RF24radio.available()) 
	{
		RF24radio.read(&dataArray, sizeof(dataArray));
		return checkCRC();
	}
	else
	{
		return 0;
	}
}