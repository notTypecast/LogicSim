#include "utils.hpp"

namespace logicsim
{
    namespace utils
    {
        StringSplitter::StringSplitter()
        {
        }

        StringSplitter::StringSplitter(const std::string &str, char delimiter)
            : _str(str), _delimiter(delimiter)
        {
        }

        bool StringSplitter::has_next() const
        {
            return !_done;
        }

        std::string StringSplitter::next()
        {
            size_t split_pos = _str.find(_delimiter, _pos);
            if (split_pos == std::string::npos)
            {
                _done = true;
            }
            size_t prev = _pos;
            _pos = split_pos + 1;
            return _str.substr(prev, split_pos - prev);
        }

        void StringSplitter::reset(const std::string &str, char delimiter)
        {
            _str = str;
            _delimiter = delimiter;
            _pos = 0;
            _done = false;
        }
    }
}