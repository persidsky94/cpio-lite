#include <stdexcept>

class CpioException : public std::runtime_exception {
	public:
		enum ErrorType 
		{
			InvalidInputArchive,
		}

		CpioException(ErrorType errType, const string& description) 
			: std::runtime_exception(description) 
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

class PosixException : public std::runtime_exception {
	
	using errType = int;
	
	public:
		PosixException(errType errorCode, const string& description) 
			: std::runtime_exception(description) 
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