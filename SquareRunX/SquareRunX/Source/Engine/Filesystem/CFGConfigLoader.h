#pragma once
#include <sstream>
#include <vector>

typedef std::stringstream DataStream;

enum class DataType
{
	INTEGER,
	BOOLEAN,
	FLOAT,
	DOUBLE,
	STRING,
	UNDEFINED
};

struct CFGElementData
{
	std::string Name;
	DataType Type = DataType::UNDEFINED;
	std::string Value;

	std::string Comment;

	bool operator==(const CFGElementData& Other) { return this->Name == Other.Name; }
};

class CFGConfigLoader
{
	friend class EngineCore;
private:
	std::vector<CFGElementData> ConfigElements;
private:
	CFGConfigLoader();
	~CFGConfigLoader();

	DataType IdentifyDataPrefix(const std::string& Prefix) const;
	std::string QueryDataPrefixStr(DataType Type) const;
public:
	void OpenFile(const std::string& FileName);
	void CloseFile();

	void WriteElement(const CFGElementData& Data);
	const CFGElementData* QueryElement(const std::string& Name);
public:
	static CFGConfigLoader& GetSingleton();
};

namespace CFGElement
{
	template<typename T> static CFGElementData ToElementData(const std::string Name, const T& Data);
	template<> static CFGElementData ToElementData<int>(const std::string Name, const int& Data);
	template<> static CFGElementData ToElementData<bool>(const std::string Name, const bool& Data);
	template<> static CFGElementData ToElementData<float>(const std::string Name, const float& Data);
	template<> static CFGElementData ToElementData<std::string>(const std::string Name, const std::string& Data);

	template<typename T> static T ToDataValue(const CFGElementData& Data);
	template<> static int ToDataValue<int>(const CFGElementData& Data);
	template<> static bool ToDataValue<bool>(const CFGElementData& Data);
	template<> static float ToDataValue<float>(const CFGElementData& Data);
	template<> static double ToDataValue<double>(const CFGElementData& Data);
	template<> static std::string ToDataValue<std::string>(const CFGElementData& Data);
}

#include "CFGConfigLoader.tpp"