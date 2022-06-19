#include <FL/Fl.H>
#include "cmultitabs.h"
#include <FL/fl_draw.H>
#include <FL/Fl_Scroll.H>

#define BORDER 10
#define TABSLOPE 5
#define EXTRASPACE 2
#define TAB_ROW_HEIGHT 20

class CTabInfo {
public:
   int       x, y, width, height;
   int       id;
   Fl_Group *group;
public:
   CTabInfo(int x,int y,int width,int height,int id,Fl_Group *group);
   CTabInfo(const CTabInfo& );
   bool includesCoord(int x,int y);
};

class CMultiTabsList {
   CTabInfo   *m_list[32];
   unsigned    m_count;
   int         m_active;
public:
   CMultiTabsList();
   void     add(CTabInfo *ti);
   void     clear();
   unsigned count() { return m_count; }
   CTabInfo * operator [] (unsigned index) { return m_list[index]; }
   int       active() { return m_active; }
   void      active(int activeIndex) { m_active = activeIndex; }
   void      sety(int ycoord);
   CTabInfo *activeTab();
   CTabInfo *tabAt(int x,int y);
   int       indexOf(CTabInfo *ti);
   void      extend(int width);
};

class CMultiTabsMatrix {
   unsigned        m_count;
   CMultiTabsList *m_list[16];
public:
   CMultiTabsMatrix() { m_count = 0; }
   ~CMultiTabsMatrix() { clear(); }
   void     add(CMultiTabsList *tl);
   void     clear();
   unsigned count() { return m_count; }
   CMultiTabsList * operator [] (unsigned index) { return (CMultiTabsList *)m_list[index]; }
   void     activate(int rowIndex,int tabIndex);
   void     activate(CTabInfo *);
   CTabInfo *tabAt(int x,int y);
};

CTabInfo::CTabInfo(int tx,int ty,int twidth,int theight,int tid,Fl_Group *tgroup) {
   x = tx;
   y = ty;
   width = twidth;
   height = theight;
   id = tid;
   group = tgroup;
}

CTabInfo::CTabInfo(const CTabInfo& ti) {
   x = ti.x;
   y = ti.y;
   width = ti.width;
   height = ti.height;
   id = ti.id;
   group = ti.group;
}

bool CTabInfo::includesCoord(int xx,int yy) {
   return ( xx > x && xx < x + width && yy > y && yy < y + height );
}

CMultiTabsList::CMultiTabsList()  {
   m_active = -1;
   m_count = 0;
}

void CMultiTabsList::extend(int width) {
   if (!m_count) return;
   CTabInfo *ti = m_list[m_count-1];
   int totalWidth = ti->x + ti->width;
   float extendX = (width - totalWidth) / m_count;
   for (unsigned t=0; t < m_count; t++) {
      m_list[t]->x += int( t * extendX + 0.5 );
      m_list[t]->width += int( extendX );
   }
}

void CMultiTabsList::add(CTabInfo *ti) {
   if (m_count >= 32) return;
   m_list[m_count] = ti;
   m_count++;
}

void CMultiTabsList::clear() {
   for (unsigned i=0; i < m_count; i++) {
      delete m_list[i];
   }
   m_count = 0;
}

CTabInfo *CMultiTabsList::activeTab() {
   if (m_active == -1) return 0;
   return m_list[m_active];
}

void CMultiTabsList::sety(int ycoord) {
   for (unsigned i=0; i < m_count; i++) {
      m_list[i]->y = ycoord;
   }
}

CTabInfo *CMultiTabsList::tabAt(int x,int y) {
   for (unsigned i=0; i < m_count; i++) {
      CTabInfo * tab = m_list[i];
      if (tab->includesCoord(x,y))
         return tab;
   }
   return 0;
}

int CMultiTabsList::indexOf(CTabInfo *ti) {
   for (unsigned i=0; i < m_count; i++) {
      CTabInfo * tab = m_list[i];
      if (tab == ti)
         return i;
   }
   return -1;
}

