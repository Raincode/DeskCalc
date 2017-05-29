////////////////////////////////////////////////////////////////////////////////
// Generic RAII wrapper class to handle automatic destruction for cases
// where smart pointers do not work well.
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

#ifndef MPS_HANDLE_HPP
#define MPS_HANDLE_HPP

#include <functional>

////////////////////////////////////////////////////////////////////////////////
namespace mps { 
////////////////////////////////////////////////////////////////////////////////
    
template<class HandleType, class Ret>
class Handle {
public:
    using Deleter = std::function<Ret(HandleType)>;
    Handle(HandleType h, const Deleter& d)
        :handle{h}, deleter{d} { }
    ~Handle() { if (handle) deleter(handle); }

    operator HandleType() { return handle; }
    void release() { handle = nullptr; }

private:
    HandleType handle;
    Deleter deleter;
};

////////////////////////////////////////////////////////////////////////////////
}   /* namespace mps */
////////////////////////////////////////////////////////////////////////////////

#endif // MPS_HANDLE_HPP

////////////////////////////////////////////////////////////////////////////////