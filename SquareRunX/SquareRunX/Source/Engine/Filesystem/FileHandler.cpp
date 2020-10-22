#include "FileHandler.h"
#include <filesystem>

namespace FSystem = std::filesystem;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FileHandler::FileHandler() :
	NumFileSlots(10), FileSlots(nullptr)
{}

FileHandler::~FileHandler() {}

void FileHandler::InitHandler()
{
	this->FileSlots = new FileObject[this->NumFileSlots];
}

void FileHandler::DestroyHandler()
{
	delete[] this->FileSlots;
	this->FileSlots = nullptr;
}

FileHandler& FileHandler::GetSingleton()
{
	static FileHandler Singleton;
	return Singleton;
}

FileObject* FileHandler::GetFileIterator(const std::string& Key) const
{
	for (size_t i = 0; i < this->NumFileSlots; i++) // Iterate through file object slots
	{
		if (this->FileSlots[i].Key == Key && this->FileSlots[i].Stream.is_open())
			return &this->FileSlots[i];
	}

	return nullptr;
}

FileObject* FileHandler::GetFileIterator(const std::function<bool(FileObject*)>& ComparatorFunc) const
{
	for (size_t i = 0; i < this->NumFileSlots; i++) // Iterate through file object slots
	{
		FileObject* File = &this->FileSlots[i];
		if (ComparatorFunc(File))
			return File;
	}

	return nullptr;
}

std::string FileHandler::TransformPathStr(const std::string& Path) const
{
	std::string TransformedPath = Path;
	std::replace(TransformedPath.begin(), TransformedPath.end(), '\\', '/');

	return TransformedPath;
}

void FileHandler::SetFileSlotsSize(size_t NumSlots)
{
	/* Allocate memory needed, then copy the memory of old slot array to new allocated slot array */
	FileObject* SlotsArray = new FileObject[NumSlots];
	this->NumFileSlots = std::min(this->NumFileSlots, NumSlots);
	memcpy(SlotsArray, this->FileSlots, this->NumFileSlots * sizeof(FileObject));

	delete[] this->FileSlots; // Delete old slot array before assigning the pointer to another memory location
	this->FileSlots = SlotsArray;
}

bool FileHandler::OpenFile(const std::string& Key, const char* RelativeFilePath, uint8_t OpenMode)
{
	// Make sure file is not already loaded or key is not already in use
	bool PreCheckStatus = !this->GetFileIterator([=](FileObject* File)
		{ return (File->Key == Key) || (File->FilePath.find(RelativeFilePath) != std::string::npos); });

	if (PreCheckStatus)
	{
		// Get the absolute path of the file to be opened
		FSystem::path AbsCurrentPath = FSystem::absolute(RelativeFilePath);
		std::string FilePath = this->TransformPathStr(AbsCurrentPath.string());

		// Now finally open the file
		for (size_t i = 0; i < this->NumFileSlots; i++)
		{
			auto& File = this->FileSlots[i];
			if (!File.Stream.is_open())
			{
				if ((int)(OpenMode & FileFlags::WRITE_MODE) == 0x02)
				{
					OpenMode |= 0x08;
				}

				File.Key = Key;
				File.FilePath = FilePath;
				File.Stream.open(FilePath, (int)OpenMode);
				File.OpenModeFlags = OpenMode;

				return !File.Stream.fail();
			}
		}
	}

	return false;
}

void FileHandler::CloseFile(const std::string& Key)
{
	FileObject* File = this->GetFileIterator(Key);
	if (File != nullptr)
	{
		File->Key = "";
		File->FilePath = "";
		File->OpenModeFlags = 0;

		File->Stream.close();
	}
}

void FileHandler::ClearFile(const std::string& Key)
{
	FileObject* File = this->GetFileIterator(Key);
	if (File != nullptr)
	{
		// Clear the file
		File->Stream.close();
		File->Stream.open(File->FilePath, std::ios::in | std::ios::out | std::ios::trunc);
		File->Stream.close();

		// Reopen the file with the original flags
		File->Stream.open(File->FilePath, File->OpenModeFlags);
	}
}

