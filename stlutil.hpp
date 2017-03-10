////////////////////////////////////////////////////////////////////////////////
// Provides convenient wrapper functions for STL algorithms
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
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <map>

////////////////////////////////////////////////////////////////////////////////

template<typename C>
inline void sort(C& c)
{
    std::sort(begin(c), end(c));
}

template<typename C>
inline auto find(const C& c, const auto& val)
{
    return std::find(begin(c), end(c), val);
}

template<typename C, typename Pred>
inline auto find_if(const C& c, const Pred& pred)
{
    return std::find_if(begin(c), end(c), pred);
}

template<typename C>
inline bool contains(const C& c, const auto& val)
{
    return find(c, val) != end(c);
}

template<typename K, typename V>
inline bool contains(const std::map<K,V>& map, const auto& value)
{
    return map.find(value) != cend(map);
}

template<typename C, typename Pred>
inline auto erase_if(C& c, const Pred& pred)
{
    return c.erase(std::remove_if(begin(c), end(c), pred), end(c));
}

////////////////////////////////////////////////////////////////////////////////
