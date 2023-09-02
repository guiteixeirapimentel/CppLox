import time


BOARD_SIZE = 100

curBoard = ""

for i in range(BOARD_SIZE - 1):
    curBoard = curBoard + " "

curBoard = curBoard + "x"

newBoard = curBoard

MAX_GENS = 100

def getVal(x1, x2, x3):
    # 7
    if(x1 == "x" and x2 == "x" and x3 == "x"):
        return " "
    # 6
    if(x1 == "x" and x2 == "x" and x3 == " "):
        return "x"
    # 5
    if(x1 == "x" and x2 == " " and x3 == "x"):
        return "x"
    # 4
    if(x1 == "x" and x2 == " " and x3 == " "):
        return " "
    # 3
    if(x1 == " " and x2 == "x" and x3 == "x"):
        return "x"
    # 2
    if(x1 == " " and x2 == "x" and x3 == " "):
        return "x"
    # 1
    if(x1 == " " and x2 == " " and x3 == "x"):
        return "x"
    # 0
    if(x1 == " " and x2 == " " and x3 == " "):
        return " "
    
def advanceGeneration():
    global curBoard
    newBoard = " "
    for i in range(1, BOARD_SIZE - 1):
        newBoard = newBoard + getVal(curBoard[i - 1], curBoard[i], curBoard[i + 1])
    

    newBoard = newBoard + "x"

    curBoard = newBoard


def printCurrentGen():

    print(curBoard)


t1 = time.time()

for i in range(MAX_GENS):
    advanceGeneration()
    printCurrentGen()


t2 = time.time()

print("110 rule took the following seconds")
print(t2 - t1)