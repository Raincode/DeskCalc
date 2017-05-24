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

#ifndef MPS_DATE_HPP
#define MPS_DATE_HPP

////////////////////////////////////////////////////////////////////////////////

#include <ctime>
#include <iostream>
#include <stdexcept>

#include "month.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace {
////////////////////////////////////////////////////////////////////////////////

struct YMD {
    int day{};
    Month month{};
    int year{};
};

////////////////////////////////////////////////////////////////////////////////

class Date {    // Perhaps store as (Modified Julian Day) instead of y m d?
public:
    class Invalid {};  // Exception Type

    Date() = default;
    Date(int d, Month m, int y);

    void add_days(int n);
    void add_months(int n);
    void add_years(int n);

    int daysTo(const Date& date);

    int day() const { return d; }
    Month month() const { return m; }
    int year() const { return y; }

    static Date today();
    static void set_exceptions_enabled(bool b) { exceptions = b; }
    explicit operator bool() const;
    
private:
    static bool exceptions;
    void check();

    int d{ 1 };
    Month m{ Month::jan };
    int y{ 2001 };
};

bool Date::exceptions{false};

////////////////////////////////////////////////////////////////////////////////

bool operator==(const Date& lhs, const Date& rhs)
{
    return lhs.day() == rhs.day() &&
        lhs.month() == rhs.month() &&
        lhs.year() == rhs.year();
}

////////////////////////////////////////////////////////////////////////////////

Date Date::today()
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    return {tm.tm_mday, Month(tm.tm_mon + 1), 1900 + tm.tm_year};
}

////////////////////////////////////////////////////////////////////////////////

bool operator!=(const Date& lhs, const Date& rhs)
{
    return !(lhs == rhs);
}

////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const Date& dd)
{
    return os << dd.day() << '.' << int(dd.month()) << '.' << dd.year();
}

////////////////////////////////////////////////////////////////////////////////

bool is_leap_year(int y)
{
    return y % 4 == 0 && (y % 100 || y % 400 == 0);
}

////////////////////////////////////////////////////////////////////////////////

int last_day(Month m, int y)
{
    switch (m) {
    case Month::jan: case Month::mar: case Month::may:
    case Month::jul: case Month::aug: case Month::oct: case Month::dec:
        return 31;
    case Month::apr: case Month::jun: case Month::sep: case Month::nov:
        return 30;
    case Month::feb:
        return is_leap_year(y) ? 29 : 28;
    default:
        throw std::runtime_error("last_day: Month out of range");
    }
}

////////////////////////////////////////////////////////////////////////////////

bool is_valid_day(int d, Month m, int y)
{
    return d > 0 && d <= last_day(m, y);
}

////////////////////////////////////////////////////////////////////////////////

bool is_date(int d, Month m, int y)
{
    return is_month(m) && is_valid_day(d, m, y);
}

////////////////////////////////////////////////////////////////////////////////

bool is_date(const Date &d)
{
    return is_date(d.day(), d.month(), d.year());
}


////////////////////////////////////////////////////////////////////////////////

Date::Date(int dd, Month mm, int yy)
    :d{ dd }, m{ mm }, y{ yy }
{
    check();
}

////////////////////////////////////////////////////////////////////////////////

void Date::check()
{
    if (!is_date(d, m, y) && exceptions)
        throw Invalid{};
}

////////////////////////////////////////////////////////////////////////////////

void Date::add_days(int n)
{

}

////////////////////////////////////////////////////////////////////////////////

void Date::add_months(int n)
{
    int months = n % 12;
    int years = n / 12;
    if (int(m) + months > 12)
    {
        ++years;
        months -= 12;
    }
    Month newMonth = Month(int(m) + months);
    if (d <= last_day(newMonth, y + years))
    {
        m = newMonth;
    }
    else
    {
        m = Month(int(newMonth) + 1);
        d = d - last_day(newMonth, y + years);
    }
    add_years(years);
}

////////////////////////////////////////////////////////////////////////////////

void Date::add_years(int n)
{
    if (d == 29 && m == Month::feb && !is_leap_year(y + n))
    {
        d = 1;
        m = Month::mar;
    }
    y += n;
}

////////////////////////////////////////////////////////////////////////////////

Date::operator bool() const
{
    return is_date(*this);
}

////////////////////////////////////////////////////////////////////////////////
}   /* anonymous namespace */
}   /* namespace mps */
////////////////////////////////////////////////////////////////////////////////

#endif  // MPS_DATE_HPP

////////////////////////////////////////////////////////////////////////////////
