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
			std::runtime_error(msgIn), file(fileIn), func(funcIn), line(lineIn) {};

		virtual const char* what() const
		{
			std::ostringstream builder;
			builder << "ERROR: " << std::runtime_error::what() << "\nFile: " << file << " at: " << func << ":" << line;
			auto p_str = new std::string(builder.str());
			return p_str->c_str();
		}

	private:
		const std::string file, func;
		const int line;
	};
}

#endif // ERROR_H
