#include <iostream>
#include <cassert>

static constexpr auto BOARD_SIZE = 10;

int currBoard[BOARD_SIZE];
int newBoard[BOARD_SIZE];

int getVal(int x1, int x2, int x3)
{
    // 7
    if(x1 && x2 && x3)
    {
        return 0;
    }

    // 6
    if(x1 && x2 && x3 == 0)
    {
        return 1;
    }

    // 5
    if(x1 && x2 == 0 && x3)
    {
        return 1;
    }

    // 4
    if(x1 && x2 == 0 && x3 == 0)
    {
        return 0;
    }

    // 3
    if(x1 == 0 && x2 && x3)
    {
        return 1;
    }

    // 2
    if(x1 == 0 && x2 && x3 == 0)
    {
        return 1;
    }

    // 1
    if(x1 == 0 && x2 == 0 && x3)
    {
        return 1;
    }

    // 0
    if(x1 == 0 && x2 == 0 && x3 == 0)
    {
        return 0;
    }

    assert(0 && "unreachable");

    return -1;
}

void initBoard()
{
    for(auto i = 0; i < BOARD_SIZE; i++)
    {
        currBoard[i] = 0;
    }
    currBoard[BOARD_SIZE - 1] = 1;
}

void advanceGeneration()
{
    for(auto i = 1; i < BOARD_SIZE - 1; i++)
    {
        newBoard[i] = getVal(currBoard[i - 1], currBoard[i], currBoard[i + 1]);
    }
    newBoard[BOARD_SIZE - 1] = 1;

    for(auto i = 0; i < BOARD_SIZE; i++)
    {
        currBoard[i] = newBoard[i];
    }
}

void printCurrentGen()
{
    for(auto i = 0; i < BOARD_SIZE; i++)
    {
        std::cout << (currBoard[i] ? 'x' : ' ');
    }

    std::cout << std::endl;
}

int main()
{
    static constexpr auto generations = 10;

    initBoard();

    for(auto i = 0; i < generations; i++)
    {
        advanceGeneration();
        printCurrentGen();
    }


    return 0;
}