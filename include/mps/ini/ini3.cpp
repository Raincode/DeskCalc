#include <iostream>
#include <map>
#include <string>
#include <sstream> // istringstream
#include <fstream>
 
struct Ini
{
    template< typename T >
    T get( const std::string& key ) const
    {
        std::map< std::string, std::string >::const_iterator i = values_.find( key );
        if( i != values_.end() )
        {
            std::istringstream buf( i->second );
            T x;
            if( buf >> x )
                return x;
        }
        return T(); // Wert war nicht konfiguriert oder im falschen Format
    }
    std::map< std::string, std::string > values_;
};
std::istream& operator>>( std::istream& in, Ini& ini )
{
    ini.values_.clear();
    for( std::string key, value; getline( getline( in, key, '=' ), value ); )
        ini.values_.insert( make_pair( key, value ) );
    return in;
}
 
int main()
{
    using namespace std;
    ifstream iniDatei("input.txt");
    if( !iniDatei.is_open() )
    {
        cerr << "Fehler beim Oeffnen der Datei" << endl;
        return -2;
    }
    Ini konfig;
    if( (iniDatei >> konfig).eof() ) // bis Dateiende gelesen, dann ist es Ok
    {
        cout << "Anzahl Panzer: " << konfig.get< int >("AnzPanzer") << endl;
        cout << "Spielerstaerke: " << konfig.get< double >("Force") << endl;
        // usw.
    }
    return 0;
}