#include "CFGConfigLoader.h"
#include "FileHandler.h"

CFGConfigLoader::CFGConfigLoader() {}

CFGConfigLoader::~CFGConfigLoader() {}

CFGConfigLoader& CFGConfigLoader::GetSingleton()
{
	static CFGConfigLoader Singleton;
	return Singleton;
}

void CFGConfigLoader::OpenFile(const std::string& FileName)
{
	// Append the .cfg extension if it's not included in file name
	if(FileName.find(".cfg") == std::string::npos)
		FileHandler::GetSingleton().OpenFile("CONFIG_FILE", ("GameFiles/Configs/" + FileName + ".cfg").c_str());
	else
		FileHandler::GetSingleton().OpenFile("CONFIG_FILE", ("GameFiles/Configs/" + FileName).c_str());

	// Parse the entire configuration file
	const int FILE_LENGTH = FileHandler::GetSingleton().GetNumLinesInFile("CONFIG_FILE");
	if (FILE_LENGTH != -1)
	{
		for (int i = 0; i < FILE_LENGTH; i++)
		{
			const std::string DATA = FileHandler::GetSingleton().ReadData("CONFIG_FILE", i + 1).str();
			const size_t ASSIGNMENT_TOKEN = DATA.find_first_of('=', 3);
			CFGElementData ExtractedData;

			// Get the element name from the loaded string
			if (ASSIGNMENT_TOKEN != std::string::npos)
			{
				if (DATA[ASSIGNMENT_TOKEN - 1] == ' ' && DATA[ASSIGNMENT_TOKEN + 1] == ' ')
				{
					ExtractedData.Name = DATA.substr(3, ASSIGNMENT_TOKEN - 4);
				}
				else
				{
					// Throw exception if assignment operator isn't used correctly e.g. not formatted correctly.
					throw std::runtime_error("RUNTIME EXCEPTION: Invalid assignment operator use found at line [" + 
						std::to_string(i) + "], char index[" + std::to_string(ASSIGNMENT_TOKEN) + "]");
				}

				// Get the data type from the prefix in the loaded string
				ExtractedData.Type = this->IdentifyDataPrefix(DATA.substr(0, 2));

				// Get the value of the config element in the loaded string
				ExtractedData.Value = DATA.substr(ASSIGNMENT_TOKEN + 2, std::string::npos);

				// Fetch the comment directly above the config element (if any)
				if (i > 0)
				{
					const std::string PREVIOUS_LINE = FileHandler::GetSingleton().ReadData("CONFIG_FILE", i).str();
					if (PREVIOUS_LINE.front() == '#')
						ExtractedData.Comment = PREVIOUS_LINE.substr(2, std::string::npos);
				}

				// Then finally push the extracted element to the vector array
				this->ConfigElements.emplace_back(ExtractedData);
			}
		}
	}
}

void CFGConfigLoader::CloseFile()
{
	FileHandler::GetSingleton().CloseFile("CONFIG_FILE");
	this->ConfigElements.clear();
}

DataType CFGConfigLoader::IdentifyDataPrefix(const std::string& Prefix) const
{
	if (Prefix == "I:")
		return DataType::INTEGER;
	else if (Prefix == "B:")
		return DataType::BOOLEAN;
	else if (Prefix == "F:")
		return DataType::FLOAT;
	else if (Prefix == "D:")
		return DataType::DOUBLE;
	else if (Prefix == "S:")
		return DataType::STRING;

	return DataType::UNDEFINED;
}

std::string CFGConfigLoader::QueryDataPrefixStr(DataType Type) const
{
	switch (Type)
	{
	case DataType::INTEGER: 
		return "I: "; 
		break;
	case DataType::BOOLEAN:
		return "B: ";
		break;
	case DataType::FLOAT:
		return "F: ";
		break;
	case DataType::DOUBLE:
		return "D: ";
		break;
	case DataType::STRING:
		return "S: ";
		break;
	default:
		throw std::runtime_error("RUNTIME EXCEPTION: The data type inputted is undefined.");
		break;
	}

	return "UNDEFINED";
}

void CFGConfigLoader::WriteElement(const CFGElementData& Data)
{
	// Overwrite the element's value if it already exists in config element vector array
	bool WriteComment = !Data.Comment.empty();
	bool ElementExists = false;

	for (size_t i = 0; i < this->ConfigElements.size(); i++)
	{
		if (this->ConfigElements[i] == Data)
		{
			this->ConfigElements[i] = Data;
			ElementExists = true;
			break;
		}
	}

	// Update the data in the config file
	const std::string GENERATED_ELEMENT_DATA = this->QueryDataPrefixStr(Data.Type) + Data.Name + " = " + Data.Value;
	const std::string GENERATED_COMMENT = "# " + Data.Comment;

	if (ElementExists) // The config element already exists
	{
		int LineIndex = FileHandler::GetSingleton().GetStringLineIndex("CONFIG_FILE", Data.Name);
		if (LineIndex > 0) // Just a precaution check to make sure element exists in file
		{
			FileHandler::GetSingleton().WriteData("CONFIG_FILE", GENERATED_ELEMENT_DATA, LineIndex);
		}
	}
	else // The config element doesn't exist
	{
		if (WriteComment)
			FileHandler::GetSingleton().WriteData("CONFIG_FILE", GENERATED_COMMENT);

		FileHandler::GetSingleton().WriteData("CONFIG_FILE", GENERATED_ELEMENT_DATA);
	}
}

const CFGElementData* CFGConfigLoader::QueryElement(const std::string& Name)
{
	for (const auto& Element : this->ConfigElements)
	{
		if (Element.Name == Name)
			return &Element;
	}

	return nullptr;
}