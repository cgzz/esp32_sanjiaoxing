#include "my_io.h"

TwoWire myWire = TwoWire(0);

void my_io_init()
{
    myWire.begin(19, 18, 400000);
    myWire.setClock(400000);
}
