#include <stdexcept>

class CpioException : public std::runtime_error {
	public:
		enum ErrorType 
		{
			InvalidInputArchive,
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