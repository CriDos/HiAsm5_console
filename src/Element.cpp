//Project
#include "Element.h"
#include "StringList.h"

//STL

//Native

//GTKMM

//Qt


std::vector<double> dash(1, 4);

Gdk::Color lightColor;
Gdk::Color darkColor;
Gdk::Color hintColor;

TypePixbuf(*stockRender)(const ustring &name) = NULL;

ElementPoint *ElementPoints::getByName(const ustring &name)
{
    ustring sp = name.lowercase();
    for(iterator p = begin(); p != end(); ++p)
        if((*p)->name.lowercase() == sp)
            return *p;
    return NULL;
}

int ElementPoints::indexOf(ElementPoint *point)
{
    int i = 0;
    for(iterator p = begin(); p != end(); ++p)
        if(*p == point)
            return i;
        else
            i++;
    return -1;
}

int ElementPoints::indexOfType(ElementPoint *point)
{
    int i = 0;
    for(iterator p = begin(); p != end(); ++p)
        if((*p)->type == point->type) {
            i++;
            if(*p == point)
                break;
        }
    return i - 1;
}

//----------------------------------------------------------------------------------------

ElementProperty *ElementProperties::getByName(const ustring &name)
{
    ustring prop = name.lowercase();
    for(iterator p = begin(); p != end(); ++p)
        if((*p)->name.lowercase() == prop)
            return *p;
    return NULL;
}

ElementProperty *ElementProperties::getByPointName(const ustring &name)
{
    if(name.length() < 3) return NULL;

    ustring tmp(name.lowercase(), 2, name.length() - 2);
    for(iterator p = begin(); p != end(); ++p)
        if(((*p)->flag & VF_MAKEPOINT) && (*p)->name.lowercase() == tmp)
            return *p;
    return NULL;
}

void ElementProperties::change()
{
    for(iterator p = begin(); p != end(); ++p)
        (*p)->parent->on_change_property(*p);
}

void ElementProperties::free()
{
    for(iterator p = begin(); p != end(); ++p)
        delete(*p);
    clear();
}

void ElementProperties::setParent(Element *parent)
{
    for(iterator p = begin(); p != end(); ++p)
        (*p)->parent = parent;
}

//----------------------------------------------------------------------------------------

void LinkHints::addHint(int x, int y, int width, int height, ElementProperty *prop)
{
    LinkHint *h = new LinkHint(prop->parent);
    h->x = x;
    h->y = y;
    h->width = width;
    h->height = height;
    h->prop = prop;
    push_back(h);
}

void LinkHints::free()
{
    for(iterator h = begin(); h != end(); ++h)
        delete *h;
    clear();
}

void LinkHints::saveToText(ustring &text, const ustring offset)
{
    for(iterator h = begin(); h != end(); ++h)
        if((*h)->prop) {
            ustring pref = (*h)->parent->sysProps.getByName((*h)->prop->name) ? "@" : "";
            text += offset +  " AddHint(" + int_to_str((*h)->x) + "," +
                    int_to_str((*h)->y) + "," +
                    int_to_str((*h)->width) + "," +
                    int_to_str((*h)->height) + "," + pref + (*h)->prop->name + ")" + LINE_END;
        }
}


//----------------------------------------------------------------------------------------

void LinkHint::updateText()
{
    if(!prop) {
        text = "(empty)";
        return;
    }

    switch(prop->type) {
    case data_int:
        text = int_to_str(prop->readInt());
        break;
    case data_str:
    case data_list:
    case data_stock:
    case data_element:
        text = prop->readStr();
        break;
    case data_combo:
    case data_comboEx: {
        StringList list;
        list.split(prop->list.c_str(), ',');
        text = list[prop->readInt()];
        break;
    }
    case data_data: {
        TData *data = prop->readData();
        if(data->type == data_null)
            text = "NULL";
        else
            text = data->toStr();
        break;
    }
    default:
        text = "(no support)";
    }
}

void LinkHint::setWidth(int value)
{
    width = value;
}

//----------------------------------------------------------------------------------------

