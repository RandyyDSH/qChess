#include "chessplayer.h"

chessPlayer *chessPlayer::create(QString typeName)
{
    if (typeName == "AI"){
        return (chessPlayer*)(new AI());
    }
    else if (typeName == "human"){
        return (chessPlayer*)(new human());
    }
    return nullptr;
}

chessPlayer::~chessPlayer()
{

}

void chessPlayer::think(const chessGameState &cgs)
{
    cgs.getLastMove();
}

chessPlayer::chessPlayer(QObject *p) : QObject(p), lIsWhite(false), lIsThinking(false), lIsHuman(false), lIsTrustworthy(false)
{
}



void human::newGame()
{

}

void human::startGame()
{

}

bool human::needMove()
{
    return true;
}

void human::undoMove()
{

}

void AI::newGame()
{

}

void AI::startGame()
{

}


void AI::think(const chessGameState &cgs)
{
    thPl = new AI();
    *thPl = *this;
    thPl->b = cgs.getBoard();
    th = new QThread;
    thPl->moveToThread(th);
    connect(th, SIGNAL(started()), thPl, SLOT(aiPlay()));
    //connect(th, SIGNAL(finished()), thPl, SLOT(quit()));
    connect(thPl, SIGNAL(haveMove()), this, SLOT(quit()));
    connect(th, SIGNAL(finished()), thPl, SLOT(deleteLater()));
    connect(th, SIGNAL(finished()), th, SLOT(deleteLater()));
    th->start();
}

bool AI::needMove()
{
    return true;
}

void AI::undoMove()
{

}

AI &AI::operator =(const AI &o)
{
    this->lIsWhite = o.lIsWhite;
    this->lIsHuman = o.lIsHuman;
    this->lIsTrustworthy = o.lIsTrustworthy;
    this->lIsThinking = o.lIsThinking;
    this->lPly = o.lPly;
    return *this;
}

int AI::evaluateBoard(const board &b, piece::color c)
{
    QList<boardPosition>  locations[6];

    int balance = 0;
    int endgamecount = 0;

    int queenval = 900, rookval = 500, bishopval = 325, knightval = 310, pawnval = 100, kingval = 100000;

    for(int i=0; i < 64; i++) {
        boardPosition bp = boardPosition(i);
        if(b.isOccupied(bp)) {
            piece *p = b.getPiece(bp);
            switch(p->getType()) {
            case piece::QUEEN:
                locations[piece::QUEEN].append(bp);
                balance += (p->getColor() == c) ? queenval : -queenval;
                endgamecount += queenval;
                break;
            case piece::ROOK:
                locations[piece::ROOK].append(bp);
                balance += (p->getColor() == c) ? rookval : -rookval;
                endgamecount += rookval;
                break;
            case piece::BISHOP:
                locations[piece::BISHOP].append(bp);
                balance += (p->getColor() == c) ? bishopval : -bishopval;
                endgamecount += bishopval;
                break;
            case piece::KNIGHT:
                locations[piece::KNIGHT].append(bp);
                balance += (p->getColor() == c) ? knightval : -knightval;
                endgamecount += knightval;
                break;
            case piece::PAWN:
                locations[piece::PAWN].append(bp);
                balance += (p->getColor() == c) ? pawnval : -pawnval;
                endgamecount += pawnval;
                break;
            case piece::KING:
                locations[piece::KING].append(bp);
                if ((p->getColor() == c) && !b.isMate(c)) {
                    balance += kingval;
                }
                else if ((p->getColor() != c) && !b.isMate(c)) {
                    balance -= kingval;
                }
                break;
            default:
                break;
            }
        }
    }


    bool endgame = false;
    if (endgamecount < 3500) {
        endgame = true;
    }

    for(int i=0; i < locations[piece::QUEEN].size(); i++) {
        balance += queenBonus(locations[piece::QUEEN][i], b, c, endgame);
    }
    for(int i=0; i < locations[piece::ROOK].size(); i++) {
        balance += rookBonus(locations[piece::ROOK][i], b);
    }
    for(int i=0; i < locations[piece::BISHOP].size(); i++) {
        balance += bishopBonus(locations[piece::BISHOP][i], b, c);
    }
    for(int i=0; i < locations[piece::KNIGHT].size(); i++) {
        balance += knightBonus(locations[piece::KNIGHT][i], b, c);
    }
    for(int i=0; i < locations[piece::PAWN].size(); i++) {
        balance += pawnBonus(locations[piece::PAWN][i], b, c);
    }
    for(int i=0; i < locations[piece::KING].size(); i++) {
        balance += kingBonus(locations[piece::KING][i], b, c, endgame);
    }

    return balance;
}

