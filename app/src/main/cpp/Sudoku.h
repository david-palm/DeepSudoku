#pragma once

#include <array>

struct Sudoku
{
    std::array<std::array<int, 9>, 9> m_ScannedDigits;
    std::array<std::array<int, 9>, 9> m_Solution;
};