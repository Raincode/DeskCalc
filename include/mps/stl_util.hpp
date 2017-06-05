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

#ifndef MPS_STL_UTIL_HPP
#define MPS_STL_UTIL_HPP

////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <map>
#include <string>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace stl {
////////////////////////////////////////////////////////////////////////////////

template<typename C>
inline void sort(C& c)
{
    std::sort(begin(c), end(c));
}

template<typename C, typename V>
inline auto find(const C& c, const V& val)
{
    return std::find(begin(c), end(c), val);
}

template<typename C, typename Pred>
inline auto find_if(const C& c, Pred pred)
{
    return std::find_if(begin(c), end(c), pred);
}

template<typename C, typename V>
inline bool contains(const C& c, const V& val)
{
    return find(c, val) != end(c);
}

template<typename K, typename V>
inline bool contains(const std::map<K,V>& map, const K& key)
{
    return map.find(key) != cend(map);
}

template<class K, class V, class Pred>
inline bool contains_if(const std::map<K, V>& map, const K& key, Pred pred)
{
    auto found = map.find(key);
    return found != cend(map) && pred(found->second);
}

template<typename C, typename Pred>
inline auto erase_if(C& c, Pred pred)
{
    return c.erase(std::remove_if(begin(c), end(c), pred), end(c));
}

template<class K, class V, class Pred>
inline void erase_if(std::map<K,V>& map, Pred pred)
{
	for (auto i = begin(map); i != end(map); ) {
		if (pred(*i))
			i = map.erase(i);
		else
			++i;
	}
}

template<typename C, typename Iter>
inline void copy(const C& c, Iter i)
{
    std::copy(begin(c), end(c), i);
}

template<class C, class Func>
inline void for_each(C& c, Func f)
{
	std::for_each(begin(c), end(c), f);
}

template<class K, class V>
inline void insert_unique(std::map<K,V>& map, K&& key, V&& val)
{
	auto found = map.find(key);
	if (found != cend(map))
		throw std::runtime_error{ "insert_unique failed - " + key + " already in map" };
	map[key] = val;
}

template<class K, class V>
inline typename std::map<K,V>::const_iterator find_or_throw(const std::map<K,V>& map, const K& key, const std::string& msg)
{
	auto found = map.find(key);
	if (found == cend(map))
		throw std::runtime_error{ msg };
	return found;
}

////////////////////////////////////////////////////////////////////////////////
} // namespace stl
} // namespace mps
////////////////////////////////////////////////////////////////////////////////

#endif // MPS_STL_UTIL_HPP

////////////////////////////////////////////////////////////////////////////////