int AI::search(board b, piece::color c, int depth, int alpha, int beta, boardMove &bm)
{
    boardMove testMove;
    board testBoard = b;
    int moveScore, bestScore = -INT_MAX;
    QList<boardMove> moves = b.getLegalMoves(c);

    bool gotmove = false;

    for(int i=0; i < moves.size(); i++) {
        if(!gotmove) {
            bm = moves[i];
            gotmove = true;
        }

        testBoard = b;
        testBoard.update(moves[i]);

        if(depth == 0) {
            moveScore = evaluateBoard(testBoard, c);
        } else {
            moveScore = -search(testBoard, piece::opponentColor(c), depth-1, -beta, -alpha, testMove);
        }

        if(moveScore > bestScore) {
            bestScore = moveScore;
            bm = moves[i];
        }
        if(bestScore > alpha) {
            alpha = bestScore;
        }
        if(alpha >= beta) {
            return beta;
        }
    }

    return bestScore;
}

int AI::pawnBonus(const boardPosition &bp, const board &b, piece::color turn)
{
    piece::color c = b.getPiece(bp)->getColor();
    int bonus = (c == piece::WHITE) ? m_wpawn[bp.number()] : m_bpawn[bp.number()];

    if(isIsolatedPawn(bp, b)) {
        char file = bp.xToChar();
        if(file == 'a' || file == 'h') {
            bonus -= 12;
        } else if (file == 'b' || file == 'g') {
            bonus -= 14;
        } else if (file == 'c' || file == 'f') {
            bonus -= 16;
        } else if (file == 'd' || file == 'e') {
            bonus -= 20;
        }
    }
    /*
    if(isDoubledPawn(bp, board)) {
    }
    */
    return (turn == c) ? bonus : -bonus;
}

int AI::knightBonus(const boardPosition &bp, const board &b, piece::color turn)
{
    piece::color c = b.getPiece(bp)->getColor();
    int bonus = m_knight[bp.number()];

    return (turn == c) ? bonus : -bonus;
}

int AI::bishopBonus(const boardPosition &bp, const board &b, piece::color turn)
{
    piece::color c = b.getPiece(bp)->getColor();
    int bonus = m_bishop[bp.number()];

    return (turn == c) ? bonus : -bonus;
}

int AI::rookBonus(const boardPosition &bp, const board &b)
{
    int bonus = 0;

    mask rookAttacks = board::yAttacks[bp.number()][b.getYState(bp)] ||
            board::xAttacks[bp.number()][b.getXState(bp)];
    int numAttacked = numAttackedSquares(rookAttacks);
    bonus += (numAttacked < 12) ? 2*numAttacked-4 : 20;

    return bonus;
}

int AI::queenBonus(const boardPosition &bp, const board &b, piece::color turn, bool endgame)
{
    if (!endgame) {
        return 0;
    }

    piece * p = b.getPiece(bp);
    int bonus = 0;
    boardMove dist;

    if (p->getColor() == piece::WHITE) {
        dist = boardMove(bp, b.getKing(piece::BLACK), p);
    }
    else {
        dist = boardMove(bp, b.getKing(piece::WHITE), p);
    }

    bonus = -2*abs(dist.xDiff()+dist.yDiff());

    return (turn == p->getColor()) ? bonus : -bonus;
}

int AI::kingBonus(const boardPosition &bp, const board &b, piece::color turn, bool endgame)
{
    piece::color c = b.getPiece(bp)->getColor();
    int bonus = 0;

    mask attacked = b.isAttacked(bp, c);
    for(int i=0; i < 64; i++) {
        bonus -= 25*((attacked >> i) & 1);
    }

    if(endgame) {
        bonus = m_end_king[bp.number()];
    } else {
        bonus = (c == piece::WHITE) ? m_wking[bp.number()] : m_bking[bp.number()];
    }

    return (turn == c) ? bonus : -bonus;
}

int AI::numAttackedSquares(const mask &pieceAttacks)
{
    int numSquares = 0;
    for (int i = 0; i < 64; i++) {
        if (pieceAttacks && (1LL << i)) {
            numSquares++;
        }
    }
    return numSquares;
}

