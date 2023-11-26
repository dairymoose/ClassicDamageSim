#ifndef ITEM_H
#define ITEM_H

#include <cstdint>
#include "ItemSlot.h"

class Item
{
protected:
    ItemSlot slot;
public:
    Item();
    virtual ~Item() {}
    ItemSlot getSlot() const;
    void setSlot(const ItemSlot &value);
};

#endif // ITEM_H
