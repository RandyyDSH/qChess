#ifndef PIECE_H
#define PIECE_H

//Библиотеки QT
#include <QStack>
#include <QList>

//Самописные классы
#include "statsnapshot.h"


/*!
 * \brief The piece class - класс шахматной фигуры
 */
class piece
{
public:
    /*!
     * \brief The type enum - типы фигур
     */
    enum type {PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING, NOTYPE};
    /*!
     * \brief The сolor enum - цвета фигур
     */
    enum color {BLACK, WHITE, NOCOLOR};

    /*!
     * \brief opponentColor - возвращает противоположный цвет
     * \param c - цвет
     * \return - противоположный цвет
     */
    inline static color opponentColor (color c) { return c == WHITE ? BLACK : WHITE; }

    /*!
     * \brief piece - конструктор
     * \param c - цвет
     * \param t - тип
     */
    piece(piece::color c, type t);

    // Методы доступа

    /*!
     * \brief getColor - вернуть цвет
     * \return -цвет
     */
    inline color getColor() const { return lColor; }
    /*!
     * \brief getType - вернуть тип
     * \return тип
     */
    inline type getType() const { return lType; }

    // Методы работы с классом

    /*!
     * \brief clearState - отчистка статистики
     */
    inline void clearState() { lStat.clear(); }

    /*!
     * \brief turnPawnToOtherPiece - превращает пешку в фигуру с заданным типом кроме короля
     * \param t - тип фигуры
     */
    void turnPawnToOtherPiece(type t);


protected:
    /*!
     * \brief lType - Тип фигуры
     */
    type    lType;
    /*!
     * \brief lColor - Цвет фигуры
     */
    color   lColor;
    /*!
     * \brief lStat - Хранит статистику
     */
    QStack<statSnapshot>    lStat;
};

#endif // PIECE_H