bool AI::isIsolatedPawn(const boardPosition &bp, const board &b)
{
    piece::color c = b.getPiece(bp)->getColor();
    mask m = b.lColors[c] & b.lPieces[piece::PAWN];

    boardPosition test = bp.getTop();
    if(test.isValid() && (m & bitBoard::getXMask(test))) {
        return false;
    }

    test = bp.getBottom();
    if(test.isValid() && (m & bitBoard::getXMask(test))) {
        return false;
    }

    return true;
}

bool AI::isDoubledPawn(const boardPosition &bp, const board &b)
{
    b.getKing(piece::WHITE);
    bp.getBottom();
    return false;
}

void AI::aiPlay()
{
    boardMove move;
    search(b, getColor(), lPly, -INT_MAX, INT_MAX, move);
    lMove  = move;
    emit haveMove();
}

void AI::quit()
{
    lMove = thPl->lMove;
    emit haveMove();
}

int AI::m_bishop[64] = {
    -5,-5,-5,-5,-5,-5,-5,-5,
    -5,10,5,10,10,5,10,-5,
    -5,5,3,12,12,3,5,-5,
    -5,3,12,3,3,12,3,-5,
    -5,3,12,3,3,12,3,-5,
    -5,5,3,12,12,3,5,-5,
    -5,10,5,10,10,5,10,-5,
    -5,-5,-5,-5,-5,-5,-5,-5 };

int AI::m_knight[64] = {
    -10,-5,-5,-5,-5,-5,-5,-10,
    -5,0,0,3,3,0,0,-5,
    -5,0,5,5,5,5,0,-5,
    -5,0,5,10,10,5,0,-5,
    -5,0,5,10,10,5,0,-5,
    -5,0,5,5,5,5,0,-5,
    -5,0,0,3,3,0,0,-5,
    -10,-5,-5,-5,-5,-5,-5,-10 };

int AI::m_wpawn[64] = {
    0,0,0,0,0,0,0,0,
    0,0,0,-5,-5,0,0,0,
    1,2,3,4,4,3,2,1,
    2,4,6,8,8,6,4,2,
    3,6,9,12,12,9,6,3,
    4,8,12,16,16,12,8,4,
    5,10,15,20,20,15,10,5,
    0,0,0,0,0,0,0,0 };

int AI::m_bpawn[64] = {
    0,0,0,0,0,0,0,0,
    5,10,15,20,20,15,10,5,
    4,8,12,16,16,12,8,4,
    3,6,9,12,12,9,6,3,
    2,4,6,8,8,6,4,2,
    1,2,3,4,4,3,2,1,
    0,0,0,-5,-5,0,0,0,
    0,0,0,0,0,0,0,0 };

int AI::m_wking[64] = {
    2,10,4,0,0,7,10,2,
    -3,-3,-5,-5,-5,-5,-3,-3,
    -5,-5,-8,-8,-8,-8,-5,-5,
    -8,-8,-13,-13,-13,-13,-8,-8,
    -13,-13,-21,-21,-21,-21,-13,-13,
    -21,-21,-34,-34,-34,-34,-21,-21,
    -34,-34,-55,-55,-55,-55,-34,-34,
    -55,-55,-89,-89,-89,-89,-55,-55};

int AI::m_bking[64] = {
    -55,-55,-89,-89,-89,-89,-55,-55,
    -34,-34,-55,-55,-55,-55,-34,-34,
    -21,-21,-34,-34,-34,-34,-21,-21,
    -13,-13,-21,-21,-21,-21,-13,-13,
    -8,-8,-13,-13,-13,-13,-8,-8,
    -5,-5,-8,-8,-8,-8,-5,-5,
    -3,-3,-5,-5,-5,-5,-3,-3,
    2,10,4,0,0,7,10,2};

int AI::m_end_king[64] = {
    -5,-3,-1,0,0,-1,-3,-5,
    -3,5,5,5,5,5,5,-3,
    -1,5,10,10,10,10,5,-1,
    0,5,10,15,15,10,5,0,
    0,5,10,15,15,10,5,0,
    -1,5,10,10,10,10,5,-1,
    -3,5,5,5,5,5,5,-3,
    -5,-3,-1,0,0,-1,-3,-5};
