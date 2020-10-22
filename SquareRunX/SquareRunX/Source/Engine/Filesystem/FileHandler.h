#pragma once
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>

struct FileFlags
{
	static constexpr uint8_t READ_MODE = 0x01;
	static constexpr uint8_t WRITE_MODE = 0x02;
	static constexpr uint8_t TRUNCATE_MODE = 0x10;
	static constexpr uint8_t BINARY_MODE = 0x20;
};

struct FileObject
{
	std::string Key, FilePath;
	std::fstream Stream;

	uint8_t OpenModeFlags = 0;
};

class FileHandler
{
	friend class EngineCore;
private:
	FileObject* FileSlots;
	size_t NumFileSlots;
protected:
	FileHandler();
	~FileHandler();

	FileHandler(const FileHandler& Other) = delete;
	FileHandler(FileHandler&& Other) = delete;

	void operator=(const FileHandler& Other) = delete;
	void operator=(FileHandler&& Other) = delete;

	void InitHandler();
	void DestroyHandler();
public:	
	//////////////////////////////////////////////// FILE-RELATED METHODS ////////////////////////////////////////////////

	void SetFileSlotsSize(size_t NumSlots);

	bool OpenFile(const std::string& Key, const char* RelativeFilePath, uint8_t OpenMode = 0x03);
	void CloseFile(const std::string& Key);
	void ClearFile(const std::string& Key);

	void WriteData(const std::string& Key, const std::string& Data, int Line = 0, bool InsertLine = false);
	std::stringstream ReadData(const std::string& Key, int Line = 0); 

	bool CopyExistingFile(const char* RelativeFilePath, const char* NewRelFilePath) const;
	bool RenameFile(const char* RelativeDir, const char* OldFileName, const char* NewFileName) const;
	bool DestroyFile(const char* RelativeFilePath) const;
		
	//////////////////////////////////////////////// DIRECTORY-RELATED METHODS ////////////////////////////////////////////////

	bool ChangeCurrentDirectory(const char* DirPath) const;

	bool CreateNewDirectory(const char* DirPath) const;
	bool CreateNewDirectories(const char* DirPath) const;
	bool CopyExistingDirectory(const char* CurrentDirPath, const char* NewDirPath) const;
	bool RenameDirectory(const char* OldDirPath, const char* NewDirName) const;
	uint32_t DestroyDirectory(const char* dirPath) const;

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	static FileHandler& GetSingleton();

	FileObject* GetFileIterator(const std::string& Key) const;
	FileObject* GetFileIterator(const std::function<bool(FileObject*)>& ComparatorFunc) const;

	std::string QueryCurrentDirectory() const;
	std::string GetRelative(const std::string& Path) const;
	std::string GetAbsolute(const std::string& Path) const;
	std::string TransformPathStr(const std::string& Path) const;

	std::vector<std::string> GetDirectoryContents(const std::string& RelativeDirPath, bool FileNameOnly = false) const;
	std::vector<std::string> GetDirectoryContentsRecursive(const std::string& RelativeDirPath, 
		bool FileNameOnly = false) const;

	size_t GetFileSize(const std::string& FilePath) const;
	size_t GetStorageCapacity() const;
	size_t GetStorageFreeSize() const;
	size_t GetStorageAvailableSize() const;

	bool PathExists(const std::string& Path) const;
	bool IsEmpty(const std::string& Path) const;
	int GetNumLinesInFile(const std::string& Key) const;
	int GetStringLineIndex(const std::string& Key, const std::string& String) const;
};
