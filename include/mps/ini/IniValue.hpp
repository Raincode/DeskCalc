class IniValue {
public:
    enum class Type { Double, String, Int };
    
    IniValue() = default;
    explicit IniValue(double num):type{Type::Double}, d{num} { }
    explicit IniValue(const std::string& str):type{Type::String}, s{str} { }
    explicit IniValue(int num):type{Type:Int}, i{num} { }
    
     void set(double num) { 
        if (type == Type::String) s.~std::string();
        type = Type::Double;
        d = num;
     }
     
     void set(const std::string& str) {
         if (type == Type::String) s = str;
         else {
             new (&s) std::string{str};
             type = Type::String;
         }
     }
    
private:
    Type type{Type::Int};
    union {
        double d{};
        std::string s;
        int i{};
    }
};

std::ostream& operator<<(std::ostream& os, const IniValue& v)
{
    
}

std::istream& operator>>(std::istream& is, IniValue& v)
{
    
}