void CMultiTabsMatrix::add(CMultiTabsList *tl) {
   if (m_count >= 8) return;
   m_list[m_count] = tl;
   m_count++;
}

void CMultiTabsMatrix::clear() {
   for (unsigned i=0; i < m_count; i++) {
      delete m_list[i];
   }
   m_count = 0;
}

void CMultiTabsMatrix::activate(int rowIndex,int tabIndex) {
   // find currently active row
   if (!m_count) return;
   unsigned activeRowIndex = m_count-1;
   CMultiTabsList *activeRow = (CMultiTabsList *)m_list[activeRowIndex];

   CMultiTabsList *row = (CMultiTabsList *)m_list[rowIndex];
   int ycoord = TAB_ROW_HEIGHT * activeRowIndex;
   row->sety(ycoord);
   row->active(tabIndex);

   if ((int)activeRowIndex != rowIndex) {
      // move row coordinates for the currently active row
      ycoord = TAB_ROW_HEIGHT * rowIndex;
      activeRow->sety(ycoord);
      activeRow->active(-1);
      // swap the rows visually
      m_list[rowIndex] = activeRow;
      m_list[activeRowIndex] = row;
   }
}

void CMultiTabsMatrix::activate(CTabInfo *ti) {
   // find tab's row
   for (unsigned i=0; i < m_count; i++) {
      int tabIndex = m_list[i]->indexOf(ti);
      if (tabIndex > -1) {
         activate(i,tabIndex);
         break;
      }
   }
}

CTabInfo *CMultiTabsMatrix::tabAt(int x,int y) {
   for (unsigned i=0; i < m_count; i++) {
      CTabInfo *tab = m_list[i]->tabAt(x,y);
      if (tab)
         return tab;
   }
   return 0;
}

CMultiTabs::CMultiTabs(int X,int Y,int W, int H,const char *l) :
  Fl_Group(X,Y,W,H,l) {
   m_tabsMatrix = new CMultiTabsMatrix;
   box(FL_THIN_UP_BOX);
   push_ = 0;
   m_showTabs = true;
   m_page = 0L;
   m_autoColorIndex = 0;
}

CMultiTabs::~CMultiTabs() {
   delete m_tabsMatrix;
}

// this is used by fluid to pick tabs:
Fl_Widget *CMultiTabs::which(int event_x, int event_y) {
   int H = m_tabsHeight;
   if (H < 0) {
      if (event_y > y()+h() || event_y < y()+h()+H) return 0;
   } else {
      if (event_y > y()+H || event_y < y()) return 0;
   }
   if (event_x < x()) return 0;

   CTabInfo *tab = m_tabsMatrix->tabAt(event_x-x(),event_y-y());
   if (tab) {
      m_tabsMatrix->activate(tab);
      return tab->group;
   }
   return 0;
}

int CMultiTabs::handle(int event) {

  Fl_Widget *o;

  switch (event) {

  case FL_PUSH: {
    int H = m_tabsHeight;
    if (H >= 0) {
      if (Fl::event_y() > y()+H) goto DEFAULT;
    } else {
      if (Fl::event_y() < y()+h()+H) goto DEFAULT;
    }}
  case FL_DRAG:
  case FL_RELEASE:
    o = which(Fl::event_x(), Fl::event_y());
    if (event == FL_RELEASE) {push(0); if (o && value(o)) do_callback();}
    else push(o);
    return 1;

  default:
  DEFAULT:
    return Fl_Group::handle(event);

  }
}

int CMultiTabs::push(Fl_Widget *o) {
  if (push_ == o) return 0;
  if (push_ && !push_->visible() || o && !o->visible())
    damage(FL_DAMAGE_EXPOSE);
  push_ = o;
  return 1;
}

// The value() is the first visible child (or the last child if none
// are visible) and this also hides any other children.
// This allows the tabs to be deleted, moved to other groups, and
// show()/hide() called without it screwing up.
Fl_Widget* CMultiTabs::value() {
  Fl_Widget* v = 0;
  Fl_Widget*const* a = array();
  for (int i=children(); i--;) {
    Fl_Widget* o = *a++;
    if (v) o->hide();
    else if (o->visible()) v = o;
    else if (!i) {o->show(); v = o;}
  }
  return v;
}

