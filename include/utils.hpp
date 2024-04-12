#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace logicsim
{
    namespace utils
    {
        class StringSplitter
        {
        public:
            StringSplitter();
            StringSplitter(const std::string &str, char delimiter);

            bool has_next() const;
            std::string next();

            void reset(const std::string &str, char delimiter);

        protected:
            std::string _str;
            char _delimiter;
            size_t _pos = 0;
            bool _done = false;
        };
    }
}

#endif // UTILS_HPP
