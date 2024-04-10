#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <exception>

namespace logicsim
{
    class Component
    {
    public:
        virtual bool evaluate() const = 0;
    };

    class NULLInput : public std::exception
    {
    public:
        const char *what() const noexcept override
        {
            return "NULL input";
        }
    };
}

#endif // COMPONENT_HPP