Element::Element(PackElement *pe, SDK *sdk, gdouble x, gdouble y):
    on_property_change(this, CBT_ELEMENT_PROP_CHANGE)
{
    TRACE_PROC
    this->x = x;
    this->y = y;
    parent = sdk;
    minWidth = 32;
    minHeight = 32;
    width = minWidth;
    height = minHeight;
    tpl = pe;
    this->sdk = NULL;
    flag = 0;
    cIndex = 0;
    elinkList = NULL;
    userData = NULL;

    buildFromTemplate();

    sysProps.push_back(new ElementProperty(this, "Hint", "Element description", data_list));
    ElementProperty *c = new ElementProperty(this, "Color", "Element color", data_color);
    sysProps.push_back(c);
    color = (Gdk::Color *)c->value;
    c->setAsDefault();
}

Element::~Element()
{
    // points
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        delete(*p);
    points.clear();

    // elinks
    if(elinkList) {
        if(isMainELink()) {
            props.setParent(*(++elinkList->begin()));
        }
        elinkList->remove(this);
        if(elinkList->size() == 1) {
            (*elinkList->begin())->elinkList = NULL;
            delete elinkList;
        }
        elinkList = NULL;
    } else {
        // properties
        props.free();
    }
    sysProps.free();

    hints.free();
}

void Element::initView()
{
    viewProp = tpl->conf->view.empty() ? NULL : props.getByName(tpl->conf->view);
    if(viewProp) {
        switch(viewProp->type) {
        case data_int:
            viewText = int_to_str(viewProp->readInt());
            break;
        case data_str:
            viewText = viewProp->readStr();
            break;
        case data_real:
            viewText = double_to_str(viewProp->readInt());
            break;
        case data_combo:
        case data_comboEx: {
            StringList l;
            l.split(viewProp->list.c_str(), ',');
            viewText = l[viewProp->readInt()];
            break;
            break;
        }
        default:
            viewText.clear();
        }
    }
}

void Element::changeColor()
{
    colorLight.set_rgb_p(min(1.0, color->get_red_p() + ECL_LIGHT),
                         min(1.0, color->get_green_p() + ECL_LIGHT),
                         min(1.0, color->get_blue_p() + ECL_LIGHT));
    colorDark.set_rgb_p(max(0.0, color->get_red_p() + ECL_DARK),
                        max(0.0, color->get_green_p() + ECL_DARK),
                        max(0.0, color->get_blue_p() + ECL_DARK));
}

void Element::clearLinks()
{
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        (*p)->clear();
}

ElementPoint *Element::addPoint(const ustring &name, const ustring &info, int type)
{
    ElementPoint *p = new ElementPoint(this, name, info, type);
    points.push_back(p);
    return p;
}

void Element::removePoint(ElementPoint *point)
{
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        if(*p == point) {
            points.erase(p);
            break;
        }
    delete point;
}

void Element::rePosPoints()
{
    reSize();
    rePosPointsWOsize();
}

void Element::rePosPointsWOsize()
{
    int count[4] = {0};
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p) {
        int *c = &count[(*p)->type - 1];
        (*p)->pos->x = x;
        (*p)->pos->y = y;
        switch((*p)->type) {
        case pt_event:
            (*p)->pos->x += width;
        case pt_work:
            (*p)->pos->y += POINT_SPACE + *c * POINT_SPACE - 1;
            break;
        case pt_var:
            (*p)->pos->y += height;
        case pt_data:
            (*p)->pos->x += POINT_SPACE + *c * POINT_SPACE - 1;
            break;
        }
        (*c)++;
    }
}

void Element::reSize()
{
    int count[4] = {0};
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        count[(*p)->type - 1] ++;

    width = max(minWidth, max(count[2], count[3]) * POINT_SPACE + POINT_OFF);
    height = max(minHeight, max(count[0], count[1]) * POINT_SPACE + POINT_OFF);
}