void FileHandler::WriteData(const std::string& Key, const std::string& Data, int Line, bool InsertLine)
{
	FileObject* File = this->GetFileIterator(Key);
	if (File != nullptr)
	{
		// Make sure file was opened in WRITE MODE before trying to write to it
		if ((int)(File->OpenModeFlags & FileFlags::WRITE_MODE) == 2)
		{
			File->Stream.clear();
			File->Stream.seekg(0, std::ios::beg);

			if ((int)(File->OpenModeFlags & FileFlags::READ_MODE) == 1 && Line > 0)
			{
				// Get the file contents from file
				std::vector<std::string> FileContents;
				std::string LineStr;

				while (std::getline(File->Stream, LineStr))
				{
					FileContents.emplace_back(LineStr);
				}

				// Modify the file line loaded
				if (!FileContents.empty())
				{
					if (Line > static_cast<int>(FileContents.size())) // Prevent out-of-range error
					{
						Line = static_cast<int>(FileContents.size());
					}

					Line -= 1; // Vector indexing starts from zero
					if (InsertLine)
					{
						// Insert the data at the line index given
						auto Iterator = FileContents.begin() + Line;
						FileContents.insert(Iterator, Data);
					}
					else
						FileContents[Line] = Data; // Overwrite the existing line at the index
				}
				else
				{
					FileContents.emplace_back(Data);
				}

				// Write the new modified data to the file
				File->Stream.close();
				File->Stream.open(File->FilePath, (std::ios::in | std::ios::out | std::ios::trunc));

				for (const auto& NewLine : FileContents)
				{
					File->Stream << NewLine << std::endl;
				}

				File->Stream.close();
				File->Stream.open(File->FilePath, File->OpenModeFlags);
			}
			else
			{
				File->Stream << Data << std::endl;
			}

			File->Stream.flush();
		}
	}
}

std::stringstream FileHandler::ReadData(const std::string& Key, int Line)
{
	FileObject* File = this->GetFileIterator(Key);
	std::stringstream DataStream;

	if (File != nullptr)
	{
		// Make sure file was opened in READ MODE before trying to read to it
		if ((int)(File->OpenModeFlags & FileFlags::READ_MODE) == 1)
		{
			File->Stream.clear();
			File->Stream.seekg(0, std::ios_base::beg);

			if (Line < 1)
			{
				DataStream << File->Stream.rdbuf();
			}
			else
			{
				std::string FileLine;
				int LineCounter = 1;

				while (std::getline(File->Stream, FileLine)) // Find the correct line in file
				{
					if (LineCounter == Line)
					{
						DataStream << FileLine;
						break;
					}
						
					LineCounter++;
				}
			}
		}
	}

	return DataStream;
}

bool FileHandler::CopyExistingFile(const char* RelativeFilePath, const char* NewRelFilePath) const
{
	// Get the absolulte path
	FSystem::path CurrentFilePath = FSystem::absolute(RelativeFilePath);
	FSystem::path NewFilePath = FSystem::absolute(NewRelFilePath);

	return FSystem::copy_file(CurrentFilePath, NewFilePath);
}

bool FileHandler::RenameFile(const char* RelativeDir, const char* OldFileName, const char* NewFileName) const
{
	// Get the absolute path of the file
	FSystem::path AbsFilePath = FSystem::absolute(RelativeDir);
	std::string OldFilePath = this->TransformPathStr(AbsFilePath.string()) + "/" + OldFileName;
	std::string NewFilePath = this->TransformPathStr(AbsFilePath.string()) + "/" + NewFileName;

	// Check if the file to be deleted is currently open
	for (size_t i = 0; i < this->NumFileSlots; i++)
	{
		auto& File = this->FileSlots[i];
		if (File.FilePath == OldFilePath && File.Stream.is_open())
		{
			File.Stream.close();
			break;
		}
	}

	// Finally delete the file
	return !rename(OldFilePath.c_str(), NewFilePath.c_str());
}

