////////////////////////////////////////////////////////////////////////////////
// Provides a wrapped C++ API to manipulate the clipboard
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

#ifndef MPS_CLIPBOARD_HPP
#define MPS_CLIPBOARD_HPP

////////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <memory>
#include <string>
#include <sstream>

#include "handle.hpp"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace {
////////////////////////////////////////////////////////////////////////////////
namespace detail {
    void close_clipboard_wrapper(BOOL) { CloseClipboard(); }
    Handle<BOOL, void> make_clipboard_handle()
    {
        return {OpenClipboard(nullptr), detail::close_clipboard_wrapper};
    }
}

bool set_clipboard_text(const std::string& text)
{
    std::size_t len {text.size() + 1}; // +1 for terminating '\0'
    mps::Handle<HGLOBAL, HGLOBAL> mem{GlobalAlloc(GMEM_MOVEABLE, len), GlobalFree};
    if (!mem) 
        return false;

    memcpy(GlobalLock(mem), text.c_str(), len);
    GlobalUnlock(mem);

    auto cb = detail::make_clipboard_handle();
    if (!cb || !EmptyClipboard() || !SetClipboardData(CF_TEXT, mem))
        return false;
    mem.release();  // SetClipboardData takes ownership if successful
    return true;
}

std::string get_clipboard_text()
{
    if (auto cb = detail::make_clipboard_handle()) {
        if (auto data = GetClipboardData(CF_TEXT)) {
            mps::Handle<LPVOID, BOOL> lock{ GlobalLock(data), GlobalUnlock };
            return static_cast<char*>(LPVOID{lock});
        }
    }
    return "";
}

////////////////////////////////////////////////////////////////////////////////
}   /* anonymous namespace */
}   /* namespace mps */
////////////////////////////////////////////////////////////////////////////////

#else
    static_assert(false, "clipboard.hpp: Platform not supported.");
#endif  /* _WIN32 */

////////////////////////////////////////////////////////////////////////////////

#endif // MPS_CLIPBOARD_HPP

////////////////////////////////////////////////////////////////////////////////
