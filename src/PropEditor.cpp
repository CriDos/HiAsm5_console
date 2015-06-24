//STL

//Native

//GTKMM

//Qt

//Project
#include "PropEditor.h"
#include "StringList.h"

#define FONT_SIZE 11

const char *FONT_NAME = "arial";

PropertyItem::PropertyItem(const ustring &name)
{
    this->name = name;
    this->caption = name;
    value = NULL;
    checked = false;
    group = NULL;
    opened = false;
    flag = 0;
    data = NULL;
}

PropertyItem::~PropertyItem()
{
    for(Properties::iterator pi = items.begin(); pi != items.end(); pi++)
        delete *pi;
    items.clear();
}

void PropertyItem::setParent(PropEditor *editor)
{
    parent = editor;
    for(Properties::iterator pi = items.begin(); pi != items.end(); pi++)
        (*pi)->setParent(editor);
}

int PropertyItem::getLevel()
{
    int l = 0;
    PropertyItem *g = this;
    while((g = g->group)) l++;
    return l;
}

void PropertyItem::draw(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    g_assert(parent);

    Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
    dc->move_to(1, y + 20);
    dc->line_to(parent->get_width() - 1, y + 20);
    dc->stroke();

    if(flag & PROP_FLG_CHECK) {
        Gdk::Cairo::set_source_color(dc, style->get_light(Gtk::STATE_NORMAL));
        dc->rectangle(x + 4, y + 5, 9, 9);
        dc->fill();
        Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
        dc->rectangle(x + 4, y + 5, 9, 9);
        dc->stroke();
        if(checked) {
            Gdk::Cairo::set_source_color(dc, style->get_base(Gtk::STATE_SELECTED));
            dc->rectangle(x + 6, y + 7, 5, 5);
            dc->fill();
            dc->rectangle(x + 6, y + 7, 5, 5);
            dc->stroke();
        }
        /*
        style->paint_check(parent->get_window(),
                Gtk::STATE_NORMAL,
                checked ? Gtk::SHADOW_IN : Gtk::SHADOW_OUT,
                Gdk::Rectangle(x + 3, y + 3, 14, 14),
                *parent, ustring(), x + 3, y + 3, 14, 14);
        */
    }
    int c_off = x + 18;

    // caption
    Gdk::Cairo::set_source_color(dc, style->get_text(Gtk::STATE_NORMAL));
    dc->save();
    dc->rectangle(0, y + 1, parent->get_width() / 2, 20);
    dc->clip();
    dc->move_to(c_off, y + 14);

    //Cairo::RefPtr<const Cairo::ToyFontFace> ff = dc->get_font_face();
    dc->select_font_face(FONT_NAME, Cairo::FONT_SLANT_NORMAL, flag & PROP_FLG_MAIN ? Cairo::FONT_WEIGHT_BOLD : Cairo::FONT_WEIGHT_NORMAL);
    dc->show_text(caption);
    dc->restore();

    //Gdk::Cairo::set_source_color(dc, style->get_bg(Gtk::STATE_NORMAL));
    //dc->rectangle(parent->get_width() / 2 - 1, y + 1, parent->get_width() / 2, 12);
    //dc->fill();

    // value
    dc->set_source_rgb(0.0, 0.0, 0.5);
    drawValue(dc, style, parent->get_width() / 2 + 4, y);

    drawSelect(dc, style, x, y);

    // slider
    Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
    double rx = parent->get_width() / 2;
    dc->move_to(rx, y);
    dc->line_to(rx, y + 20);
    dc->stroke();
    Gdk::Cairo::set_source_color(dc, style->get_light(Gtk::STATE_NORMAL));
    dc->move_to(rx + 1, y);
    dc->line_to(rx + 1, y + 20);
    dc->stroke();
}

void PropertyItem::drawValue(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    dc->move_to(x, y + 14);
    //Cairo::RefPtr<const Cairo::ToyFontFace> ff = dc->get_font_face();
    dc->select_font_face(FONT_NAME, Cairo::FONT_SLANT_NORMAL, flag & PROP_FLG_NDEF ? Cairo::FONT_WEIGHT_BOLD : Cairo::FONT_WEIGHT_NORMAL);
    dc->show_text(editText());
}

