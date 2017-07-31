#ifndef GOODSCAR_H
#define GOODSCAR_H
#include <QString>


class GoodsCar
{
public:
    GoodsCar();
    GoodsCar(GoodsCar& car);
    QString rfid;
    QString listId;
};

#endif // GOODSCAR_H