// Setting the value hides all other children, and makes this one
// visible, iff it is really a child:
int CMultiTabs::value(Fl_Widget *newvalue) {
  Fl_Widget*const* a = array();
  int ret = 0;
  for (int i=children(); i--;) {
    Fl_Widget* o = *a++;
    if (o == newvalue) {
      if (!o->visible()) ret = 1;
      o->show();
    } else {
      o->hide();
    }
  }
  return ret;
}

enum {LEFT, RIGHT, SELECTED};

void CMultiTabs::draw() {
   //resizeTabs();
   resize(x(),y(),w(),h());

   Fl_Widget *v = value();
   int H = m_tabsHeight;
   if (damage() & FL_DAMAGE_ALL) { // redraw the entire thing:
      fl_color(color());
      fl_rectf(x(), y(), w(), h());
      fl_clip (x(), y()+H, w(), h()-H);
      draw_box(box(), x(), y(), w(), h(), v ? v->color() : color());
      fl_pop_clip();
      if (v) draw_child(*v);
   } else { // redraw the child
      if (v) update_child(*v);
   }
   if (damage() & (FL_DAMAGE_EXPOSE|FL_DAMAGE_ALL)) {
      unsigned rowCount = m_tabsMatrix->count();
      for (unsigned r = 0; r < rowCount; r++) {
         CMultiTabsList& row = *(*m_tabsMatrix)[r];
         int rowTabCount = row.count();
         for (int t = 0; t < rowTabCount; t++) {
            CTabInfo *tab = row[t];
            if (row.active()==t)
                  draw_tab(tab,true);
            else  draw_tab(tab,row.active()==t);
         }
      }
   }
}

void CMultiTabs::draw_tab(CTabInfo *tab, int selected) {
   if (!m_showTabs) return;

   //fl_color(!selected ? fl_darker( tab->group->color() ) : tab->group->color() );
   fl_color(tab->group->color());

   // Draw the background of the tab
   int x1 = x() + tab->x;
   int x2 = x1  + tab->width;
   int y1 = y() + tab->y;
   //int y2 = y1  + tab->height;
   int y2 = y()  + m_tabsHeight;
   fl_begin_polygon();
   fl_vertex(x1+2, y2+selected);
   fl_vertex(x1+2, y1+TABSLOPE);
   fl_vertex(x1+2+TABSLOPE, y1);
   fl_vertex(x2, y1);
   fl_vertex(x2, y2+selected);
   fl_end_polygon();

   // Draw the front-top highlighted edge of the tab
   //fl_color(!selected && tab->group==push_ ? fl_darker(tab->group->color()) : fl_lighter(tab->group->color()));
   fl_color(fl_lighter(fl_lighter(tab->group->color())));

   fl_line(x1+2, y2, x1+2, y1+TABSLOPE, x1+2+TABSLOPE, y1);
   fl_line(x1+2+TABSLOPE, y1, x2, y1);

   if (selected) {
      if (x1>x()) fl_xyline(x(), y2, x1);
      if (x2+TABSLOPE < x()+w()-1) fl_xyline(x2, y2, x()+w()-1);
   }
   //fl_color(!selected && tab->group==push_ ? fl_lighter(tab->group->color()) : fl_darker(tab->group->color()));
   fl_color(fl_darker(tab->group->color()));
   fl_line(x2, y1, x2, y2);

   tab->group->draw_label(x1+4,y1,tab->width-6,tab->height,FL_ALIGN_CENTER);
}