bool FileHandler::DestroyFile(const char* RelativeFilePath) const
{
	// Get the absolute path of the file
	FSystem::path AbsFilePath = FSystem::absolute(RelativeFilePath);
	std::string FilePath = this->TransformPathStr(AbsFilePath.string());

	// Check if the file to be deleted is currently open
	for (size_t i = 0; i < this->NumFileSlots; i++)
	{
		auto& File = this->FileSlots[i];
		if (File.FilePath == FilePath && File.Stream.is_open())
		{
			File.Stream.close();
			break;
		}
	}

	// Finally delete the file
	return !remove(RelativeFilePath);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool FileHandler::ChangeCurrentDirectory(const char* DirPath) const
{
	if (!this->PathExists(DirPath))
	{
		return false;
	}

	FSystem::current_path(DirPath);
	return true;
}

bool FileHandler::CreateNewDirectory(const char* DirPath) const
{
	return FSystem::create_directory(DirPath);
}

bool FileHandler::CreateNewDirectories(const char* DirPath) const
{
	return FSystem::create_directories(DirPath);
}

bool FileHandler::CopyExistingDirectory(const char* CurrentDirPath, const char* NewDirPath) const
{
	if (!this->PathExists(CurrentDirPath))
	{
		return false;
	}

	FSystem::copy(CurrentDirPath, NewDirPath, FSystem::copy_options::recursive);
	return true;
}

bool FileHandler::RenameDirectory(const char* OldDirPath, const char* NewDirName) const
{
	if (!this->PathExists(OldDirPath))
	{
		return false;
	}

	std::string ParentPath = OldDirPath;
	if (ParentPath.find("/") != std::string::npos)
	{
		ParentPath = ParentPath.substr(0, ParentPath.find_last_of("/") + 1);
		FSystem::rename(OldDirPath, ParentPath + NewDirName);
	}
	else
	{
		FSystem::rename(OldDirPath, NewDirName);
	}

	return true;
}

uint32_t FileHandler::DestroyDirectory(const char* DirPath) const
{
	return (uint32_t)FSystem::remove_all(DirPath);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string FileHandler::QueryCurrentDirectory() const
{
	return FSystem::current_path().string();
}

std::string FileHandler::GetRelative(const std::string& Path) const
{
	return FSystem::relative(Path).string();
}

std::string FileHandler::GetAbsolute(const std::string& Path) const
{
	return FSystem::absolute(Path).string();
}

std::vector<std::string> FileHandler::GetDirectoryContents(const std::string& RelativeDirPath, bool FileNameOnly) const
{
	std::vector<std::string> DirContents;

	// Iterate through the contents in the directory
	for (auto& Node : FSystem::directory_iterator(RelativeDirPath))
	{
		std::string ContentPath;
		if (FileNameOnly)
		{
			ContentPath = Node.path().filename().string();
		}
		else
		{
			ContentPath = Node.path().string();
		}

		// Push found file in directory to vector array
		ContentPath = this->TransformPathStr(ContentPath);
		DirContents.emplace_back(ContentPath);
	}

	return DirContents;
}

std::vector<std::string> FileHandler::GetDirectoryContentsRecursive(const std::string& RelativeDirPath, bool FileNameOnly) const
{
	std::vector<std::string> DirContents;

	// Iterate through contents in the directory and it's sub-directories
	for (auto& Node : FSystem::recursive_directory_iterator(RelativeDirPath))
	{
		std::string ContentPath;
		if (FileNameOnly)
		{
			ContentPath = Node.path().filename().string();
		}
		else
		{
			ContentPath = Node.path().string();
		}

		// Push found file in directory to vector array
		ContentPath = this->TransformPathStr(ContentPath);
		DirContents.emplace_back(ContentPath);
	}

	return DirContents;
}

size_t FileHandler::GetFileSize(const std::string& FilePath) const
{
	return (size_t)FSystem::file_size(FilePath);
}

size_t FileHandler::GetStorageCapacity() const
{
	return (size_t)FSystem::space(FSystem::current_path().root_path()).capacity;
}

size_t FileHandler::GetStorageFreeSize() const
{
	return (size_t)FSystem::space(FSystem::current_path().root_path()).free;
}

size_t FileHandler::GetStorageAvailableSize() const
{
	return (size_t)FSystem::space(FSystem::current_path().root_path()).available;
}

bool FileHandler::PathExists(const std::string& Path) const
{
	return FSystem::exists(Path);
}

bool FileHandler::IsEmpty(const std::string& Path) const
{
	return FSystem::is_empty(Path);
}

int FileHandler::GetNumLinesInFile(const std::string& Key) const
{
	FileObject* File = this->GetFileIterator(Key);
	int NumLines = 0;

	if (File != nullptr)
	{
		// Count the amount of lines in the file
		File->Stream.clear();
		File->Stream.seekg(0, std::ios::beg);

		std::string TempStr;
		while (std::getline(File->Stream, TempStr))
		{
			NumLines++;
		}
	}
	else
	{
		NumLines = -1;
	}

	return NumLines;
}

int FileHandler::GetStringLineIndex(const std::string& Key, const std::string& String) const
{
	FileObject* File = this->GetFileIterator(Key);
	int LineCount = 1;
	bool Found = false;

	if (File != nullptr)
	{
		File->Stream.clear();
		File->Stream.seekg(0, std::ios::beg);

		std::string FileLine;
		while (std::getline(File->Stream, FileLine))
		{
			if (FileLine.find(String) != std::string::npos)
			{
				Found = true;
				break;
			}

			LineCount++;
		}

		if (!Found)
			LineCount = -1;
	}
	else
	{
		LineCount = -2;
	}

	return LineCount;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////