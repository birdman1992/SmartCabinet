#include "goodscar.h"

GoodsCar::GoodsCar()
{

}

GoodsCar::GoodsCar(GoodsCar& car)
{
    rfid = car.rfid;
    listId = car.listId;
}
