#ifndef __CMULTITABS_H__
#define __CMULTITABS_H__

#include <FL/Fl_Group.H>

class CMultiTabsMatrix;
class CTabInfo;

class CMultiTabs : public Fl_Group {
   Fl_Widget        *value_;
   Fl_Widget        *push_;
   CMultiTabsMatrix *m_tabsMatrix;
   int               m_autoColorIndex;
protected:
   bool              m_showTabs;
   int               m_tabsHeight;
   Fl_Group*         m_page;
protected:
  virtual FL_EXPORT void draw_tab(CTabInfo *tab,int selected=0);
  FL_EXPORT void draw();
  FL_EXPORT Fl_Group* lastTab();

protected:
   FL_EXPORT virtual void resizeTabs();
   FL_EXPORT virtual void extendTabs();
   FL_EXPORT virtual Fl_Group* createNewPage(const char *label);

public:
   static const Fl_Color AutoColorTable[16];

   FL_EXPORT CMultiTabs(int x,int y,int w,int h,const char *label = 0);
   FL_EXPORT virtual ~CMultiTabs();

   FL_EXPORT virtual Fl_Group* newPage(const char *label,bool autoColor=false);
   FL_EXPORT virtual Fl_Group* newPage(const char *label,Fl_Color color);

   FL_EXPORT int handle(int);
   FL_EXPORT Fl_Widget *value();
   FL_EXPORT int value(Fl_Widget *);
   Fl_Widget *push() const {return push_;}
   FL_EXPORT int push(Fl_Widget *);
   FL_EXPORT Fl_Widget *which(int event_x, int event_y);
   FL_EXPORT void showTabs(bool st) { m_showTabs = st; }

   virtual void FL_EXPORT resize(int x, int y, int w, int h);
};

#endif