void CMultiTabs::resizeTabs() {
   int selected = -1;
   int xpos = 0;
   int wt = 0, ht = 0;
   Fl_Widget* const* a = array();

   CTabInfo       *activeTab = 0L;
   CMultiTabsList *row = 0L;

   m_tabsMatrix->clear();

   int ypos = 0;
   int ystep = 0;
   if (m_showTabs)
      ystep = TAB_ROW_HEIGHT;

   row = new CMultiTabsList;
   m_tabsMatrix->add(row);
   for (int i=0; i<children(); i++) {
      Fl_Widget * group = *a++;
      wt = 0;
      if (group->label())
         group->measure_label(wt,ht);

      wt += 6;

      if (xpos + wt > w()) {
         if (row->count() > 0) {
            xpos = 0;
            ypos += ystep;
            row = new CMultiTabsList;
            m_tabsMatrix->add(row);
         } else {
            wt = w() - xpos;
         }
      }
      CTabInfo  *tab  = new CTabInfo(xpos,ypos,wt,TAB_ROW_HEIGHT,i,(Fl_Group *)group);
      xpos += wt;
      if (group->visible()) {
         selected = i;
         activeTab = tab;
      }
      row->add(tab);
   }
   m_tabsHeight = ypos + ystep;
   m_tabsMatrix->activate(activeTab);

   extendTabs();
}

void CMultiTabs::resize(int xx, int yy, int ww, int hh) {
   Fl_Group::resize(xx,yy,ww,hh);

   resizeTabs();

   int group_x = xx + 2;
   int group_y = yy + m_tabsHeight + 2;
   int group_w = ww - 4;
   int group_h = hh - m_tabsHeight - 4;

   Fl_Widget*const* a = array();
   for (int i=children(); i--;) {
      Fl_Widget* group = *a++;
      if (group->x() != group_x || group->y() != group_y || group->w() != group_w || group->h() != group_h) {
         group->resize(group_x,group_y,group_w,group_h);
      }
   }
}

void CMultiTabs::extendTabs() {
   unsigned cnt = m_tabsMatrix->count();
   for (unsigned r = 0; r < cnt; r++) {
      (*m_tabsMatrix)[r]->extend(w()-2);
   }
}

const Fl_Color CMultiTabs::AutoColorTable[16] = {
   fl_rgb_color(240,190,190),
   fl_rgb_color(0xB0,0xD0,0xD0),
   fl_rgb_color(0xC0,0xC0,0xE0),
   fl_rgb_color(192,176,160),
   fl_rgb_color(0xD0,0xD0,0xB0),
   fl_rgb_color(0xC0,0xB0,0xC0),
   fl_rgb_color(0xC0,0xA0,0x90),
   fl_rgb_color(0xD0,0xD0,0xE8),
   fl_rgb_color(0xE8,0xC0,0xC0),
   fl_rgb_color(0xC0,0xE8,0xC0),
   fl_rgb_color(0xE8,0xC0,0xE8),
   fl_rgb_color(0xE0,0xE0,0xC0),
   fl_rgb_color(0xC0,0xE0,0xE0),
   fl_rgb_color(0xE0,0xC0,0xE0),
   fl_rgb_color(0xA0,0xB8,0xA0),
   fl_rgb_color(0xB8,0xC0,0xE8)
};

Fl_Group* CMultiTabs::createNewPage(const char *label) {
   Fl_Group* group = lastTab();
   if (group)
      group->end();
   return new Fl_Scroll(x(),y(),w(),h(),label);
}

Fl_Group* CMultiTabs::newPage(const char *label,bool autoColor) {
   Fl_Group* group = createNewPage(label);
   group->box(FL_FLAT_BOX);
   if (children() > 1)
      group->hide();
   if (autoColor) {
      Fl_Color clr = AutoColorTable[m_autoColorIndex&0xF];
      group->color( clr );
      m_autoColorIndex++;
   }
   return group;
}

Fl_Group* CMultiTabs::newPage(const char *label,Fl_Color color) {
   Fl_Group* group = newPage(label,false);
   group->color(color);
   return group;
}

Fl_Group* CMultiTabs::lastTab() {
   // find the last children
   Fl_Widget*const* a = array();
   Fl_Group* group = 0L;
   for (int i=children(); i--;) {
      group = (Fl_Group*) *a++;
   }
   return group;
}
