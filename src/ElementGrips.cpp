//Project
#include "ElementGrips.h"

//STL

//Native

//GTKMM

//Qt

ElementGrip::~ElementGrip()
{
    clear();
}

void ElementGrip::clear()
{
    for(GripList::iterator g = grips.begin(); g != grips.end(); g++)
        delete *g;
    grips.clear();
}

GripInfo *ElementGrip::addGrip(gdouble x, gdouble y, int width, int height, int sx, int sy, int cursor)
{
    GripInfo *g = new GripInfo();
    g->x = x;
    g->y = y;
    g->width = width;
    g->height = height;
    g->stepx = sx;
    g->stepy = sy;
    g->tag = 0;
    g->cursor = cursor;

    grips.push_back(g);
    return g;
}
