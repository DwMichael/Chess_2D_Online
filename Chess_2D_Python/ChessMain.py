"This class is for storing all the info about the current state of chess game. It will also be resposible for determining the void moves at curret"
"It will also keep a move log"
import pygame as p
import ChessEngine

WIDTH = HEIGHT = 720
DIMENSION = 8 #BOARD 8X8
SQ_SIZE =HEIGHT // DIMENSION
MAX_FPS = 15
IMAGES={}


#loading and transforming images
def loadImages():
    pieces = ["wieza_c","pionek_c","laufr_c","krol_c","kon_c","dama_c","wieza_b","pionek_b","laufr_b","krol_b","kon_b","dama_b"]
    for piece in pieces:
        IMAGES[piece]= p.transform.scale(p.image.load("images/"+piece+".png"),(SQ_SIZE,SQ_SIZE))

#User input and updating the graphics

def main():
    p.init()
    screen = p.display.set_mode((WIDTH,HEIGHT))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))
    gs = ChessEngine.GameState()
    loadImages() #only ones
    running = True
    sqSelected = ()
    playerClicks = [] #keep track of player clicks
    while running:
        for e in p.event.get():
            if e.type == p.QUIT:
                running =False
            elif e.type == p.MOUSEBUTTONDOWN:
                location = p.mouse.get_pos() #(x,y) location of mouse
                col = location[0] // SQ_SIZE
                row = location[1] // SQ_SIZE
                if sqSelected == (row, col):
                    sqSelected = ()
                    playerClicks = []
                else:
                    sqSelected = (row, col)
                    playerClicks.append(sqSelected)
                if len(playerClicks) == 2:
                    move = ChessEngine.Move(playerClicks[0],playerClicks[1], gs.board)
                    print(move.getChessNotation())
                    gs.makeMove(move)
                    sqSelected = () #reset user click
                    playerClicks = []

        drawGameState(screen, gs)
        clock.tick(MAX_FPS)
        p.display.flip()

#function that drawing all the graphics within game
def drawGameState(screen, gs):
    drawBoard(screen) #draw squares on board
    #add in piece highlighting or move suggestions
    drawPieces(screen, gs.board)#draw pieces on board



def drawBoard(screen):
    colors = [p.Color("white"), p.Color("black")]
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            color = colors[((r+c) % 2)]
            p.draw.rect(screen, color, p.Rect(c*SQ_SIZE,r*SQ_SIZE,SQ_SIZE,SQ_SIZE))


def drawPieces(screen, board ):
    for r in range(DIMENSION):
        for c in range(DIMENSION):
            piece = board[r][c]
            if piece != '--': #not empty square
                screen.blit(IMAGES[piece],p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE,SQ_SIZE))

if __name__ == "__main__":
    main()