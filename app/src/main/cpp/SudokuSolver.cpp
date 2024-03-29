#include <jni.h>
#include <android/log.h>


#include "SudokuSolver.h"

SudokuSolver::SudokuSolver() {}
bool SudokuSolver::isSafe(std::array<std::array<int, 9>, 9> &sudoku, int row, int col, int value)
{
    //Checking if number is already in row
    for(int i = 0; i < 9; i++)
        if(sudoku[row][i] == value)
            return false;

    //Checking if number is already in column
    for(int i = 0; i < 9; i++)
        if(sudoku[i][col] == value)
            return false;

    //Checking if number is already in field
    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for(int y = startRow; y < startRow + 3; y++)
        for(int x = startCol; x < startCol + 3; x++)
            if(sudoku[y][x] == value)
                return false;

    return true;
}

bool SudokuSolver::solveCell(std::array<std::array<int, 9>, 9> &sudoku, int row, int col)
{
    if(row == 8 && col == 9)
        return true;

    if(col == 9)
    {
        row++;
        col = 0;
    }

    if(sudoku[row][col] > 0)
        return solveCell(sudoku, row, col + 1);

    for(int digit = 1; digit <= 9; digit++)
    {
        if(isSafe(sudoku, row, col, digit)) {
            sudoku[row][col] = digit;

            if (solveCell(sudoku, row, col + 1))
                return true;
        }
        sudoku[row][col] = 0;
    }
    return false;
}

int SudokuSolver::solve(std::array<std::array<int, 9>, 9> &input, std::array<std::array<int, 9>, 9> &output)
{
    if(!solveCell(input, 0, 0))
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void SudokuSolver::print(std::array<std::array<int, 9>, 9> &sudoku)
{
    for(int row = 0; row < 9; row++)
    {
        if(row % 3 == 0)
            __android_log_print(ANDROID_LOG_DEBUG, "frugally-deep", "------------------------------");
                            __android_log_print(ANDROID_LOG_DEBUG, "frugally-deep", "[%d][%d][%d] | [%d][%d][%d] | [%d][%d][%d]",
                            sudoku[row][0], sudoku[row][1], sudoku[row][2], sudoku[row][3], sudoku[row][4], sudoku[row][5], sudoku[row][6], sudoku[row][7], sudoku[row][8]);
    }
}