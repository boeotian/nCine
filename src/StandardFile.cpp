#include <cstdlib> // for exit()
#if !(defined(_WIN32) && !defined(__MINGW32__))
	// All but MSVC: Linux, Android and MinGW.
	#include <sys/stat.h> // for open()
	#include <fcntl.h> // for open()
	#include <unistd.h> // for close()
#else
	#include <io.h> // for _access()
#endif
#include "StandardFile.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

StandardFile::~StandardFile()
{
	if (shouldCloseOnExit_)
	{
		close();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void StandardFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0 || filePointer_ != NULL)
	{
		LOGW_X("File \"%s\" is already opened", filename_.data());
	}
	else
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		// Opening with a file descriptor
		if (mode & MODE_FD)
		{
			openFD(mode);
		}
		// Opening with a file stream
		else
#endif
			openStream(mode);
	}
}

/// Closes the file (both opened or fopened)
void StandardFile::close()
{
	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		int retValue = ::close(fileDescriptor_);
		if (retValue < 0)
		{
			LOGW_X("Cannot close the file \"%s\"", filename_.data());
		}
		else
		{
			LOGI_X("File \"%s\" closed", filename_.data());
			fileDescriptor_ = -1;
		}
#endif
	}
	else if (filePointer_)
	{
		int retValue = fclose(filePointer_);
		if (retValue == EOF)
		{
			LOGW_X("Cannot close the file \"%s\"", filename_.data());
		}
		else
		{
			LOGI_X("File \"%s\" closed", filename_.data());
			filePointer_ = NULL;
		}
	}
}

long int StandardFile::seek(long int offset, int whence) const
{
	long int seekValue = -1;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		seekValue = lseek(fileDescriptor_, offset, whence);
#endif
	}
	else if (filePointer_)
	{
		seekValue = fseek(filePointer_, offset, whence);
	}

	return seekValue;
}

long int StandardFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		tellValue = lseek(fileDescriptor_, 0L, SEEK_CUR);
#endif
	}
	else if (filePointer_)
	{
		tellValue = ftell(filePointer_);
	}

	return tellValue;
}


unsigned long int StandardFile::read(void *buffer, unsigned long int bytes) const
{
	unsigned long int bytesRead = 0;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		bytesRead = ::read(fileDescriptor_, buffer, bytes);
#endif
	}
	else if (filePointer_)
	{
		bytesRead = static_cast<unsigned long int>(fread(buffer, 1, bytes, filePointer_));
	}

	return bytesRead;
}


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with `open()`
void StandardFile::openFD(unsigned char mode)
{
#if !(defined(_WIN32) && !defined(__MINGW32__))
	int openFlag = -1;

	switch (mode)
	{
		case (MODE_FD|MODE_READ):
			openFlag = O_RDONLY;
			break;
		case (MODE_FD|MODE_WRITE):
			openFlag = O_WRONLY;
			break;
		case (MODE_FD|MODE_READ|MODE_WRITE):
			openFlag = O_RDWR;
			break;
		default:
			LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
			break;
	}

	if (openFlag >= 0)
	{
		fileDescriptor_ = ::open(filename_.data(), openFlag);

		if (fileDescriptor_ < 0)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_.data());
			exit(EXIT_FAILURE);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_.data());
		}

		// Calculating file size
		fileSize_ = lseek(fileDescriptor_, 0L, SEEK_END);
		lseek(fileDescriptor_, 0L, SEEK_SET);
	}
#endif
}

/// Opens the file with `fopen()`
void StandardFile::openStream(unsigned char mode)
{
	char modeChars[3] = {'\0', '\0', '\0'};

	switch (mode)
	{
		case (MODE_READ):
			modeChars[0] = 'r';
			break;
		case (MODE_WRITE):
			modeChars[0] = 'w';
			break;
		case (MODE_READ|MODE_WRITE):
			modeChars[0] = 'r';
			modeChars[1] = '+';
			break;
		case (MODE_READ|MODE_BINARY):
			modeChars[0] = 'r';
			modeChars[1] = 'b';
			break;
		case (MODE_WRITE|MODE_BINARY):
			modeChars[0] = 'w';
			modeChars[1] = 'b';
			break;
		case (MODE_READ|MODE_WRITE|MODE_BINARY):
			modeChars[0] = 'r';
			modeChars[1] = '+';
			modeChars[2] = 'b';
			break;
		default:
			LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
			break;
	}

	if (modeChars[0] != '\0')
	{
		filePointer_ = fopen(filename_.data(), modeChars);

		if (filePointer_ == NULL)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_.data());
			exit(EXIT_FAILURE);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_.data());
		}

		// Calculating file size
		fseek(filePointer_, 0L, SEEK_END);
		fileSize_ = ftell(filePointer_);
		fseek(filePointer_, 0L, SEEK_SET);
	}
}

/// Checks if a file can be accessed with specified mode
/*! It is called by `IFile::access()` */
bool StandardFile::access(const char *filename, unsigned char mode)
{
	bool isAccessible = false;
	int accessMode = -1;

#if !(defined(_WIN32) && !defined(__MINGW32__))
	switch (mode)
	{
		case (IFile::MODE_EXISTS):
			accessMode = F_OK;
			break;
		case (IFile::MODE_CAN_READ):
			accessMode = R_OK;
			break;
		case (IFile::MODE_CAN_WRITE):
			accessMode = W_OK;
			break;
		case (IFile::MODE_CAN_READ|IFile::MODE_CAN_WRITE):
			accessMode = R_OK | W_OK;
			break;
		default:
			LOGE_X("Cannot access the file \"%s\", wrong access mode", filename);
			break;
	}

	if (accessMode != -1)
	{
		isAccessible = (::access(filename, accessMode) == 0);
	}
#else
	switch (mode)
	{
		case (IFile::MODE_EXISTS):
			accessMode = 0;
			break;
		case (IFile::MODE_CAN_READ):
			accessMode = 2;
			break;
		case (IFile::MODE_CAN_WRITE):
			accessMode = 4;
			break;
		case (IFile::MODE_CAN_READ | IFile::MODE_CAN_WRITE):
			accessMode = 6;
			break;
		default:
			LOGE_X("Cannot access the file \"%s\", wrong access mode", filename);
			break;
	}

	if (accessMode != -1)
	{
		isAccessible = (::_access(filename, accessMode) == 0);
	}
#endif

	return isAccessible;
}

}
