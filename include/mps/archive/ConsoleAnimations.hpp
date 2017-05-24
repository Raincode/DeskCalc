#ifndef MPS_WIN_CONSOLEANIMATIONS_HP
#define MPS_WIN_CONSOLEANIMATIONS_HP

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include <mps/StreamUtils.hpp>

namespace mps {
#ifdef _WIN32
namespace win {
namespace {

    void slide_in(const std::vector<std::string> &lines, const std::string &head="", unsigned ms=10)
    {
        std::size_t max{};
        for (const auto &l : lines)
            if (l.size() > max) max = l.size();

        auto pos = max;
        while (pos--)
        {
            mps::util::cls();
            std::string frame{head};
            for (auto &l : lines)
            {
                for (std::size_t i = pos; i < l.size(); ++i)
                    frame += l[i];
                frame += '\n';
            }
            std::cout << frame;
            std::cout.flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        }
    }

    void slide_in(const std::string &lines, const std::string &head="", unsigned ms=10)
    {
        std::vector<std::string> l;
        std::istringstream stream{lines};
        for (std::string s; std::getline(stream, s) ;)
            l.push_back(s);
        slide_in(l, head, ms);
    }

    void slide_out(const std::vector<std::string> &lines, const std::string &head="", unsigned ms=10)
    {
        std::size_t max{};
        for (const auto &l : lines)
            if (l.size() > max) max = l.size();

        for (std::size_t pos = 0; pos < max; ++pos)
        {
            std::string frame{head};
            for (auto &l : lines)
            {
                for (std::size_t i = pos; i < l.size(); ++i)
                    frame += l[i];
                frame += '\n';
            }
            (std::cout << frame).flush();
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            mps::util::cls();
        }
    }

} // anonymous namespace
} // namespace win
#endif
} // namespace mps
#endif // MPS_WIN_CONSOLEANIMATIONS_HP
