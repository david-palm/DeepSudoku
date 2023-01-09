#pragma once

#include <array>

class SudokuSolver
{
public:
    SudokuSolver();

    int solve(std::array<std::array<int, 9>, 9> &input, std::array<std::array<int, 9>, 9> &output);
    void print(std::array<std::array<int, 9>, 9> &sudoku);
private:
    bool isSafe(std::array<std::array<int, 9>, 9> &sudoku, int row, int col, int value);
    bool solveCell(std::array<std::array<int, 9>, 9> &sudoku, int row, int col);
};
