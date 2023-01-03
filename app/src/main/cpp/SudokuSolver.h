#pragma once

class SudokuSolver
{
public:
    SudokuSolver();

    int solve(int (&input)[9][9], int (&output)[9][9]);
    void print(int (&sudoku)[9][9]);
private:
    bool isSafe(int (&sudoku)[9][9], int row, int col, int value);
    bool solveCell(int (&sudoku)[9][9], int row, int col);
};
