#ifndef CORE_H
#define CORE_H

#include <string>
#include <exception>

namespace ParseLib
{
    class ParseException: public std::exception
    {
    protected:
        std::string msg_;
    public:
        explicit ParseException(const char* message,size_t pos): msg_(message)
        {
            msg_ += " at: ";
            msg_ += std::to_string(pos);
        }
        explicit ParseException(const char* message): msg_(message) {}
        explicit ParseException(const std::string& message,size_t pos): msg_(message)
        {
            msg_ += " at: ";
            msg_ += std::to_string(pos);
        }
        explicit ParseException(const std::string& message): msg_(message) {}

        virtual ~ParseException() noexcept {}

        virtual const char* what() const noexcept override
        {
            return msg_.c_str();
        }
    };
}

#endif // CORE_H