void PropertyItem::drawSelect(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    if(this == parent->select) {
        Gdk::Cairo::set_source_color(dc, style->get_black());
        dc->move_to(0, y + 19);
        dc->line_to(0, y + 1);
        dc->line_to(parent->get_width() - 2, y + 1);
        dc->stroke();
        Gdk::Cairo::set_source_color(dc, style->get_light(Gtk::STATE_NORMAL));
        dc->move_to(parent->get_width() - 2, y + 1);
        dc->line_to(parent->get_width() - 2, y + 19);
        dc->line_to(0, y + 19);
        dc->stroke();
    }
}

PropertyItem *PropertyItem::add(PropertyItem *item)
{
    items.push_back(item);
    item->parent = parent;
    item->group = this;
    return item;
}

void PropertyItem::open()
{
    opened = !opened;
    parent->set_size_request(150, parent->itemsHeight(parent->props) + 1);
}

//----------------------------------------------------------------------------------

PropertyGroup::PropertyGroup(const ustring &name): PropertyItem(name)
{
    //this->info = info;
    // parse info
    int i = name.find('=', 0);
    caption = ustring(name, 0, i);
    info = ustring(name, i + 1);
}

void PropertyGroup::draw(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
    dc->move_to(1, y + 20);
    dc->line_to(parent->get_width() - 1, y + 20);
    dc->stroke();

    /*
    style->paint_check(parent->get_window(),
                    Gtk::STATE_NORMAL,
                    Gtk::SHADOW_OUT,
                    Gdk::Rectangle(x + 3, y + 3, 13, 13),
                    *parent, ustring(), x + 3, y + 3, 13, 13);
    */
    Gdk::Cairo::set_source_color(dc, style->get_light(Gtk::STATE_NORMAL));
    dc->rectangle(x + 4, y + 5, 8, 8);
    dc->fill();
    Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
    dc->rectangle(x + 4, y + 5, 8, 8);
    dc->stroke();

    if(!opened) {
        dc->move_to(x + 8, y + 7);
        dc->rel_line_to(0, 5);
    }
    dc->move_to(x + 6, y + 9);
    dc->rel_line_to(5, 0);
    dc->stroke();

    bool def = false;
    for(Properties::iterator p = items.begin(); p != items.end() && !def; p++)
        if((*p)->flag & PROP_FLG_NDEF)
            def = true;
    // caption
    Gdk::Cairo::set_source_color(dc, style->get_text(Gtk::STATE_NORMAL));
    dc->move_to(x + 18, y + 14);
    //Cairo::RefPtr<const Cairo::ToyFontFace> ff = dc->get_font_face();
    dc->select_font_face(FONT_NAME, Cairo::FONT_SLANT_NORMAL, def ? Cairo::FONT_WEIGHT_BOLD : Cairo::FONT_WEIGHT_NORMAL);
    dc->show_text(caption);

    drawSelect(dc, style, x, y);
}

//----------------------------------------------------------------------------------

PropertyTitle::PropertyTitle(const ustring &caption, const ustring &info): PropertyItem(caption)
{
    this->info = info;
}

