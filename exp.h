//    Библиотека протокола обмена данными ExP
//    Стек передаваемых даных :
//    ID ID ID JJ VV VV VV VV    CR CR CR CR CR CR CR CR
//    TT TT TT TT TT TT TT TT    TT TT TT TT TT TT TT TT
//    HH HH HH HH HH HH HH HH    RR RR RR RR RR RR WW MM
//    GC GC Gc GC GC GC GC GC    GA GA GA GA GA GA GA GA
//    ID - ID передатчика
//    JJ - флаг I2C
//    VV - версия протокола обмена
//    CR - контрольная сумма CRC
//    TT - темперетару
//    HH - влажность
//    RR - резерв
//    GC - газ (текущее значение) / давление
//    GC - газ (среднее значение) / давление ( если флаг I2C включен)


#pragma once

#include <stdint.h>

class ExPstorage
{
private:

	uint16_t dataArray[4]{ 0,0,0,0 };                                           // Массив данных для передачи
	const uint64_t pipe[6] =                                                    // Массив ID для "труб"
	{
		0xE8E8F0F0E1LL, 
		0xE8E8F0F0E2LL, 
		0xE8E8F0F0A2LL, 
		0xE8E8F0F0D1LL, 
		0xE8E8F0F0C3LL, 
		0xE8E8F0F0E7LL
	};

	void insertData(uint16_t mask, uint8_t dataIndex, uint8_t offset, uint8_t data);
	                                                                            // Функция добавления данных с нужной маской со смещением в указанный адрес
	uint8_t extractData(uint16_t mask, uint8_t dataIndex, uint8_t offset);      // Функция извлечения данных с нужной маской со смещением из указанного адреса
	uint8_t calculateCRC();                                                     // Функция подсчета CRC суммы
	bool checkCRC();                                                            // Функция сравнения CRC суммы
	void writeCRC();                                                            // Функция записи CRC суммы

public:

	ExPstorage();                                                               // Конструктор класса

	void begin();                                                               // Функция создания ExP хранилища и связи его с nRF24 модулем
	void startTransit(uint8_t ID);                                              // Функция начала передачи от имени указанного ID
	void startListening();                                                      // Функция начала прослушивания

	uint8_t getID();                                                            // Функция получения ID передатчика из массива

	void pushI2C(bool I2C);                                                     // Функция добавления флага I2C в массив
	bool getI2C();                                                              // Функция получения флага I2C из массива

	void pushVersion(uint8_t version);                                          // Функция добавления версии протокола в массив
	uint8_t getVersion();                                                       // Функция получения версии протокола из массива

	void pushTemperature(float temperature);                                    // Функция добавления температуры в массив
	float getTemperature();                                                     // Функция получения температуры из массива

	void pushHumidity(uint8_t humidity);                                        // Функция добавления влажности в массив
	uint8_t getHumidity();                                                      // Функция получения влажности из массива

	void pushWater(bool water);                                                 // Функция добавления флага показаний датчика протечки воды в массив
	bool getWater();                                                            // Функция получения флага показаний датчика протечки воды из массива

	void pushMovement(bool movement);                                           // Функция добавления флага показаний датчик движения в массив
	bool getMovement();                                                         // Функция получения флага показаний датчика движения из массива

	void pushPressurePa(uint32_t pressurePa);                                   // Функция добавления давления (в Паскалях) в массив
	uint32_t getPressurePa();                                                   // Функция получения давления (в Паскалях) из массива

	void pushPressureMM(float pressureMM);                                      // Функция добавления давления (в мм.рт.ст.) в массив
	float getPressureMM();                                                      // Функция получения давления (в мм.рт.ст.) из массива

	void pushGasCurrent(uint8_t gas);                                           // Функция добавления текущей загазованности в массив
	uint8_t getGasCurrent();                                                    // Функция получения текущей загазованности из массива

	void pushGasAverage(uint8_t gas);                                           // Функция добавления средней загазованности в массив
	uint8_t getGasAverage();                                                    // Функция получения средней загазованности из массива

	void pushData(uint16_t* array);                                             // Функция добавления всего массива данных
	const uint16_t* getData();                                                  // Функция получения всего массива данных

	void transmitData();                                                        // Функция передачи данных по радиоканалу

	bool receiveData();                                                         // Функция приема данных по радиоканалу
};

