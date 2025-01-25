#ifndef LOGICSIM_UTILS_HPP
#define LOGICSIM_UTILS_HPP

#include <cmath>
#include <string>
#include <vector>

namespace logicsim
{
namespace utils
{
class StringSplitter
{
  public:
    StringSplitter();
    StringSplitter(const std::string &str, char delimiter);

    bool        has_next() const;
    std::string next();

    void reset(const std::string &str, char delimiter);

  protected:
    std::string _str;
    char        _delimiter;
    size_t      _pos  = 0;
    bool        _done = false;
};

size_t get_int_from_bools(std::vector<bool> bools);

bool is_positive_int(std::string str);
}
}

#endif // LOGICSIM_UTILS_HPP
