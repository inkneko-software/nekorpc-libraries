#include "../../SettingReader/SettingReader.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 1)
    {
        SettingReader reader;
        try
        {
            for (auto& config : reader.fromKvfmt(argv[1]))
            {
                std::cout << "[" << config.first << "]" << '=' << config.second << std::endl;
            }
        }catch(std::exception &e)
        {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}