////////////////////////////////////////////////////////////////////////////////
// Provides std::string utility functions
//
// Copyright (C) 2017  Matthias Stauber
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////

#ifndef MPS_MONTH_HPP
#define MPS_MONTH_HPP

#include <mps/stl_util.hpp>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace {
////////////////////////////////////////////////////////////////////////////////

enum class Month {
    jan = 1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec
};

////////////////////////////////////////////////////////////////////////////////

Month previous(const Month &m)
{
    return m == Month::jan ? Month::dec : Month(int(m) - 1);
}

Month next(const Month &m)
{
    return m == Month::dec ? Month::jan : Month(int(m) + 1);
}

////////////////////////////////////////////////////////////////////////////////

bool is_month(Month m)
{
    return m >= Month::jan && m <= Month::dec;
}

////////////////////////////////////////////////////////////////////////////////

static const std::map<Month, std::string> MonthToStr {
    {Month::jan, "Jan"}, {Month::feb, "Feb"}, {Month::mar, "Mar"}, {Month::apr, "Apr"},
    {Month::may, "May"}, {Month::jun, "Jun"}, {Month::jul, "Jul"}, {Month::aug, "Aug"},
    {Month::sep, "Sep"}, {Month::oct, "Oct"}, {Month::nov, "Nov"}, {Month::dec, "Dec"}
};

std::string month_to_str(const Month &m)
{
    auto found = MonthToStr.find(m);
    return found != end(MonthToStr) ? found->second : "Invalid Month";
}

////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream &os, const Month &m)
{
    return os << int(m);
}

////////////////////////////////////////////////////////////////////////////////

std::istream& operator>>(std::istream& is, Month &month)
{
    int m{};
    if (is >> m)
    {
        if (is_month(Month(m))) month = Month(m);
        else is.clear(std::ios_base::failbit);
    }
    return is;
}

////////////////////////////////////////////////////////////////////////////////
}   /* anonymous namespace */
}   /* namespace mps */
////////////////////////////////////////////////////////////////////////////////

#endif  /* MPS_MONTH_HPP */

////////////////////////////////////////////////////////////////////////////////
