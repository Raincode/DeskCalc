#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <functional> // function<>
#include <limits> // numeric_limits<>
 
// --   der INI-Reader, für jedes Attribut wird ein Funktor benötigt
//      (allgemeiner Teil)
template< typename C >
struct ini_reader
{
    explicit ini_reader( const C& reader_catalog )
        : reader_catalog_( &reader_catalog )
    {}
    friend std::istream& operator>>( std::istream& in, ini_reader ir )
    {
        for( std::string token; getline( in, token, '=' ); )
        {
            auto i = ir.reader_catalog_->find( token );
            if( i == end(*ir.reader_catalog_) )
            {   // unbekanntes Token überspringen (skipline)
                in.ignore( std::numeric_limits< std::streamsize >::max(), '\n' );
                continue;
            }
            i->second( in ); // <--- hier wird der Wert gleich mit dem 'richtigen' Typ gelesen
            if( in.eof() || (in >> std::ws).eof() ) // diese Konstruktion sorgt dafür, dass 'in' bei EOF nicht nach fail geht!
                break;
        }
        return in;
    }
private:
    const C* reader_catalog_;
};
template< typename C >
ini_reader< C > ini( const C& cat )
{
    return ini_reader< C >( cat );
}
 
// --   Beispiel für eine User-defined Klasse/struct
struct Size
{
    Size( float x, float y )
        : x_(x), y_(y)
    {}
    float x_, y_;
};
std::istream& operator>>( std::istream& in, Size& sz )
{
    return (in >> sz.x_).ignore(1) >> sz.y_; // Alternative: in >> sz.x_ >> Char<'x'> >> sz.y_; // (s.u.)
}
 
int main()
{
    using namespace std;
    // --   Attribute mit ihrer Defaultbelegung
    Size bildGroesse( 1024.f, 768.f );
    Size aufloesung( 800.f, 600.f );
    double fps = 25.;
    int faeden = 1;
    {
        string filename = "./Spiel.ini";
        ifstream file( filename.c_str() );
        if( !file.is_open() )
        {
            cerr << "Fehler beim Oeffnen der Datei '" << filename << "'" << endl;
            return -2;
        }
        // --   einlesen
        map< string, function< void( istream& ) > > reader;
        //      folgende 4 Zeilen sind der User-spezifische Teil des INI-Lesens
        reader["bild"] = [&bildGroesse]( istream& in ) { in >> bildGroesse; };
        reader["aufl"] = [&aufloesung]( istream& in ) { in >> aufloesung; };
        reader["fps"] = [&fps]( istream& in ) { if( in >> fps && fps < 12. ) in.setstate( std::ios_base::failbit ); }; // Beispiel für eine Bereichsüberprüfung
        reader["faeden"] = [&faeden]( istream& in ) { in >> faeden; };
        if( (file >> ini( reader )).fail() )
        {
            cerr << "Fehler beim Lesen der ini-Datei '" << filename << "'" << endl;
            return -3;
        }
    }
    // --   Start der Applikation
    cout << "Alle ok" << endl;
    // usw.
 
    return 0;
}