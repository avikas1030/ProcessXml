/*
 * Logger.h
 *
 *  Created on: Nov 16, 2019
 *      Author: LavishK1
 */

#ifndef LOGGER_LOGGER_H_
#define LOGGER_LOGGER_H_

#include "LogControl.h"

#ifdef _LOG_ENABLED
#include <iostream>

#define LOG_OUT(X)  std::cout << X << std::endl
#define LOG_PRINT  printf

#else
#define LOGAPI_EMPTYSTMT do {} while(0)

#define LOG_OUT(...)  	LOGAPI_EMPTYSTMT
#define LOG_PRINT(...)  LOGAPI_EMPTYSTMT

#endif

#endif /* LOGGER_LOGGER_H_ */
