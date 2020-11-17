#include "LoggingManager.h"
#include "Engine/Filesystem/FileHandler.h"

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <chrono>

LogManager::LogManager() {}

LogManager::~LogManager() {}

void LogManager::InitManager()
{
	// Create the logging directory and generate the runtime log file
#ifndef _DEBUG
	if (FileHandler::GetSingleton().PathExists("GameFiles") && !FileHandler::GetSingleton().PathExists("GameFiles/Logs"))
	{
		if (!FileHandler::GetSingleton().CreateNewDirectory("GameFiles/Logs"))
		{
			throw std::runtime_error("RuntimeException: An error occured while creating logging directory.");
		}
	}

	if (!FileHandler::GetSingleton().OpenFile("RUNTIME_LOG", "GameFiles/Logs/RuntimeLog.txt", 
		FileFlags::READ_MODE | FileFlags::WRITE_MODE | FileFlags::TRUNCATE_MODE))
	{
		throw std::runtime_error("RuntimeException: An error occured while opening the runtime log file.");
	}
#endif
}

void LogManager::OutputConsoleLog(const std::string& Message, LogLevel Severity) const
{
	// Change the color of the log text based on the severity
	HANDLE Console = GetStdHandle(STD_OUTPUT_HANDLE);
	switch (Severity)
	{
	case LogLevel::INFO:
		SetConsoleTextAttribute(Console, 15); // WHITE COLOR
		std::cout << "INFO: " << Message << std::endl;
		break;
	case LogLevel::WARNING:
		SetConsoleTextAttribute(Console, 14); // YELLOW COLOR
		std::cout << "WARNING: " << Message << std::endl;
		break;
	case LogLevel::FATAL:
		SetConsoleTextAttribute(Console, 12); // RED COLOR
		
		std::cout << "ERROR: " << Message << std::endl;
		std::cin.get();
		std::exit(-1);
		break;
	}
}

void LogManager::OutputFileLog(const std::string& Message, LogLevel Severity) const
{
	// Retrieve the current local time
	time_t CurrentTime;
	struct tm LocalTime;
	time(&CurrentTime);
	localtime_s(&LocalTime, &CurrentTime);

	// Construct strings containing time data
	std::string HoursStr, MinutesStr, SecondsStr;
	LocalTime.tm_hour < 10 ? HoursStr = "0" + std::to_string(LocalTime.tm_hour) : HoursStr = std::to_string(LocalTime.tm_hour);
	LocalTime.tm_min < 10 ? MinutesStr = "0" + std::to_string(LocalTime.tm_min) : MinutesStr = std::to_string(LocalTime.tm_min);
	LocalTime.tm_sec < 10 ? SecondsStr = "0" + std::to_string(LocalTime.tm_sec) : SecondsStr = std::to_string(LocalTime.tm_sec);

	std::stringstream LogDataStream;
	switch (Severity)
	{
	case LogLevel::INFO:
		LogDataStream << "[" << LocalTime.tm_mday << "/" << LocalTime.tm_mon + 1 << "/" << LocalTime.tm_year + 1900 << " " <<
			HoursStr << ":" << MinutesStr << ":" << SecondsStr << "] INFO: " << Message << std::endl;
		break;
	case LogLevel::WARNING:
		LogDataStream << "[" << LocalTime.tm_mday << "/" << LocalTime.tm_mon + 1 << "/" << LocalTime.tm_year + 1900 << " " <<
			HoursStr << ":" << MinutesStr << ":" << SecondsStr << "] WARNING: " << Message << std::endl;
		break;
	case LogLevel::FATAL:
		LogDataStream << "[" << LocalTime.tm_mday << "/" << LocalTime.tm_mon + 1 << "/" << LocalTime.tm_year + 1900 << " " <<
			HoursStr << ":" << MinutesStr << ":" << SecondsStr << "] ERROR: " << Message << std::endl;
		break;
	}

	FileHandler::GetSingleton().WriteData("RUNTIME_LOG", LogDataStream.str());
	if (Severity == LogLevel::FATAL)
	{
		std::exit(-1);
	}
}

void LogManager::OutputLog(const std::string& Message, LogLevel Severity) const
{
#ifdef _DEBUG
	this->OutputConsoleLog(Message, Severity);
#else
	this->OutputFileLog(Message, Severity);
#endif
}

LogManager& LogManager::GetSingleton()
{
	static LogManager Singleton;
	return Singleton;
}