void PropertyTitle::draw(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    Gdk::Cairo::set_source_color(dc, style->get_dark(Gtk::STATE_NORMAL));
    dc->move_to(x + 1, y + 20);
    dc->line_to(parent->get_width() - 1, y + 20);
    dc->stroke();

    // caption
    Gdk::Cairo::set_source_color(dc, style->get_text(Gtk::STATE_NORMAL));
    dc->move_to(x + 4, y + 14);
    //Cairo::RefPtr<const Cairo::ToyFontFace> ff = dc->get_font_face();
    dc->select_font_face(FONT_NAME, Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
    dc->show_text(caption);

    drawSelect(dc, style, x, y);
}

//----------------------------------------------------------------------------------

PropertyInteger::PropertyInteger(const ustring &name, void *value): PropertyItem(name)
{
    type = data_int;
    this->value = value;
    flag = PROP_FLG_EDIT;
}

PropertyString::PropertyString(const ustring &name, void *value): PropertyItem(name)
{
    type = data_str;
    this->value = value;
    flag = PROP_FLG_EDIT | PROP_FLG_BTN;
}

PropertyDouble::PropertyDouble(const ustring &name, void *value): PropertyItem(name)
{
    type = data_int;
    this->value = value;
    flag = PROP_FLG_EDIT;
}

PropertyEnum::PropertyEnum(const ustring &name, void *value, const ustring &list): PropertyItem(name)
{
    type = data_combo;
    this->value = value;
    flag = PROP_FLG_EDIT | PROP_FLG_BTN;

    const char *buf = list.c_str();
    const char *c = buf;
    while(*buf) {
        if(*buf == ',') {
            this->list.push_back(ustring(c, buf - c));
            c = buf + 1;
        }
        buf++;
    }
    this->list.push_back(c);
}

void PropertyEnum::setValue(const ustring &value)
{
    int i = 0;
    for(std::vector<ustring>::iterator l = list.begin(); l != list.end(); l++) {
        if(*l == value) {
            *(int *)this->value = i;
            return;
        }
        i++;
    }
}

PropertyEnumText::PropertyEnumText(const ustring &name, void *value, const ustring &list):
    PropertyEnum(name, value, list)
{

}

void PropertyEnumText::setValue(const ustring &value)
{
    for(std::vector<ustring>::iterator l = list.begin(); l != list.end(); l++) {
        if(*l == value) {
            *(ustring *)this->value = value;
            return;
        }
    }
}

int PropertyEnumText::getSelectIndex()
{
    int i = 0;
    for(std::vector<ustring>::iterator l = list.begin(); l != list.end(); l++) {
        if(*l == *(ustring *)value)
            return i;
        i++;
    }

    return 0;
}

PropertyTData::PropertyTData(const ustring &name, void *value): PropertyItem(name)
{
    type = data_data;
    this->value = value;
    flag = PROP_FLG_EDIT;
}

void PropertyTData::setValue(const ustring &value)
{
    if(value.empty())
        ((TData *)this->value)->clear();
    else if(value[0] == '#')
        *(TData *)this->value = ustring(value, 1, value.length() - 1);
    else if(value.find(',') != (std::string::size_type) - 1 || value.find('.') != (std::string::size_type) - 1) {
        *(TData *)this->value = atof(value.c_str());
    } else
        *(TData *)this->value = atoi(value.c_str());
}

const gchar *emptyName = "(empty)";

PropertyStock::PropertyStock(const ustring &name, void *value): PropertyItem(name)
{
    type = data_stock;
    this->value = value;
    flag = PROP_FLG_EDIT | PROP_FLG_BTN;
}

void PropertyStock::drawValue(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    if(!(*(ustring *)value).empty()) {
        TypePixbuf pix = parent->render_icon(StockID(*(ustring *)value), ICON_SIZE_MENU, ustring());

        if(pix) {
            dc->save();
            dc->translate(x, y + 2);
            Gdk::Cairo::set_source_pixbuf(dc, pix, 0.0, 0.0);
            dc->rectangle(0, 0, 16, 16);
            dc->clip();
            dc->paint();
            dc->restore();

            x += 18;
        } else
            dc->set_source_rgb(1.0, 0.0, 0.0);
    }
    PropertyItem::drawValue(dc, style, x, y);
}

PropertyPixbuf::PropertyPixbuf(const ustring &name, void *value): PropertyItem(name)
{
    type = data_pixbuf;
    this->value = value;
    flag = PROP_FLG_BTN;
}

PropertyColor::PropertyColor(const ustring &name, void *value): PropertyItem(name)
{
    type = data_color;
    this->value = value;
    flag = PROP_FLG_EDIT | PROP_FLG_BTN;
}

void PropertyColor::drawValue(DrawContext dc, Glib::RefPtr<Style> style, int x, int y)
{
    Gdk::Color c = *(Gdk::Color *)value;

    dc->set_source_rgb(c.get_red_p(), c.get_green_p(), c.get_blue_p());
    dc->rectangle(x + 1, y + 3, 12, 12);
    dc->fill();

    dc->set_source_rgb(1.0 - c.get_red_p(), 1.0 -  c.get_green_p(), 1.0 -  c.get_blue_p());
    dc->rectangle(x + 1, y + 3, 12, 12);
    dc->stroke();

    dc->set_source_rgb(0, 0, 0.5);

    PropertyItem::drawValue(dc, style, x + 16, y);
}

PropertyArray::PropertyArray(const ustring &name, void *value): PropertyItem(name)
{
    type = data_array;
    this->value = value;
    flag = PROP_FLG_BTN;
}

//----------------------------------------------------------------------------------

PropertyFont::PropertyFont(const ustring &name, void *value): PropertyGroup(name)
{
    type = data_font;
    this->value = value;
    flag = PROP_FLG_BTN;

    FontRecord *font = (FontRecord *)value;
    PropertyItem *p;
    p = new PropertyString("Name", &font->name);
    add(p);
    p = new PropertyInteger("Size", &font->size);
    add(p);
    p = new PropertyInteger("Style", &font->style);
    add(p);
    p = new PropertyInteger("Color", &font->color);
    add(p);
}

//----------------------------------------------------------------------------------

PropertyFlags::PropertyFlags(const ustring &name, void *value, const ustring &list): PropertyGroup(name)
{
    type = data_flags;
    this->value = value;

    StringList flg_list;
    flg_list.split(list.c_str(), ',');
    for(unsigned int i = 0; i < flg_list.size(); i++) {
        *(int *)add(new PropertyFlagsItem(flg_list[i], this, i))->value = ((*(int *)value >> i) & 1) ? 1 : 0;
    }
}

ustring PropertyFlags::editText()
{
    return "[FLAGS]";
}

void PropertyFlags::_change(PropertyFlagsItem *prop)
{
    *(int *)value = (*(int *)value & ~(1 << prop->shift)) | (prop->bit() << prop->shift);
    parent->on_change_property.run(this);
}

//-------------------------------------------------------------------------------------------

PropEditor::PropEditor():
    on_change_property(this, CBT_CHANGE_PROPERTY),
    on_select_property(this, CBT_SELECT_PROPERTY),
    on_check_property(this, CBT_CHECK_PROPERTY),
    on_edit_property(this, CBT_EDIT_PROPERTY)
{
    set_events(Gdk::BUTTON_PRESS_MASK | Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_RELEASE_MASK);
    set_size_request(150, 20);

    select = NULL;
    edit = NULL;
    downButton = 0;
}

PropEditor::~PropEditor()
{
    clear();
}

void PropEditor::add(PropertyItem *p)
{
    props.push_back(p);
    p->setParent(this);
    set_size_request(150, itemsHeight(props) + 1);
}

int PropEditor::itemsHeight(Properties &items)
{
    int h = 0;
    for(Properties::iterator p = items.begin(); p != items.end(); p++) {
        h += 20;
        if((*p)->opened)
            h += itemsHeight((*p)->items);
    }
    return h;
}

void PropEditor::reDraw()
{
    get_window()->invalidate(false);
}

void PropEditor::clear()
{
    if(props.empty()) return;

    for(Properties::iterator p = props.begin(); p != props.end(); p++)
        delete *p;
    props.clear();
    set_size_request(150, 20);
    edit->set_visible(false);
    select = NULL;
}

void PropEditor::drawProperties(DrawContext dc, Properties &items, int x, int &y)
{
    for(Properties::iterator p = items.begin(); p != items.end(); p++) {
        (*p)->draw(dc, get_style(), x, y);
        y += 20;
        if((*p)->opened)
            drawProperties(dc, (*p)->items, x + 14, y);
    }
}

bool PropEditor::on_expose_event(GdkEventExpose *event)
{
    DrawContext cr = get_window()->create_cairo_context();

    cr->set_antialias(Cairo::ANTIALIAS_NONE);
    cr->set_line_width(1);

    cr->set_font_size(FONT_SIZE);

    int y = 0;
    drawProperties(cr, props, 0, y);

    return true;
}

void PropEditor::on_edit_changed()
{
    if(!select) return;

    select->setValue(edit->get_text());
    on_change_property.run(select);
}

void PropEditor::on_icon_press(EntryIconPosition icon, const GdkEventButton *button)
{
    if(select->type == data_combo) {
        int w, h;
        get_size_request(w, h);

        Fixed *fx = (Fixed *)get_parent();
        fx->move(*comboPanel, w - 0.8 * w, select_y + 20);

        TreePath p = comboList->get_path(comboList->children()[((PropertyEnum *)select)->getSelectIndex()]);
        // prevent on_row_activated emitted
        PropertyItem *old = select;
        select = NULL;

        tree->set_cursor(p);
        tree->scroll_to_row(p);

        comboPanel->show_all();

        Gdk::Rectangle r;
        //p = comboList->get_path(comboList->children()[0]);
        tree->get_cell_area(p, *tree->get_column(0), r);
        comboPanel->set_size_request(0.8 * w, min(170, r.get_height() * (comboList->children().size() + 1)));

        select = old;
    } else if(select->type == data_stock) {
        int w, h;
        get_size_request(w, h);

        Fixed *fx = (Fixed *)get_parent();
        fx->move(*stockPanel, w - 0.8 * w, select_y + 20);
        stockPanel->set_size_request(0.8 * w, 170);

        TreePath p = stockList->get_path(stockList->children()[0]);
        for(unsigned long i = 1; i < stockList->children().size(); i++)
            if(stockList->children()[i][stockColumns.param] == *(ustring *)select->value) {
                p = stockList->get_path(stockList->children()[i]);
                break;
            }
        PropertyItem *old = select;
        select = NULL;

        stock->set_cursor(p);
        stock->scroll_to_row(p);

        stockPanel->show_all();

        select = old;
    } else if(select->type == data_color) {
        ColorSelectionDialog cs("Change color");
        cs.get_colorsel()->set_previous_color(*(Gdk::Color *)select->value);
        cs.get_colorsel()->set_current_color(*(Gdk::Color *)select->value);
        cs.get_colorsel()->set_has_palette(true);

        if(cs.run())
            edit->set_text(cs.get_colorsel()->get_current_color().to_string());
        //*(Gdk::Color*)select->value = cs.get_colorsel()->get_current_color();
    } else
        on_edit_property.run(select);
}

void PropEditor::fillStockPanel()
{
    Gtk::TreeRow row = *(stockList->append());
    row[stockColumns.param] = emptyName;

    typedef Glib::SListHandle<Gtk::StockID, Gtk::StockID_Traits> GList;
    GList list = Gtk::Stock::get_ids();
    for(GList::iterator l = list.begin(); l != list.end(); l++) {
        row = *(stockList->append());
        row[stockColumns.param] = (*l).get_string();
        row[stockColumns.icon] = render_icon(*l, ICON_SIZE_MENU, ustring());
    }
}

void PropEditor::on_row_activated()
{
    if(!select) return;

    TreeModel::Path p;
    TreeViewColumn *c;
    tree->get_cursor(p, c);

    TreeModel::Row r = *comboList->get_iter(p);
    comboPanel->hide();

    edit->set_text(ustring(r[comboColumns.param]));
}

void PropEditor::on_stock_row_activated()
{
    if(!select) return;

    TreeModel::Path p;
    TreeViewColumn *c;
    stock->get_cursor(p, c);

    TreeModel::Row r = *stockList->get_iter(p);
    stockPanel->hide();

    edit->set_text(ustring(r[stockColumns.param]));
}

void PropEditor::on_btn_click()
{
    on_edit_property.run(select);
}

void PropEditor::rePosChild()
{
    if(!edit) {
        edit = new Entry();
        get_parent()->add(*edit);
        edit->set_has_frame(false);
        edit->set_icon_from_stock(Gtk::Stock::EDIT, ENTRY_ICON_SECONDARY);
        edit->set_icon_tooltip_text("Edit value in extended editor", ENTRY_ICON_SECONDARY);
        edit->signal_changed().connect(sigc::mem_fun(*this, &PropEditor::on_edit_changed));
        edit->signal_icon_press().connect(sigc::mem_fun(*this, &PropEditor::on_icon_press));

        objBtn = new Button("-");
        objBtn->set_size_request(17, 17);
        objBtn->signal_clicked().connect(sigc::mem_fun(*this, &PropEditor::on_btn_click));
        objBtn->show();
        EventBox *cont = new EventBox();
        cont->add(*objBtn);
        get_parent()->add(*cont);

        //--- enum list
        comboList = Gtk::ListStore::create(comboColumns);

        tree = new TreeView(comboList);
        tree->append_column("param", comboColumns.param);
        tree->set_headers_visible(false);
        tree->signal_cursor_changed().connect(sigc::mem_fun(*this, &PropEditor::on_row_activated));

        comboPanel = new EventBox();
        ScrolledWindow *sw = new ScrolledWindow();
        sw->set_policy(POLICY_NEVER, POLICY_AUTOMATIC);
        sw->add(*tree);
        Frame *frm = new Frame();
        frm->add(*sw);
        comboPanel->add(*frm);

        get_parent()->add(*comboPanel);

        //--- stock list
        stockList = Gtk::ListStore::create(stockColumns);

        stock = new TreeView(stockList);
        stock->append_column("icon", stockColumns.icon);
        stock->append_column("param", stockColumns.param);
        stock->set_headers_visible(false);
        stock->signal_cursor_changed().connect(sigc::mem_fun(*this, &PropEditor::on_stock_row_activated));

        stockPanel = new EventBox();
        sw = new ScrolledWindow();
        sw->set_policy(POLICY_NEVER, POLICY_AUTOMATIC);
        sw->add(*stock);
        frm = new Frame();
        frm->add(*sw);
        stockPanel->add(*frm);

        get_parent()->add(*stockPanel);

        fillStockPanel();
    }

    int w, h;
    get_size_request(w, h);

    edit->set_visible(select && (select->flag & PROP_FLG_EDIT));
    Fixed *fx = (Fixed *)get_parent();
    fx->move(*edit, w / 2 + 2, select_y + 2);
    edit->set_size_request(w / 2 - 3, 17);

    objBtn->get_parent()->set_visible(select && (select->flag & PROP_FLG_BTN) && !(select->flag & PROP_FLG_EDIT));
    objBtn->show();
    fx->move(*objBtn->get_parent(), w - 19, select_y + 2);

    if(edit->get_visible()) {
        edit->set_icon_sensitive(ENTRY_ICON_SECONDARY, select->flag & PROP_FLG_BTN);

        ustring sel_text = select->editText();
        // prevent on_edit_changed emitted
        PropertyItem *old = select;
        select = NULL;

        edit->set_text(sel_text);
        edit->select_region(0, -1);
        //edit->grab_focus();

        select = old;

        if(select->type == data_combo) {
            comboList->clear();
            PropertyEnum *e = (PropertyEnum *)select;
            for(std::vector<ustring>::iterator l = e->list.begin(); l != e->list.end(); l++) {
                Gtk::TreeRow row = *(comboList->append());
                row[comboColumns.param] = *l;
            }
        }
    }

    comboPanel->hide();
    stockPanel->hide();
}

void PropEditor::changeSize(int width)
{
    int w, h;
    get_size_request(w, h);
    if(w == width - 1) return;
    set_size_request(width - 1, h);

    rePosChild();
}

PropertyItem *getPropertyByY(Properties &items, int &index, double y)
{
    int i = y / 20;
    for(Properties::iterator p = items.begin(); p != items.end(); p++) {
        if(index++ == i)
            return *p;
        else if((*p)->opened) {
            PropertyItem *pr = getPropertyByY((*p)->items, index, y);
            if(pr) return pr;
        }
    }
    return NULL;
}

void PropEditor::getPropertyAtPos(double y)
{
    int c = 0;
    PropertyItem *prop = getPropertyByY(props, c, y);
    if(prop != select) {
        select_y = (c - 1) * 20;
        select = prop;
        rePosChild();
        if(edit->is_visible())
            edit->grab_focus();
        on_select_property.run(select);
    }
}

bool PropEditor::on_button_press_event(GdkEventButton *event)
{
    downButton = event->button;

    getPropertyAtPos(event->y);
    if(select && (event->x > select->getLevel() * 16) && (event->x < select->getLevel() * 16 + 16)) {
        if(select->flag & PROP_FLG_CHECK) {
            select->checked = !select->checked;
            on_check_property.run(select);
        } else if(!select->items.empty()) {
            select->open();
        }
    }
    reDraw();

    return true;
}

bool PropEditor::on_button_release_event(GdkEventButton *event)
{
    downButton = 0;
    return true;
}

bool PropEditor::on_motion_notify_event(GdkEventMotion *event)
{
    if(downButton) {
        getPropertyAtPos(event->y);
        reDraw();
    }

    return true;
}

ustring PropEditor::getPath()
{
    if(select) {
        ustring res = select->caption;
        PropertyItem *p = select;
        while(p->group) {
            p = p->group;
            res = p->caption + "/" + res;
        }
        return res;
    }
    return ustring();
}

void PropEditor::selectPropertyByPath(const ustring &path)
{
    if(path.empty()) return;

    char *buf = new char[path.length() + 1];
    path.copy(buf, path.length());
    buf[path.length()] = '\0';
    Properties *p = &props;
    PropertyItem *selProp = NULL;
    int y = 0;
    while(char *tok = getTok(&buf, '/')) {
        selProp = NULL;
        for(Properties::iterator prop = p->begin(); prop != p->end(); prop++)
            if((*prop)->caption == tok) {
                selProp = *prop;
                p = &selProp->items;
                if(!p->empty())
                    y += 20;
                break;
            } else y += 20;
        if(!selProp || p->empty()) break;
    }

    if(selProp) {
        select_y = y;
        select = selProp;
        while(selProp->group) {
            selProp->group->open();
            selProp = selProp->group;
        }
        rePosChild();
        reDraw();
    }
}

void PropEditor::update()
{
    get_window()->invalidate(false);
    if(edit->get_visible())
        edit->set_text(select->editText());
}
