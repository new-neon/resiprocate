#include "resiprocate/os/FileSystem.hxx"

using namespace resip;

FileSystem::Directory::Directory(const Data& path)
   : mPath(path)
{
}

#ifndef WIN32

FileSystem::Directory::iterator::iterator() :
   mNixDir(0),
   mDirent(0)
{}

FileSystem::Directory::iterator::iterator(const Directory& dir)
{
   mNixDir = opendir( dir.getPath().c_str() );
   mDirent = readdir(mNixDir);
   if (mDirent)
   {
      mFile = mDirent->d_name;
   }
}

FileSystem::Directory::iterator::~iterator()
{
   closedir(mNixDir);
}

FileSystem::Directory::iterator& 
FileSystem::Directory::iterator::operator++()
{
   if (mDirent)
   {
      mDirent = readdir(mNixDir);
      mFile = mDirent->d_name;
   }
   return *this;
}

bool 
FileSystem::Directory::iterator::operator!=(const iterator& rhs) const
{
   return !(*this == rhs);
}

bool 
FileSystem::Directory::iterator::operator==(const iterator& rhs) const
{
   if (mDirent && rhs.mDirent)
   {
      return **this == *rhs;
   }
   else
   {
      return mDirent == rhs.mDirent;
   }
}

const Data& 
FileSystem::Directory::iterator::operator*() const
{
   return mFile;
}

const Data*
FileSystem::Directory::iterator::operator->() const
{
   return &mFile;
}

#else

FileSystem::Directory::iterator::iterator() :
   mWinSearch(0)
   mDirent(0)
{}

FileSystem::Directory::iterator::iterator(const Directory& dir)
{
   Data searchPath;
   if (dir.getPath().postfix("/") || dir.getPath().postfix("\\"))
   {
      searchPath = dir.getPath() + Data("*");
   }
   else
   {
      searchPath = dir.getPath() + Data("/*");
   }
   WIN32_FIND_DATA fileData;
   mWinSearch = FindFirstFile( searchPath.c_str(), &fileData);
   
   if (mWinSearch == INVALID_HANDLE_VALUE)
   {
      mWinSearch = 0;
   }
   else
   {
      mFile = filedata.cFileName;
   }
}

FileSystem::Directory::iterator::~iterator()
{
   if (mWinSearch)
   {
      FindClose(mWinSearch);
   }
}

FileSystem::Directory::iterator 
FileSystem::Directory::iterator::operator++()
{
   WIN32_FIND_DATA fileData;

   if (!FindNextFile(mWinSearch, &fileData))
   {
      if (GetLastError() == ERROR_NO_MORE_FILES)
      {
         FindClose(mWinSearch);
         mWinSearch = 0;
      }
   }
   else
   {
      mFile = filedata.cFileName;
   }  
}

bool 
FileSystem::Directory::iterator::operator!=(const iterator& rhs)
{
   return !(*this == rhs);
}

bool 
FileSystem::Directory::iterator::operator==(const iterator& rhs)
{
   if (mWinSearch && rhs.mWinSearch)
   {
      return **this == *rhs;
   }
   else
   {
      return mWinSearch == rhs.mWinSearch;
   }
}

const Data& 
FileSystem::Directory::iterator::operator*()
{
   return mFile;
}

const Data* 
FileSystem::Directory::iterator::operator->()
{
   return &mFile;
}

#endif

FileSystem::Directory::iterator FileSystem::Directory::begin() const
{
   return iterator(*this);   
}

static FileSystem::Directory::iterator staticEnd;

FileSystem::Directory::iterator FileSystem::Directory::end() const
{
   return staticEnd;   
}
