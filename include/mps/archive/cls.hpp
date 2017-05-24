#pragma once

namespace {
    void cls()
    {   // unix: std::cout << "\x1B[2J\x1B[H"; ?
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_SCREEN_BUFFER_INFO buf_info{};
        if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;

        DWORD conSize{static_cast<DWORD>(buf_info.dwSize.X * buf_info.dwSize.Y)};
        DWORD charsWritten{};
        COORD coordScreen{0, 0};
        if (!FillConsoleOutputCharacter(hConsole, ' ', conSize, coordScreen, &charsWritten)) return;

        if (!GetConsoleScreenBufferInfo(hConsole, &buf_info)) return;
        if (!FillConsoleOutputAttribute(hConsole, buf_info.wAttributes, conSize, coordScreen, &charsWritten)) return;
        SetConsoleCursorPosition(hConsole, coordScreen);
    }
}