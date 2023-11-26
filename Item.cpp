#include "Item.h"

ItemSlot Item::getSlot() const
{
    return slot;
}

void Item::setSlot(const ItemSlot &value)
{
    slot = value;
}

Item::Item()
{
    
}
