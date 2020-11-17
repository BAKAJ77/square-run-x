#pragma once
#include <string>

enum class LogLevel
{
	INFO,
	WARNING,
	FATAL
};

class LogManager
{
	friend class EngineCore;
private:
	LogManager();
	~LogManager();

	void InitManager();

	void OutputConsoleLog(const std::string& Message, LogLevel Severity) const;
	void OutputFileLog(const std::string& Message, LogLevel Severity) const;
public:
	void OutputLog(const std::string& Message, LogLevel Severity) const;
public:
	static LogManager& GetSingleton();
};