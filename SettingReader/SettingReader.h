#include <regex>
#include <map>
#include <string>
#include <fstream>

class SettingReader
{
public:
    std::map<std::string, std::string> fromKvfmt(std::string path);

private:
    std::string strip(const std::string & str, const std::string chr = " ");
};