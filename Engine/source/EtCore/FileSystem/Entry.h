#pragma once

#include <string>
#include <vector>

#include "./Facade/FileHandle.h"
#include "./Facade/FileAccessFlags.h"
#include "./Facade/FileAccessMode.h"

class Directory;

//---------------------------------
// Entry
//
// Content of any kind that lives in a directory
//
class Entry
{
public:
	Entry(std::string name, Directory* pParent);
    virtual ~Entry();

    std::string GetName();
	std::string GetNameOnly();
	std::string GetExtension();
    Directory* GetParent(){ return m_Parent; }
    virtual std::string GetPath();

	virtual bool Delete() = 0;

    enum EntryType
    {
        ENTRY_FILE,
        ENTRY_DIRECTORY,
        ENTRY_UNDEF
    };
    virtual EntryType GetType()
    {
        return Entry::EntryType::ENTRY_UNDEF; 
    }
protected:
	std::string m_Filename;
    Directory* m_Parent;
	std::string m_Path; //only used if there is no parent
};

//---------------------------------
// File
//
// Entry that contains data
//
class File : public Entry
{
public:
	File(std::string name, Directory* pParent);
	virtual ~File();

	bool Open(FILE_ACCESS_MODE mode, 
		FILE_ACCESS_FLAGS flags = FILE_ACCESS_FLAGS());
	void Close();

	std::vector<uint8> Read();
	std::vector<uint8> ReadChunk(uint64 const offset, uint64 const numBytes);
	bool Write(const std::vector<uint8> &lhs);
	Entry::EntryType GetType()
    	{
            return Entry::EntryType::ENTRY_FILE;
        }

	bool IsOpen(){ return m_IsOpen; }

	uint64 GetSize();

	bool Delete() override;

private:
	bool m_IsOpen;

	FILE_HANDLE m_Handle;
};

//---------------------------------
// Directory
//
// Entry that contains more entries
//
class Directory : public Entry
{
public:
    Directory(std::string name, Directory* pParent, bool ensureExists = false);
    virtual ~Directory();
    //Inherited Methods
    Entry::EntryType GetType()
    {
        return Entry::EntryType::ENTRY_DIRECTORY;
    }
    
    bool Mount(bool recursive = false);
    void Unmount();
    std::vector<Entry*> GetChildren() { return m_pChildren; }
	std::vector<Entry*> GetChildrenByExt(std::string ext);
	void GetChildrenRecursive(std::vector<File*>& children);

	bool Exists();
	bool Create();

    bool IsMounted(){ return m_IsMounted; }

	bool Delete() override;

	void RemoveChild( Entry* child );

private:
	friend class File;

	bool DeleteDir();
    void RecursiveMount();

    std::vector<Entry*> m_pChildren;
    bool m_IsMounted = false;
};