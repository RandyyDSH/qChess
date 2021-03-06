#ifndef CHESSGAMESTATE_H
#define CHESSGAMESTATE_H

//Библиотеки QT
#include <QPair>

//Самописные классы
#include "board.h"

/*!
 * \brief The chessGameState class - класс описывающий игровое соостояние
 */
class chessGameState
{
public:
    chessGameState();
    /*!
     * \brief init - приводит игровую статистику к стартовому состаянию
     */
    void init();
    /*!
     * \brief makeMove - сделать ход
     * \param bm - ход
     */
    void makeMove(const boardMove &bm);
    /*!
     * \brief getBoard - получить доску
     * \return доска
     */
    inline board getBoard() const { return lBoard; }
    /*!
     * \brief getLastMove - получить последний совершенный ход
     * \return - ход
     */
    inline boardMove getLastMove() const { return lLastMove; }
    /*!
     * \brief getStepNumber - получить номер хода
     * \return номер хода
     */
    inline int getStepNumber() const { return lStepNumber; }
    /*!
     * \brief isChek - шах на доске
     * \return является ли положение шаховым
     */
    inline bool isChek() const { return lIsCheck; }
    /*!
      * \brief isWhiteStep - какой цвет сходил в последний раз
      * \return - был ли это белый цвет?
      */
     inline bool isWhiteStep() const { return lIsWhiteStep; }
     /*!
      * \brief getColor - вернуть цвет хода
      * \return цвет
      */
     inline piece::color getColor() const { return (lIsWhiteStep) ? piece::WHITE : piece::BLACK; }
     /*!
      * \brief isPositionSelectable - можно ли эту клетку
      * \param bp - позиция
      * \return можно ли
      */
     bool isPositionSelectable(const boardPosition& bp) const;
private:
     /*!
     * \brief lThreefoldCount - История "бесполезных" ходов
     */
    QList<QPair<serialBoard, int> > lThreefoldCount;
    /*!
     * \brief lPieces - Фигуры на 'хеше'
     */
    piece* lPieces[64];
    /*!
     * \brief lBoard - Доска
     */
    board lBoard;
    /*!
     * \brief lLastMove - Последний сделаный ход
     */
    boardMove lLastMove;
    /*!
     * \brief lIsWhiteStep - Цвет
     */
    bool lIsWhiteStep;
    /*!
     * \brief lIsCheck - Шах
     */
    bool lIsCheck;
    /*!
     * \brief isThreefold - Удавлетворяет ли ход правилу 50 ходов
     */
    bool isThreefold;
    /*!
     * \brief lStepNumber - Номер хода
     */
    int lStepNumber;
    /*!
     * \brief l50Moves - Счетчик для правила 50 ходов
     */
    int l50Moves;
};

#endif // CHESSGAMESTATE_H