void Element::buildFromTemplate()
{
    // points ----------------------------------------------------
    for(ConfMethods::iterator mt = tpl->conf->mtd.begin(); mt != tpl->conf->mtd.end(); ++mt) {
        ElementPoint *p = addPoint((*mt)->name, (*mt)->info, (*mt)->mType);
        p->dataType = (*mt)->dataType;
    }
    rePosPoints();

    // propertys -------------------------------------------------
    for(ConfPropertys::iterator pt = tpl->conf->prop.begin(); pt != tpl->conf->prop.end(); ++pt) {
        ElementProperty *p = new ElementProperty(this, *pt);
        props.push_back(p);
    }
}

void Element::move(gdouble dx, gdouble dy)
{
    if(isFreeze()) return;

    x += dx;
    y += dy;

    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        (*p)->move(dx, dy);
}

void Element::injectElementAtLine(ElementPoint *pl1, ElementPoint *pl2, ElementPoint *pl3, PointPos *p1)
{
    ElementPoint *pl4 = pl1->point;
    PointPos *p2 = p1->next;

    PointPos *op;
    if(pl1->pos->next == pl4->pos)
        op = NULL;
    else
        op = pl1->pos->next;

    pl1->connect(pl2);

    if(op) {
        pl1->pos->next = op;
        p1->next = pl2->pos;
        pl2->pos->prev = p1;
    }

    if(pl4->pos->prev == pl1->pos)
        op = NULL;
    else
        op = pl4->pos->prev;

    pl4->connect(pl3);

    if(op) {
        pl4->pos->prev = op;
        p2->prev = pl3->pos;
        pl3->pos->next = p2;
        /*
        if EMouse.SelectState = selPointLine then
         if P1.X = P1.Prev.X then
           SDK.Last.Move(P1.Prev.X - pl2.Pos.X, 0, 0)
         else if P1.Y = P1.Prev.Y then
           SDK.Last.Move(0, P1.Prev.Y - pl2.Pos.Y, 0);
        */
    }
}

void Element::insertInLine(ElementPoint *point, PointPos *pos)
{
    TRACE_PROC
    if(point->type == pt_event) {
        ElementPoint *work = getFirstFreePoint(pt_work);
        ElementPoint *event = getFirstFreePoint(pt_event);
        if(!work || !event) {
            ElementPoint *sp = point->point;
            point->clear();
            if(work)  point->connect(work)->createPath();
            if(event) event->connect(sp)->createPath();
        } else {
            PointPos *p2 = pos->next;
            injectElementAtLine(point, work, event, pos);
            if(pos->y == p2->y)
                move(0, pos->y - event->pos->y);
        }
    }
    invalidate();
}

void Element::connectToPoint(ElementPoint *point)
{
    ElementPoint *fp = getFirstFreePoint(point->getPair());
    if(fp && point->canConnect(fp))
        point->connect(fp)->createPath();
}

void Element::remove()
{
    // do nothing
}

void Element::removeFromLine(bool we, bool vd)
{
    for(int i = 0; i < 2; i++)
        if((i == 0 && we) || (i == 1 && vd)) {
            int k = i * 2 + 1;
            ElementPoint *p1 = NULL, *p2 = NULL;

            for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
                if((*p)->point) {
                    if((*p)->type == k && p1 == NULL)
                        p1 = *p;
                    else if((*p)->type == (k + 1) && p2 == NULL)
                        p2 = *p;
                }

            if(p1 && p2) {
                PointPos *pp = ElementPoint::addLinePoint(NULL, 0, 0);
                PointPos *fp = pp;
                PointPos *pt = p1->point->pos;
                while(pt->next->next) {
                    fp = ElementPoint::addLinePoint(fp, pt->next->x, pt->next->y);
                    pt = pt->next;
                }
                pt = p2->pos;
                while(pt->next->next) {
                    fp = ElementPoint::addLinePoint(fp, pt->next->x, pt->next->y);
                    pt = pt->next;
                }
                ElementPoint *np1 = p1->point;
                ElementPoint *np2 = p2->point;
                p1->clear();
                p2->clear();
                np1->connect(np2);
                if(pp->next) {
                    np1->pos->next = pp->next;
                    pp->next->prev = np1->pos;
                    fp->next = np2->pos;
                    np2->pos->prev = fp;
                }
                delete pp;
            }
        }
}

