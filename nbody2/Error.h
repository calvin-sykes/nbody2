#ifndef ERROR_H
#define ERROR_H

#define MAKE_ERROR(message) Error((message), __FILE__, __func__, __LINE__)

#include <exception>
#include <sstream>
#include <string>

namespace nbody
{
	class Error : public std::runtime_error
	{
	public:
		Error() = default;

		Error(const std::string& msgIn, const std::string& fileIn, const std::string& funcIn, const int lineIn) :
			std::runtime_error(msgIn), m_file(fileIn), m_func(funcIn), m_line(lineIn) {};

		virtual const char* what() const noexcept
		{
			std::ostringstream builder;
			builder << "ERROR: " << std::runtime_error::what() << "\nFile: " << m_file << " at: " << m_func << ":" << m_line;
			auto p_str = new std::string(builder.str());
			return p_str->c_str();
		}

	private:
		const std::string m_file, m_func;
		const int m_line;
	};
}

#endif // ERROR_H
