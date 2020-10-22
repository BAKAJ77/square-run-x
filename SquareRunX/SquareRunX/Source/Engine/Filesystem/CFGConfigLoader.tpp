#include "CFGConfigLoader.h"

namespace CFGElement
{
	///////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T> 
	static CFGElementData ToElementData(const std::string Name, const T& Data) { static_assert(false); }

	template<> static CFGElementData ToElementData<int>(const std::string Name, const int& Data)
	{
		return { Name, DataType::INTEGER, std::to_string(Data) };
	}

	template<> static CFGElementData ToElementData<bool>(const std::string Name, const bool& Data)
	{
		return { Name, DataType::BOOLEAN, Data ? "true" : "false" };
	}

	template<> static CFGElementData ToElementData<float>(const std::string Name, const float& Data)
	{
		return { Name, DataType::FLOAT, std::to_string(Data) };
	}

	template<> static CFGElementData ToElementData<double>(const std::string Name, const double& Data)
	{
		return { Name, DataType::DOUBLE, std::to_string(Data) };
	}

	template<> static CFGElementData ToElementData<std::string>(const std::string Name, const std::string& Data)
	{
		return { Name, DataType::STRING, "\"" + Data + "\"" };
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	template<typename T> 
	T ToDataValue(const CFGElementData& Data) { static_assert(false); }

	template<> 
	int ToDataValue<int>(const CFGElementData& Data)
	{
		// If the data type doesn't match, throw an exception
		if (Data.Type != DataType::INTEGER)
		{
			throw std::runtime_error("RUNTIME EXCEPTION: The data type of the config element \"" + Data.Name +
				"\" is not a integer.");
		}

		return std::stoi(Data.Value);
	}

	template<>
	bool ToDataValue<bool>(const CFGElementData& Data)
	{
		// If the data type doesn't match, throw an exception
		if (Data.Type != DataType::BOOLEAN)
		{
			throw std::runtime_error("RUNTIME EXCEPTION: The data type of the config element \"" + Data.Name +
				"\" is not a boolean.");
		}

		if (Data.Value == "true")
			return true;
		return false;
	}

	template<>
	float ToDataValue<float>(const CFGElementData& Data)
	{
		// If the data type doesn't match, throw an exception
		if (Data.Type != DataType::FLOAT)
		{
			throw std::runtime_error("RUNTIME EXCEPTION: The data type of the config element \"" + Data.Name +
				"\" is not a float.");
		}

		return std::stof(Data.Value);
	}

	template<>
	double ToDataValue<double>(const CFGElementData& Data)
	{
		// If the data type doesn't match, throw an exception
		if (Data.Type != DataType::DOUBLE)
		{
			throw std::runtime_error("RUNTIME EXCEPTION: The data type of the config element \"" + Data.Name +
				"\" is not a double.");
		}

		return std::stod(Data.Value);
	}

	template<>
	std::string ToDataValue<std::string>(const CFGElementData& Data)
	{
		// If the data type doesn't match, throw an exception
		if (Data.Type != DataType::STRING)
		{
			throw std::runtime_error("RUNTIME EXCEPTION: The data type of the config element \"" + Data.Name +
				"\" is not a string.");
		}

		return Data.Value.substr(1, Data.Value.size() - 2); // This is to remove the quotation marks
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////
}