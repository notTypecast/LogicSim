#ifndef LOGICSIM_MODEL_UTILS_HPP
#define LOGICSIM_MODEL_UTILS_HPP

#include <string>
#include <vector>
#include <cmath>

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

        size_t get_int_from_bools(std::vector<bool> bools);

        bool is_positive_int(std::string str);
    }
}

#endif // UTILS_HPP
