"Main Driver file Its will be resposible for handling user input and displaying the current GameState Object"
class GameState():
    def __init__(self):
        #board is 8x8 2d list , 2 characters in list
        #"b" or "c" represent color of chess piece c is black from polish word "czarny" what mean "black"
        #word after "b" or "c" is name of chaess piece in polish language
        #"--" mean empty space
        self.board = [
            ["cW", "cH", "cL", "cD", "cK", "cL", "cH", "cW"],
            ["cP", "cP", "cP", "cP", "cP", "cP", "cP", "cP"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"],
            ["bW", "bH", "bL", "bD", "bK", "bL", "bH", "bW"]]
        self.moveFunction = {"P":self.getPawnMoves ,"W":self.getWiezaMoves,"H":self.getKonMoves, "L": self.getLaufrMoves,"D":self.getDamaMoves,"K":self.getKrolMoves}
        self.whiteToMove = True
        self.moveLog= []
        self.whiteKingLocation = (7,4)
        self.blackKingLocation = (0,4)
        self.inCheck =False
        self.pins = []
        self.chceks = []
    #dobre
    
    def makeMove(self, move):
        print(self.board[move.startRow][move.startCol])
        print(move.startRow, move.startCol)
        if self.board[move.startRow][move.startCol] != '--':
            self.board[move.startRow][move.startCol] = '--'
            self.board[move.endRow][move.endCol] = move.pieceMoved
            self.moveLog.append(move)
            self.whiteToMove = not self.whiteToMove
            #update king position
            if move.pieceMoved =='bK':
                self.whiteKingLocation = ( move.endRow, move.endCol)
            elif move.pieceMoved == "cK":
                self.blackKingLocation= ( move.endRow, move.endCol)

    def undoMove(self):
        if(self.moveLog) != 0:
            move = self.moveLog.pop()
            self.board[move.startRow][move.startCol] = move.pieceMoved
            self.board[move.endRow][move.endCol] = move.pieceCaptured
            self.whiteToMove = not self.whiteToMove
                        #update king position
            if move.pieceMoved =='bK':
                self.whiteKingLocation = ( move.endRow, move.endCol)
            elif move.pieceMoved == "cK":
                self.blackKingLocation= ( move.endRow, move.endCol)


    def getValidMoves(self):
        moves=[]
        self.inCheck, self.pins, self.chceks = self.checkForPinsAndChecks()
        if self.whiteToMove:
            kingRow = self.whiteKingLocation[0]
            kingCol = self.whiteKingLocation[1]
        else:
            kingRow = self.blackKingLocation[0]
            kingCol = self.blackKingLocation[1]
        if self.inCheck:
            if len(self.chceks) == 1:
                moves = self.getAllPasibleMoves()
                check =self.chceks[0]
                checkRow = check[0]
                checkCol = check[1]
                pieceChecking = self.board[checkRow][checkCol]
                validSquares = []
                if pieceChecking[1]=='H':
                    validSquares = [(checkRow,checkCol)]
                else:
                    for i in range(1,8):
                        validSquares = (kingRow + check[2]*i,kingCol+ check[3]*i)
                        validSquares.append(validSquares)
                        if validSquares[0] == checkRow and validSquares[1]==checkCol:
                            break
                for i in range(len(moves)-1,-1,-1):
                    if moves[i].pieceMoved[1] != 'K':
                        if not (moves[i].endRow,moves[i].endCol) in validSquares:
                            moves.remove(moves[i])
            else:
                self.getKingMoves(kingRow,kingCol, moves)
        else:
            moves = self.getAllPasibleMoves()
        

        return moves



    def getAllPasibleMoves(self):
        moves=[]
        for r in range(len(self.board)):
            for c in range(len(self.board[r])):
                turn = self.board[r][c][0]
                if( turn == "b" and self.whiteToMove ) or ( turn == "c" and not self.whiteToMove ):
                    piece=self.board[r][c][1]
                    self.moveFunction[piece](r,c,moves)
        return moves

    def getPawnMoves(self, r,c,moves):
        piecePinned = False
        pinDirection = ()
        for i in range(len(self.pins)-1,-1,-1):
            if self.pins[i][0] == r and self.pins[i][1] == c:
                piecePinned = True
                pinDirection = (self.pins[i][2], self.pins[i][3])
                self.pins.remove(self.pins[i])
                break

        if self.whiteToMove:
            if self.board[r-1][c] == "--" :
                if not piecePinned or pinDirection == (-1,0):
                    moves.append(Move((r,c), (r-1,c),self.board))
                    if r == 6 and self.board[r-2][c] == "--":
                        moves.append(Move((r,c),(r-2,c),self.board))
            #capture
            if c-1>=0:
                if self.board[r-1][c-1][0] == 'c':
                    if not piecePinned or pinDirection == (-1,-1):
                        moves.append(Move((r, c), (r - 1, c-1), self.board))
            if c+1<=7:
                if self.board[r-1][c+1][0] == 'c':
                    if not piecePinned or pinDirection == (-1,1):
                        moves.append(Move((r, c), (r - 1, c+1), self.board))
        else:#black pawn moves
            if self.board[r + 1][c] == "--":
                if not piecePinned or pinDirection == (1,1):
                    moves.append(Move((r, c), (r + 1, c), self.board))
                    if r == 1 and self.board[r + 2][c] == "--":
                        moves.append(Move((r, c), (r + 2, c), self.board))
                #capture            
            if c - 1 >= 0:#capture left
                if self.board[r + 1][c - 1][0] == 'b':
                    if not piecePinned or pinDirection == (1,-1):
                        moves.append(Move((r, c), (r + 1, c - 1), self.board))
            if c + 1 <= 7:#capture right
                if self.board[r + 1][c + 1][0] == 'b':
                    if not piecePinned or pinDirection == (1,1):
                        moves.append(Move((r, c), (r + 1, c + 1), self.board))


    def getWiezaMoves(self,r,c,moves):
        piecePinned = False
        pinDirection = ()
        for i in range(len(self.pins)-1,-1,-1):
            if self.pins[i][0] == r and self.pins[i][1] == c:
                piecePinned = True
                pinDirection = (self.pins[i][2], self.pins[i][3])
                if self.board[r][c][1] != 'D': 
                    self.pins.remove(self.pins[i])
                break
        directions = ((-1,0),(0,-1),(1,0),(0,1))
        enemyColor = "c" if self.whiteToMove else "b"
        for d in directions:
            for i in range(1, 8):
                endRow = r + d[0] * i
                endCol = c + d[1] * i
                if 0 <= endRow < 8 and 0 <= endCol < 8:
                    if not piecePinned or pinDirection == d or pinDirection == (-d[0], -d[1]):
                        endPiece = self.board[endRow][endCol]
                        if endPiece == "--":
                            moves.append(Move((r, c), (endRow, endCol), self.board))
                        elif endPiece[0] == enemyColor:
                            moves.append(Move((r, c), (endRow, endCol), self.board))
                            break
                        else:
                            break
                else:
                    break


    def getKonMoves(self,r,c,moves):
        piecePinned = False
        for i in range(len(self.pins)-1,-1,-1):
            if self.pins[i][0] == r and self.pins[i][1] == c:
                piecePinned = True
                self.pins.remove(self.pins[i])
                break
        konMoves = ((-2,-1),(-2,1),(-1,-2),(-1,2),(1,-2),(1,2),(2,-1),(2,1))
        allyColor = "b" if self.whiteToMove else "c"
        for m in konMoves:
            endRow = r + m[0]
            endCol = c + m[1]
            if 0 <= endRow < 8 and 0 <= endCol < 8:
                if not piecePinned:
                    endPiece = self.board[endRow][endCol]
                    if endPiece[0] != allyColor:
                        moves.append(Move((r, c), (endRow, endCol), self.board))


    def getLaufrMoves(self,r,c,moves):
        piecePinned = False
        pinDirection = ()
        for i in range(len(self.pins)-1,-1,-1):
            if self.pins[i][0] == r and self.pins[i][1] == c:
                piecePinned = True
                pinDirection = (self.pins[i][2], self.pins[i][3])
                self.pins.remove(self.pins[i])
                break
        directions=((-1,-1),(-1,1),(1,-1),(1,1))
        enemyColor = "c" if self.whiteToMove else "b"
        for d in directions:
            for i in range(1,8):
                endRow = r + d[0] * i
                endCol = c + d[1] * i
                if 0 <= endRow < 8 and 0 <= endCol < 8:
                    if not piecePinned or pinDirection == d or pinDirection == (-d[0], -d[-1]):
                        endPiece = self.board[endRow][endCol]
                        if endPiece == "--":
                            moves.append(Move((r, c), (endRow, endCol), self.board))
                        elif endPiece[0] == enemyColor:
                            moves.append(Move((r, c), (endRow, endCol), self.board))
                            break
                        else:
                            break
                else:
                    break

    def getDamaMoves(self,r,c,moves):
         self.getWiezaMoves(r,c,moves)
         self.getLaufrMoves(r,c,moves)

    def getKrolMoves(self,r,c,moves):
        rowMoves =(-1,-1,-1,0,0,1,1,1)
        colMoves = (-1,0,1,-1,1,-1,0,1)
        allyColor = "b" if self.whiteToMove else "c"
        for i in range(8):
            endRow = r + rowMoves[i]
            endCol = c + colMoves[i]
            if 0 <= endRow < 8 and 0 <= endCol < 8 :
                endPiece = self.board[endRow][endCol]
                if endPiece[0] != allyColor:
                    if allyColor == 'b':
                        self.whiteKingLocation = (endRow,endCol)
                    else:
                        self.blackKingLocation = (endRow,endCol)
                    inCheck, pins, checks = self.checkForPinsAndChecks()
                    if not inCheck:
                        moves.append(Move((r,c),(endRow,endCol),self.board))
                    if allyColor == 'b':
                        self.whiteKingLocation = (r,c)
                    else:
                        self.blackKingLocation = (r,c)


    def checkForPinsAndChecks(self):
        pins=[ ]#pola, w których znajduje się sojuszniczy przyszpilony pionek i kierunek, z którego jest przyszpilony
        checks = [] #pole z którego przeciwnik szachuje
        inCheck = False
        if self.whiteToMove:
            enemyColor = 'c'
            allyColor = 'b'
            startRow = self.whiteKingLocation[0]
            startCol = self.whiteKingLocation[1]
        else:
            enemyColor = 'b'
            allyColor = 'c'
            startRow = self.blackKingLocation[0]
            startCol = self.blackKingLocation[1]
        direction = ((-1,0),(0,-1),(1,0),(0,-1),(-1,-1),(-1,1),(1,-1),(1,1))
        for j in range(len(direction)):
            d = direction[j]
            possiblePin=() # reset możliwych ataków
            for i in range(1,8):
                endRow = startRow + d[0]*i
                endCol = startCol + d[1]*i
                if 0 <= endRow <8 and 0 <= endCol <8:
                    endPiece = self.board[endRow][endCol]
                    if endPiece[0] == allyColor and endPiece[1] != 'K':
                        if possiblePin == ():
                            possiblePin= (endRow,endRow,d[0],d[1])
                        else:
                            break
                    elif endPiece[0]==enemyColor:
                        type = endPiece[1]
                        if(0 <= j <=3  and type == 'W') or \
                                (4<=j<=7 and type == "L") or \
                                (i == 1 and type == 'P' and ((enemyColor == 'b' and 6<= j <=7) or (enemyColor=='c' and 4<= j <=5 )) ) or \
                                (type == 'D') or  (i == 1 and type == 'K'):
                            if possiblePin == ():
                                inCheck =True
                                checks.append((endRow,endCol, d[0],d[1]))
                                break
                            else:
                                pins.append(possiblePin)
                                break
                        else:
                            break
        konMoves = ((-2,-1),(-2,1),(-1,-2),(-1,2),(1,-2),(1,2),(2,-1),(2,1),)
        for m in konMoves:
            endRow = startRow + m[0]
            endCol = startCol + m[1]
            if 0 <= endRow <8 and 0<= endCol <8:
                endPiece = self.board[endRow][endCol]
                if endPiece[0] == enemyColor and endPiece[1] == 'H':
                    inCheck = True
                    checks.append((endRow,endCol, m[0],m[1]))
        return inCheck, pins, checks

class Move():

    ranksToRows = {"1":7,"2":6,"3":5,"4":4, "5":3, "6":2, "7":1, "8":0}
    rowsToRanks = {v : k for k, v in ranksToRows.items()}
    filesToCols = {"a":0,"b":1,"c":2,"d":3,"e":4,"f":5,"g":6,"h":7}
    colsToFiles = {v: k for k, v in filesToCols.items()}


    def __init__(self,startSq, endSq, board):
        self.startSq = startSq
        self.endSq = endSq

        if isinstance(startSq, str):
            # print("here")
            self.startSq = [self.ranksToRows[startSq[1]], self.filesToCols[startSq[0]]]
        # print(self.startSq)

        if isinstance(endSq, str):
            self.endSq = [self.ranksToRows[endSq[1]], self.filesToCols[endSq[0]]]

        self.startRow = self.startSq[0]
        self.startCol = self.startSq[1]
        self.endRow = self.endSq[0]
        self.endCol = self.endSq[1]
        # print(self.startRow)
        self.pieceMoved = board[self.startRow][self.startCol]
        self.pieceCaptured = board[self.endRow][self.endCol]
        self.moveID = self.startRow*1000+self.startCol*100+self.endRow*10+self.endCol



    def __eq__(self,other):
        if isinstance(other,Move):
            return self.moveID == other.moveID
        return False
    def getChessNotation(self):
        return self.getRankFile(self.startRow, self.startCol) + self.getRankFile(self.endRow, self.endCol)

    def getRankFile(self, r, c):
        return self.colsToFiles[c] + self.rowsToRanks[r]

