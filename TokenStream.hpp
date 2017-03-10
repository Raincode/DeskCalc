////////////////////////////////////////////////////////////////////////////////
// DeskCalc is based on the calculator described in Bjarne Stroustrup's
//     "The C++ Programming Language"
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

#include <istream>
#include <string>

#include "Token.hpp"

////////////////////////////////////////////////////////////////////////////////
namespace mps {
namespace math {
////////////////////////////////////////////////////////////////////////////////

class TokenStream {
public:
    TokenStream() = default;
    explicit TokenStream(std::istream& is);
    explicit TokenStream(std::istream *is);
    explicit TokenStream(const std::string& str);

    TokenStream(const TokenStream&) = delete;
    TokenStream& operator=(const TokenStream&) = delete;

    ~TokenStream();

    void set_input(std::istream& is);
    void set_input(std::istream *is);
    void set_input(const std::string& str);

    Token get();
    const Token& current() const;

    bool is_newline_print() const;

    bool match(Kind kind) const { return ct.kind == kind; }

    std::istream& stream() { if (!input) throw std::runtime_error("No input"); return *input; }

private:
    void cleanup();
    Token parse_double_op(char second, Kind atMatch, Kind firstKind);
    Token parse_identifier(char ch);

    ////////////////////////////////////////////////////////////////////////////

    std::istream* input {};
    Token ct {Kind::End};
    bool ownsInput {false};
    bool newlineRead {false};
};

////////////////////////////////////////////////////////////////////////////////
}  // namespace math
}  // namespace mps
////////////////////////////////////////////////////////////////////////////////
