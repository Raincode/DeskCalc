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
#pragma once
////////////////////////////////////////////////////////////////////////////////

#include <cctype>
#include <memory>
#include <string>
#include <sstream>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace detail {
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

void close_clipboard_wrapper(BOOL) { CloseClipboard(); }

////////////////////////////////////////////////////////////////////////////////
} // namespace detail
namespace {
////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32

bool set_clipboard_text(const std::string& text)
{
    std::size_t len {text.size() + 1}; // +1 for terminating '\0'
    detail::Handle<HGLOBAL, HGLOBAL> mem{GlobalAlloc(GMEM_MOVEABLE, len), GlobalFree};
    if (!mem) return false;

    memcpy(GlobalLock(mem), text.c_str(), len);
    GlobalUnlock(mem);

    detail::Handle<BOOL, void> cb{OpenClipboard(nullptr), detail::close_clipboard_wrapper};
    if (!cb || !EmptyClipboard() || !SetClipboardData(CF_TEXT, mem)) return false;
    mem.release();
    return true;
}

std::string get_clipboard_text()
{
    detail::Handle<BOOL, void> cb{OpenClipboard(nullptr), detail::close_clipboard_wrapper};
    if (cb)
    {
        if (auto data = GetClipboardData(CF_TEXT))
        {
            detail::Handle<LPVOID, BOOL> lock{ GlobalLock(data), GlobalUnlock };
            return static_cast<char*>(LPVOID{lock});
        }
    }
    return "";
}

#else
    static_assert(false, "Clipboard.hpp: Platform not supported.");
#endif

////////////////////////////////////////////////////////////////////////////////
}  // anonymous namespace
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
