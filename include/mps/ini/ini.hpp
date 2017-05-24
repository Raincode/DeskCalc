#ifndef MPS_INI_HPP
#define MPS_INI_HPP

#include "IniValue.hpp"

/*
    For the start, only reads files, and preserves format, but doesnt allow programatically writing
    
    mps::IniConf convenient{"myfile.ini"};
    mps::IniConf conf{file};

    
    // if a value doesn't exist an exception is thrown
    bool on = conf["settings"]["soundEnabled"];
    

    




*/

namespace mps {
namespace {





class IniFile {
public:
    IniFile() = default;
    explicit IniFile(const std::string& filename);
    
    void load(const std::string& filename);
    void save(const std::string& filename);
    
private:    
    std::vector<IniSection> sections;
};

}   /* anonymous namespace */
}   /* namespace mps */

#endif  /* MPS_INI_HPP */