ustring Element::getHint()
{
    ElementProperty *hint = sysProps.getByName("Hint");
    return hint ? hint->readStr() : ustring();
}

bool Test(gdouble v1, gdouble v2, gdouble tp)
{
    if(v2 < v1)
        return((v2 - 4 < tp) && (tp < v1 + 4));
    else
        return((v1 - 4 < tp) && (tp < v2 + 4));
}

int isLine(gdouble _x, gdouble _y, gdouble lx1, gdouble ly1, gdouble lx2, gdouble ly2)
{
    int p, k, C;

    if(Test(lx1, lx2, _x) && Test(ly1, ly2, _y)) {
        p = ly2 - ly1;
        k = lx2 - lx1;
        C = ly1 * k - lx1 * p;
        return abs(p * _x - k * _y + C);
    } else return 500;
}

bool Element::findLine(gdouble x, gdouble y, ObjectType *obj)
{
    PointPos *pb;
    void *tmp;

    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        if((*p)->pos->next) {
            pb = (*p)->pos;
            do {
                tmp = pb;
                if(isLine(x, y, pb->x, pb->y, pb->next->x, pb->next->y) < 200) {
                    *obj = ObjectType(pb, *p);
                    obj->type = SDK_OBJ_LINE;
                    return true;
                }
                pb = pb->next;
            } while(pb->next);
        }
    return false;
}

bool Element::getObjectAtPos(gdouble x, gdouble y, ObjectType *obj)
{
    if(flag & ELEMENT_FLG_IS_NOMOUSE)
        return false;

    PointPos *pb;
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        if((*p)->pos->next) {
            pb = (*p)->pos->next;
            while(pb->next)
                if((abs(pb->x - x) < POINT_OFF + 1) && (abs(pb->y - y) < POINT_OFF + 1)) {
                    *obj = ObjectType(pb, *p);
                    return true;
                } else pb = pb->next;
        }



    if(!(y < this->y - 3 || y > this->y + height + 3 || x < this->x - 3 || x > this->x + width + 3)) {
        for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
            if((abs((*p)->pos->x - x) <= POINT_OFF + 1) && (abs((*p)->pos->y - y) <= POINT_OFF + 1)) {
                *obj = ObjectType(*p);
                return true;
            }

        if(checkCollise(x, y)) {
            *obj = ObjectType(this);
            return true;
        }
    }
    return false;
}

bool Element::checkCollise(gdouble x, gdouble y)
{
    return (y >= this->y && y <= this->y + height && x >= this->x && x <= this->x + width);
}

bool Element::checkColliseRect(gdouble x1, gdouble y1, gdouble x2, gdouble y2)
{
    return !((x > x2) || (y > y2) || (x + width < x1) || (y + height < y1));
}

Gdk::Rectangle Element::drawRect()
{
    Gdk::Rectangle r(x - POINT_OFF, y - POINT_OFF, width + POINT_SPACE, height + POINT_SPACE);
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        r.join((*p)->drawRect());


    return r;
}

void Element::invalidate()
{
    //Gdk::Rectangle r = drawRect();
    //parent->on_redraw_rect.run(&r);
}

ElementPoint *Element::getFirstFreePoint(int type)
{
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        if((*p)->type == type && !(*p)->point)
            return *p;
    return NULL;
}

ElementPoint *Element::getLastFreePoint(int type)
{
    for(ElementPoints::reverse_iterator p = points.rbegin(); p != points.rend(); ++p)
        if((*p)->type == type)
            return (*p)->point ? NULL : *p;
    return NULL;
}

ElementPoint *Element::getLastPoint(int type)
{
    for(ElementPoints::reverse_iterator p = points.rbegin(); p != points.rend(); ++p)
        if((*p)->type == type)
            return *p;
    return NULL;
}

int Element::getPointsCount(int type)
{
    int c = 0;
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p)
        if((*p)->type == type)
            c++;
    return c;
}

