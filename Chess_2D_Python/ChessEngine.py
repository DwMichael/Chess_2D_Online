"Main Driver file Its will be resposible for handling user input and displaying the current GameState Object"
class GameState():
    def __init__(self):
        #board is 8x8 2d list , 2 characters in list
        #"b" or "c" represent color of chess piece c is black from polish word "czarny" what mean "black"
        #word after "b" or "c" is name of chaess piece in polish language
        #"--" mean empty space
        self.board = [
            ["wieza_b", "kon_b","laufr_b","dama_b","krol_b","laufr_b", "kon_b","wieza_b"],
            ["pionek_b","pionek_b","pionek_b","pionek_b","pionek_b","pionek_b","pionek_b","pionek_b"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["pionek_c", "pionek_c", "pionek_c", "pionek_c", "pionek_c", "pionek_c", "pionek_c","pionek_c"],
            ["wieza_c", "kon_c", "laufr_c", "dama_c", "krol_c", "laufr_c", "kon_c", "wieza_c"]]
        self.whiteToMove = True
        self.moveLog=[]
#dobre
    
    def makeMove(self, move):
        if self.board[move.startRow][move.startCol] != '--':
            self.board[move.startRow][move.startCol] = '--'
            self.board[move.endRow][move.endCol] = move.pieceMoved
            self.moveLog.append(move)
            self.whiteToMove = not self.whiteToMove

class Move():

    ranksToRows = {"1":7,"2":6,"3":5,"4":4, "5":3, "6":2, "7":1, "8":0}
    rowsToRanks = {v : k for k, v in ranksToRows.items()}
    filesToCols = {"a":0,"b":1,"c":2,"d":3,"e":4,"f":5,"g":6,"h":7}
    colsToFiles = {v: k for k, v in filesToCols.items()}


    def __init__(self,startSq, endSq, board):
        self.startRow = startSq[0]
        self.startCol = startSq[1]
        self.endRow = endSq[0]
        self.endCol = endSq[1]
        self.pieceMoved = board[self.startRow][self.startCol]
        self.pieceCaptured = board[self.endRow][self.endCol]

    def getChessNotation(self):
        return self.getRankFile(self.startRow, self.startCol) + self.getRankFile(self.endRow, self.endCol)

    def getRankFile(self, r, c):
        return self.colsToFiles[c] + self.rowsToRanks[r]