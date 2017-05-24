#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
 
class ProgramOptions
{
public:
    ProgramOptions();
 
    // --   Schnittstelle zum Lesen und Schreiben einzelner Werte
    class Ref
    {
    public:
        template< typename T >
        Ref& operator=( const T& value );
    };
    template< typename T >
    T get( const std::string& valuename ) const;
    Ref operator[]( const std::string& valuename );
 
    // ---  ini-File IO
    friend std::ostream& operator<<( std::ostream& out, const ProgramOptions& opts );
    friend std::istream& operator>>( std::istream& in, ProgramOptions& opts );
};
 
// --   Beispiel nur für Demozwecke
struct Colour
{
    Colour() : r_(0), g_(0), b_(0) {}
    Colour( unsigned char r, unsigned char g, unsigned char b )
        : r_(r), g_(g), b_(b)
    {}
    friend std::ostream& operator<<( std::ostream& out, Colour c )
    {
        const char old_fill = out.fill( '0' );
        const std::ios_base::fmtflags oldflags = out.flags();
        out << std::hex << std::setw(2) << c.r_ << std::setw(2) << c.g_ << std::setw(2) << c.r_;
        out.flags( oldflags );
        out.fill( old_fill );
        return out;
    }
    unsigned char r_, g_, b_;
};
namespace
{
    const Colour rot( 255, 0, 0 );
}
 
int main()
{
    using namespace std;
    ProgramOptions options;
    {
        ifstream ini( "MeinKonfig.ini" );
        if( !ini.is_open() )
        {
            cerr << "Fehler beim Oeffnen des Ini-Files" << endl;
            return -2;
        }
        if( !(ini >> options) )
        {
            cerr << "Fehler beim Lesen des Ini-Files" << endl;
            return -3;
        }
    }
    // Lesen von Werten 'beliebigen' Typs
    int val1 = options.get< int >( "Wert1" );
    double gain = options.get< double >( "Faktor" );
 
    // Schreiben von Werten 'beliebigen' Typs
    options["Farbe"] = rot;
    options["SavePath"] = "C:/egal/meinFile.txt";
    options["Wert1"] = val1;
 
    // .....
 
    {
        ofstream ini( "MeinKonfig.ini" );
        if( !(ini << options) )
        {
            cerr << "Fehler beim Schreiben des Ini-Files" << endl;
        }
    }
    return 0;
}