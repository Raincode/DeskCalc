class IniProperty {
public:


private:
    bool comment_same_line{};
    std::string comment;
    std::string key;
    IniValue value;
};