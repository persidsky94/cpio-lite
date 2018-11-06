#ifndef CUSTOMEXCEPTIONS_H
#define CUSTOMEXCEPTIONS_H

#include <stdexcept>

using namespace std;

class CpioException : public std::runtime_error {
	public:
		enum ErrorType 
		{
			InvalidInputArchive,
			InvalidInputFileToArchive,
		};

		CpioException(ErrorType errType, const string& description) 
			: std::runtime_error(description) 
		{
			m_errType = errType;
		}

		ErrorType errorType() const 
		{
			return m_errType;
		}

	protected:
		ErrorType m_errType;
};

class PosixException : public std::runtime_error {
	
	using errType = int;
	
	public:
		PosixException(errType errorCode, const string& description) 
			: std::runtime_error(description) 
		{
			m_errorCode = errorCode;
		}

		errType errorCode() const 
		{
			return m_errorCode;
		}

	protected:
		errType m_errorCode;
};

#endif //CUSTOMEXCEPTIONS_H