void Element::saveToText(ustring &text, const ustring offset, int flags)
{
    text += offset + "Add(" + tpl->name + "," + int_to_str(id) + "," + int_to_str((int)x) + "," + int_to_str((int)y) + ")"LINE_END;
    text += offset + "{"LINE_END;
    if(isELink()) {
        if(isMainELink()) {
            if(elinkList->size() > 1)
                text += offset + "  @IsLib=true"LINE_END;
        } else
            text += offset + "  elink(" + int_to_str(getMainELink()->id) + ")"LINE_END;
    }
    saveSysParamsToText(text, offset, flags);
    saveParamsToText(text, offset, flags);
    savePointsToText(text, offset, flags);
    hints.saveToText(text, offset);
    text += offset + "}"LINE_END;
}

void Element::saveAsLink(ustring &text)
{
    text += "Add(" + tpl->name + "," + int_to_str(parent->getMSDK()->genNextID()) + "," + int_to_str((int)x) + "," + int_to_str((int)y) + ")"LINE_END;
    text += "{"LINE_END;
    text += "  elink(" + int_to_str(isELink() ? getMainELink()->id : id) + ")"LINE_END;
    saveSysParamsToText(text, "  ", ELEMENT_SAVE_CHANGED);
    saveParamsToText(text, "  ", ELEMENT_SAVE_CHANGED);
    text += "}"LINE_END;

    parent->getMSDK()->sdkLib.add(this);
}

void Element::saveParamsToText(ustring &text, const ustring offset, int flags)
{
    for(ElementProperties::iterator p = props.begin(); p != props.end(); ++p)
        if(!(flags & ELEMENT_SAVE_CHANGED) || !(*p)->isDefault())
            text += offset + " " + (*p)->name + "=" + (*p)->serialize() + LINE_END;
}

void Element::saveSysParamsToText(ustring &text, const ustring offset, int flags)
{
    for(ElementProperties::iterator p = sysProps.begin(); p != sysProps.end(); ++p)
        if(!(flags & ELEMENT_SAVE_CHANGED) || !(*p)->isDefault())
            text += offset + " @" + (*p)->name + "=" + (*p)->serialize() + LINE_END;
}

void Element::savePointsToText(ustring &text, const ustring offset, int flags)
{
    for(ElementPoints::iterator p = points.begin(); p != points.end(); ++p) {
        if(props.getByPointName((*p)->name) || tpl->conf->def.getByName((*p)->name))
            text += offset + " Point(" + (*p)->name + ")"LINE_END;

        if((*p)->point && (*p)->isPrimary() && (!(flags & ELEMENT_SAVE_SELECTED) || (*p)->point->parent->isSelect()))
            text += offset + " link(" + (*p)->name + "," +
                    int_to_str((*p)->point->parent->id) + ":" + (*p)->point->name + ",[" +
                    (*p)->serializePath() + "])"LINE_END;
    }
}

void Element::on_change_property(ElementProperty *prop)
{
    if(prop->value == color) {
        changeColor();
        invalidate();
    } else if(prop == viewProp) {
        initView();
        invalidate();
    }

    if(isMainELink()) {

    }


    on_property_change.run(prop);
}

Element *Element::getParentElement()
{
    if(sdk && sdk->elements.size() > 1) {
        auto it = sdk->elements.begin();
        ++it;
        return (*it)->isParent() ? *it : nullptr;
    }

    return NULL;
}

bool Element::checkInterfaceProvide(const ustring &name)
{
    //TODO correct check...
    return (tpl->conf->interfaces + ",").find(name + ",") != (std::string::size_type) - 1;
}

void Element::storePropPointers()
{
    initView();
}

void Element::createMainELink()
{
    if(elinkList) return;

    elinkList = new ElementsList();
    elinkList->push_back(this);
}

void Element::createELink(Element *owner)
{
    owner->createMainELink();
    owner->elinkList->push_back(this);
    elinkList = owner->elinkList;
    props.free();
    props.assign(owner->props.begin(), owner->props.end());
    storePropPointers();
}
