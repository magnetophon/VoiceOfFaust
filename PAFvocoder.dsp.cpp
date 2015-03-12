//-----------------------------------------------------
// name: "PAFvocoder"
// version: "0.7"
// author: "Bart Brouns"
// license: "GNU 3.0"
// copyright: "(c) Bart Brouns 2014"
//
// Code generated with Faust 0.9.70 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#include <math.h>
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
#endif
/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections : 
	the ARCHITECTURE section (in two parts) and the USER section. Each section 
	is governed by its own copyright and license. Please check individually 
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 Thomas Charbonnel and GRAME
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it 
    and/or modify it under the terms of the GNU General Public License 
	as published by the Free Software Foundation; either version 3 of 
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License 
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work 
	that contains this FAUST architecture section and distribute  
	that work under terms of your choice, so long as this FAUST 
	architecture section is not modified. 


 ************************************************************************
 ************************************************************************/

#include <libgen.h>
#include <stdlib.h>
#include <iostream>
#include <list>

#ifndef FAUST_FUI_H
#define FAUST_FUI_H

#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust User Interface
 * This abstract class contains only the method that the faust compiler can
 * generate to describe a DSP interface.
 ******************************************************************************/

class UI
{

 public:

	UI() {}

	virtual ~UI() {}

    // -- widget's layouts

    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};

#endif

#include <string>
#include <map>
#include <set>
#include <vector>
#include <stack>

#include <iostream>
#include <fstream>

//using namespace std;

#if 1

/*******************************************************************************
 * FUI : used to save and recall the state of the user interface
 * This class provides essentially two new methods saveState() and recallState()
 * used to save on file and recall from file the state of the user interface.
 * The file is human readble and editable
 ******************************************************************************/

class FUI : public UI
{
    
    std::stack<std::string>             fGroupStack;
	std::vector<std::string>            fNameList;
	std::map<std::string, FAUSTFLOAT*>	fName2Zone;

 protected:

 	// labels are normalized by replacing white spaces by underscores and by
 	// removing parenthesis
	std::string normalizeLabel(const char* label)
	{
		std::string 	s;
		char 	c;

		while ((c=*label++)) {
			if (isspace(c)) 				{ s += '_'; }
			//else if ((c == '(') | (c == ')') ) 	{ }
			else 							{ s += c; }
		}
		return s;
	}

	// add an element by relating its full name and memory zone
	virtual void addElement(const char* label, FAUSTFLOAT* zone)
	{
		std::string fullname (fGroupStack.top() + '/' + normalizeLabel(label));
		fNameList.push_back(fullname);
		fName2Zone[fullname] = zone;
	}

	// keep track of full group names in a stack
	virtual void pushGroupLabel(const char* label)
	{
		if (fGroupStack.empty()) {
			fGroupStack.push(normalizeLabel(label));
		} else {
			fGroupStack.push(fGroupStack.top() + '/' + normalizeLabel(label));
		}
	}

	virtual void popGroupLabel()
	{
		fGroupStack.pop();
	};

 public:

	FUI() 			{}
	virtual ~FUI() 	{}

	// -- Save and recall methods

	// save the zones values and full names
	virtual void saveState(const char* filename)
	{
		std::ofstream f(filename);

		for (unsigned int i=0; i<fNameList.size(); i++) {
			std::string	n = fNameList[i];
			FAUSTFLOAT*	z = fName2Zone[n];
			f << *z << ' ' << n.c_str() << std::endl;
		}

		f << std::endl;
		f.close();
	}

	// recall the zones values and full names
	virtual void recallState(const char* filename)
	{
		std::ifstream f(filename);
		FAUSTFLOAT	v;
		std::string	n;

		while (f.good()) {
			f >> v >> n;
			if (fName2Zone.count(n)>0) {
				*(fName2Zone[n]) = v;
			} else {
				std::cerr << "recallState : parameter not found : " << n.c_str() << " with value : " << v << std::endl;
			}
		}
		f.close();
	}


    // -- widget's layouts (just keep track of group labels)

    virtual void openTabBox(const char* label) 			{ pushGroupLabel(label); }
    virtual void openHorizontalBox(const char* label) 	{ pushGroupLabel(label); }
    virtual void openVerticalBox(const char* label)  	{ pushGroupLabel(label); }
    virtual void closeBox() 							{ popGroupLabel(); };

    // -- active widgets (just add an element)

    virtual void addButton(const char* label, FAUSTFLOAT* zone) 		{ addElement(label, zone); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 	{ addElement(label, zone); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT , FAUSTFLOAT)
    																{ addElement(label, zone); }

    // -- passive widgets (are ignored)

    virtual void addHorizontalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {};
    virtual void addVerticalBargraph(const char*, FAUSTFLOAT*, FAUSTFLOAT, FAUSTFLOAT) {};

	// -- metadata are not used

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
};
#endif

#endif

#ifndef FAUST_PUI_H
#define FAUST_PUI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#ifndef FAUST_PathUI_H
#define FAUST_PathUI_H

#include <vector>
#include <string>
#include <algorithm>

/*******************************************************************************
 * PathUI : Faust User Interface
 * Helper class to build complete path for items.
 ******************************************************************************/

class PathUI : public UI
{

    protected:
    
        std::vector<std::string> fControlsLevel;
       
    public:
    
        std::string buildPath(const std::string& label) 
        {
            std::string res = "/";
            for (size_t i = 0; i < fControlsLevel.size(); i++) {
                res += fControlsLevel[i];
                res += "/";
            }
            res += label;
            replace(res.begin(), res.end(), ' ', '_');
            return res;
        }
    
};

#endif
#include <vector>
#include <string>

/*******************************************************************************
 * PrintUI : Faust User Interface
 * This class print arguments given to calls to UI methods and build complete path for labels.
 ******************************************************************************/

class PrintUI : public PathUI
{

    public:

        PrintUI() {}

        virtual ~PrintUI() {}

        // -- widget's layouts

        virtual void openTabBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openTabBox label : " << label << std::endl;
        }
        virtual void openHorizontalBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openHorizontalBox label : " << label << std::endl;
        }
        virtual void openVerticalBox(const char* label)
        {
            fControlsLevel.push_back(label);
            std::cout << "openVerticalBox label : " << label << std::endl;
        }
        virtual void closeBox()
        {
            fControlsLevel.pop_back();
            std::cout << "closeBox" << std::endl;
        }

        // -- active widgets

        virtual void addButton(const char* label, FAUSTFLOAT* zone)
        {
            std::cout << "addButton label : " << buildPath(label) << std::endl;
        }
        virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
        {
            std::cout << "addCheckButton label : " << buildPath(label) << std::endl;
        }
        virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addVerticalSlider label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }
        virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addHorizontalSlider label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }
        virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
        {
            std::cout << "addNumEntry label : " << buildPath(label) << " init : " << init << " min : " << min << " max : " << max << " step : " << step << std::endl;
        }

        // -- passive widgets

        virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
        {
            std::cout << "addHorizontalBargraph label : " << buildPath(label) << " min : " << min << " max : " << max << std::endl;
        }
        virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
        {
            std::cout << "addVerticalBargraph label : " << buildPath(label) << " min : " << min << " max : " << max << std::endl;
        }

        // -- metadata declarations

        virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val)
        {
            std::cout << "declare key : " << key << " val : " << val << std::endl;
        }
    
};

#endif
/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __misc__
#define __misc__

#include <algorithm>
#include <map>
#include <string.h>
#include <stdlib.h>

/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

 ************************************************************************
 ************************************************************************/
 
#ifndef __meta__
#define __meta__

struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};

#endif


using std::max;
using std::min;

struct XXXX_Meta : std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

struct MY_Meta : Meta, std::map<const char*, const char*>
{
    void declare(const char* key, const char* value) { (*this)[key]=value; }
};

inline int	lsr(int x, int n)	{ return int(((unsigned int)x) >> n); }
inline int 	int2pow2(int x)		{ int r=0; while ((1<<r)<x) r++; return r; }

long lopt(char *argv[], const char *name, long def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return atoi(argv[i+1]);
	return def;
}

bool isopt(char *argv[], const char *name)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return true;
	return false;
}

const char* lopts(char *argv[], const char *name, const char* def)
{
	int	i;
	for (i = 0; argv[i]; i++) if (!strcmp(argv[i], name)) return argv[i+1];
	return def;
}
#endif

#ifndef FAUST_GTKUI_H
#define FAUST_GTKUI_H

#ifndef FAUST_GUI_H
#define FAUST_GUI_H

#include <list>
#include <map>

/*******************************************************************************
 * GUI : Abstract Graphic User Interface
 * Provides additional macchanismes to synchronize widgets and zones. Widgets
 * should both reflect the value of a zone and allow to change this value.
 ******************************************************************************/

class uiItem;
typedef void (*uiCallback)(FAUSTFLOAT val, void* data);

class clist : public std::list<uiItem*>
{
    public:
    
        virtual ~clist();
        
};

class GUI : public UI
{
    
	typedef std::map<FAUSTFLOAT*, clist*> zmap;
	
 private:
 	static std::list<GUI*>	fGuiList;
	zmap                    fZoneMap;
	bool                    fStopped;
	
 public:
		
    GUI() : fStopped(false) 
    {	
		fGuiList.push_back(this);
	}
	
    virtual ~GUI() 
    {   
        // delete all 
        zmap::iterator g;
        for (g = fZoneMap.begin(); g != fZoneMap.end(); g++) {
            delete (*g).second;
        }
        // suppress 'this' in static fGuiList
        fGuiList.remove(this);
    }

	// -- registerZone(z,c) : zone management
	
	void registerZone(FAUSTFLOAT* z, uiItem* c)
	{
  		if (fZoneMap.find(z) == fZoneMap.end()) fZoneMap[z] = new clist();
		fZoneMap[z]->push_back(c);
	} 	

	void updateAllZones();
	
	void updateZone(FAUSTFLOAT* z);
	
	static void updateAllGuis()
	{
		std::list<GUI*>::iterator g;
		for (g = fGuiList.begin(); g != fGuiList.end(); g++) {
			(*g)->updateAllZones();
		}
	}
    void addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data);
    virtual void show() {};	
    virtual void run() {};
	
	virtual void stop()		{ fStopped = true; }
	bool stopped() 	{ return fStopped; }

    virtual void declare(FAUSTFLOAT* , const char* , const char*) {}
};

/**
 * User Interface Item: abstract definition
 */

class uiItem
{
  protected :
		  
	GUI*            fGUI;
	FAUSTFLOAT*		fZone;
	FAUSTFLOAT		fCache;
	
	uiItem (GUI* ui, FAUSTFLOAT* zone) : fGUI(ui), fZone(zone), fCache(-123456.654321) 
	{ 
 		ui->registerZone(zone, this); 
 	}
	
  public :
  
	virtual ~uiItem() 
    {}
	
	void modifyZone(FAUSTFLOAT v) 	
	{ 
		fCache = v;
		if (*fZone != v) {
			*fZone = v;
			fGUI->updateZone(fZone);
		}
	}
		  	
	FAUSTFLOAT		cache()			{ return fCache; }
	virtual void 	reflectZone() 	= 0;	
};

/**
 * Callback Item
 */

struct uiCallbackItem : public uiItem
{
	uiCallback	fCallback;
	void*		fData;
	
	uiCallbackItem(GUI* ui, FAUSTFLOAT* zone, uiCallback foo, void* data) 
			: uiItem(ui, zone), fCallback(foo), fData(data) {}
	
	virtual void 	reflectZone() {		
		FAUSTFLOAT 	v = *fZone;
		fCache = v; 
		fCallback(v, fData);	
	}
};

// en cours d'installation de call back. a finir!!!!!

/**
 * Update all user items reflecting zone z
 */

inline void GUI::updateZone(FAUSTFLOAT* z)
{
	FAUSTFLOAT 	v = *z;
	clist* 	l = fZoneMap[z];
	for (clist::iterator c = l->begin(); c != l->end(); c++) {
		if ((*c)->cache() != v) (*c)->reflectZone();
	}
}

/**
 * Update all user items not up to date
 */

inline void GUI::updateAllZones()
{
	for (zmap::iterator m = fZoneMap.begin(); m != fZoneMap.end(); m++) {
		FAUSTFLOAT* z = m->first;
		clist*	l = m->second;
        if (z) {
            FAUSTFLOAT	v = *z;
            for (clist::iterator c = l->begin(); c != l->end(); c++) {
                if ((*c)->cache() != v) (*c)->reflectZone();
            }
        }
	}
}

inline void GUI::addCallback(FAUSTFLOAT* zone, uiCallback foo, void* data) 
{ 
	new uiCallbackItem(this, zone, foo, data); 
};

inline clist::~clist() 
{
    std::list<uiItem*>::iterator it;
    for (it = begin(); it != end(); it++) {
        delete (*it);
    }
}

#endif

/******************************************************************************
*******************************************************************************

                                GRAPHIC USER INTERFACE
                                  gtk interface

*******************************************************************************
*******************************************************************************/
#include <string>
#include <set>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <assert.h>

#define stackSize 256

// Insertion modes

#define kSingleMode 0
#define kBoxMode 1
#define kTabMode 2

static inline bool startWith(const std::string& str, const std::string& prefix)
{
    return (str.substr(0, prefix.size()) == prefix);
}

//------------ calculate needed precision
static int precision(double n)
{
	if (n < 0.009999) return 3;
	else if (n < 0.099999) return 2;
	else if (n < 0.999999) return 1;
	else return 0;
}

namespace gtk_knob
{

class GtkKnob
{
private:
	double start_x, start_y, max_value;
public:
	GtkRange parent;
	int last_quadrant;
	GtkKnob();
	~GtkKnob();
	GtkWidget* gtk_knob_new_with_adjustment(GtkAdjustment *_adjustment);
	
};

#define GTK_TYPE_KNOB          (gtk_knob_get_type())
#define GTK_KNOB(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_KNOB, GtkKnob))
#define GTK_IS_KNOB(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_KNOB))
#define GTK_KNOB_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass),  GTK_TYPE_KNOB, GtkKnobClass))
#define GTK_IS_KNOB_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass),  GTK_TYPE_KNOB))

GtkKnob::GtkKnob()
// GtkKnob constructor
{}

GtkKnob::~GtkKnob()
{
	// Nothing specific to do...
}

struct GtkKnobClass {
	GtkRangeClass parent_class;
	int knob_x;
	int knob_y;
	int knob_step;
	int button_is;

};

//------forward declaration
GType gtk_knob_get_type ();

/****************************************************************
 ** calculate the knop pointer with dead zone
 */

const double scale_zero = 20 * (M_PI/180); // defines "dead zone" for knobs

static void knob_expose(GtkWidget* widget, int knob_x, int knob_y, GdkEventExpose *event, int arc_offset)
{
	/** check resize **/
	int grow;
	if(widget->allocation.width > widget->allocation.height) {
		grow = widget->allocation.height;
	} else {
		grow =  widget->allocation.width;
	}
	knob_x = grow-4;
	knob_y = grow-4;
	/** get values for the knob **/
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	int knobx = (widget->allocation.x+2 + (widget->allocation.width-4 - knob_x) * 0.5);
	int knoby = (widget->allocation.y+2 + (widget->allocation.height-4 - knob_y) * 0.5);
	int knobx1 = (widget->allocation.x+2 + (widget->allocation.width-4)* 0.5);
	int knoby1 = (widget->allocation.y+2 + (widget->allocation.height-4) * 0.5);
	double knobstate = (adj->value - adj->lower) / (adj->upper - adj->lower);
	double angle = scale_zero + knobstate * 2 * (M_PI - scale_zero);
	double knobstate1 = (0. - adj->lower) / (adj->upper - adj->lower);
	double pointer_off = knob_x/6;
	double radius = std::min(knob_x-pointer_off, knob_y-pointer_off) / 2;
	double lengh_x = (knobx+radius+pointer_off/2) - radius * sin(angle);
	double lengh_y = (knoby+radius+pointer_off/2) + radius * cos(angle);
	double radius1 = std::min(knob_x, knob_y) / 2 ;

	/** get widget forground color convert to cairo **/
	GtkStyle *style = gtk_widget_get_style (widget);
	double r = std::min(0.6,style->fg[gtk_widget_get_state(widget)].red/65535.0),
		   g = std::min(0.6,style->fg[gtk_widget_get_state(widget)].green/65535.0),
		   b = std::min(0.6,style->fg[gtk_widget_get_state(widget)].blue/65535.0);

	/** paint focus **/
	if (GTK_WIDGET_HAS_FOCUS(widget)== TRUE) {
		gtk_paint_focus(widget->style, widget->window, GTK_STATE_NORMAL, NULL, widget, NULL,
		                knobx-2, knoby-2, knob_x+4, knob_y+4);
	}
	/** create clowing knobs with cairo **/
	cairo_t *cr = gdk_cairo_create(GDK_DRAWABLE(widget->window));
	GdkRegion *region;
	region = gdk_region_rectangle (&widget->allocation);
	gdk_region_intersect (region, event->region);
	gdk_cairo_region (cr, region);
	cairo_clip (cr);
	
	cairo_arc(cr,knobx1+arc_offset, knoby1+arc_offset, knob_x/2.1, 0, 2 * M_PI );
	cairo_pattern_t*pat =
		cairo_pattern_create_radial (knobx1+arc_offset-knob_x/6,knoby1+arc_offset-knob_x/6, 1,knobx1+arc_offset,knoby1+arc_offset,knob_x/2.1 );
	if(adj->lower<0 && adj->value>0.) {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4, g+0.4 + knobstate-knobstate1, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15, g+0.15 + (knobstate-knobstate1)*0.5, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	} else if(adj->lower<0 && adj->value<=0.) {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4 +knobstate1- knobstate, g+0.4, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15 +(knobstate1- knobstate)*0.5, g+0.15, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	} else {
		cairo_pattern_add_color_stop_rgb (pat, 0, r+0.4, g+0.4 +knobstate, b+0.4);
		cairo_pattern_add_color_stop_rgb (pat, 0.7, r+0.15, g+0.15 + knobstate*0.5, b+0.15);
		cairo_pattern_add_color_stop_rgb (pat, 1, r, g, b);
	}
	cairo_set_source (cr, pat);
	cairo_fill_preserve (cr);
	gdk_cairo_set_source_color(cr, gtk_widget_get_style (widget)->fg);
	cairo_set_line_width(cr, 2.0);
	cairo_stroke(cr);

	/** create a rotating pointer on the kob**/
	cairo_set_source_rgb(cr,  0.1, 0.1, 0.1);
	cairo_set_line_width(cr,std::max(3, std::min(7, knob_x/15)));
	cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND); 
	cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
	cairo_move_to(cr, knobx+radius1, knoby+radius1);
	cairo_line_to(cr,lengh_x,lengh_y);
	cairo_stroke(cr);
	cairo_set_source_rgb(cr,  0.9, 0.9, 0.9);
	cairo_set_line_width(cr,std::min(5, std::max(1,knob_x/30)));
	cairo_move_to(cr, knobx+radius1, knoby+radius1);
	cairo_line_to(cr,lengh_x,lengh_y);
	cairo_stroke(cr);
	cairo_pattern_destroy (pat);
	gdk_region_destroy (region);
	cairo_destroy(cr);
}

/****************************************************************
 ** general expose events for all "knob" controllers
 */

//----------- draw the Knob when moved
static gboolean gtk_knob_expose (GtkWidget* widget, GdkEventExpose *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	knob_expose(widget, klass->knob_x, klass->knob_y, event, 0);
	return TRUE;
}

/****************************************************************
 ** set initial size for GdkDrawable per type
 */

static void gtk_knob_size_request (GtkWidget* widget, GtkRequisition *requisition)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	requisition->width = klass->knob_x;
	requisition->height = klass->knob_y;
}

/****************************************************************
 ** set value from key bindings
 */

static void gtk_knob_set_value (GtkWidget* widget, int dir_down)
{
	g_assert(GTK_IS_KNOB(widget));

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));

	int oldstep = (int)(0.5f + (adj->value - adj->lower) / adj->step_increment);
	int step;
	int nsteps = (int)(0.5f + (adj->upper - adj->lower) / adj->step_increment);
	if (dir_down)
		step = oldstep - 1;
	else
		step = oldstep + 1;
	FAUSTFLOAT value = adj->lower + step * double(adj->upper - adj->lower) / nsteps;
	gtk_widget_grab_focus(widget);
	gtk_range_set_value(GTK_RANGE(widget), value);
}

/****************************************************************
 ** keyboard bindings
 */

static gboolean gtk_knob_key_press (GtkWidget* widget, GdkEventKey *event)
{
	g_assert(GTK_IS_KNOB(widget));

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	switch (event->keyval) {
	case GDK_Home:
		gtk_range_set_value(GTK_RANGE(widget), adj->lower);
		return TRUE;
	case GDK_End:
		gtk_range_set_value(GTK_RANGE(widget), adj->upper);
		return TRUE;
	case GDK_Up:
		gtk_knob_set_value(widget, 0);
		return TRUE;
	case GDK_Right:
		gtk_knob_set_value(widget, 0);
		return TRUE;
	case GDK_Down:
		gtk_knob_set_value(widget, 1);
		return TRUE;
	case GDK_Left:
		gtk_knob_set_value(widget, 1);
		return TRUE;
	}

	return FALSE;
}

/****************************************************************
 ** alternative (radial) knob motion mode (ctrl + mouse pressed)
 */

static void knob_pointer_event(GtkWidget* widget, gdouble x, gdouble y, int knob_x, int knob_y,
                               bool drag, int state)
{
	static double last_y = 2e20;
	GtkKnob *knob = GTK_KNOB(widget);
	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(widget));
	double radius =  std::min(knob_x, knob_y) / 2;
	int  knobx = (widget->allocation.width - knob_x) / 2;
	int  knoby = (widget->allocation.height - knob_y) / 2;
	double posx = (knobx + radius) - x; // x axis right -> left
	double posy = (knoby + radius) - y; // y axis top -> bottom
	double value;
	if (!drag) {
		if (state & GDK_CONTROL_MASK) {
			last_y = 2e20;
			return;
		} else {
			last_y = posy;
		}
	}
	if (last_y < 1e20) { // in drag started with Control Key
		const double scaling = 0.005;
		double scal = (state & GDK_SHIFT_MASK ? scaling*0.1 : scaling);
		value = (last_y - posy) * scal;
		last_y = posy;
		gtk_range_set_value(GTK_RANGE(widget), adj->value - value * (adj->upper - adj->lower));
		return;
	}

	double angle = atan2(-posx, posy) + M_PI; // clockwise, zero at 6 o'clock, 0 .. 2*M_PI
	if (drag) {
		// block "forbidden zone" and direct moves between quadrant 1 and 4
		int quadrant = 1 + int(angle/M_PI_2);
		if (knob->last_quadrant == 1 && (quadrant == 3 || quadrant == 4)) {
			angle = scale_zero;
		} else if (knob->last_quadrant == 4 && (quadrant == 1 || quadrant == 2)) {
			angle = 2*M_PI - scale_zero;
		} else {
			if (angle < scale_zero) {
				angle = scale_zero;
			} else if (angle > 2*M_PI - scale_zero) {
				angle = 2*M_PI - scale_zero;
			}
			knob->last_quadrant = quadrant;
		}
	} else {
		if (angle < scale_zero) {
			angle = scale_zero;
		} else if (angle > 2*M_PI - scale_zero) {
			angle = 2*M_PI - scale_zero;
		}
		knob->last_quadrant = 0;
	}
	angle = (angle - scale_zero) / (2 * (M_PI-scale_zero)); // normalize to 0..1
	gtk_range_set_value(GTK_RANGE(widget), adj->lower + angle * (adj->upper - adj->lower));
}

/****************************************************************
 ** mouse button pressed set value
 */

static gboolean gtk_knob_button_press (GtkWidget* widget, GdkEventButton *event)
{
	g_assert(GTK_IS_KNOB(widget));
	
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	

	switch (event->button) {
	case 1:  // left button
		gtk_widget_grab_focus(widget);
		gtk_widget_grab_default (widget);
		gtk_grab_add(widget);
		klass->button_is = 1;
		knob_pointer_event(widget, event->x, event->y, klass->knob_x, klass->knob_y,
						   false, event->state);
		break;
	case 2: //wheel
		klass->button_is = 2;
		break;
	case 3:  // right button 
		klass->button_is = 3;
		break;
	default: // do nothing
		break;
	}
	return TRUE;
}

/****************************************************************
 ** mouse button release
 */

static gboolean gtk_knob_button_release (GtkWidget* widget, GdkEventButton *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget))->button_is = 0;
	if (GTK_WIDGET_HAS_GRAB(widget))
		gtk_grab_remove(widget);
	return FALSE;
}

/****************************************************************
 ** set the value from mouse movement
 */

static gboolean gtk_knob_pointer_motion (GtkWidget* widget, GdkEventMotion *event)
{
	g_assert(GTK_IS_KNOB(widget));
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));
	
	gdk_event_request_motions (event);
	
	if (GTK_WIDGET_HAS_GRAB(widget)) {
		knob_pointer_event(widget, event->x, event->y, klass->knob_x, klass->knob_y,
						   true, event->state);
	}
	return FALSE;
}

/****************************************************************
 ** set value from mouseweel
 */

static gboolean gtk_knob_scroll (GtkWidget* widget, GdkEventScroll *event)
{
	usleep(5000);
	gtk_knob_set_value(widget, event->direction);
	return FALSE;
}

/****************************************************************
 ** init the GtkKnobClass
 */

static void gtk_knob_class_init (GtkKnobClass *klass)
{
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

	/** set here the sizes and steps for the used knob **/
//--------- small knob size and steps
	
	klass->knob_x = 30;
	klass->knob_y = 30;
	klass->knob_step = 86;

//--------- event button
	klass->button_is = 0;

//--------- connect the events with funktions
	widget_class->expose_event = gtk_knob_expose;
	widget_class->size_request = gtk_knob_size_request;
	widget_class->button_press_event = gtk_knob_button_press;
	widget_class->button_release_event = gtk_knob_button_release;
	widget_class->motion_notify_event = gtk_knob_pointer_motion;
	widget_class->key_press_event = gtk_knob_key_press;
	widget_class->scroll_event = gtk_knob_scroll;
}

/****************************************************************
 ** init the Knob type/size
 */

static void gtk_knob_init (GtkKnob *knob)
{
	GtkWidget* widget = GTK_WIDGET(knob);
	GtkKnobClass *klass =  GTK_KNOB_CLASS(GTK_OBJECT_GET_CLASS(widget));

	GTK_WIDGET_SET_FLAGS (GTK_WIDGET(knob), GTK_CAN_FOCUS);
	GTK_WIDGET_SET_FLAGS (GTK_WIDGET(knob), GTK_CAN_DEFAULT);

	widget->requisition.width = klass->knob_x;
	widget->requisition.height = klass->knob_y;
}

/****************************************************************
 ** redraw when value changed
 */

static gboolean gtk_knob_value_changed(gpointer obj)
{
	GtkWidget* widget = (GtkWidget* )obj;
	gtk_widget_queue_draw(widget);
	return FALSE;
}

/****************************************************************
 ** create small knob
 */

GtkWidget* GtkKnob::gtk_knob_new_with_adjustment(GtkAdjustment *_adjustment)
{
	GtkWidget* widget = GTK_WIDGET( g_object_new (GTK_TYPE_KNOB, NULL ));
	GtkKnob *knob = GTK_KNOB(widget);
	knob->last_quadrant = 0;
	if (widget) {
		gtk_range_set_adjustment(GTK_RANGE(widget), _adjustment);
		g_signal_connect(GTK_OBJECT(widget), "value-changed",
		                 G_CALLBACK(gtk_knob_value_changed), widget);
	}
	return widget;
}

/****************************************************************
 ** get the Knob type
 */

GType gtk_knob_get_type (void)
{
	static GType kn_type = 0;
	if (!kn_type) {
		static const GTypeInfo kn_info = {
			sizeof(GtkKnobClass), NULL,  NULL, (GClassInitFunc)gtk_knob_class_init, NULL, NULL, sizeof(GtkKnob), 0, (GInstanceInitFunc)gtk_knob_init, NULL
		};
		kn_type = g_type_register_static(GTK_TYPE_RANGE,  "GtkKnob", &kn_info, (GTypeFlags)0);
	}
	return kn_type;
}
}/* end of gtk_knob namespace */

gtk_knob::GtkKnob myGtkKnob;

/**
 * rmWhiteSpaces(): Remove the leading and trailing white spaces of a string
 * (but not those in the middle of the string)
 */
static std::string rmWhiteSpaces(const std::string& s)
{
    size_t i = s.find_first_not_of(" \t");
    size_t j = s.find_last_not_of(" \t");

    if (i != std::string::npos & j != std::string::npos) {
        return s.substr(i, 1+j-i);
    } else {
        return "";
    }
}

/**
 * Extracts metdata from a label : 'vol [unit: dB]' -> 'vol' + metadata
 */
static void extractMetadata(const std::string& fulllabel, std::string& label, std::map<std::string, std::string>& metadata)
{
    enum {kLabel, kEscape1, kEscape2, kEscape3, kKey, kValue};
    int state = kLabel; int deep = 0;
    std::string key, value;

    for (unsigned int i=0; i < fulllabel.size(); i++) {
        char c = fulllabel[i];
        switch (state) {
            case kLabel :
                assert (deep == 0);
                switch (c) {
                    case '\\' : state = kEscape1; break;
                    case '[' : state = kKey; deep++; break;
                    default : label += c;
                }
                break;

            case kEscape1 :
                label += c;
                state = kLabel;
                break;

            case kEscape2 :
                key += c;
                state = kKey;
                break;

            case kEscape3 :
                value += c;
                state = kValue;
                break;

            case kKey :
                assert (deep > 0);
                switch (c) {
                    case '\\' :  state = kEscape2;
                                break;

                    case '[' :  deep++;
                                key += c;
                                break;

                    case ':' :  if (deep == 1) {
                                    state = kValue;
                                } else {
                                    key += c;
                                }
                                break;
                    case ']' :  deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)] = "";
                                    state = kLabel;
                                    key="";
                                    value="";
                                } else {
                                    key += c;
                                }
                                break;
                    default :   key += c;
                }
                break;

            case kValue :
                assert (deep > 0);
                switch (c) {
                    case '\\' : state = kEscape3;
                                break;

                    case '[' :  deep++;
                                value += c;
                                break;

                    case ']' :  deep--;
                                if (deep < 1) {
                                    metadata[rmWhiteSpaces(key)]=rmWhiteSpaces(value);
                                    state = kLabel;
                                    key="";
                                    value="";
                                } else {
                                    value += c;
                                }
                                break;
                    default :   value += c;
                }
                break;

            default :
                std::cerr << "ERROR unrecognized state " << state << std::endl;
        }
    }
    label = rmWhiteSpaces(label);
}

class GTKUI : public GUI
{
 private :
    static bool                         		fInitialized;
    static std::map<FAUSTFLOAT*, FAUSTFLOAT> 	fGuiSize;       // map widget zone with widget size coef
    static std::map<FAUSTFLOAT*, std::string>   fTooltip;       // map widget zone with tooltip strings
    static std::set<FAUSTFLOAT*>             	fKnobSet;       // set of widget zone to be knobs
	std::string									gGroupTooltip;
    
    bool isKnob(FAUSTFLOAT* zone) {return fKnobSet.count(zone) > 0;}
    
 protected :
    GtkWidget*  fWindow;
    int         fTop;
    GtkWidget*  fBox[stackSize];
    int         fMode[stackSize];
    bool        fStopped;

    GtkWidget* addWidget(const char* label, GtkWidget* w);
    virtual void pushBox(int mode, GtkWidget* w);

        
 public :
    static const gboolean expand = TRUE;
    static const gboolean fill = TRUE;
    static const gboolean homogene = FALSE;
         
    GTKUI(char * name, int* pargc, char*** pargv);

    // -- Labels and metadata

    virtual void declare(FAUSTFLOAT* zone, const char* key, const char* value);
    virtual int  checkLabelOptions(GtkWidget* widget, const std::string& fullLabel, std::string& simplifiedLabel);
    virtual void checkForTooltip(FAUSTFLOAT* zone, GtkWidget* widget);
    
    // -- layout groups
    
    virtual void openTabBox(const char* label = "");
    virtual void openHorizontalBox(const char* label = "");
    virtual void openVerticalBox(const char* label = "");
    virtual void closeBox();

    // -- active widgets
    
    virtual void addButton(const char* label, FAUSTFLOAT* zone);
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone);
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);   
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step); 
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);

    // -- passive display widgets
    
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max);
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max);
    
    // -- layout groups - internal
    
    virtual void openFrameBox(const char* label);   
   
    // -- extra widget's layouts

    virtual void openDialogBox(const char* label, FAUSTFLOAT* zone);
    virtual void openEventBox(const char* label = "");
    virtual void openHandleBox(const char* label = "");
    virtual void openExpanderBox(const char* label, FAUSTFLOAT* zone);
    
    virtual void adjustStack(int n);
    
    // -- active widgets - internal
    virtual void addToggleButton(const char* label, FAUSTFLOAT* zone);
    virtual void addKnob(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step);
    
    // -- passive display widgets - internal
    
    virtual void addNumDisplay(const char* label, FAUSTFLOAT* zone, int precision);
    virtual void addTextDisplay(const char* label, FAUSTFLOAT* zone, const char* names[], FAUSTFLOAT min, FAUSTFLOAT max);
   
    virtual void show();
    virtual void run();
    
};

/******************************************************************************
*******************************************************************************

                                GRAPHIC USER INTERFACE (v2)
                                  gtk implementation

*******************************************************************************
*******************************************************************************/

// global static fields

bool                             GTKUI::fInitialized = false;
std::map<FAUSTFLOAT*, FAUSTFLOAT>     GTKUI::fGuiSize;
std::map<FAUSTFLOAT*, std::string>    GTKUI::fTooltip;
std::set<FAUSTFLOAT*>                 GTKUI::fKnobSet;       // set of widget zone to be knobs

/**
* Format tooltip string by replacing some white spaces by 
* return characters so that line width doesn't exceed n.
* Limitation : long words exceeding n are not cut 
*/
static std::string formatTooltip(unsigned int n, const std::string& tt)
{
	std::string  ss = tt;	// ss string we are going to format
	unsigned int lws = 0;	// last white space encountered
	unsigned int lri = 0;	// last return inserted
	for (unsigned int i = 0; i < tt.size(); i++) {
		if (tt[i] == ' ') lws = i;
		if (((i-lri) >= n) && (lws > lri)) {
			// insert return here
			ss[lws] = '\n';
			lri = lws;
		}
	}
	//std::cout << ss;
	return ss;
}

static gint delete_event(GtkWidget* widget, GdkEvent *event, gpointer data)
{
    return FALSE; 
}

static void destroy_event(GtkWidget* widget, gpointer data)
{
    gtk_main_quit ();
}

GTKUI::GTKUI(char * name, int* pargc, char*** pargv) 
{
    if (!fInitialized) {
        gtk_init(pargc, pargv);
        fInitialized = true;
    }
    
    fWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    //gtk_container_set_border_width (GTK_CONTAINER (fWindow), 10);
    gtk_window_set_title (GTK_WINDOW (fWindow), name);
    gtk_signal_connect (GTK_OBJECT (fWindow), "delete_event", GTK_SIGNAL_FUNC (delete_event), NULL);
    gtk_signal_connect (GTK_OBJECT (fWindow), "destroy", GTK_SIGNAL_FUNC (destroy_event), NULL);

    fTop = 0;
    fBox[fTop] = gtk_vbox_new (homogene, 4);
    fMode[fTop] = kBoxMode;
    gtk_container_add (GTK_CONTAINER (fWindow), fBox[fTop]);
    fStopped = false;
}

// empilement des boites

void GTKUI::pushBox(int mode, GtkWidget* w)
{
    ++fTop;
    assert(fTop < stackSize);
    fMode[fTop] = mode;
    fBox[fTop] = w;
}

/**
 * Remove n levels from the stack S before the top level
 * adjustStack(n): S -> S' with S' = S(0),S(n+1),S(n+2),...
 */
void GTKUI::adjustStack(int n)
{
    if (n > 0) {
        assert(fTop >= n);
        fTop -= n; 
        fMode[fTop] = fMode[fTop+n];
        fBox[fTop] = fBox[fTop+n];
    }
}

void GTKUI::closeBox()
{
    --fTop;
    assert(fTop >= 0);
}

/**
 * Analyses the widget zone metadata declarations and takes
 * appropriate actions 
 */
void GTKUI::declare(FAUSTFLOAT* zone, const char* key, const char* value)
{
	if (zone == 0) {
		// special zone 0 means group metadata
		if (strcmp(key,"tooltip") == 0) {
			// only group tooltip are currently implemented
			gGroupTooltip = formatTooltip(30, value);
		}
	} else {
		if (strcmp(key,"size")==0) {
			fGuiSize[zone] = atof(value);
		}
		else if (strcmp(key,"tooltip") == 0) {
			fTooltip[zone] = formatTooltip(30,value) ;
		}
		else if (strcmp(key,"style") == 0) {
			if (strcmp(value,"knob") == 0) {
				fKnobSet.insert(zone);
			}
		}
	}
}
    
/**
 * Analyses a full label and activates the relevant options. returns a simplified
 * label (without options) and an amount of stack adjustement (in case additional
 * containers were pushed on the stack). 
 */

int GTKUI::checkLabelOptions(GtkWidget* widget, const std::string& fullLabel, std::string& simplifiedLabel)
{   
    std::map<std::string, std::string> metadata;
    extractMetadata(fullLabel, simplifiedLabel, metadata);

    if (metadata.count("tooltip")) {
        gtk_tooltips_set_tip (gtk_tooltips_new (), widget, metadata["tooltip"].c_str(), NULL);
    }
    if (metadata["option"] == "detachable") {
        openHandleBox(simplifiedLabel.c_str());
        return 1;
    }

	//---------------------
	if (gGroupTooltip != std::string()) {
		gtk_tooltips_set_tip (gtk_tooltips_new (), widget, gGroupTooltip.c_str(), NULL);
		gGroupTooltip = std::string();
	}
	
	//----------------------
    // no adjustement of the stack needed
    return 0;
}

/**
 * Check if a tooltip is associated to a zone and add it to the corresponding widget
 */
void GTKUI::checkForTooltip(FAUSTFLOAT* zone, GtkWidget* widget)
{
    if (fTooltip.count(zone)) {
        gtk_tooltips_set_tip (gtk_tooltips_new (), widget, fTooltip[zone].c_str(), NULL);
    }
}

// The different boxes

void GTKUI::openFrameBox(const char* label)
{
    GtkWidget* box = gtk_frame_new (label);
    //gtk_container_set_border_width (GTK_CONTAINER (box), 10);
            
    pushBox(kSingleMode, addWidget(label, box));
}

void GTKUI::openTabBox(const char* fullLabel)
{
    std::string label;
    GtkWidget* widget = gtk_notebook_new();

    int adjust = checkLabelOptions(widget, fullLabel, label);
    
    pushBox(kTabMode, addWidget(label.c_str(), widget));

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openHorizontalBox(const char* fullLabel)
{   
    std::string label;
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    int adjust = checkLabelOptions(box, fullLabel, label);

    gtk_container_set_border_width (GTK_CONTAINER (box), 10);
    label = startWith(label, "0x") ? "" : label;
            
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label.c_str(), gtk_frame_new (label.c_str()));
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label.c_str(), box));
    }

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openVerticalBox(const char* fullLabel)
{
    std::string label;
    GtkWidget* box = gtk_vbox_new (homogene, 4);
    int adjust = checkLabelOptions(box, fullLabel, label);

    gtk_container_set_border_width (GTK_CONTAINER (box), 10);
    label = startWith(label, "0x") ? "" : label;
            
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label.c_str(), gtk_frame_new (label.c_str()));
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label.c_str(), box));
    }

    // adjust stack because otherwise Handlebox will remain open
    adjustStack(adjust);
}

void GTKUI::openHandleBox(const char* label)
{
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_handle_box_new ());
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

void GTKUI::openEventBox(const char* label)
{
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_event_box_new ());
        gtk_container_add (GTK_CONTAINER(frame), box);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

struct uiExpanderBox : public uiItem
{
    GtkExpander* fButton;
    uiExpanderBox(GUI* ui, FAUSTFLOAT* zone, GtkExpander* b) : uiItem(ui, zone), fButton(b) {}
    static void expanded (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = gtk_expander_get_expanded  (GTK_EXPANDER(widget));
        if (v == 1.000000) {
            v = 0;
        } else {
            v = 1;
        }
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()
    {
        FAUSTFLOAT v = *fZone;
        fCache = v;
        gtk_expander_set_expanded(GTK_EXPANDER(fButton), v);
    }
};

void GTKUI::openExpanderBox(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* box = gtk_hbox_new (homogene, 4);
    gtk_container_set_border_width (GTK_CONTAINER (box), 2);
    label = startWith(label, "0x") ? "" : label;
    if (fMode[fTop] != kTabMode && label[0] != 0) {
        GtkWidget* frame = addWidget(label, gtk_expander_new (label));
        gtk_container_add (GTK_CONTAINER(frame), box);
        uiExpanderBox* c = new uiExpanderBox(this, zone, GTK_EXPANDER(frame));
        gtk_signal_connect (GTK_OBJECT (frame), "activate", GTK_SIGNAL_FUNC (uiExpanderBox::expanded), (gpointer)c);
        gtk_widget_show(box);
        pushBox(kBoxMode, box);
    } else {
        pushBox(kBoxMode, addWidget(label, box));
    }
}

GtkWidget* GTKUI::addWidget(const char* label, GtkWidget* w)
{ 
    switch (fMode[fTop]) {
        case kSingleMode    : gtk_container_add (GTK_CONTAINER(fBox[fTop]), w);                             break;
        case kBoxMode       : gtk_box_pack_start (GTK_BOX(fBox[fTop]), w, expand, fill, 0);                 break;
        case kTabMode       : gtk_notebook_append_page (GTK_NOTEBOOK(fBox[fTop]), w, gtk_label_new(label)); break;
    }
    gtk_widget_show (w);
    return w;
}

// --------------------------- Press button ---------------------------

struct uiButton : public uiItem
{
    GtkButton*  fButton;
    
    uiButton(GUI* ui, FAUSTFLOAT* zone, GtkButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void pressed(GtkWidget* widget, gpointer data)
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(1.0);
    }

    static void released(GtkWidget* widget, gpointer data)
    {
        uiItem* c = (uiItem*) data;
        c->modifyZone(0.0);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        if (v > 0.0) gtk_button_pressed(fButton); else gtk_button_released(fButton);
    }
};

void GTKUI::addButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_button_new_with_label (label);
    addWidget(label, button);
    
    uiButton* c = new uiButton(this, zone, GTK_BUTTON(button));
    
    gtk_signal_connect (GTK_OBJECT (button), "pressed", GTK_SIGNAL_FUNC (uiButton::pressed), (gpointer) c);
    gtk_signal_connect (GTK_OBJECT (button), "released", GTK_SIGNAL_FUNC (uiButton::released), (gpointer) c);

    checkForTooltip(zone, button);
}

// ---------------------------  Toggle Buttons ---------------------------

struct uiToggleButton : public uiItem
{
    GtkToggleButton* fButton;
    
    uiToggleButton(GUI* ui, FAUSTFLOAT* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void toggled (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_toggle_button_set_active(fButton, v > 0.0); 
    }
};

void GTKUI::addToggleButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_toggle_button_new_with_label (label);
    addWidget(label, button);
    
    uiToggleButton* c = new uiToggleButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC (uiToggleButton::toggled), (gpointer) c);

    checkForTooltip(zone, button);
}

void show_dialog(GtkWidget* widget, gpointer data)
{
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget)) == TRUE)
    {
        gtk_widget_show(GTK_WIDGET(data));
        gint root_x, root_y;
        gtk_window_get_position (GTK_WINDOW(data), &root_x, &root_y);
        root_y -= 120;
        gtk_window_move(GTK_WINDOW(data), root_x, root_y);
    }
    else gtk_widget_hide(GTK_WIDGET(data));
}

static gboolean deleteevent( GtkWidget* widget, gpointer   data )
{
    return TRUE;
} 

void GTKUI::openDialogBox(const char* label, FAUSTFLOAT* zone)
{
    // create toplevel window and set properties
    GtkWidget* dialog = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_decorated(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_deletable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
    gtk_window_set_gravity(GTK_WINDOW(dialog), GDK_GRAVITY_SOUTH);
    gtk_window_set_transient_for (GTK_WINDOW(dialog), GTK_WINDOW(fWindow));
    gtk_window_set_position (GTK_WINDOW(dialog), GTK_WIN_POS_MOUSE);
    gtk_window_set_keep_below (GTK_WINDOW(dialog), FALSE);
    gtk_window_set_title (GTK_WINDOW (dialog), label);
    g_signal_connect (G_OBJECT (dialog), "delete_event", G_CALLBACK (deleteevent), NULL); 
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    GtkWidget* box = gtk_hbox_new (homogene, 4);
 
    *zone = 0.0;
    GtkWidget* button = gtk_toggle_button_new ();
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC (show_dialog), (gpointer) dialog);
 
    gtk_container_add (GTK_CONTAINER(fBox[fTop]), button);
    gtk_container_add (GTK_CONTAINER(dialog), box);
    gtk_widget_show (button);
    gtk_widget_show(box);
    pushBox(kBoxMode, box);
}

// ---------------------------  Check Button ---------------------------

struct uiCheckButton : public uiItem
{
    GtkToggleButton* fButton;
    
    uiCheckButton(GUI* ui, FAUSTFLOAT* zone, GtkToggleButton* b) : uiItem(ui, zone), fButton(b) {}
    
    static void toggled (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = (GTK_TOGGLE_BUTTON (widget)->active) ? 1.0 : 0.0; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_toggle_button_set_active(fButton, v > 0.0); 
    }
};

void GTKUI::addCheckButton(const char* label, FAUSTFLOAT* zone)
{
    *zone = 0.0;
    GtkWidget* button = gtk_check_button_new_with_label (label);
    addWidget(label, button);
    
    uiCheckButton* c = new uiCheckButton(this, zone, GTK_TOGGLE_BUTTON(button));
    gtk_signal_connect (GTK_OBJECT (button), "toggled", GTK_SIGNAL_FUNC(uiCheckButton::toggled), (gpointer) c);

    checkForTooltip(zone, button);
}

// ---------------------------  Adjustmenty based widgets ---------------------------

struct uiAdjustment : public uiItem
{
    GtkAdjustment* fAdj;
    
    uiAdjustment(GUI* ui, FAUSTFLOAT* zone, GtkAdjustment* adj) : uiItem(ui, zone), fAdj(adj) {}
    
    static void changed (GtkWidget* widget, gpointer data)
    {
        FAUSTFLOAT v = GTK_ADJUSTMENT (widget)->value; 
        ((uiItem*)data)->modifyZone(v);
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_adjustment_set_value(fAdj, v);  
    }
};

// --------------------------- format knob value display ---------------------------

struct uiValueDisplay : public uiItem
{
	GtkLabel* fLabel;
	int	fPrecision ;

	uiValueDisplay(GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, int precision)
		: uiItem(ui, zone), fLabel(label), fPrecision(precision) {}

	virtual void reflectZone()
		{
			FAUSTFLOAT v = *fZone;
			fCache = v;
			char s[64];
			if (fPrecision <= 0)
				snprintf(s, 63, "%d", int(v));

			else if (fPrecision > 3)
				snprintf(s, 63, "%f", v);

			else if (fPrecision == 1)
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[1-1], v);
			}
			else if (fPrecision == 2)
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[2-1], v);
			}
			else
			{
				const char* format[] = {"%.1f", "%.2f", "%.3f"};
				snprintf(s, 63, format[3-1], v);
			}
			gtk_label_set_text(fLabel, s);
		}
};

// ------------------------------- Knob -----------------------------------------

void GTKUI::addKnob(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	*zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
	GtkWidget* slider = gtk_vbox_new (FALSE, 0);
	GtkWidget* fil = gtk_vbox_new (FALSE, 0);
	GtkWidget* rei = gtk_vbox_new (FALSE, 0);
	GtkWidget* re = myGtkKnob.gtk_knob_new_with_adjustment(GTK_ADJUSTMENT(adj));
	GtkWidget* lw = gtk_label_new("");
	new uiValueDisplay(this, zone, GTK_LABEL(lw),precision(step));
	gtk_container_add (GTK_CONTAINER(rei), re);
	if(fGuiSize[zone]) {
		FAUSTFLOAT size = 30 * fGuiSize[zone];
		gtk_widget_set_size_request(rei, size, size );
		gtk_box_pack_start (GTK_BOX(slider), fil, TRUE, TRUE, 0);
		gtk_box_pack_start (GTK_BOX(slider), rei, FALSE, FALSE, 0);
	} else {
		gtk_container_add (GTK_CONTAINER(slider), fil);
		gtk_container_add (GTK_CONTAINER(slider), rei);
	}
	gtk_container_add (GTK_CONTAINER(slider), lw);
	gtk_widget_show_all(slider);
	
    label = startWith(label, "0x") ? "" : label;
	if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }

    checkForTooltip(zone, slider);
}

// -------------------------- Vertical Slider -----------------------------------

void GTKUI::addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
	GtkWidget* slider = gtk_vscale_new (GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(slider), precision(step));
	FAUSTFLOAT size = 160;
	if(fGuiSize[zone]) {
		size = 160 * fGuiSize[zone];
	}
	gtk_widget_set_size_request(slider, -1, size);
	
    gtk_range_set_inverted (GTK_RANGE(slider), TRUE);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }

    checkForTooltip(zone, slider);
}

// -------------------------- Horizontal Slider -----------------------------------

void GTKUI::addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, 0);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));

    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    
    GtkWidget* slider = gtk_hscale_new (GTK_ADJUSTMENT(adj));
	gtk_scale_set_digits(GTK_SCALE(slider), precision(step));
	FAUSTFLOAT size = 160;
	if(fGuiSize[zone]) {
		size = 160 * fGuiSize[zone];
	}
	gtk_widget_set_size_request(slider, size, -1);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, slider);
        closeBox();
    } else {
        addWidget(label, slider);
    }             

    checkForTooltip(zone, slider);
}

// ------------------------------ Num Entry -----------------------------------

void GTKUI::addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
{
	if (isKnob(zone)) { 
		addKnob(label, zone, init, min, max, step);
		return;
	} 
    *zone = init;
    GtkObject* adj = gtk_adjustment_new(init, min, max, step, 10*step, step);
    
    uiAdjustment* c = new uiAdjustment(this, zone, GTK_ADJUSTMENT(adj));
    gtk_signal_connect (GTK_OBJECT (adj), "value-changed", GTK_SIGNAL_FUNC (uiAdjustment::changed), (gpointer) c);
    GtkWidget* spinner = gtk_spin_button_new (GTK_ADJUSTMENT(adj), 0.005, precision(step));
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, spinner);
        closeBox();
    } else {
        addWidget(label, spinner);
    }

    checkForTooltip(zone, spinner);
}

// ==========================   passive widgets ===============================

// ------------------------------ Progress Bar -----------------------------------

struct uiBargraph : public uiItem
{
    GtkProgressBar*     fProgressBar;
    FAUSTFLOAT          fMin;
    FAUSTFLOAT          fMax;
    
    uiBargraph(GUI* ui, FAUSTFLOAT* zone, GtkProgressBar* pbar, FAUSTFLOAT lo, FAUSTFLOAT hi) 
            : uiItem(ui, zone), fProgressBar(pbar), fMin(lo), fMax(hi) {}

    FAUSTFLOAT scale(FAUSTFLOAT v)        { return (v-fMin)/(fMax-fMin); }
    
    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v; 
        gtk_progress_bar_set_fraction(fProgressBar, scale(v));  
    }
};

void GTKUI::addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* pb = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(pb), GTK_PROGRESS_BOTTOM_TO_TOP);
    gtk_widget_set_size_request(pb, 8, -1);
    new uiBargraph(this, zone, GTK_PROGRESS_BAR(pb), lo, hi);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, pb);
        closeBox();
    } else {
        addWidget(label, pb);
    }

    checkForTooltip(zone, pb);
}
    
void GTKUI::addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* pb = gtk_progress_bar_new();
    gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(pb), GTK_PROGRESS_LEFT_TO_RIGHT);
    gtk_widget_set_size_request(pb, -1, 8);
    new uiBargraph(this, zone, GTK_PROGRESS_BAR(pb), lo, hi);
    
    label = startWith(label, "0x") ? "" : label;
    if (label && label[0] != 0) {
        openFrameBox(label);
        addWidget(label, pb);
        closeBox();
    } else {
        addWidget(label, pb);
    }

    checkForTooltip(zone, pb);
}

// ------------------------------ Num Display -----------------------------------

struct uiNumDisplay : public uiItem
{
    GtkLabel* fLabel;
    int fPrecision;
    
    uiNumDisplay(GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, int precision) 
            : uiItem(ui, zone), fLabel(label), fPrecision(precision) {}

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v;
        char s[64]; 
        if (fPrecision <= 0) { 
            snprintf(s, 63, "%d", int(v)); 
        } else if (fPrecision>3) {
            snprintf(s, 63, "%f", v);
        } else {
            const char* format[] = {"%.1f", "%.2f", "%.3f"};
            snprintf(s, 63, format[fPrecision-1], v);
        }
        gtk_label_set_text(fLabel, s);
    }
};

void GTKUI::addNumDisplay(const char* label, FAUSTFLOAT* zone, int precision)
{
    GtkWidget* lw = gtk_label_new("");
    new uiNumDisplay(this, zone, GTK_LABEL(lw), precision);
    openFrameBox(label);
    addWidget(label, lw);
    closeBox();

    checkForTooltip(zone, lw);
}

// ------------------------------ Text Display -----------------------------------

struct uiTextDisplay : public uiItem
{
    GtkLabel*       fLabel;
    const char**    fNames;
    FAUSTFLOAT      fMin;
    FAUSTFLOAT      fMax;
    int             fNum;
    
    uiTextDisplay (GUI* ui, FAUSTFLOAT* zone, GtkLabel* label, const char* names[], FAUSTFLOAT lo, FAUSTFLOAT hi)
                    : uiItem(ui, zone), fLabel(label), fNames(names), fMin(lo), fMax(hi)
    {
        fNum = 0;
        while (fNames[fNum] != 0) fNum++;
    }

    virtual void reflectZone()  
    { 
        FAUSTFLOAT v = *fZone;
        fCache = v;
        
        int idx = int(fNum*(v-fMin)/(fMax-fMin));
        
        if      (idx < 0)       idx = 0; 
        else if (idx >= fNum)   idx = fNum-1;
                
        gtk_label_set_text(fLabel, fNames[idx]); 
    }
};
    
void GTKUI::addTextDisplay(const char* label, FAUSTFLOAT* zone, const char* names[], FAUSTFLOAT lo, FAUSTFLOAT hi)
{
    GtkWidget* lw = gtk_label_new("");
    new uiTextDisplay (this, zone, GTK_LABEL(lw), names, lo, hi);
    openFrameBox(label);
    addWidget(label, lw);
    closeBox();

    checkForTooltip(zone, lw);
}

void GTKUI::show() 
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
}

/**
 * Update all user items reflecting zone z
 */
    
static gboolean callUpdateAllGuis(gpointer)
{ 
    GUI::updateAllGuis(); 
    return TRUE;
}

void GTKUI::run() 
{
    assert(fTop == 0);
    gtk_widget_show  (fBox[0]);
    gtk_widget_show  (fWindow);
    gtk_timeout_add(40, callUpdateAllGuis, 0);
    gtk_main ();
    stop();
}

#endif

/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/
/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
#ifndef __jack_dsp__
#define __jack_dsp__

#include <stdio.h>
#include <cstdlib>
#include <list>
#include <string.h>
#include <jack/jack.h>
#ifdef JACK_IOS
#include <jack/custom.h>
#endif
/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

						An abstraction layer over audio layer

*******************************************************************************
*******************************************************************************/

#ifndef __audio__
#define __audio__
			
class dsp;

typedef void (* shutdown_callback)(const char* message, void* arg);

typedef void (* buffer_size_callback)(int frames, void* arg);

class audio {
    
 public:
			 audio() {}
	virtual ~audio() {}
	
	virtual bool init(const char* name, dsp*)               = 0;
	virtual bool start()                                    = 0;
	virtual void stop()                                     = 0;
    virtual void shutdown(shutdown_callback cb, void* arg)  {}
    
    virtual int get_buffer_size() = 0;
    virtual int get_sample_rate() = 0;
    
};
					
#endif
/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

								FAUST DSP

*******************************************************************************
*******************************************************************************/

#ifndef __dsp__
#define __dsp__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;

//----------------------------------------------------------------
//  signal processor definition
//----------------------------------------------------------------

class dsp {
 protected:
	int fSamplingFreq;
 public:
	dsp() {}
	virtual ~dsp() {}

	virtual int getNumInputs() 										= 0;
	virtual int getNumOutputs() 									= 0;
	virtual void buildUserInterface(UI* ui_interface) 				= 0;
	virtual void init(int samplingRate) 							= 0;
 	virtual void compute(int len, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) 	= 0;
};

// On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
// flags to avoid costly denormals
#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif

#if defined(_WIN32) && !defined(__MINGW32__)
#define snprintf _snprintf
#endif

/******************************************************************************
*******************************************************************************

							JACK AUDIO INTERFACE

*******************************************************************************
*******************************************************************************/

class jackaudio : public audio {
    
    protected:

        dsp*			fDsp;               // FAUST DSP
        jack_client_t*	fClient;            // JACK client
    
        int				fNumInChans;		// number of input channels
        int				fNumOutChans;       // number of output channels
    
        jack_port_t**	fInputPorts;        // JACK input ports
        jack_port_t**	fOutputPorts;       // JACK output ports
    
        shutdown_callback fShutdown;        // Shutdown callback to be called by libjack
        void*           fShutdownArg;       // Shutdown callback data
        void*           fIconData;          // iOS specific
        int             fIconSize;          // iOS specific
        bool            fAutoConnect;       // autoconnect with system in/out ports
        
        std::list<std::pair<std::string, std::string> > fConnections;		// Connections list
    
        static int _jack_srate(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The sample rate is now %u/sec\n", nframes);
            return 0;
        }
        
        static void _jack_shutdown(void* arg) 
        {}
       
        static void _jack_info_shutdown(jack_status_t code, const char* reason, void* arg)
        {
            fprintf(stderr, "%s\n", reason);
            static_cast<jackaudio*>(arg)->shutdown(reason);
        }
        
        static int _jack_process(jack_nframes_t nframes, void* arg)
        {
            return static_cast<jackaudio*>(arg)->process(nframes);
        }
        
        static int _jack_buffersize(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The buffer size is now %u/sec\n", nframes);
            return 0;
        }
     
        #ifdef _OPENMP
        static void* _jack_thread(void* arg)
        {
            jackaudio* audio = (jackaudio*)arg;
            audio->process_thread();
            return 0;
        }
        #endif
        
        void shutdown(const char* message)
        {
            fClient = NULL;
            
            if (fShutdown) {
                fShutdown(message, fShutdownArg);
            } else {
                exit(1); // By default
            }
        }
        
        // Save client connections
        void save_connections()
        {
            fConnections.clear();
            
             for (int i = 0; i < fNumInChans; i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fInputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(connected_port[port], jack_port_name(fInputPorts[i])));
//                        printf("INPUT %s ==> %s\n", connected_port[port], jack_port_name(fInputPorts[i]));
                    }
                    jack_free(connected_port);
                }
            }
       
            for (int i = 0; i < fNumOutChans; i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fOutputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(jack_port_name(fOutputPorts[i]), connected_port[port]));
//                        printf("OUTPUT %s ==> %s\n", jack_port_name(fOutputPorts[i]), connected_port[port]);
                    }
                    jack_free(connected_port);
                }
            }
        }

        // Load previous client connections
        void load_connections()
        {
            std::list<std::pair<std::string, std::string> >::const_iterator it;
            for (it = fConnections.begin(); it != fConnections.end(); it++) {
                std::pair<std::string, std::string> connection = *it;
                jack_connect(fClient, connection.first.c_str(), connection.second.c_str());
            }
        }

    public:
    
        jackaudio(const void* icon_data = 0, size_t icon_size = 0, bool auto_connect = true) 
            : fDsp(0), fClient(0), fNumInChans(0), fNumOutChans(0), 
            fInputPorts(0), fOutputPorts(0), 
            fShutdown(0), fShutdownArg(0),
            fAutoConnect(auto_connect)
        {
            if (icon_data) {
                fIconData = malloc(icon_size);
                fIconSize = icon_size;
                memcpy(fIconData, icon_data, icon_size);
            } else {
                fIconData = NULL;
                fIconSize = 0;
            }
        }
        
        virtual ~jackaudio() 
        { 
            if(fClient){
                stop();
                
                for (int i = 0; i < fNumInChans; i++) {
                    jack_port_unregister(fClient, fInputPorts[i]);
                }
                for (int i = 0; i < fNumOutChans; i++) {
                    jack_port_unregister(fClient, fOutputPorts[i]);
                }
                jack_client_close(fClient);
                
                delete[] fInputPorts;
                delete[] fOutputPorts;
                
                if (fIconData) {
                    free(fIconData);
                }
            }
        }
        
        jack_client_t* getClient() { return fClient; }

        virtual bool init(const char* name, dsp* DSP) 
        {
            if (!init(name)) {
                return false;
            }
            set_dsp(DSP);
            return true;
        }

        virtual bool init(const char* name) 
        {
            if ((fClient = jack_client_open(name, JackNullOption, NULL)) == 0) {
                fprintf(stderr, "JACK server not running ?\n");
                return false;
            }
        #ifdef JACK_IOS
            jack_custom_publish_data(fClient, "icon.png", fIconData, fIconSize);
        #endif
        
        #ifdef _OPENMP
            jack_set_process_thread(fClient, _jack_thread, this);
        #else
            jack_set_process_callback(fClient, _jack_process, this);
        #endif
        
            jack_set_sample_rate_callback(fClient, _jack_srate, this);
            jack_set_buffer_size_callback(fClient, _jack_buffersize, this);
            jack_on_info_shutdown(fClient, _jack_info_shutdown, this);
        
            return true;
        }    
    
        virtual bool set_dsp(dsp* DSP){
            fDsp = DSP;
            
            fNumInChans  = fDsp->getNumInputs();
            fNumOutChans = fDsp->getNumOutputs();
            
            fInputPorts = new jack_port_t*[fNumInChans];
            fOutputPorts = new jack_port_t*[fNumOutChans];
            
            for (int i = 0; i < fNumInChans; i++) {
                char buf[256];
                snprintf(buf, 256, "in_%d", i);
                fInputPorts[i] = jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
            }
            for (int i = 0; i < fNumOutChans; i++) {
                char buf[256];
                snprintf(buf, 256, "out_%d", i);
                fOutputPorts[i] = jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            }
            
            fDsp->init(jack_get_sample_rate(fClient));
            return true;
        }
    
        virtual bool start() 
        {
            if (jack_activate(fClient)) {
                fprintf(stderr, "Cannot activate client");
                return false;
            }
            
            if (fConnections.size() > 0) {
                load_connections();
            } else if (fAutoConnect) {
                default_connections();
            }
            return true;
        }

        virtual void stop() 
        {
            if(fClient){

                save_connections();
                jack_deactivate(fClient);
            }
        }
    
        virtual void shutdown(shutdown_callback cb, void* arg)
        {
            fShutdown = cb;
            fShutdownArg = arg;
        }
        
        virtual int get_buffer_size() { return jack_get_buffer_size(fClient); }
        virtual int get_sample_rate() { return jack_get_sample_rate(fClient); }

        // jack callbacks
        virtual int	process(jack_nframes_t nframes) 
        {
            AVOIDDENORMALS;
            // Retrieve JACK inputs/output audio buffers
			float** fInChannel = (float**)alloca(fNumInChans*sizeof(float*));
            for (int i = 0; i < fNumInChans; i++) {
                fInChannel[i] = (float*)jack_port_get_buffer(fInputPorts[i], nframes);
            }
			float** fOutChannel = (float**)alloca(fNumOutChans*sizeof(float*));
            for (int i = 0; i < fNumOutChans; i++) {
                fOutChannel[i] = (float*)jack_port_get_buffer(fOutputPorts[i], nframes);
            }
            fDsp->compute(nframes, fInChannel, fOutChannel);
            return 0;
        }
    
        // Connect to physical inputs/outputs
        void default_connections()
        {
            // To avoid feedback at launch time, don't connect hardware inputs
            /*
            char** physicalOutPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsOutput);
            if (physicalOutPorts != NULL) {
                for (int i = 0; i < fNumInChans && physicalOutPorts[i]; i++) {
                jack_connect(fClient, physicalOutPorts[i], jack_port_name(fInputPorts[i]));
            }
                jack_free(physicalOutPorts);
            }
            */
            
            char** physicalInPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsInput);
            if (physicalInPorts != NULL) {
                for (int i = 0; i < fNumOutChans && physicalInPorts[i]; i++) {
                    jack_connect(fClient, jack_port_name(fOutputPorts[i]), physicalInPorts[i]);
                }
                jack_free(physicalInPorts);
            }
        }

    #ifdef _OPENMP
        void process_thread() 
        {
            jack_nframes_t nframes;
            while (1) {
                nframes = jack_cycle_wait(fClient);
                process(nframes);
                jack_cycle_signal(fClient, 0);
            }
        }
    #endif
};

#endif

#ifdef OSCCTRL
/*
   Copyright (C) 2011 Grame - Lyon
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted.
*/

#ifndef __OSCUI__
#define __OSCUI__

/*

  Faust Project

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __OSCControler__
#define __OSCControler__

#include <string>
/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __FaustFactory__
#define __FaustFactory__

#include <stack>
#include <string>
#include <sstream>

/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __FaustNode__
#define __FaustNode__

#include <string>
#include <vector>

/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __MessageDriven__
#define __MessageDriven__

#include <string>
#include <vector>

/*

  Copyright (C) 2010  Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __MessageProcessor__
#define __MessageProcessor__

namespace oscfaust
{

class Message;
//--------------------------------------------------------------------------
/*!
	\brief an abstract class for objects able to process OSC messages	
*/
class MessageProcessor
{
	public:
		virtual		~MessageProcessor() {}
		virtual void processMessage( const Message* msg ) = 0;
};

} // end namespoace

#endif
/*

  Copyright (C) 2011 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __smartpointer__
#define __smartpointer__

#include <cassert>

namespace oscfaust
{

/*!
\brief the base class for smart pointers implementation

	Any object that want to support smart pointers should
	inherit from the smartable class which provides reference counting
	and automatic delete when the reference count drops to zero.
*/
class smartable {
	private:
		unsigned 	refCount;		
	public:
		//! gives the reference count of the object
		unsigned refs() const         { return refCount; }
		//! addReference increments the ref count and checks for refCount overflow
		void addReference()           { refCount++; assert(refCount != 0); }
		//! removeReference delete the object when refCount is zero		
		void removeReference()		  { if (--refCount == 0) delete this; }
		
	protected:
		smartable() : refCount(0) {}
		smartable(const smartable&): refCount(0) {}
		//! destructor checks for non-zero refCount
		virtual ~smartable()    
        { 
            /* 
                See "Static SFaustNode create (const char* name, C* zone, C init, C min, C max, const char* prefix, GUI* ui)" comment.
                assert (refCount == 0); 
            */
         }
		smartable& operator=(const smartable&) { return *this; }
};

/*!
\brief the smart pointer implementation

	A smart pointer is in charge of maintaining the objects reference count 
	by the way of pointers operators overloading. It supports class 
	inheritance and conversion whenever possible.
\n	Instances of the SMARTP class are supposed to use \e smartable types (or at least
	objects that implements the \e addReference and \e removeReference
	methods in a consistent way).
*/
template<class T> class SMARTP {
	private:
		//! the actual pointer to the class
		T* fSmartPtr;

	public:
		//! an empty constructor - points to null
		SMARTP()	: fSmartPtr(0) {}
		//! build a smart pointer from a class pointer
		SMARTP(T* rawptr) : fSmartPtr(rawptr)              { if (fSmartPtr) fSmartPtr->addReference(); }
		//! build a smart pointer from an convertible class reference
		template<class T2> 
		SMARTP(const SMARTP<T2>& ptr) : fSmartPtr((T*)ptr) { if (fSmartPtr) fSmartPtr->addReference(); }
		//! build a smart pointer from another smart pointer reference
		SMARTP(const SMARTP& ptr) : fSmartPtr((T*)ptr)     { if (fSmartPtr) fSmartPtr->addReference(); }

		//! the smart pointer destructor: simply removes one reference count
		~SMARTP()  { if (fSmartPtr) fSmartPtr->removeReference(); }
		
		//! cast operator to retrieve the actual class pointer
		operator T*() const  { return fSmartPtr;	}

		//! '*' operator to access the actual class pointer
		T& operator*() const {
			// checks for null dereference
			assert (fSmartPtr != 0);
			return *fSmartPtr;
		}

		//! operator -> overloading to access the actual class pointer
		T* operator->() const	{ 
			// checks for null dereference
			assert (fSmartPtr != 0);
			return fSmartPtr;
		}

		//! operator = that moves the actual class pointer
		template <class T2>
		SMARTP& operator=(T2 p1_)	{ *this=(T*)p1_; return *this; }

		//! operator = that moves the actual class pointer
		SMARTP& operator=(T* p_)	{
			// check first that pointers differ
			if (fSmartPtr != p_) {
				// increments the ref count of the new pointer if not null
				if (p_ != 0) p_->addReference();
				// decrements the ref count of the old pointer if not null
				if (fSmartPtr != 0) fSmartPtr->removeReference();
				// and finally stores the new actual pointer
				fSmartPtr = p_;
			}
			return *this;
		}
		//! operator < to support SMARTP map with Visual C++
		bool operator<(const SMARTP<T>& p_)	const			  { return fSmartPtr < ((T *) p_); }
		//! operator = to support inherited class reference
		SMARTP& operator=(const SMARTP<T>& p_)                { return operator=((T *) p_); }
		//! dynamic cast support
		template<class T2> SMARTP& cast(T2* p_)               { return operator=(dynamic_cast<T*>(p_)); }
		//! dynamic cast support
		template<class T2> SMARTP& cast(const SMARTP<T2>& p_) { return operator=(dynamic_cast<T*>(p_)); }
};

}

#endif

namespace oscfaust
{

class Message;
class OSCRegexp;
class MessageDriven;
typedef class SMARTP<MessageDriven>	SMessageDriven;

//--------------------------------------------------------------------------
/*!
	\brief a base class for objects accepting OSC messages
	
	Message driven objects are hierarchically organized in a tree.
	They provides the necessary to dispatch an OSC message to its destination
	node, according to the message OSC address. 
	
	The principle of the dispatch is the following:
	- first the processMessage() method should be called on the top level node
	- next processMessage call propose 
	
	
*/
class MessageDriven : public MessageProcessor, public smartable
{
	std::string						fName;			///< the node name
	std::string						fOSCPrefix;		///< the node OSC address prefix (OSCAddress = fOSCPrefix + '/' + fName)
	std::vector<SMessageDriven>		fSubNodes;		///< the subnodes of the current node

	protected:
				 MessageDriven(const char *name, const char *oscprefix) : fName (name), fOSCPrefix(oscprefix) {}
		virtual ~MessageDriven() {}

	public:
		static SMessageDriven create (const char* name, const char *oscprefix)	{ return new MessageDriven(name, oscprefix); }

		/*!
			\brief OSC message processing method.
			\param msg the osc message to be processed
			The method should be called on the top level node.
		*/
		virtual void	processMessage( const Message* msg );

		/*!
			\brief propose an OSc message at a given hierarchy level.
			\param msg the osc message currently processed
			\param regexp a regular expression based on the osc address head
			\param addrTail the osc address tail
			
			The method first tries to match the regular expression with the object name. 
			When it matches:
			- it calls \c accept when \c addrTail is empty 
			- or it \c propose the message to its subnodes when \c addrTail is not empty. 
			  In this case a new \c regexp is computed with the head of \c addrTail and a new \c addrTail as well.
		*/
		virtual void	propose( const Message* msg, const OSCRegexp* regexp, const std::string addrTail);

		/*!
			\brief accept an OSC message. 
			\param msg the osc message currently processed
			\return true when the message is processed by the node
			
			The method is called only for the destination nodes. The real message acceptance is the node 
			responsability and may depend on the message content.
		*/
		virtual bool	accept( const Message* msg );

		/*!
			\brief handler for the \c 'get' message
			\param ipdest the output message destination IP
			
			The \c 'get' message is supported by every node:
			- it is propagated to the subnodes until it reaches terminal nodes
			- a terminal node send its state on \c 'get' request to the IP address given as parameter.
			The \c get method is basically called by the accept method.
		*/
		virtual void	get (unsigned long ipdest) const;

		/*!
			\brief handler for the \c 'get' 'attribute' message
			\param ipdest the output message destination IP
			\param what the requested attribute
			
			The \c 'get' message is supported by every node:
			- it is propagated to the subnodes until it reaches terminal nodes
			- a terminal node send its state on \c 'get' request to the IP address given as parameter.
			The \c get method is basically called by the accept method.
		*/
		virtual void	get (unsigned long ipdest, const std::string & what) const {}

		void			add ( SMessageDriven node )	{ fSubNodes.push_back (node); }
		const char*		getName() const				{ return fName.c_str(); }
		std::string		getOSCAddress() const;
		int				size () const				{ return fSubNodes.size (); }
		
		const std::string&	name() const			{ return fName; }
		SMessageDriven	subnode (int i) 			{ return fSubNodes[i]; }
};

} // end namespoace

#endif
/*

  Copyright (C) 2011  Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/


#ifndef __Message__
#define __Message__

#include <string>
#include <vector>

namespace oscfaust
{

class OSCStream;
template <typename T> class MsgParam;
class baseparam;
typedef SMARTP<baseparam>	Sbaseparam;

//--------------------------------------------------------------------------
/*!
	\brief base class of a message parameters
*/
class baseparam : public smartable
{
	public:
		virtual ~baseparam() {}

		/*!
		 \brief utility for parameter type checking
		*/
		template<typename X> bool isType() const { return dynamic_cast<const MsgParam<X>*> (this) != 0; }
		/*!
		 \brief utility for parameter convertion
		 \param errvalue the returned value when no conversion applies
		 \return the parameter value when the type matches
		*/
		template<typename X> X	value(X errvalue) const 
			{ const MsgParam<X>* o = dynamic_cast<const MsgParam<X>*> (this); return o ? o->getValue() : errvalue; }
		/*!
		 \brief utility for parameter comparison
		*/
		template<typename X> bool	equal(const baseparam& p) const 
			{ 
				const MsgParam<X>* a = dynamic_cast<const MsgParam<X>*> (this); 
				const MsgParam<X>* b = dynamic_cast<const MsgParam<X>*> (&p);
				return a && b && (a->getValue() == b->getValue());
			}
		/*!
		 \brief utility for parameter comparison
		*/
		bool operator==(const baseparam& p) const 
			{ 
				return equal<float>(p) || equal<int>(p) || equal<std::string>(p);
			}
		bool operator!=(const baseparam& p) const
			{ 
				return !equal<float>(p) && !equal<int>(p) && !equal<std::string>(p);
			}
			
		virtual SMARTP<baseparam> copy() const = 0;
};

//--------------------------------------------------------------------------
/*!
	\brief template for a message parameter
*/
template <typename T> class MsgParam : public baseparam
{
	T fParam;
	public:
				 MsgParam(T val) : fParam(val)	{}
		virtual ~MsgParam() {}
		
		T	getValue() const { return fParam; }
		
		virtual SMARTP<baseparam> copy() const { return new MsgParam<T>(fParam); }
};

//--------------------------------------------------------------------------
/*!
	\brief a message description
	
	A message is composed of an address (actually an OSC address),
	a message string that may be viewed as a method name
	and a list of message parameters.
*/
class Message
{
	public:
		typedef SMARTP<baseparam>		argPtr;		///< a message argument ptr type
		typedef std::vector<argPtr>		argslist;	///< args list type

	private:
		unsigned long	fSrcIP;			///< the message source IP number
		std::string	fAddress;			///< the message osc destination address
		argslist	fArguments;			///< the message arguments
	
	public:
			/*!
				\brief an empty message constructor
			*/
			 Message() {}
			/*!
				\brief a message constructor
				\param address the message destination address
			*/
			 Message(const std::string& address) : fAddress(address) {}
			/*!
				\brief a message constructor
				\param address the message destination address
				\param args the message parameters
			*/
			 Message(const std::string& address, const argslist& args) 
				: fAddress(address), fArguments(args) {}
			/*!
				\brief a message constructor
				\param msg a message
			*/
			 Message(const Message& msg);
	virtual ~Message() {} //{ freed++; std::cout << "running messages: " << (allocated - freed) << std::endl; }

	/*!
		\brief adds a parameter to the message
		\param val the parameter
	*/
	template <typename T> void add(T val)	{ fArguments.push_back(new MsgParam<T>(val)); }
	/*!
		\brief adds a float parameter to the message
		\param val the parameter value
	*/
	void	add(float val)					{ add<float>(val); }
	/*!
		\brief adds an int parameter to the message
		\param val the parameter value
	*/
	void	add(int val)					{ add<int>(val); }
	/*!
		\brief adds a string parameter to the message
		\param val the parameter value
	*/
	void	add(const std::string& val)		{ add<std::string>(val); }
	
	/*!
		\brief adds a parameter to the message
		\param val the parameter
	*/
	void	add( argPtr val )				{ fArguments.push_back( val ); }

	/*!
		\brief sets the message address
		\param addr the address
	*/
	void				setSrcIP(unsigned long addr)		{ fSrcIP = addr; }

	/*!
		\brief sets the message address
		\param addr the address
	*/
	void				setAddress(const std::string& addr)		{ fAddress = addr; }
	/*!
		\brief print the message
		\param out the output stream
	*/
	void				print(std::ostream& out) const;
	/*!
		\brief send the message to OSC
		\param out the OSC output stream
	*/
	void				print(OSCStream& out) const;
	/*!
		\brief print message arguments
		\param out the OSC output stream
	*/
	void				printArgs(OSCStream& out) const;

	/// \brief gives the message address
	const std::string&	address() const		{ return fAddress; }
	/// \brief gives the message parameters list
	const argslist&		params() const		{ return fArguments; }
	/// \brief gives the message parameters list
	argslist&			params()			{ return fArguments; }
	/// \brief gives the message source IP 
	unsigned long		src() const			{ return fSrcIP; }
	/// \brief gives the message parameters count
	int					size() const		{ return fArguments.size(); }
	
	bool operator == (const Message& other) const;	


	/*!
		\brief gives a message float parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, float& val) const		{ val = params()[i]->value<float>(val); return params()[i]->isType<float>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, int& val) const		{ val = params()[i]->value<int>(val); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, unsigned int& val) const		{ val = params()[i]->value<int>(val); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
		\note a boolean value is handled as integer
	*/
	bool	param(int i, bool& val) const		{ int ival = 0; ival = params()[i]->value<int>(ival); val = ival!=0; return params()[i]->isType<int>(); }
	/*!
		\brief gives a message int parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, long int& val) const	{ val = long(params()[i]->value<int>(val)); return params()[i]->isType<int>(); }
	/*!
		\brief gives a message string parameter
		\param i the parameter index (0 <= i < size())
		\param val on output: the parameter value when the parameter type matches
		\return false when types don't match
	*/
	bool	param(int i, std::string& val) const { val = params()[i]->value<std::string>(val); return params()[i]->isType<std::string>(); }
};


} // end namespoace

#endif

class GUI;

namespace oscfaust
{

/**
 * map (rescale) input values to output values
 */
template <typename C> struct mapping
{
//	const C fMinIn;	
//	const C fMaxIn;
	const C fMinOut;
	const C fMaxOut;
//	const C fScale;

//	mapping(C imin, C imax, C omin, C omax) : fMinIn(imin), fMaxIn(imax), 
//											fMinOut(omin), fMaxOut(omax), 
//											fScale( (fMaxOut-fMinOut)/(fMaxIn-fMinIn) ) {}
	mapping(C omin, C omax) : fMinOut(omin), fMaxOut(omax) {}
//	C scale (C x) { C z = (x < fMinIn) ? fMinIn : (x > fMaxIn) ? fMaxIn : x; return fMinOut + (z - fMinIn) * fScale; }
	C clip (C x) { return (x < fMinOut) ? fMinOut : (x > fMaxOut) ? fMaxOut : x; }
};

//--------------------------------------------------------------------------
/*!
	\brief a faust node is a terminal node and represents a faust parameter controler
*/
template <typename C> class FaustNode : public MessageDriven, public uiItem
{
	mapping<C>	fMapping;
	
	bool	store (C val)			{ *fZone = fMapping.clip(val); return true; }
	void	sendOSC () const;


	protected:
		FaustNode(const char *name, C* zone, C init, C min, C max, const char* prefix, GUI* ui, bool initZone) 
			: MessageDriven (name, prefix), uiItem (ui, zone), fMapping(min, max)
			{ 
                if(initZone)
                    *zone = init; 
            }
			
		virtual ~FaustNode() {}

	public:
		typedef SMARTP<FaustNode<C> > SFaustNode;
		static SFaustNode create (const char* name, C* zone, C init, C min, C max, const char* prefix, GUI* ui, bool initZone)	
        { 
            SFaustNode node = new FaustNode(name, zone, init, min, max, prefix, ui, initZone); 
            /*
                Since FaustNode is a subclass of uiItem, the pointer will also be kept in the GUI class, and it's desallocation will be done there.
                So we don't want to have smartpointer logic desallocate it and we increment the refcount.
            */
            node->addReference(); 
            return node; 
        }

		virtual bool	accept( const Message* msg )			///< handler for the 'accept' message
		{
			if (msg->size() == 1) {			// checks for the message parameters count
											// messages with a param count other than 1 are rejected
				int ival; float fval;
				if (msg->param(0, fval)) return store (C(fval));				// accepts float values
				else if (msg->param(0, ival)) return store (C(ival));	// but accepts also int values
			}
			return MessageDriven::accept(msg);
		}

		virtual void	get (unsigned long ipdest) const;		///< handler for the 'get' message
		virtual void 	reflectZone()			{ sendOSC (); fCache = *fZone;}
};



} // end namespoace

#endif

class GUI;
namespace oscfaust
{

class OSCIO;
class RootNode;
typedef class SMARTP<RootNode>		SRootNode;
class MessageDriven;
typedef class SMARTP<MessageDriven>	SMessageDriven;

//--------------------------------------------------------------------------
/*!
	\brief a factory to build a OSC UI hierarchy
	
	Actually, makes use of a stack to build the UI hierarchy.
	It includes a pointer to a OSCIO controler, but just to give it to the root node.
*/
class FaustFactory
{
	std::stack<SMessageDriven>	fNodes;		///< maintains the current hierarchy level
	SRootNode					fRoot;		///< keep track of the root node
	OSCIO * 					fIO;		///< hack to support audio IO via OSC, actually the field is given to the root node
	GUI *						fGUI;		///< a GUI pointer to support updateAllGuis(), required for bi-directionnal OSC

	private:
		std::string addressFirst (const std::string& address) const;
		std::string addressTail  (const std::string& address) const;

	public:
				 FaustFactory(GUI* ui, OSCIO * io=0); // : fIO(io), fGUI(ui) {}
		virtual ~FaustFactory(); // {}

		template <typename C> void addnode (const char* label, C* zone, C init, C min, C max, bool initZone);
		template <typename C> void addAlias (const std::string& fullpath, C* zone, C imin, C imax, C init, C min, C max, const char* label);
		void addAlias (const char* alias, const char* address, float imin, float imax, float omin, float omax);
		void opengroup (const char* label);
		void closegroup ();

		SRootNode		root() const; //	{ return fRoot; }
};

/**
 * Add a node to the OSC UI tree in the current group at the top of the stack 
 */
template <typename C> void FaustFactory::addnode (const char* label, C* zone, C init, C min, C max, bool initZone) 
{
//	SMessageDriven top = fNodes.size() ? fNodes.top() : fRoot;
	SMessageDriven top;
	if (fNodes.size()) top = fNodes.top();
	if (top) {
		std::string prefix = top->getOSCAddress();
		top->add( FaustNode<C>::create (label, zone, init, min, max, prefix.c_str(), fGUI, initZone));
	}
}

/**
 * Add an alias (actually stored and handled at root node level
 */
template <typename C> void FaustFactory::addAlias (const std::string& fullpath, C* zone, C imin, C imax, C init, C min, C max, const char* label)
{
	std::istringstream 	ss(fullpath);
	std::string 		realpath; 

	ss >> realpath >> imin >> imax;
	SMessageDriven top = fNodes.top();
	if (top ) {
		std::string target = top->getOSCAddress() + "/" + label;
		addAlias (realpath.c_str(), target.c_str(), float(imin), float(imax), float(min), float(max));
	}
}

} // end namespoace

#endif

class GUI;

typedef void (*ErrorCallback)(void*);  

namespace oscfaust
{

class OSCIO;
class OSCSetup;

//--------------------------------------------------------------------------
/*!
	\brief the main Faust OSC Lib API
	
	The OSCControler is essentially a glue between the memory representation (in charge of the FaustFactory),
	and the network services (in charge of OSCSetup).
*/
class OSCControler
{
	int fUDPPort, fUDPOut, fUPDErr;		// the udp ports numbers
	std::string		fDestAddress;		// the osc messages destination address, used at initialization only
										// to collect the address from the command line
	OSCSetup*		fOsc;				// the network manager (handles the udp sockets)
	OSCIO*			fIO;				// hack for OSC IO support (actually only relayed to the factory)
	FaustFactory *	fFactory;			// a factory to build the memory represetnatin

    bool            fInit;
    
	public:
		/*
			base udp port is chosen in an unassigned range from IANA PORT NUMBERS (last updated 2011-01-24)
			see at http://www.iana.org/assignments/port-numbers
			5507-5552  Unassigned
		*/
		enum { kUDPBasePort = 5510};
            
        OSCControler (int argc, char *argv[], GUI* ui, OSCIO* io = 0, ErrorCallback errCallback = NULL, void* arg = NULL, bool init = true);

        virtual ~OSCControler ();
	
		//--------------------------------------------------------------------------
		// addnode, opengroup and closegroup are simply relayed to the factory
		//--------------------------------------------------------------------------
		// Add a node in the current group (top of the group stack)
		template <typename T> void addnode (const char* label, T* zone, T init, T min, T max)
							{ fFactory->addnode (label, zone, init, min, max, fInit); }
		
		//--------------------------------------------------------------------------
		// This method is used for alias messages. The arguments imin and imax allow
		// to map incomming values from the alias input range to the actual range 
		template <typename T> void addAlias (const std::string& fullpath, T* zone, T imin, T imax, T init, T min, T max, const char* label)
							{ fFactory->addAlias (fullpath, zone, imin, imax, init, min, max, label); }

		void opengroup (const char* label)		{ fFactory->opengroup (label); }
		void closegroup ()						{ fFactory->closegroup(); }
	   
		//--------------------------------------------------------------------------
		void run ();				// starts the network services
		void quit ();				// stop the network services
		
		int	getUDPPort() const			{ return fUDPPort; }
		int	getUDPOut()	const			{ return fUDPOut; }
		int	getUDPErr()	const			{ return fUPDErr; }
		const char*	getDestAddress() const { return fDestAddress.c_str(); }
		const char*	getRootName() const;	// probably useless, introduced for UI extension experiments

		static float version();				// the Faust OSC library version number
		static const char* versionstr();	// the Faust OSC library version number as a string
		static bool	gXmit;				// a static variable to control the transmission of values
										// i.e. the use of the interface as a controler
};

}

#endif
#include <vector>

/******************************************************************************
*******************************************************************************

					OSC (Open Sound Control) USER INTERFACE

*******************************************************************************
*******************************************************************************/
/*

Note about the OSC addresses and the Faust UI names:
----------------------------------------------------
There are potential conflicts between the Faust UI objects naming scheme and 
the OSC address space. An OSC symbolic names is an ASCII string consisting of
printable characters other than the following:
	space 
#	number sign
*	asterisk
,	comma
/	forward
?	question mark
[	open bracket
]	close bracket
{	open curly brace
}	close curly brace

a simple solution to address the problem consists in replacing 
space or tabulation with '_' (underscore)
all the other osc excluded characters with '-' (hyphen)

This solution is implemented in the proposed OSC UI;
*/

///using namespace std;

//class oscfaust::OSCIO;

class OSCUI : public GUI 
{
     
	oscfaust::OSCControler*	fCtrl;
	std::vector<const char*> fAlias;
	
	const char* tr(const char* label) const;
	
	// add all accumulated alias
	void addalias(FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, const char* label)
	{
		for (unsigned int i=0; i<fAlias.size(); i++) {
			fCtrl->addAlias(fAlias[i], zone, (FAUSTFLOAT)0, (FAUSTFLOAT)1, init, min, max, label);
		}
		fAlias.clear();
	}
	
 public:

    OSCUI(const char* /*applicationname*/, int argc, char *argv[], oscfaust::OSCIO* io=0, ErrorCallback errCallback = NULL, void* arg = NULL, bool init = true) : GUI() 
    { 
		fCtrl = new oscfaust::OSCControler(argc, argv, this, io, errCallback, arg, init); 
        //		fCtrl->opengroup(applicationname);
	}
    
	virtual ~OSCUI() { delete fCtrl; }
    
    // -- widget's layouts
    
  	virtual void openTabBox(const char* label) 			{ fCtrl->opengroup( tr(label)); }
	virtual void openHorizontalBox(const char* label) 	{ fCtrl->opengroup( tr(label)); }
	virtual void openVerticalBox(const char* label) 	{ fCtrl->opengroup( tr(label)); }
	virtual void closeBox() 							{ fCtrl->closegroup(); }

	
	// -- active widgets
	virtual void addButton(const char* label, FAUSTFLOAT* zone) 		{ const char* l= tr(label); addalias(zone, 0, 0, 1, l); fCtrl->addnode( l, zone, (FAUSTFLOAT)0, (FAUSTFLOAT)0, (FAUSTFLOAT)1); }
	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 	{ const char* l= tr(label); addalias(zone, 0, 0, 1, l); fCtrl->addnode( l, zone, (FAUSTFLOAT)0, (FAUSTFLOAT)0, (FAUSTFLOAT)1); }
	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT /*step*/)
																		{ const char* l= tr(label); addalias(zone, init, min, max, l); fCtrl->addnode( l, zone, init, min, max); }
	
	// -- passive widgets
	
	virtual void addHorizontalBargraph(const char* /*label*/, FAUSTFLOAT* /*zone*/, FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {}
	virtual void addVerticalBargraph(const char* /*label*/, FAUSTFLOAT* /*zone*/, FAUSTFLOAT /*min*/, FAUSTFLOAT /*max*/) {}
		
	// -- metadata declarations
    
	virtual void declare(FAUSTFLOAT* , const char* key , const char* alias) 
	{ 
		if (strcasecmp(key,"OSC")==0) fAlias.push_back(alias);
	}

	virtual void show() {}

	void run()
    {
        fCtrl->run(); 
    }
	const char* getRootName()		{ return fCtrl->getRootName(); }
    int getUDPPort()                { return fCtrl->getUDPPort(); }
    int	getUDPOut()                 { return fCtrl->getUDPOut(); }
    int	getUDPErr()                 { return fCtrl->getUDPErr(); }
    const char* getDestAddress()    {return fCtrl->getDestAddress();}
};

const char* OSCUI::tr(const char* label) const
{
	static char buffer[1024];
	char * ptr = buffer; int n=1;
	while (*label && (n++ < 1024)) {
		switch (*label) {
			case ' ': case '	':
				*ptr++ = '_';
				break;
			case '#': case '*': case ',': case '/': case '?':
			case '[': case ']': case '{': case '}':
				*ptr++ = '_';
				break;
			default: 
				*ptr++ = *label;
		}
		label++;
	}
	*ptr = 0;
	return buffer;
}

#endif
#endif

#ifdef HTTPCTRL
/*
   Copyright (C) 2012 Grame - Lyon
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted.
*/

#ifndef __httpdUI__
#define __httpdUI__

//#ifdef _WIN32
//#include "HTTPDControler.h"
//#include "UI.h"
//#else
/*

  Faust Project

  Copyright (C) 2012 Grame

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

  Grame Research Laboratory, 9 rue du Garet, 69001 Lyon - France
  research@grame.fr

*/

#ifndef __HTTPDControler__
#define __HTTPDControler__

#include <string>
#include <map>

namespace httpdfaust
{

class HTTPDSetup;
class JSONDesc;
class FaustFactory;
class jsonfactory;
class htmlfactory;

//--------------------------------------------------------------------------
/*!
	\brief the main Faust HTTPD Lib API
	
	The HTTPDControler is essentially a glue between the memory representation (in charge of the FaustFactory), 
	and the network services (in charge of HTTPDSetup).
*/
class HTTPDControler
{
	int fTCPPort;				// the tcp port number
	FaustFactory *	fFactory;	// a factory to build the memory representation
	jsonfactory*	fJson;
	htmlfactory*	fHtml;
	HTTPDSetup*		fHttpd;		// the network manager
	std::string		fHTML;		// the corresponding HTML page
	std::map<std::string, std::string>	fCurrentMeta;	// the current meta declarations 

    bool            fInit;
    
	public:
		/*
			base udp port is chosen in an unassigned range from IANA PORT NUMBERS (last updated 2011-01-24)
			see at http://www.iana.org/assignments/port-numbers
			5507-5552  Unassigned
		*/
		enum { kTCPBasePort = 5510};

				 HTTPDControler (int argc, char *argv[], const char* applicationname, bool init = true);
		virtual ~HTTPDControler ();
	
		//--------------------------------------------------------------------------
		// addnode, opengroup and closegroup are simply relayed to the factory
		//--------------------------------------------------------------------------
		template <typename C> void addnode (const char* type, const char* label, C* zone);
		template <typename C> void addnode (const char* type, const char* label, C* zone, C min, C max);
		template <typename C> void addnode (const char* type, const char* label, C* zone, C init, C min, C max, C step);
							  void declare (const char* key, const char* val ) { fCurrentMeta[key] = val; }

		void opengroup (const char* type, const char* label);
		void closegroup ();

		//--------------------------------------------------------------------------
		void run ();				// start the httpd server
		void quit ();				// stop the httpd server
		
		int	getTCPPort()			{ return fTCPPort; }
        std::string getJSONInterface();
        void        setInputs(int numInputs);
        void        setOutputs(int numOutputs);

		static float version();				// the Faust httpd library version number
		static const char* versionstr();	// the Faust httpd library version number as a string
};

}

#endif
//#endif
/******************************************************************************
*******************************************************************************

					HTTPD USER INTERFACE

*******************************************************************************
*******************************************************************************/
/*

Note about URLs and the Faust UI names:
----------------------------------------------------
Characters in a url could be:
1. Reserved: ; / ? : @ & = + $ ,
   These characters delimit URL parts.
2. Unreserved: alphanum - _ . ! ~ * ' ( )
   These characters have no special meaning and can be used as is.
3. Excluded: control characters, space, < > # % ", { } | \ ^ [ ] `

To solve potential conflicts between the Faust UI objects naming scheme and
the URL allowed characters, the reserved and excluded characters are replaced
with '-' (hyphen).
Space or tabulation are replaced with '_' (underscore)
*/

//using namespace std;

class httpdUI : public UI 
{
	httpdfaust::HTTPDControler*	fCtrl;	
	const char* tr(const char* label) const;

 public:
		
	httpdUI(const char* applicationname, int inputs, int outputs, int argc, char *argv[], bool init = true) 
    { 
		fCtrl = new httpdfaust::HTTPDControler(argc, argv, applicationname, init); 
        fCtrl->setInputs(inputs);
        fCtrl->setOutputs(outputs);
	}
	
	virtual ~httpdUI() { delete fCtrl; }
		
    // -- widget's layouts
	virtual void openTabBox(const char* label) 			{ fCtrl->opengroup( "tgroup", tr(label)); }
	virtual void openHorizontalBox(const char* label) 	{ fCtrl->opengroup( "hgroup", tr(label)); }
	virtual void openVerticalBox(const char* label) 	{ fCtrl->opengroup( "vgroup", tr(label)); }
	virtual void closeBox() 							{ fCtrl->closegroup(); }
	
	// -- active widgets
	virtual void addButton(const char* label, FAUSTFLOAT* zone)			{ fCtrl->addnode( "button", tr(label), zone); }
	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)	{ fCtrl->addnode( "checkbox", tr(label), zone); }
	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
									{ fCtrl->addnode( "vslider", tr(label), zone, init, min, max, step); }
	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 	
									{ fCtrl->addnode( "hslider", tr(label), zone, init, min, max, step); }
	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) 			
									{ fCtrl->addnode( "nentry", tr(label), zone, init, min, max, step); }
	
	// -- passive widgets	
	virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 
									{ fCtrl->addnode( "hbargraph", tr(label), zone, min, max); }
	virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
									{ fCtrl->addnode( "vbargraph", tr(label), zone, min, max); }
	
    virtual void declare (FAUSTFLOAT*, const char* key, const char* val) { fCtrl->declare(key, val); }

	void run()						{ fCtrl->run(); }
	int getTCPPort()                { return fCtrl->getTCPPort(); }
    
    std::string getJSONInterface(){ return fCtrl->getJSONInterface(); }

};
					
const char* httpdUI::tr(const char* label) const
{
	static char buffer[1024];
	char * ptr = buffer; int n=1;
	while (*label && (n++ < 1024)) {
		switch (*label) {
			case ' ': case '	':
				*ptr++ = '_';
				break;
			case ';': case '/': case '?': case ':': case '@': 
			case '&': case '=': case '+': case '$': case ',':
			case '<': case '>': case '#': case '%': case '"': 
			case '{': case '}': case '|': case '\\': case '^': 
			case '[': case ']': case '`':
				*ptr++ = '_';
				break;
			default: 
				*ptr++ = *label;
		}
		label++;
	}
	*ptr = 0;
	return buffer;
}

#endif
#endif

#ifdef OCVCTRL
/************************************************
* 			OpenCV User Interface			  	*
*												*
* This architecture file allows the user to	  	*
*	use the OpenCV library in order to perform	*
*	image processing and use the result 		*
*	to control audio parameters.				*
*												*
* To use this mode, just add the option -ocv in *
* 	the command line. 							*
*												*
************************************************/
#ifndef _OCVUI_H
#define _OCVUI_H


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

// OpenCV includes
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

// Basic includes
#include <iostream>
#include <ctype.h>

// std::thread
#include <pthread.h>

// Main Loop Function Prototype

static void* mainLoop(void*);	// changer nom

//********	OpenCV User Interface CLASS DEFINITION ********//
class OCVUI : public UI
{
    
    public :
    
    ////////////////////////////////////////////
	////									////
	////			STRUCTURES				////
	////									////
	////////////////////////////////////////////
	
	struct object
	{
		int color;
		float centerX;
		float centerY;
		float area;
		int radius;
	};
	
	struct metadata
	{
		FAUSTFLOAT* zone;
		int color;
		std::string param;
		bool used;
	};
	
	////////////////////////////////////////////
	////									////
	////			FUNCTIONS				////
	////									////
	////////////////////////////////////////////
	
	/**********************************************/
	/*******	UI Functions Redefinition	*******/
	/**********************************************/
		// Functions inherited from the UI class
	
    // Constructor
	OCVUI() : objects_storage_(0), parameters_storage_(0), height_(0), width_(0){};
	
	// Destructor
	~OCVUI() 
	{exit_=true;};
	
	
	// -- WIDGETS LAYOUTS
	void openTabBox(const char* label){}
	void openHorizontalBox(const char* label){}
	void openVerticalBox(const char* label){}
	void closeBox(){}

	// -- ACTIVE WIDGETS
	void addButton(const char* label, FAUSTFLOAT* zone){}
	void addCheckButton(const char* label, FAUSTFLOAT* zone){}
	void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min,
	FAUSTFLOAT max, FAUSTFLOAT step){}
	void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, 
	FAUSTFLOAT max, FAUSTFLOAT step){}
	void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, 
	FAUSTFLOAT max, FAUSTFLOAT step){}
	
	// -- PASSIVE WIDGETS
	void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){}
	void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){}


	// -- METADATA DECLARATION
	
	
	//******** Parsing function	********//
		// This function analyses a string and stores
		// the different parameters in a metadata structure.
		
	bool parser(std::string string2parse, metadata *pmeta)
	{
	    int SPACE = 32; // Parameters separator
	    std::vector<std::string> parameters(0);
	    
	    //*** String analysis ***//
	    for (int i = 0 ; i < string2parse.size() ; i++)
	    {
	    	if (string2parse[i]==SPACE)
	    	{
	    	    std::string oneParameter= string2parse.substr(0,i);
	    	    parameters.push_back(oneParameter);
	    	    string2parse.erase(string2parse.begin(), string2parse.begin()+i+1);
	    	    i=0;
	    	}	
	    }
	    std::string lastParameter = string2parse;
	    parameters.push_back(lastParameter);
	    	    
	    //*** Store Parameters in a Metadata Structure ***//
	    
	    // Parameters count must be 2
	    if (parameters.size()==2)
	    {
	    	// Associate every color to a digit
	    		// red		= 1	;	green 	= 3	;	blue 	= 5	;
	    		// yellow	= 2	;	cyan	= 4	;	magenta = 6	.
	    		
	    	if (parameters[0]=="red")
	    	{
	    		pmeta->color = 1;
	    	}
	    	else if (parameters[0]=="yellow")
	    	{
	    		pmeta->color = 2;
	    	}
	    	else if (parameters[0]=="green")
	    	{
	    		pmeta->color = 3;
	    	}
	    	else if (parameters[0]=="cyan")
	    	{
	    		pmeta->color = 4;
	    	}
	    	else if (parameters[0]=="blue")
	    	{
	    		pmeta->color = 5;
	    	}
	    	else if (parameters[0]=="magenta")
	    	{
	    		pmeta->color = 6;
	    	}
	    	pmeta->param = parameters[1];
	    	pmeta->used = false;
	    	return true;
	    }
	    else
	    {
	    	std::cout<<"Wrong count of parameters. Please check if the OpenCV"
	    			 <<"metadata is correctly defined"<<std::endl;
	    	return false;
	    }
	}
	//******** Declare Function ********//
	// This function calls the parsing function if the declared key is "ocv"
		// and creates a new metadata structure.
	void declare(FAUSTFLOAT* zone, const char* key, const char* val) 
	{
		if (key=="ocv")
		{
			metadata newMeta;
			bool string_parsed = false;
						
			if (zone != 0)
			{
				newMeta.zone = zone;
			}
			string_parsed = parser(val, &newMeta);
			
			if (string_parsed)
			{
				parameters_storage_.push_back(newMeta);
			}
		}
	}
	
	/**************************************************/
    /*******	Image Processing Functions		*******/
    /**************************************************/
       
    //*** Contours processing ***//   
    	// This function approximates contours to rectangles,
    	// keeps the bigest one,
    	// and stores it as a new object.
    void contoursProcess(std::vector<std::vector<cv::Point> > contours, int color)
	{
		int tempArea=0;
		cv::Rect myRect;
		for (int j=0 ; j<contours.size() ; j++)								// for each contour
		{
			std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
			std::vector<cv::Rect> boundRect( contours.size() );
		
			if (contours[j].size()>40)										// Do not take care about small
																				// contours
			{
				approxPolyDP( cv::Mat(contours[j]), contours_poly[j], 3, true );// Approximate contours to 
																				// a polygone
				boundRect[j] = cv::boundingRect( cv::Mat(contours_poly[j]) );		// Bound a contour in a 
																				// rectangle
				if ((int)boundRect[j].area()>tempArea)	
				{
					tempArea=(int)boundRect[j].area();
					myRect = boundRect[j];
				}
			}	
		}
		if (tempArea != 0)
		{
			// Create a new object structure to store the object properties
			object newObject;
			newObject.color = color;
			newObject.centerX = myRect.x+myRect.width/2;
			newObject.centerY = myRect.y+myRect.height/2;
			newObject.area = 1.5*(float)tempArea/(width_*height_);
			newObject.radius= (int)MIN(myRect.width/2, myRect.height/2);
				
			// Put the new object in the objects storage.
			objects_storage_.push_back(newObject);
		}
	}
	//*** Morphological Opening (Erosion and Dilatation) ***//
		// Improve a mask shape
		// See OpenCV documentation for more informations :
		// http://docs.opencv.org/doc/tutorials/imgproc/erosion_dilatation/erosion_dilatation.html
	
	void erodeAndDilate(cv::Mat image)
	{
		cv::Mat element;
		element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
		
		// Erase small alone pixels
			// http://docs.opencv.org/modules/imgproc/doc/filtering.html#dilate
		for (int i = 0; i<2 ; i++)
		{
			cv::erode(image, image, element);
		}
		
		// Enlarge blocks of pixels
			// http://docs.opencv.org/modules/imgproc/doc/filtering.html#erode
		for (int i = 0; i<10 ; i++)
		{
			cv::dilate(image, image, element);
		}
	}

	
	//*** Image Threshold ***//
		// This function creates a mask for every defined color
	void thresholdHsv(cv::Mat image)
	{
		// Mask matrices (red, yellow, green, cyan, blue and magenta)
		cv::Mat r_mask, y_mask, g_mask, c_mask, b_mask, m_mask;
	
		// Objects contours
		std::vector<std::vector<cv::Point> > r_contours, y_contours, g_contours, 
											 c_contours, b_contours, m_contours;
		std::vector<cv::Vec4i> hierarchy;
		
		// Get every pixel whose value is between _min and _max
			// and put it into a mask
		cv::inRange(image, red_min, red_max, r_mask);
		cv::inRange(image, yellow_min, yellow_max, y_mask);
		cv::inRange(image, green_min, green_max, g_mask);
		cv::inRange(image, cyan_min, cyan_max, c_mask);
		cv::inRange(image, blue_min, blue_max, b_mask);
		cv::inRange(image, magenta_min, magenta_max, m_mask);
		
		// Improve masks shapes
		erodeAndDilate(r_mask);
		erodeAndDilate(y_mask);
		erodeAndDilate(g_mask);
		erodeAndDilate(c_mask);
		erodeAndDilate(b_mask);
		erodeAndDilate(m_mask);
	
		// Get the shapes contours from masks
		cv::findContours(r_mask, r_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(y_mask, y_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(g_mask, g_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(c_mask, c_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(b_mask, b_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
		cv::findContours(m_mask, m_contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
	
		// Process every contour. Note that color is taken in account.
		for (int i=1 ; i<=6 ; i++)
		{
			switch (i)
			{
			case 1:		// RED
				contoursProcess(r_contours, 1);
			
				break;
			
			case 2:		// YELLOW
				contoursProcess(y_contours, 2);
			
				break;
			
			case 3:		// GREEN
				contoursProcess(g_contours, 3);
			
				break;
			
			case 4:		// CYAN
				contoursProcess(c_contours, 4);
			
				break;
		
			case 5:		// BLUE
				contoursProcess(b_contours, 5);
			
				break;
			
			case 6:		// MAGENTA
				contoursProcess(m_contours, 6);
			
				break;
		
			default:	// You'll have to add a new color...
				break;
			}
		}
	
	}

	//*** Object Detection ***//
		// This function calls the previous image processing functions
		// and uses the objects and parameters storages to set the audio parameters
		// depending on the detected objects.
	void detectObjects(cv::Mat hsvImage, cv::Mat originalFrame)
	{	
		height_ = originalFrame.rows;
		width_ = originalFrame.cols;
		
		// Objects detection and storage 
		thresholdHsv(hsvImage);
		
		// Audio parameters setting
		for (int i=0 ; i<objects_storage_.size() ; i++)
		{
			for (int j=0 ; j<parameters_storage_.size() ; j++)
			{
				if(objects_storage_[i].color==parameters_storage_[j].color
					&& !parameters_storage_[j].used)
				{
					if (parameters_storage_[j].param=="color")
					{
						*parameters_storage_[j].zone=(float)objects_storage_[i].color;
					}
					else if (parameters_storage_[j].param=="x")
					{
						*parameters_storage_[j].zone=objects_storage_[i].centerX/width_;
					}
					else if (parameters_storage_[j].param=="y")
					{
						*parameters_storage_[j].zone=objects_storage_[i].centerY/height_;
					}
					else if (parameters_storage_[j].param=="area")
					{
						*parameters_storage_[j].zone=(float)objects_storage_[i].area;
					}
					parameters_storage_[j].used=true;
				}
			}
			
			
			
			// Draw a circle around the object for each object in the objects storage.
				// Circle color depends on the object color, of course !
			cv::Scalar bgr_color;
			switch (objects_storage_[i].color)
			{
				case 1: // RED
					bgr_color = cv::Scalar (0,0,255); // red in BGR (not RGB)
				
					break;
					
				case 2: //YELLOW
					bgr_color = cv::Scalar (0, 255, 255); // yellow in BGR
					
					break;
				
				case 3: // GREEN
					bgr_color = cv::Scalar (0, 255, 0);
				
					break;
				case 4: // CYAN
					bgr_color = cv::Scalar (255, 255, 0);
					
					break;
				
				case 5: // BLUE
					bgr_color = cv::Scalar (255,0,0); // blue in BGR (not RGB)
		
					break;
				
				case 6: // MAGENTA
					bgr_color = cv::Scalar (255, 0, 255);
					
					break;
	
				default: // Add a color !
					break;
			}
			// draw circle around every detected object
			cv::circle(originalFrame, cv::Point(objects_storage_[i].centerX, objects_storage_[i].centerY),
					   objects_storage_[i].radius, bgr_color, 2, 8, 0);
		}
	}
	
	/*******************************************/
	/********	Other Usefull Functions	********/
	/*******************************************/
	
	// Empty Function
		// This function empties both objects and parameters storages
	void empty()
    {
    	while (objects_storage_.size()>0)
    	{
    		objects_storage_.pop_back();
    	}
		
		for(int l=0 ; l<parameters_storage_.size() ; l++)
		{
			parameters_storage_[l].used=false;
		}
	}
	
	bool exit()
	{
		return exit_;
	}
	
	void run()
	{		
		exit_=false;
		int create_thread = 1;
	
		create_thread = pthread_create(&loop_, NULL, mainLoop, (void *) this);
		
		if (create_thread)
		{
			std::cout<<"Could not create thread. Thread Creation failed."<< std::endl;
		}

	}
       	
    ////////////////////////////////////////////
	////									////
	////		  MEMBER VARIABLES  		////
	////									////
	////////////////////////////////////////////
	
    private :
    	
	// HSV min and max values variables
	// #1 : RED
	static cv::Scalar red_min;
	static cv::Scalar red_max;
	
	// #2 : YELLOW
	static cv::Scalar yellow_min;
	static cv::Scalar yellow_max;
	
	// #3 : GREEN
	static cv::Scalar green_min;
	static cv::Scalar green_max;
	
	// #4 : CYAN
	static cv::Scalar cyan_min;
	static cv::Scalar cyan_max;
	
	// #5 : BLUE
	static cv::Scalar blue_min;	
	static cv::Scalar blue_max;
	
	// #6 : MAGENTA
	static cv::Scalar magenta_min;
	static cv::Scalar magenta_max;

	// Objects Storage
		// Where all the objects are stored
	std::vector<object> objects_storage_;
	
	// Parameters Storage
		// Where all the "ocv" metadata parameters are stored
	std::vector<metadata> parameters_storage_;
	
	// Matrix height and width
	int height_, width_;
	
	// Loop thread;
	pthread_t loop_;
	
	// Thread EXIT variable
	bool exit_;
		
};

// HSV min and max values
	// Note that H is between 0 and 180 
	// in openCV
	
// #1 = RED
cv::Scalar OCVUI::red_min = cv::Scalar (0,200,55);
cv::Scalar OCVUI::red_max = cv::Scalar (1,255,255);

// #2 = YELLOW
cv::Scalar OCVUI::yellow_min = cv::Scalar (25, 200, 55);
cv::Scalar OCVUI::yellow_max = cv::Scalar (35, 255, 255);

// #3 = GREEN
cv::Scalar OCVUI::green_min = cv::Scalar (30,155,55);
cv::Scalar OCVUI::green_max = cv::Scalar (40,255,255);

// #4 = CYAN
cv::Scalar OCVUI::cyan_min = cv::Scalar (85,200,55);
cv::Scalar OCVUI::cyan_max = cv::Scalar (95,200,55);

// #5 = BLUE
cv::Scalar OCVUI::blue_min = cv::Scalar (115,155,55);
cv::Scalar OCVUI::blue_max = cv::Scalar (125,255,255);

// #6 = MAGENTA
cv::Scalar OCVUI::magenta_min = cv::Scalar (145,200,55);
cv::Scalar OCVUI::magenta_max = cv::Scalar (155,255,255);


// Main Loop Function Implementation
	// This function is a loop that gets every frame from a camera
	// and calls the image processing functions.
	// This is the main function.
void* mainLoop(void* ocv_object)
{
	// The camera index allows to select the camera.
		// 0 stands for the default camera.
	int camIndex=0;
	//std::cout<<"camera index ?"<<std::endl;
	//std::cin>>camIndex;

	cv::Mat frame, hsv;
	OCVUI* ocv = (OCVUI*) ocv_object;
	cv::VideoCapture cap(camIndex);
	std::cout<<"Video Capture from camera n°"<<camIndex<<std::endl;
	
	if(!cap.isOpened())  // check if we succeeded to read frames
							// from camera
	{
		std::cout<<"Could not open camera n°"<<camIndex<<" !"<<std::endl;
		
	}
    	  
	cap.set(CV_CAP_PROP_FPS, 60); 	// Set frames rate
		
	cv::namedWindow( "Tracking", 1 );	// Create a window

	while(!ocv->exit())
   	{

   	    cap >> frame;							// Get frame from camera
   	    cv::cvtColor(frame, hsv, CV_BGR2HSV);		// Convert frame to HSV format 
   	    											// in order to use "inRange"
   	            														
		ocv->detectObjects(hsv, frame);				// Objects Detection function
	
   		/*** Show image ***/
   		cv::imshow("Tracking", frame);

   		ocv->empty();								// Empty the objects and parameters storages
     	
   		/*** break ***/
  		//if(cv::waitKey(27) >= 0) break;

   	}

}

#endif
#endif

/**************************BEGIN USER SECTION **************************/

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/



#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
  private:
	class SIG0 {
	  private:
		int 	fSamplingFreq;
		int 	iRec172[2];
	  public:
		int getNumInputs() 	{ return 0; }
		int getNumOutputs() 	{ return 1; }
		void init(int samplingFreq) {
			fSamplingFreq = samplingFreq;
			for (int i=0; i<2; i++) iRec172[i] = 0;
		}
		void fill (int count, float output[]) {
			// SECTION : 1
			for (int i=0; i<count; i++) {
				iRec172[0] = (1 + iRec172[1]);
				output[i] = (0 - (0.0016f * faustpower<2>((iRec172[0] - 101))));
				// post processing
				iRec172[1] = iRec172[0];
			}
		}
	};


	FAUSTFLOAT 	fslider0;
	float 	fRec2_perm[4];
	int 	iConst0;
	float 	fConst1;
	float 	fRec0_perm[4];
	float 	fRec1_perm[4];
	float 	fConst2;
	float 	fConst3;
	float 	fYec0_perm[4];
	float 	fConst4;
	float 	fRec8_perm[4];
	float 	fConst5;
	float 	fRec7_perm[4];
	int 	iRec9_perm[4];
	int 	iRec6_perm[4];
	int 	iRec5_perm[4];
	float 	fRec4_perm[4];
	FAUSTFLOAT 	fentry0;
	float 	fVec0[524288];
	int 	fVec0_idx;
	int 	fVec0_idx_save;
	FAUSTFLOAT 	fentry1;
	float 	fVec1[4096];
	int 	fVec1_idx;
	int 	fVec1_idx_save;
	int 	iConst6;
	float 	fRec3_perm[4];
	float 	fConst7;
	float 	fConst8;
	float 	fRec10_perm[4];
	FAUSTFLOAT 	fslider1;
	float 	fRec11_perm[4];
	FAUSTFLOAT 	fslider2;
	float 	fRec17_perm[4];
	FAUSTFLOAT 	fslider3;
	float 	fRec19_perm[4];
	float 	fRec24_perm[4];
	float 	fRec23_perm[4];
	float 	fRec22_perm[4];
	float 	fRec21_perm[4];
	float 	fRec28_perm[4];
	float 	fRec27_perm[4];
	float 	fRec26_perm[4];
	float 	fRec25_perm[4];
	FAUSTFLOAT 	fslider4;
	float 	fRec29_perm[4];
	float 	fConst9;
	float 	fRec20_perm[4];
	float 	fRec18_perm[4];
	FAUSTFLOAT 	fslider5;
	float 	fRec30_perm[4];
	float 	fConst10;
	float 	fRec16_perm[4];
	float 	fConst11;
	float 	fConst12;
	float 	fRec15_perm[4];
	float 	fConst13;
	float 	fConst14;
	float 	fRec14_perm[4];
	float 	fConst15;
	float 	fRec12_perm[4];
	float 	fRec13_perm[4];
	FAUSTFLOAT 	fslider6;
	float 	fRec32_perm[4];
	FAUSTFLOAT 	fslider7;
	float 	fConst16;
	float 	fRec31_perm[4];
	float 	fConst17;
	float 	fRec34_perm[4];
	float 	fRec35_perm[4];
	float 	fRec33_perm[4];
	float 	fRec40_perm[4];
	float 	fRec39_perm[4];
	float 	fRec38_perm[4];
	float 	fRec36_perm[4];
	float 	fRec37_perm[4];
	FAUSTFLOAT 	fslider8;
	float 	fRec42_perm[4];
	float 	fYec1_perm[4];
	float 	fRec41_perm[4];
	float 	fRec44_perm[4];
	float 	fRec45_perm[4];
	float 	fRec43_perm[4];
	float 	fRec50_perm[4];
	float 	fRec49_perm[4];
	float 	fRec48_perm[4];
	float 	fRec46_perm[4];
	float 	fRec47_perm[4];
	float 	fYec2_perm[4];
	float 	fRec51_perm[4];
	float 	fRec53_perm[4];
	float 	fRec54_perm[4];
	float 	fRec52_perm[4];
	float 	fRec59_perm[4];
	float 	fRec58_perm[4];
	float 	fRec57_perm[4];
	float 	fRec55_perm[4];
	float 	fRec56_perm[4];
	float 	fYec3_perm[4];
	float 	fRec60_perm[4];
	float 	fRec62_perm[4];
	float 	fRec63_perm[4];
	float 	fRec61_perm[4];
	float 	fRec68_perm[4];
	float 	fRec67_perm[4];
	float 	fRec66_perm[4];
	float 	fRec64_perm[4];
	float 	fRec65_perm[4];
	float 	fYec4_perm[4];
	float 	fRec69_perm[4];
	float 	fRec71_perm[4];
	float 	fRec72_perm[4];
	float 	fRec70_perm[4];
	float 	fRec77_perm[4];
	float 	fRec76_perm[4];
	float 	fRec75_perm[4];
	float 	fRec73_perm[4];
	float 	fRec74_perm[4];
	float 	fYec5_perm[4];
	float 	fRec78_perm[4];
	float 	fRec80_perm[4];
	float 	fRec81_perm[4];
	float 	fRec79_perm[4];
	float 	fRec86_perm[4];
	float 	fRec85_perm[4];
	float 	fRec84_perm[4];
	float 	fRec82_perm[4];
	float 	fRec83_perm[4];
	float 	fYec6_perm[4];
	float 	fRec87_perm[4];
	float 	fRec89_perm[4];
	float 	fRec90_perm[4];
	float 	fRec88_perm[4];
	float 	fRec95_perm[4];
	float 	fRec94_perm[4];
	float 	fRec93_perm[4];
	float 	fRec91_perm[4];
	float 	fRec92_perm[4];
	float 	fYec7_perm[4];
	float 	fRec96_perm[4];
	float 	fRec98_perm[4];
	float 	fRec99_perm[4];
	float 	fRec97_perm[4];
	float 	fRec104_perm[4];
	float 	fRec103_perm[4];
	float 	fRec102_perm[4];
	float 	fRec100_perm[4];
	float 	fRec101_perm[4];
	float 	fYec8_perm[4];
	float 	fRec105_perm[4];
	float 	fRec107_perm[4];
	float 	fRec108_perm[4];
	float 	fRec106_perm[4];
	float 	fRec113_perm[4];
	float 	fRec112_perm[4];
	float 	fRec111_perm[4];
	float 	fRec109_perm[4];
	float 	fRec110_perm[4];
	float 	fYec9_perm[4];
	float 	fRec114_perm[4];
	float 	fRec116_perm[4];
	float 	fRec117_perm[4];
	float 	fRec115_perm[4];
	float 	fRec122_perm[4];
	float 	fRec121_perm[4];
	float 	fRec120_perm[4];
	float 	fRec118_perm[4];
	float 	fRec119_perm[4];
	float 	fYec10_perm[4];
	float 	fRec123_perm[4];
	float 	fRec125_perm[4];
	float 	fRec126_perm[4];
	float 	fRec124_perm[4];
	float 	fRec131_perm[4];
	float 	fRec130_perm[4];
	float 	fRec129_perm[4];
	float 	fRec127_perm[4];
	float 	fRec128_perm[4];
	float 	fYec11_perm[4];
	float 	fRec132_perm[4];
	float 	fRec134_perm[4];
	float 	fRec135_perm[4];
	float 	fRec133_perm[4];
	float 	fRec140_perm[4];
	float 	fRec139_perm[4];
	float 	fRec138_perm[4];
	float 	fRec136_perm[4];
	float 	fRec137_perm[4];
	float 	fYec12_perm[4];
	float 	fRec141_perm[4];
	float 	fRec143_perm[4];
	float 	fRec144_perm[4];
	float 	fRec142_perm[4];
	float 	fRec149_perm[4];
	float 	fRec148_perm[4];
	float 	fRec147_perm[4];
	float 	fRec145_perm[4];
	float 	fRec146_perm[4];
	float 	fYec13_perm[4];
	float 	fRec150_perm[4];
	float 	fRec152_perm[4];
	float 	fRec153_perm[4];
	float 	fRec151_perm[4];
	float 	fRec158_perm[4];
	float 	fRec157_perm[4];
	float 	fRec156_perm[4];
	float 	fRec154_perm[4];
	float 	fRec155_perm[4];
	float 	fYec14_perm[4];
	float 	fRec159_perm[4];
	float 	fRec161_perm[4];
	float 	fRec162_perm[4];
	float 	fRec160_perm[4];
	float 	fRec167_perm[4];
	float 	fRec166_perm[4];
	float 	fRec165_perm[4];
	float 	fRec163_perm[4];
	float 	fRec164_perm[4];
	float 	fYec15_perm[4];
	float 	fRec168_perm[4];
	float 	fRec170_perm[4];
	float 	fRec171_perm[4];
	float 	fRec169_perm[4];
	static float 	ftbl0[200];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "PAFvocoder");
		m->declare("version", "0.7");
		m->declare("author", "Bart Brouns");
		m->declare("license", "GNU 3.0");
		m->declare("copyright", "(c) Bart Brouns 2014");
		m->declare("credits", "PitchTracker by Tiziano Bole, qompander by Katja Vetter,supersaw by ADAM SZABO,CZ oscillators by Mike Moser-Booth, saw and square oscillators adapted from the faust library");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/version", "1.11");
		m->declare("oscillator.lib/license", "STK-4.3");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("maxmsp.lib/name", "MaxMSP compatibility Library");
		m->declare("maxmsp.lib/author", "GRAME");
		m->declare("maxmsp.lib/copyright", "GRAME");
		m->declare("maxmsp.lib/version", "1.1");
		m->declare("maxmsp.lib/license", "LGPL");
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/version", "1.33");
		m->declare("effect.lib/license", "STK-4.3");
		m->declare("effect.lib/exciter_name", "Harmonic Exciter");
		m->declare("effect.lib/exciter_author", "Priyanka Shekar (pshekar@ccrma.stanford.edu)");
		m->declare("effect.lib/exciter_copyright", "Copyright (c) 2013 Priyanka Shekar");
		m->declare("effect.lib/exciter_version", "1.0");
		m->declare("effect.lib/exciter_license", "MIT License (MIT)");
		m->declare("qompander/qompander.lib/name", "qompander");
		m->declare("qompander/qompander.lib/version", "1.3");
		m->declare("qompander/qompander.lib/author", "Bart Brouns");
		m->declare("qompander/qompander.lib/license", "GNU 3.0");
		m->declare("qompander/qompander.lib/copyright", "(c) Bart Brouns 2014");
		m->declare("qompander/qompander.lib/credits", "ported from qompander in pd by Katja Vetter");
		m->declare("qompander/qompander.lib/see", "http://www.katjaas.nl/compander/compander.html");
		m->declare("qompander/qompander.lib/additional", "filter coefficients by Olli Niemitalo");
		m->declare("lib/mixer.lib/name", "Mixer");
		m->declare("lib/mixer.lib/version", "0.2");
		m->declare("lib/mixer.lib/author", "Bart Brouns");
		m->declare("lib/mixer.lib/license", "GNU 3.0");
		m->declare("lib/mixer.lib/copyright", "(c) Bart Brouns 2014");
	}

	virtual int getNumInputs() 	{ return 1; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
		SIG0 sig0;
		sig0.init(samplingFreq);
		sig0.fill(200,ftbl0);
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 25.0f;
		for (int i=0; i<4; i++) fRec2_perm[i]=0;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (0.012f * iConst0);
		for (int i=0; i<4; i++) fRec0_perm[i]=0;
		for (int i=0; i<4; i++) fRec1_perm[i]=0;
		fConst2 = (193.94747910642732f / float(iConst0));
		fConst3 = (1 - fConst2);
		for (int i=0; i<4; i++) fYec0_perm[i]=0;
		fConst4 = (1.0f / (1 + fConst2));
		for (int i=0; i<4; i++) fRec8_perm[i]=0;
		fConst5 = (3.141592653589793f / float(iConst0));
		for (int i=0; i<4; i++) fRec7_perm[i]=0;
		for (int i=0; i<4; i++) iRec9_perm[i]=0;
		for (int i=0; i<4; i++) iRec6_perm[i]=0;
		for (int i=0; i<4; i++) iRec5_perm[i]=0;
		for (int i=0; i<4; i++) fRec4_perm[i]=0;
		fentry0 = 61.7354f;
		for (int i=0; i<524288; i++) fVec0[i]=0;
		fVec0_idx = 0;
		fVec0_idx_save = 0;
		fentry1 = 0.0f;
		for (int i=0; i<4096; i++) fVec1[i]=0;
		fVec1_idx = 0;
		fVec1_idx_save = 0;
		iConst6 = int((2 * iConst0));
		for (int i=0; i<4; i++) fRec3_perm[i]=0;
		fConst7 = float(iConst0);
		fConst8 = (4.0f * fConst7);
		for (int i=0; i<4; i++) fRec10_perm[i]=0;
		fslider1 = 1.0f;
		for (int i=0; i<4; i++) fRec11_perm[i]=0;
		fslider2 = 8.0f;
		for (int i=0; i<4; i++) fRec17_perm[i]=0;
		fslider3 = 1.0f;
		for (int i=0; i<4; i++) fRec19_perm[i]=0;
		for (int i=0; i<4; i++) fRec24_perm[i]=0;
		for (int i=0; i<4; i++) fRec23_perm[i]=0;
		for (int i=0; i<4; i++) fRec22_perm[i]=0;
		for (int i=0; i<4; i++) fRec21_perm[i]=0;
		for (int i=0; i<4; i++) fRec28_perm[i]=0;
		for (int i=0; i<4; i++) fRec27_perm[i]=0;
		for (int i=0; i<4; i++) fRec26_perm[i]=0;
		for (int i=0; i<4; i++) fRec25_perm[i]=0;
		fslider4 = 2e+01f;
		for (int i=0; i<4; i++) fRec29_perm[i]=0;
		fConst9 = (1e+03f / float(iConst0));
		for (int i=0; i<4; i++) fRec20_perm[i]=0;
		for (int i=0; i<4; i++) fRec18_perm[i]=0;
		fslider5 = -4e+01f;
		for (int i=0; i<4; i++) fRec30_perm[i]=0;
		fConst10 = (0.5f * iConst0);
		for (int i=0; i<4; i++) fRec16_perm[i]=0;
		fConst11 = expf((0 - (2.5e+02f / float(iConst0))));
		fConst12 = (1.0f - fConst11);
		for (int i=0; i<4; i++) fRec15_perm[i]=0;
		fConst13 = expf((0 - (5e+02f / float(iConst0))));
		fConst14 = (1.0f - fConst13);
		for (int i=0; i<4; i++) fRec14_perm[i]=0;
		fConst15 = (0.003f * iConst0);
		for (int i=0; i<4; i++) fRec12_perm[i]=0;
		for (int i=0; i<4; i++) fRec13_perm[i]=0;
		fslider6 = 0.9238795325112867f;
		for (int i=0; i<4; i++) fRec32_perm[i]=0;
		fslider7 = -1.0f;
		fConst16 = (1.0f / fConst7);
		for (int i=0; i<4; i++) fRec31_perm[i]=0;
		fConst17 = (0.3f * iConst0);
		for (int i=0; i<4; i++) fRec34_perm[i]=0;
		for (int i=0; i<4; i++) fRec35_perm[i]=0;
		for (int i=0; i<4; i++) fRec33_perm[i]=0;
		for (int i=0; i<4; i++) fRec40_perm[i]=0;
		for (int i=0; i<4; i++) fRec39_perm[i]=0;
		for (int i=0; i<4; i++) fRec38_perm[i]=0;
		for (int i=0; i<4; i++) fRec36_perm[i]=0;
		for (int i=0; i<4; i++) fRec37_perm[i]=0;
		fslider8 = 11.313708498984761f;
		for (int i=0; i<4; i++) fRec42_perm[i]=0;
		for (int i=0; i<4; i++) fYec1_perm[i]=0;
		for (int i=0; i<4; i++) fRec41_perm[i]=0;
		for (int i=0; i<4; i++) fRec44_perm[i]=0;
		for (int i=0; i<4; i++) fRec45_perm[i]=0;
		for (int i=0; i<4; i++) fRec43_perm[i]=0;
		for (int i=0; i<4; i++) fRec50_perm[i]=0;
		for (int i=0; i<4; i++) fRec49_perm[i]=0;
		for (int i=0; i<4; i++) fRec48_perm[i]=0;
		for (int i=0; i<4; i++) fRec46_perm[i]=0;
		for (int i=0; i<4; i++) fRec47_perm[i]=0;
		for (int i=0; i<4; i++) fYec2_perm[i]=0;
		for (int i=0; i<4; i++) fRec51_perm[i]=0;
		for (int i=0; i<4; i++) fRec53_perm[i]=0;
		for (int i=0; i<4; i++) fRec54_perm[i]=0;
		for (int i=0; i<4; i++) fRec52_perm[i]=0;
		for (int i=0; i<4; i++) fRec59_perm[i]=0;
		for (int i=0; i<4; i++) fRec58_perm[i]=0;
		for (int i=0; i<4; i++) fRec57_perm[i]=0;
		for (int i=0; i<4; i++) fRec55_perm[i]=0;
		for (int i=0; i<4; i++) fRec56_perm[i]=0;
		for (int i=0; i<4; i++) fYec3_perm[i]=0;
		for (int i=0; i<4; i++) fRec60_perm[i]=0;
		for (int i=0; i<4; i++) fRec62_perm[i]=0;
		for (int i=0; i<4; i++) fRec63_perm[i]=0;
		for (int i=0; i<4; i++) fRec61_perm[i]=0;
		for (int i=0; i<4; i++) fRec68_perm[i]=0;
		for (int i=0; i<4; i++) fRec67_perm[i]=0;
		for (int i=0; i<4; i++) fRec66_perm[i]=0;
		for (int i=0; i<4; i++) fRec64_perm[i]=0;
		for (int i=0; i<4; i++) fRec65_perm[i]=0;
		for (int i=0; i<4; i++) fYec4_perm[i]=0;
		for (int i=0; i<4; i++) fRec69_perm[i]=0;
		for (int i=0; i<4; i++) fRec71_perm[i]=0;
		for (int i=0; i<4; i++) fRec72_perm[i]=0;
		for (int i=0; i<4; i++) fRec70_perm[i]=0;
		for (int i=0; i<4; i++) fRec77_perm[i]=0;
		for (int i=0; i<4; i++) fRec76_perm[i]=0;
		for (int i=0; i<4; i++) fRec75_perm[i]=0;
		for (int i=0; i<4; i++) fRec73_perm[i]=0;
		for (int i=0; i<4; i++) fRec74_perm[i]=0;
		for (int i=0; i<4; i++) fYec5_perm[i]=0;
		for (int i=0; i<4; i++) fRec78_perm[i]=0;
		for (int i=0; i<4; i++) fRec80_perm[i]=0;
		for (int i=0; i<4; i++) fRec81_perm[i]=0;
		for (int i=0; i<4; i++) fRec79_perm[i]=0;
		for (int i=0; i<4; i++) fRec86_perm[i]=0;
		for (int i=0; i<4; i++) fRec85_perm[i]=0;
		for (int i=0; i<4; i++) fRec84_perm[i]=0;
		for (int i=0; i<4; i++) fRec82_perm[i]=0;
		for (int i=0; i<4; i++) fRec83_perm[i]=0;
		for (int i=0; i<4; i++) fYec6_perm[i]=0;
		for (int i=0; i<4; i++) fRec87_perm[i]=0;
		for (int i=0; i<4; i++) fRec89_perm[i]=0;
		for (int i=0; i<4; i++) fRec90_perm[i]=0;
		for (int i=0; i<4; i++) fRec88_perm[i]=0;
		for (int i=0; i<4; i++) fRec95_perm[i]=0;
		for (int i=0; i<4; i++) fRec94_perm[i]=0;
		for (int i=0; i<4; i++) fRec93_perm[i]=0;
		for (int i=0; i<4; i++) fRec91_perm[i]=0;
		for (int i=0; i<4; i++) fRec92_perm[i]=0;
		for (int i=0; i<4; i++) fYec7_perm[i]=0;
		for (int i=0; i<4; i++) fRec96_perm[i]=0;
		for (int i=0; i<4; i++) fRec98_perm[i]=0;
		for (int i=0; i<4; i++) fRec99_perm[i]=0;
		for (int i=0; i<4; i++) fRec97_perm[i]=0;
		for (int i=0; i<4; i++) fRec104_perm[i]=0;
		for (int i=0; i<4; i++) fRec103_perm[i]=0;
		for (int i=0; i<4; i++) fRec102_perm[i]=0;
		for (int i=0; i<4; i++) fRec100_perm[i]=0;
		for (int i=0; i<4; i++) fRec101_perm[i]=0;
		for (int i=0; i<4; i++) fYec8_perm[i]=0;
		for (int i=0; i<4; i++) fRec105_perm[i]=0;
		for (int i=0; i<4; i++) fRec107_perm[i]=0;
		for (int i=0; i<4; i++) fRec108_perm[i]=0;
		for (int i=0; i<4; i++) fRec106_perm[i]=0;
		for (int i=0; i<4; i++) fRec113_perm[i]=0;
		for (int i=0; i<4; i++) fRec112_perm[i]=0;
		for (int i=0; i<4; i++) fRec111_perm[i]=0;
		for (int i=0; i<4; i++) fRec109_perm[i]=0;
		for (int i=0; i<4; i++) fRec110_perm[i]=0;
		for (int i=0; i<4; i++) fYec9_perm[i]=0;
		for (int i=0; i<4; i++) fRec114_perm[i]=0;
		for (int i=0; i<4; i++) fRec116_perm[i]=0;
		for (int i=0; i<4; i++) fRec117_perm[i]=0;
		for (int i=0; i<4; i++) fRec115_perm[i]=0;
		for (int i=0; i<4; i++) fRec122_perm[i]=0;
		for (int i=0; i<4; i++) fRec121_perm[i]=0;
		for (int i=0; i<4; i++) fRec120_perm[i]=0;
		for (int i=0; i<4; i++) fRec118_perm[i]=0;
		for (int i=0; i<4; i++) fRec119_perm[i]=0;
		for (int i=0; i<4; i++) fYec10_perm[i]=0;
		for (int i=0; i<4; i++) fRec123_perm[i]=0;
		for (int i=0; i<4; i++) fRec125_perm[i]=0;
		for (int i=0; i<4; i++) fRec126_perm[i]=0;
		for (int i=0; i<4; i++) fRec124_perm[i]=0;
		for (int i=0; i<4; i++) fRec131_perm[i]=0;
		for (int i=0; i<4; i++) fRec130_perm[i]=0;
		for (int i=0; i<4; i++) fRec129_perm[i]=0;
		for (int i=0; i<4; i++) fRec127_perm[i]=0;
		for (int i=0; i<4; i++) fRec128_perm[i]=0;
		for (int i=0; i<4; i++) fYec11_perm[i]=0;
		for (int i=0; i<4; i++) fRec132_perm[i]=0;
		for (int i=0; i<4; i++) fRec134_perm[i]=0;
		for (int i=0; i<4; i++) fRec135_perm[i]=0;
		for (int i=0; i<4; i++) fRec133_perm[i]=0;
		for (int i=0; i<4; i++) fRec140_perm[i]=0;
		for (int i=0; i<4; i++) fRec139_perm[i]=0;
		for (int i=0; i<4; i++) fRec138_perm[i]=0;
		for (int i=0; i<4; i++) fRec136_perm[i]=0;
		for (int i=0; i<4; i++) fRec137_perm[i]=0;
		for (int i=0; i<4; i++) fYec12_perm[i]=0;
		for (int i=0; i<4; i++) fRec141_perm[i]=0;
		for (int i=0; i<4; i++) fRec143_perm[i]=0;
		for (int i=0; i<4; i++) fRec144_perm[i]=0;
		for (int i=0; i<4; i++) fRec142_perm[i]=0;
		for (int i=0; i<4; i++) fRec149_perm[i]=0;
		for (int i=0; i<4; i++) fRec148_perm[i]=0;
		for (int i=0; i<4; i++) fRec147_perm[i]=0;
		for (int i=0; i<4; i++) fRec145_perm[i]=0;
		for (int i=0; i<4; i++) fRec146_perm[i]=0;
		for (int i=0; i<4; i++) fYec13_perm[i]=0;
		for (int i=0; i<4; i++) fRec150_perm[i]=0;
		for (int i=0; i<4; i++) fRec152_perm[i]=0;
		for (int i=0; i<4; i++) fRec153_perm[i]=0;
		for (int i=0; i<4; i++) fRec151_perm[i]=0;
		for (int i=0; i<4; i++) fRec158_perm[i]=0;
		for (int i=0; i<4; i++) fRec157_perm[i]=0;
		for (int i=0; i<4; i++) fRec156_perm[i]=0;
		for (int i=0; i<4; i++) fRec154_perm[i]=0;
		for (int i=0; i<4; i++) fRec155_perm[i]=0;
		for (int i=0; i<4; i++) fYec14_perm[i]=0;
		for (int i=0; i<4; i++) fRec159_perm[i]=0;
		for (int i=0; i<4; i++) fRec161_perm[i]=0;
		for (int i=0; i<4; i++) fRec162_perm[i]=0;
		for (int i=0; i<4; i++) fRec160_perm[i]=0;
		for (int i=0; i<4; i++) fRec167_perm[i]=0;
		for (int i=0; i<4; i++) fRec166_perm[i]=0;
		for (int i=0; i<4; i++) fRec165_perm[i]=0;
		for (int i=0; i<4; i++) fRec163_perm[i]=0;
		for (int i=0; i<4; i++) fRec164_perm[i]=0;
		for (int i=0; i<4; i++) fYec15_perm[i]=0;
		for (int i=0; i<4; i++) fRec168_perm[i]=0;
		for (int i=0; i<4; i++) fRec170_perm[i]=0;
		for (int i=0; i<4; i++) fRec171_perm[i]=0;
		for (int i=0; i<4; i++) fRec169_perm[i]=0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(0, "0", "");
		interface->openHorizontalBox("OSC");
		interface->declare(&fentry0, "0", "");
		interface->addNumEntry("pitch", &fentry0, 61.7354f, 61.7354f, 493.6635f, 0.001f);
		interface->declare(&fentry1, "1", "");
		interface->addNumEntry("fidelity", &fentry1, 0.0f, 0.0f, 1.0f, 0.001f);
		interface->closeBox();
		interface->declare(0, "1", "");
		interface->openTabBox("0x00");
		interface->declare(0, "0", "");
		interface->openHorizontalBox("synths");
		interface->declare(0, "3", "");
		interface->declare(0, "tooltip", "a vocoder built from oscilators that sound like they have a bandpass-filter on them");
		interface->openHorizontalBox("PAFvocoder");
		interface->declare(0, "0", "");
		interface->openVerticalBox("levels");
		interface->declare(&fslider7, "3", "");
		interface->addVerticalSlider("octave", &fslider7, -1.0f, -2.0f, 2.0f, 1.0f);
		interface->closeBox();
		interface->declare(0, "1", "");
		interface->openVerticalBox("parameters");
		interface->declare(&fslider8, "1", "");
		interface->declare(&fslider8, "style", "knob");
		interface->declare(&fslider8, "tooltip", "the lowest frequency of the band-pass filters");
		interface->addVerticalSlider("top", &fslider8, 11.313708498984761f, 1.0f, 64.0f, 0.001f);
		interface->declare(&fslider6, "2", "");
		interface->declare(&fslider6, "style", "knob");
		interface->declare(&fslider6, "tooltip", "the highest frequency of the band-pass filters");
		interface->addVerticalSlider("bottom", &fslider6, 0.9238795325112867f, 0.5f, 7.0f, 0.001f);
		interface->declare(&fslider0, "3", "");
		interface->declare(&fslider0, "style", "knob");
		interface->declare(&fslider0, "tooltip", "PAF index");
		interface->addVerticalSlider("index", &fslider0, 25.0f, 1.0f, 1e+02f, 0.001f);
		interface->declare(&fslider1, "4", "");
		interface->declare(&fslider1, "style", "knob");
		interface->declare(&fslider1, "tooltip", "stereo-width");
		interface->addVerticalSlider("width", &fslider1, 1.0f, 0.0f, 2.0f, 0.001f);
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
		interface->declare(0, "1", "");
		interface->openHorizontalBox("effects");
		interface->declare(0, "0", "");
		interface->declare(0, "tooltip", "Reference: http://www.katjaas.nl/compander/compander.html");
		interface->openVerticalBox("input compression and expansion");
		interface->declare(&fslider2, "0", "");
		interface->declare(&fslider2, "style", "knob");
		interface->declare(&fslider2, "unit", ":1");
		interface->addHorizontalSlider("factor", &fslider2, 8.0f, 0.8f, 8.0f, 0.01f);
		interface->declare(&fslider5, "1", "");
		interface->declare(&fslider5, "style", "knob");
		interface->declare(&fslider5, "unit", "dB");
		interface->addHorizontalSlider("threshold", &fslider5, -4e+01f, -96.0f, -2e+01f, 0.01f);
		interface->declare(&fslider3, "2", "");
		interface->declare(&fslider3, "style", "knob");
		interface->declare(&fslider3, "unit", "ms");
		interface->addHorizontalSlider("attack", &fslider3, 1.0f, 1.0f, 2e+01f, 0.01f);
		interface->declare(&fslider4, "3", "");
		interface->declare(&fslider4, "style", "knob");
		interface->declare(&fslider4, "unit", "ms");
		interface->addHorizontalSlider("release", &fslider4, 2e+01f, 2e+01f, 1e+03f, 0.01f);
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
		interface->closeBox();
	}
	virtual void compute (int fullcount, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fRec2_tmp[1024+4];
		float 	fZec0[1024];
		float 	fRec0_tmp[1024+4];
		float 	fRec1_tmp[1024+4];
		float 	fYec0_tmp[1024+4];
		float 	fRec8_tmp[1024+4];
		float 	fZec1[1024];
		float 	fZec2[1024];
		float 	fRec7_tmp[1024+4];
		int 	iZec3[1024];
		int 	iRec9_tmp[1024+4];
		int 	iZec4[1024];
		int 	iRec6_tmp[1024+4];
		int 	iZec5[1024];
		int 	iRec5_tmp[1024+4];
		float 	fRec4_tmp[1024+4];
		float 	fRec3_tmp[1024+4];
		float 	fRec10_tmp[1024+4];
		float 	fRec11_tmp[1024+4];
		float 	fRec17_tmp[1024+4];
		float 	fRec19_tmp[1024+4];
		float 	fRec24_tmp[1024+4];
		float 	fRec23_tmp[1024+4];
		float 	fRec22_tmp[1024+4];
		float 	fRec21_tmp[1024+4];
		float 	fRec28_tmp[1024+4];
		float 	fRec27_tmp[1024+4];
		float 	fRec26_tmp[1024+4];
		float 	fRec25_tmp[1024+4];
		float 	fRec29_tmp[1024+4];
		float 	fZec6[1024];
		float 	fZec7[1024];
		float 	fZec8[1024];
		float 	fZec9[1024];
		float 	fRec20_tmp[1024+4];
		float 	fZec10[1024];
		float 	fRec18_tmp[1024+4];
		float 	fRec30_tmp[1024+4];
		float 	fZec11[1024];
		float 	fZec12[1024];
		float 	fZec13[1024];
		float 	fZec14[1024];
		float 	fZec15[1024];
		float 	fRec16_tmp[1024+4];
		float 	fZec16[1024];
		float 	fRec15_tmp[1024+4];
		float 	fRec14_tmp[1024+4];
		float 	fZec17[1024];
		float 	fRec12_tmp[1024+4];
		float 	fRec13_tmp[1024+4];
		float 	fRec32_tmp[1024+4];
		float 	fZec18[1024];
		float 	fZec19[1024];
		float 	fZec20[1024];
		int 	iZec21[1024];
		float 	fRec31_tmp[1024+4];
		float 	fZec22[1024];
		float 	fRec34_tmp[1024+4];
		float 	fRec35_tmp[1024+4];
		float 	fRec33_tmp[1024+4];
		float 	fZec23[1024];
		float 	fZec24[1024];
		float 	fZec25[1024];
		float 	fRec40_tmp[1024+4];
		float 	fZec26[1024];
		float 	fRec39_tmp[1024+4];
		float 	fRec38_tmp[1024+4];
		float 	fZec27[1024];
		float 	fRec36_tmp[1024+4];
		float 	fRec37_tmp[1024+4];
		float 	fRec42_tmp[1024+4];
		float 	fZec28[1024];
		float 	fYec1_tmp[1024+4];
		float 	fRec41_tmp[1024+4];
		float 	fZec29[1024];
		float 	fRec44_tmp[1024+4];
		float 	fRec45_tmp[1024+4];
		float 	fRec43_tmp[1024+4];
		float 	fZec30[1024];
		float 	fZec31[1024];
		float 	fZec32[1024];
		float 	fRec50_tmp[1024+4];
		float 	fZec33[1024];
		float 	fRec49_tmp[1024+4];
		float 	fRec48_tmp[1024+4];
		float 	fZec34[1024];
		float 	fRec46_tmp[1024+4];
		float 	fRec47_tmp[1024+4];
		float 	fYec2_tmp[1024+4];
		float 	fRec51_tmp[1024+4];
		float 	fZec35[1024];
		float 	fRec53_tmp[1024+4];
		float 	fRec54_tmp[1024+4];
		float 	fRec52_tmp[1024+4];
		float 	fZec36[1024];
		float 	fZec37[1024];
		float 	fZec38[1024];
		float 	fRec59_tmp[1024+4];
		float 	fZec39[1024];
		float 	fRec58_tmp[1024+4];
		float 	fRec57_tmp[1024+4];
		float 	fZec40[1024];
		float 	fRec55_tmp[1024+4];
		float 	fRec56_tmp[1024+4];
		float 	fYec3_tmp[1024+4];
		float 	fRec60_tmp[1024+4];
		float 	fZec41[1024];
		float 	fRec62_tmp[1024+4];
		float 	fRec63_tmp[1024+4];
		float 	fRec61_tmp[1024+4];
		float 	fZec42[1024];
		float 	fZec43[1024];
		float 	fZec44[1024];
		float 	fRec68_tmp[1024+4];
		float 	fZec45[1024];
		float 	fRec67_tmp[1024+4];
		float 	fRec66_tmp[1024+4];
		float 	fZec46[1024];
		float 	fRec64_tmp[1024+4];
		float 	fRec65_tmp[1024+4];
		float 	fYec4_tmp[1024+4];
		float 	fRec69_tmp[1024+4];
		float 	fZec47[1024];
		float 	fRec71_tmp[1024+4];
		float 	fRec72_tmp[1024+4];
		float 	fRec70_tmp[1024+4];
		float 	fZec48[1024];
		float 	fZec49[1024];
		float 	fZec50[1024];
		float 	fRec77_tmp[1024+4];
		float 	fZec51[1024];
		float 	fRec76_tmp[1024+4];
		float 	fRec75_tmp[1024+4];
		float 	fZec52[1024];
		float 	fRec73_tmp[1024+4];
		float 	fRec74_tmp[1024+4];
		float 	fYec5_tmp[1024+4];
		float 	fRec78_tmp[1024+4];
		float 	fZec53[1024];
		float 	fRec80_tmp[1024+4];
		float 	fRec81_tmp[1024+4];
		float 	fRec79_tmp[1024+4];
		float 	fZec54[1024];
		float 	fZec55[1024];
		float 	fZec56[1024];
		float 	fRec86_tmp[1024+4];
		float 	fZec57[1024];
		float 	fRec85_tmp[1024+4];
		float 	fRec84_tmp[1024+4];
		float 	fZec58[1024];
		float 	fRec82_tmp[1024+4];
		float 	fRec83_tmp[1024+4];
		float 	fYec6_tmp[1024+4];
		float 	fRec87_tmp[1024+4];
		float 	fZec59[1024];
		float 	fRec89_tmp[1024+4];
		float 	fRec90_tmp[1024+4];
		float 	fRec88_tmp[1024+4];
		float 	fZec60[1024];
		float 	fZec61[1024];
		float 	fZec62[1024];
		float 	fRec95_tmp[1024+4];
		float 	fZec63[1024];
		float 	fRec94_tmp[1024+4];
		float 	fRec93_tmp[1024+4];
		float 	fZec64[1024];
		float 	fRec91_tmp[1024+4];
		float 	fRec92_tmp[1024+4];
		float 	fYec7_tmp[1024+4];
		float 	fRec96_tmp[1024+4];
		float 	fZec65[1024];
		float 	fRec98_tmp[1024+4];
		float 	fRec99_tmp[1024+4];
		float 	fRec97_tmp[1024+4];
		float 	fZec66[1024];
		float 	fZec67[1024];
		float 	fZec68[1024];
		float 	fRec104_tmp[1024+4];
		float 	fZec69[1024];
		float 	fRec103_tmp[1024+4];
		float 	fRec102_tmp[1024+4];
		float 	fZec70[1024];
		float 	fRec100_tmp[1024+4];
		float 	fRec101_tmp[1024+4];
		float 	fYec8_tmp[1024+4];
		float 	fRec105_tmp[1024+4];
		float 	fZec71[1024];
		float 	fRec107_tmp[1024+4];
		float 	fRec108_tmp[1024+4];
		float 	fRec106_tmp[1024+4];
		float 	fZec72[1024];
		float 	fZec73[1024];
		float 	fZec74[1024];
		float 	fRec113_tmp[1024+4];
		float 	fZec75[1024];
		float 	fRec112_tmp[1024+4];
		float 	fRec111_tmp[1024+4];
		float 	fZec76[1024];
		float 	fRec109_tmp[1024+4];
		float 	fRec110_tmp[1024+4];
		float 	fYec9_tmp[1024+4];
		float 	fRec114_tmp[1024+4];
		float 	fZec77[1024];
		float 	fRec116_tmp[1024+4];
		float 	fRec117_tmp[1024+4];
		float 	fRec115_tmp[1024+4];
		float 	fZec78[1024];
		float 	fZec79[1024];
		float 	fZec80[1024];
		float 	fRec122_tmp[1024+4];
		float 	fZec81[1024];
		float 	fRec121_tmp[1024+4];
		float 	fRec120_tmp[1024+4];
		float 	fZec82[1024];
		float 	fRec118_tmp[1024+4];
		float 	fRec119_tmp[1024+4];
		float 	fYec10_tmp[1024+4];
		float 	fRec123_tmp[1024+4];
		float 	fZec83[1024];
		float 	fRec125_tmp[1024+4];
		float 	fRec126_tmp[1024+4];
		float 	fRec124_tmp[1024+4];
		float 	fZec84[1024];
		float 	fZec85[1024];
		float 	fZec86[1024];
		float 	fRec131_tmp[1024+4];
		float 	fZec87[1024];
		float 	fRec130_tmp[1024+4];
		float 	fRec129_tmp[1024+4];
		float 	fZec88[1024];
		float 	fRec127_tmp[1024+4];
		float 	fRec128_tmp[1024+4];
		float 	fYec11_tmp[1024+4];
		float 	fRec132_tmp[1024+4];
		float 	fZec89[1024];
		float 	fRec134_tmp[1024+4];
		float 	fRec135_tmp[1024+4];
		float 	fRec133_tmp[1024+4];
		float 	fZec90[1024];
		float 	fZec91[1024];
		float 	fZec92[1024];
		float 	fRec140_tmp[1024+4];
		float 	fZec93[1024];
		float 	fRec139_tmp[1024+4];
		float 	fRec138_tmp[1024+4];
		float 	fZec94[1024];
		float 	fRec136_tmp[1024+4];
		float 	fRec137_tmp[1024+4];
		float 	fYec12_tmp[1024+4];
		float 	fRec141_tmp[1024+4];
		float 	fZec95[1024];
		float 	fRec143_tmp[1024+4];
		float 	fRec144_tmp[1024+4];
		float 	fRec142_tmp[1024+4];
		float 	fZec96[1024];
		float 	fZec97[1024];
		float 	fZec98[1024];
		float 	fRec149_tmp[1024+4];
		float 	fZec99[1024];
		float 	fRec148_tmp[1024+4];
		float 	fRec147_tmp[1024+4];
		float 	fZec100[1024];
		float 	fRec145_tmp[1024+4];
		float 	fRec146_tmp[1024+4];
		float 	fYec13_tmp[1024+4];
		float 	fRec150_tmp[1024+4];
		float 	fZec101[1024];
		float 	fRec152_tmp[1024+4];
		float 	fRec153_tmp[1024+4];
		float 	fRec151_tmp[1024+4];
		float 	fZec102[1024];
		float 	fZec103[1024];
		float 	fZec104[1024];
		float 	fRec158_tmp[1024+4];
		float 	fZec105[1024];
		float 	fRec157_tmp[1024+4];
		float 	fRec156_tmp[1024+4];
		float 	fZec106[1024];
		float 	fRec154_tmp[1024+4];
		float 	fRec155_tmp[1024+4];
		float 	fYec14_tmp[1024+4];
		float 	fRec159_tmp[1024+4];
		float 	fZec107[1024];
		float 	fRec161_tmp[1024+4];
		float 	fRec162_tmp[1024+4];
		float 	fRec160_tmp[1024+4];
		float 	fZec108[1024];
		float 	fZec109[1024];
		float 	fZec110[1024];
		float 	fRec167_tmp[1024+4];
		float 	fZec111[1024];
		float 	fRec166_tmp[1024+4];
		float 	fRec165_tmp[1024+4];
		float 	fZec112[1024];
		float 	fRec163_tmp[1024+4];
		float 	fRec164_tmp[1024+4];
		float 	fYec15_tmp[1024+4];
		float 	fRec168_tmp[1024+4];
		float 	fZec113[1024];
		float 	fRec170_tmp[1024+4];
		float 	fRec171_tmp[1024+4];
		float 	fRec169_tmp[1024+4];
		float 	fZec114[1024];
		float 	fZec115[1024];
		float 	fZec116[1024];
		float 	fZec117[1024];
		float 	fZec118[1024];
		float 	fZec119[1024];
		float 	fZec120[1024];
		float 	fZec121[1024];
		float 	fZec122[1024];
		float 	fZec123[1024];
		float 	fZec124[1024];
		float 	fZec125[1024];
		float 	fZec126[1024];
		float 	fZec127[1024];
		float 	fZec128[1024];
		float 	fZec129[1024];
		float 	fZec130[1024];
		float 	fZec131[1024];
		float 	fZec132[1024];
		float 	fZec133[1024];
		float 	fZec134[1024];
		float 	fZec135[1024];
		float 	fZec136[1024];
		float 	fZec137[1024];
		float 	fZec138[1024];
		float 	fZec139[1024];
		float 	fZec140[1024];
		float 	fZec141[1024];
		float 	fZec142[1024];
		float 	fZec143[1024];
		float 	fZec144[1024];
		float 	fZec145[1024];
		float 	fZec146[1024];
		float 	fZec147[1024];
		float 	fZec148[1024];
		float 	fZec149[1024];
		float 	fZec150[1024];
		float 	fZec151[1024];
		float 	fZec152[1024];
		float 	fZec153[1024];
		float 	fZec154[1024];
		float 	fZec155[1024];
		float 	fZec156[1024];
		float 	fZec157[1024];
		float 	fZec158[1024];
		float 	fZec159[1024];
		float 	fZec160[1024];
		float 	fZec161[1024];
		float 	fZec162[1024];
		float 	fZec163[1024];
		float 	fZec164[1024];
		float 	fZec165[1024];
		float 	fZec166[1024];
		float 	fSlow0 = (0.0010000000000000009f * float(fslider0));
		float* 	fRec2 = &fRec2_tmp[4];
		float* 	fRec0 = &fRec0_tmp[4];
		float* 	fRec1 = &fRec1_tmp[4];
		float* 	fYec0 = &fYec0_tmp[4];
		float* 	fRec8 = &fRec8_tmp[4];
		float* 	fRec7 = &fRec7_tmp[4];
		int* 	iRec9 = &iRec9_tmp[4];
		int* 	iRec6 = &iRec6_tmp[4];
		int* 	iRec5 = &iRec5_tmp[4];
		float* 	fRec4 = &fRec4_tmp[4];
		float 	fSlow1 = float(fentry0);
		float 	fSlow2 = float(fentry1);
		float* 	fRec3 = &fRec3_tmp[4];
		float* 	fRec10 = &fRec10_tmp[4];
		float 	fSlow3 = (0.0010000000000000009f * float(fslider1));
		float* 	fRec11 = &fRec11_tmp[4];
		float 	fSlow4 = (0.0010000000000000009f * float(fslider2));
		float* 	fRec17 = &fRec17_tmp[4];
		float 	fSlow5 = (0.0010000000000000009f * float(fslider3));
		float* 	fRec19 = &fRec19_tmp[4];
		float* 	fRec24 = &fRec24_tmp[4];
		float* 	fRec23 = &fRec23_tmp[4];
		float* 	fRec22 = &fRec22_tmp[4];
		float* 	fRec21 = &fRec21_tmp[4];
		float* 	fRec28 = &fRec28_tmp[4];
		float* 	fRec27 = &fRec27_tmp[4];
		float* 	fRec26 = &fRec26_tmp[4];
		float* 	fRec25 = &fRec25_tmp[4];
		float 	fSlow6 = (0.0010000000000000009f * float(fslider4));
		float* 	fRec29 = &fRec29_tmp[4];
		float* 	fRec20 = &fRec20_tmp[4];
		float* 	fRec18 = &fRec18_tmp[4];
		float 	fSlow7 = (0.0010000000000000009f * float(fslider5));
		float* 	fRec30 = &fRec30_tmp[4];
		float* 	fRec16 = &fRec16_tmp[4];
		float* 	fRec15 = &fRec15_tmp[4];
		float* 	fRec14 = &fRec14_tmp[4];
		float* 	fRec12 = &fRec12_tmp[4];
		float* 	fRec13 = &fRec13_tmp[4];
		float 	fSlow8 = (0.0010000000000000009f * faustpower<2>(float(fslider6)));
		float* 	fRec32 = &fRec32_tmp[4];
		float 	fSlow9 = float(fslider7);
		float 	fSlow10 = (fConst16 * ((0.25f * (fSlow9 == -2)) + ((0.5f * (fSlow9 == -1)) + ((fSlow9 == 0) + ((2 * (fSlow9 == 1)) + (4 * (fSlow9 == 2)))))));
		float* 	fRec31 = &fRec31_tmp[4];
		float* 	fRec34 = &fRec34_tmp[4];
		float* 	fRec35 = &fRec35_tmp[4];
		float* 	fRec33 = &fRec33_tmp[4];
		float* 	fRec40 = &fRec40_tmp[4];
		float* 	fRec39 = &fRec39_tmp[4];
		float* 	fRec38 = &fRec38_tmp[4];
		float* 	fRec36 = &fRec36_tmp[4];
		float* 	fRec37 = &fRec37_tmp[4];
		float 	fSlow11 = (0.0010000000000000009f * faustpower<2>(float(fslider8)));
		float* 	fRec42 = &fRec42_tmp[4];
		float* 	fYec1 = &fYec1_tmp[4];
		float* 	fRec41 = &fRec41_tmp[4];
		float* 	fRec44 = &fRec44_tmp[4];
		float* 	fRec45 = &fRec45_tmp[4];
		float* 	fRec43 = &fRec43_tmp[4];
		float* 	fRec50 = &fRec50_tmp[4];
		float* 	fRec49 = &fRec49_tmp[4];
		float* 	fRec48 = &fRec48_tmp[4];
		float* 	fRec46 = &fRec46_tmp[4];
		float* 	fRec47 = &fRec47_tmp[4];
		float* 	fYec2 = &fYec2_tmp[4];
		float* 	fRec51 = &fRec51_tmp[4];
		float* 	fRec53 = &fRec53_tmp[4];
		float* 	fRec54 = &fRec54_tmp[4];
		float* 	fRec52 = &fRec52_tmp[4];
		float* 	fRec59 = &fRec59_tmp[4];
		float* 	fRec58 = &fRec58_tmp[4];
		float* 	fRec57 = &fRec57_tmp[4];
		float* 	fRec55 = &fRec55_tmp[4];
		float* 	fRec56 = &fRec56_tmp[4];
		float* 	fYec3 = &fYec3_tmp[4];
		float* 	fRec60 = &fRec60_tmp[4];
		float* 	fRec62 = &fRec62_tmp[4];
		float* 	fRec63 = &fRec63_tmp[4];
		float* 	fRec61 = &fRec61_tmp[4];
		float* 	fRec68 = &fRec68_tmp[4];
		float* 	fRec67 = &fRec67_tmp[4];
		float* 	fRec66 = &fRec66_tmp[4];
		float* 	fRec64 = &fRec64_tmp[4];
		float* 	fRec65 = &fRec65_tmp[4];
		float* 	fYec4 = &fYec4_tmp[4];
		float* 	fRec69 = &fRec69_tmp[4];
		float* 	fRec71 = &fRec71_tmp[4];
		float* 	fRec72 = &fRec72_tmp[4];
		float* 	fRec70 = &fRec70_tmp[4];
		float* 	fRec77 = &fRec77_tmp[4];
		float* 	fRec76 = &fRec76_tmp[4];
		float* 	fRec75 = &fRec75_tmp[4];
		float* 	fRec73 = &fRec73_tmp[4];
		float* 	fRec74 = &fRec74_tmp[4];
		float* 	fYec5 = &fYec5_tmp[4];
		float* 	fRec78 = &fRec78_tmp[4];
		float* 	fRec80 = &fRec80_tmp[4];
		float* 	fRec81 = &fRec81_tmp[4];
		float* 	fRec79 = &fRec79_tmp[4];
		float* 	fRec86 = &fRec86_tmp[4];
		float* 	fRec85 = &fRec85_tmp[4];
		float* 	fRec84 = &fRec84_tmp[4];
		float* 	fRec82 = &fRec82_tmp[4];
		float* 	fRec83 = &fRec83_tmp[4];
		float* 	fYec6 = &fYec6_tmp[4];
		float* 	fRec87 = &fRec87_tmp[4];
		float* 	fRec89 = &fRec89_tmp[4];
		float* 	fRec90 = &fRec90_tmp[4];
		float* 	fRec88 = &fRec88_tmp[4];
		float* 	fRec95 = &fRec95_tmp[4];
		float* 	fRec94 = &fRec94_tmp[4];
		float* 	fRec93 = &fRec93_tmp[4];
		float* 	fRec91 = &fRec91_tmp[4];
		float* 	fRec92 = &fRec92_tmp[4];
		float* 	fYec7 = &fYec7_tmp[4];
		float* 	fRec96 = &fRec96_tmp[4];
		float* 	fRec98 = &fRec98_tmp[4];
		float* 	fRec99 = &fRec99_tmp[4];
		float* 	fRec97 = &fRec97_tmp[4];
		float* 	fRec104 = &fRec104_tmp[4];
		float* 	fRec103 = &fRec103_tmp[4];
		float* 	fRec102 = &fRec102_tmp[4];
		float* 	fRec100 = &fRec100_tmp[4];
		float* 	fRec101 = &fRec101_tmp[4];
		float* 	fYec8 = &fYec8_tmp[4];
		float* 	fRec105 = &fRec105_tmp[4];
		float* 	fRec107 = &fRec107_tmp[4];
		float* 	fRec108 = &fRec108_tmp[4];
		float* 	fRec106 = &fRec106_tmp[4];
		float* 	fRec113 = &fRec113_tmp[4];
		float* 	fRec112 = &fRec112_tmp[4];
		float* 	fRec111 = &fRec111_tmp[4];
		float* 	fRec109 = &fRec109_tmp[4];
		float* 	fRec110 = &fRec110_tmp[4];
		float* 	fYec9 = &fYec9_tmp[4];
		float* 	fRec114 = &fRec114_tmp[4];
		float* 	fRec116 = &fRec116_tmp[4];
		float* 	fRec117 = &fRec117_tmp[4];
		float* 	fRec115 = &fRec115_tmp[4];
		float* 	fRec122 = &fRec122_tmp[4];
		float* 	fRec121 = &fRec121_tmp[4];
		float* 	fRec120 = &fRec120_tmp[4];
		float* 	fRec118 = &fRec118_tmp[4];
		float* 	fRec119 = &fRec119_tmp[4];
		float* 	fYec10 = &fYec10_tmp[4];
		float* 	fRec123 = &fRec123_tmp[4];
		float* 	fRec125 = &fRec125_tmp[4];
		float* 	fRec126 = &fRec126_tmp[4];
		float* 	fRec124 = &fRec124_tmp[4];
		float* 	fRec131 = &fRec131_tmp[4];
		float* 	fRec130 = &fRec130_tmp[4];
		float* 	fRec129 = &fRec129_tmp[4];
		float* 	fRec127 = &fRec127_tmp[4];
		float* 	fRec128 = &fRec128_tmp[4];
		float* 	fYec11 = &fYec11_tmp[4];
		float* 	fRec132 = &fRec132_tmp[4];
		float* 	fRec134 = &fRec134_tmp[4];
		float* 	fRec135 = &fRec135_tmp[4];
		float* 	fRec133 = &fRec133_tmp[4];
		float* 	fRec140 = &fRec140_tmp[4];
		float* 	fRec139 = &fRec139_tmp[4];
		float* 	fRec138 = &fRec138_tmp[4];
		float* 	fRec136 = &fRec136_tmp[4];
		float* 	fRec137 = &fRec137_tmp[4];
		float* 	fYec12 = &fYec12_tmp[4];
		float* 	fRec141 = &fRec141_tmp[4];
		float* 	fRec143 = &fRec143_tmp[4];
		float* 	fRec144 = &fRec144_tmp[4];
		float* 	fRec142 = &fRec142_tmp[4];
		float* 	fRec149 = &fRec149_tmp[4];
		float* 	fRec148 = &fRec148_tmp[4];
		float* 	fRec147 = &fRec147_tmp[4];
		float* 	fRec145 = &fRec145_tmp[4];
		float* 	fRec146 = &fRec146_tmp[4];
		float* 	fYec13 = &fYec13_tmp[4];
		float* 	fRec150 = &fRec150_tmp[4];
		float* 	fRec152 = &fRec152_tmp[4];
		float* 	fRec153 = &fRec153_tmp[4];
		float* 	fRec151 = &fRec151_tmp[4];
		float* 	fRec158 = &fRec158_tmp[4];
		float* 	fRec157 = &fRec157_tmp[4];
		float* 	fRec156 = &fRec156_tmp[4];
		float* 	fRec154 = &fRec154_tmp[4];
		float* 	fRec155 = &fRec155_tmp[4];
		float* 	fYec14 = &fYec14_tmp[4];
		float* 	fRec159 = &fRec159_tmp[4];
		float* 	fRec161 = &fRec161_tmp[4];
		float* 	fRec162 = &fRec162_tmp[4];
		float* 	fRec160 = &fRec160_tmp[4];
		float* 	fRec167 = &fRec167_tmp[4];
		float* 	fRec166 = &fRec166_tmp[4];
		float* 	fRec165 = &fRec165_tmp[4];
		float* 	fRec163 = &fRec163_tmp[4];
		float* 	fRec164 = &fRec164_tmp[4];
		float* 	fYec15 = &fYec15_tmp[4];
		float* 	fRec168 = &fRec168_tmp[4];
		float* 	fRec170 = &fRec170_tmp[4];
		float* 	fRec171 = &fRec171_tmp[4];
		float* 	fRec169 = &fRec169_tmp[4];
		for (int index = 0; index < fullcount; index += 1024) {
			int count = min(1024, fullcount-index);
			FAUSTFLOAT* input0 = &input[0][index];
			FAUSTFLOAT* output0 = &output[0][index];
			FAUSTFLOAT* output1 = &output[1][index];
			// SECTION : 1
			// LOOP 0xa13a408
			// pre processing
			for (int i=0; i<4; i++) fYec0_tmp[i]=fYec0_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec0[i] = (float)input0[i];
			}
			// post processing
			for (int i=0; i<4; i++) fYec0_perm[i]=fYec0_tmp[count+i];
			
			// SECTION : 2
			// LOOP 0xa146fc8
			// pre processing
			for (int i=0; i<4; i++) fRec24_tmp[i]=fRec24_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec24[i] = ((float)input0[i] + (0.161758f * fRec24[i-2]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec24_perm[i]=fRec24_tmp[count+i];
			
			// LOOP 0xa1499b8
			// pre processing
			for (int i=0; i<4; i++) fRec28_tmp[i]=fRec28_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec28[i] = (fYec0[i-1] + (0.479401f * fRec28[i-2]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec28_perm[i]=fRec28_tmp[count+i];
			
			// SECTION : 3
			// LOOP 0xa146da8
			// pre processing
			for (int i=0; i<4; i++) fRec23_tmp[i]=fRec23_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec23[i] = (((0.161758f * fRec24[i]) + (0.733029f * fRec23[i-2])) - fRec24[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec23_perm[i]=fRec23_tmp[count+i];
			
			// LOOP 0xa149798
			// pre processing
			for (int i=0; i<4; i++) fRec27_tmp[i]=fRec27_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec27[i] = (((0.479401f * fRec28[i]) + (0.876218f * fRec27[i-2])) - fRec28[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec27_perm[i]=fRec27_tmp[count+i];
			
			// SECTION : 4
			// LOOP 0xa139e98
			// pre processing
			for (int i=0; i<4; i++) fRec8_tmp[i]=fRec8_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec8[i] = (fConst4 * (((float)input0[i] - fYec0[i-1]) + (fConst3 * fRec8[i-1])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec8_perm[i]=fRec8_tmp[count+i];
			
			// LOOP 0xa146b88
			// pre processing
			for (int i=0; i<4; i++) fRec22_tmp[i]=fRec22_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec22[i] = (((0.733029f * fRec23[i]) + (0.94535f * fRec22[i-2])) - fRec23[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec22_perm[i]=fRec22_tmp[count+i];
			
			// LOOP 0xa149578
			// pre processing
			for (int i=0; i<4; i++) fRec26_tmp[i]=fRec26_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec26[i] = (((0.876218f * fRec27[i]) + (0.976599f * fRec26[i-2])) - fRec27[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec26_perm[i]=fRec26_tmp[count+i];
			
			// SECTION : 5
			// LOOP 0xa1397c8
			// pre processing
			for (int i=0; i<4; i++) fRec7_tmp[i]=fRec7_perm[i];
			for (int i=0; i<4; i++) iRec9_tmp[i]=iRec9_perm[i];
			for (int i=0; i<4; i++) iRec6_tmp[i]=iRec6_perm[i];
			for (int i=0; i<4; i++) iRec5_tmp[i]=iRec5_perm[i];
			for (int i=0; i<4; i++) fRec4_tmp[i]=fRec4_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec1[i] = (1.0f / tanf((fConst5 * max(123.4708f, fRec4[i-1]))));
				fZec2[i] = (1 + fZec1[i]);
				fRec7[i] = ((fRec7[i-1] * (0 - ((1 - fZec1[i]) / fZec2[i]))) + ((fRec8[i] + fRec8[i-1]) / fZec2[i]));
				iZec3[i] = ((fRec7[i-1] < 0) & (fRec7[i] >= 0));
				iRec9[i] = (iZec3[i] + (iRec9[i-1] % 8));
				iZec4[i] = (1 + iRec6[i-1]);
				iRec6[i] = (iZec4[i] * (1 - (iZec3[i] & (iRec9[i] == 8.0f))));
				iZec5[i] = (iRec6[i] == 0);
				iRec5[i] = (((1 - iZec5[i]) * iRec5[i-1]) + (iZec5[i] * iZec4[i]));
				fRec4[i] = min(493.6635f, (iConst0 * ((8.0f / float(max((int)iRec5[i], 1))) - (8.0f * (iRec5[i] == 0)))));
			}
			// post processing
			for (int i=0; i<4; i++) fRec4_perm[i]=fRec4_tmp[count+i];
			for (int i=0; i<4; i++) iRec5_perm[i]=iRec5_tmp[count+i];
			for (int i=0; i<4; i++) iRec6_perm[i]=iRec6_tmp[count+i];
			for (int i=0; i<4; i++) iRec9_perm[i]=iRec9_tmp[count+i];
			for (int i=0; i<4; i++) fRec7_perm[i]=fRec7_tmp[count+i];
			
			// LOOP 0xa13f700
			// pre processing
			fVec0_idx = (fVec0_idx+fVec0_idx_save)&524287;
			// exec code
			for (int i=0; i<count; i++) {
				fVec0[(fVec0_idx+i)&524287] = fSlow1;
			}
			// post processing
			fVec0_idx_save = count;
			
			// LOOP 0xa140180
			// pre processing
			fVec1_idx = (fVec1_idx+fVec1_idx_save)&4095;
			// exec code
			for (int i=0; i<count; i++) {
				fVec1[(fVec1_idx+i)&4095] = fSlow2;
			}
			// post processing
			fVec1_idx_save = count;
			
			// LOOP 0xa146968
			// pre processing
			for (int i=0; i<4; i++) fRec21_tmp[i]=fRec21_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec21[i] = (((0.94535f * fRec22[i]) + (0.990598f * fRec21[i-2])) - fRec22[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec21_perm[i]=fRec21_tmp[count+i];
			
			// LOOP 0xa1493b0
			// pre processing
			for (int i=0; i<4; i++) fRec25_tmp[i]=fRec25_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec25[i] = (((0.976599f * fRec26[i]) + (0.9975f * fRec25[i-2])) - fRec26[i-2]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec25_perm[i]=fRec25_tmp[count+i];
			
			// SECTION : 6
			// LOOP 0xa1396a0
			// pre processing
			for (int i=0; i<4; i++) fRec3_tmp[i]=fRec3_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec3[i] = ((0.9984f * fRec3[i-1]) + (0.0016000000000000458f * ((int(((fVec0[(fVec0_idx+i-iConst6)&524287] == fSlow1) & (fVec1[(fVec1_idx+i-1060)&4095] == fSlow2))))?max(61.7354f, fRec4[i]):fSlow1)));
			}
			// post processing
			for (int i=0; i<4; i++) fRec3_perm[i]=fRec3_tmp[count+i];
			
			// LOOP 0xa14bd58
			// pre processing
			for (int i=0; i<4; i++) fRec29_tmp[i]=fRec29_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec29[i] = (fSlow6 + (0.999f * fRec29[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec29_perm[i]=fRec29_tmp[count+i];
			
			// LOOP 0xa14d528
			// exec code
			for (int i=0; i<count; i++) {
				fZec7[i] = ((0.9975f * fRec25[i]) - fRec25[i-2]);
			}
			
			// LOOP 0xa14da08
			// exec code
			for (int i=0; i<count; i++) {
				fZec8[i] = ((0.990598f * fRec21[i]) - fRec21[i-2]);
			}
			
			// SECTION : 7
			// LOOP 0xa141f60
			// pre processing
			for (int i=0; i<4; i++) fRec10_tmp[i]=fRec10_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec10[i] = fmodf((1.0f + fRec10[i-1]),(fConst8 / fRec3[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec10_perm[i]=fRec10_tmp[count+i];
			
			// LOOP 0xa145720
			// pre processing
			for (int i=0; i<4; i++) fRec19_tmp[i]=fRec19_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec19[i] = (fSlow5 + (0.999f * fRec19[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec19_perm[i]=fRec19_tmp[count+i];
			
			// LOOP 0xa14ccd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec6[i] = expf((0 - (fConst9 / fRec29[i])));
			}
			
			// LOOP 0xa14d3b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec9[i] = fabsf(min((float)100, max(1e-05f, sqrtf((faustpower<2>(fZec8[i]) + faustpower<2>(fZec7[i]))))));
			}
			
			// SECTION : 8
			// LOOP 0xa146740
			// pre processing
			for (int i=0; i<4; i++) fRec20_tmp[i]=fRec20_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec20[i] = ((max(fZec9[i], fRec20[i-1]) * fZec6[i]) + (fZec9[i] * (1.0f - fZec6[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec20_perm[i]=fRec20_tmp[count+i];
			
			// LOOP 0xa14ee78
			// exec code
			for (int i=0; i<count; i++) {
				fZec10[i] = expf((0 - (fConst9 / fRec19[i])));
			}
			
			// LOOP 0xa14fbd0
			// pre processing
			for (int i=0; i<4; i++) fRec30_tmp[i]=fRec30_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec30[i] = (fSlow7 + (0.999f * fRec30[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec30_perm[i]=fRec30_tmp[count+i];
			
			// LOOP 0xa150b98
			// exec code
			for (int i=0; i<count; i++) {
				fZec11[i] = tanf((fConst5 * min(fConst10, (0.7071067811865476f * fRec3[i]))));
			}
			
			// LOOP 0xa1593a8
			// pre processing
			for (int i=0; i<4; i++) fRec32_tmp[i]=fRec32_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec32[i] = (fSlow8 + (0.999f * fRec32[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec32_perm[i]=fRec32_tmp[count+i];
			
			// LOOP 0xa15a7a8
			// exec code
			for (int i=0; i<count; i++) {
				fZec18[i] = (fSlow10 * (fRec3[i] * fRec10[i]));
			}
			
			// LOOP 0xa1601e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec23[i] = tanf((fConst5 * min(fConst10, (1.414213562373095f * fRec3[i]))));
			}
			
			// LOOP 0xa166e70
			// pre processing
			for (int i=0; i<4; i++) fRec42_tmp[i]=fRec42_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec42[i] = (fSlow11 + (0.999f * fRec42[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec42_perm[i]=fRec42_tmp[count+i];
			
			// LOOP 0xa16c528
			// exec code
			for (int i=0; i<count; i++) {
				fZec30[i] = tanf((fConst5 * min(fConst10, (2.8284271247461894f * fRec3[i]))));
			}
			
			// LOOP 0xa176d48
			// exec code
			for (int i=0; i<count; i++) {
				fZec36[i] = tanf((fConst5 * min(fConst10, (5.656854249492377f * fRec3[i]))));
			}
			
			// LOOP 0xa181540
			// exec code
			for (int i=0; i<count; i++) {
				fZec42[i] = tanf((fConst5 * min(fConst10, (11.313708498984754f * fRec3[i]))));
			}
			
			// LOOP 0xa18be20
			// exec code
			for (int i=0; i<count; i++) {
				fZec48[i] = tanf((fConst5 * min(fConst10, (22.6274169979695f * fRec3[i]))));
			}
			
			// LOOP 0xa196630
			// exec code
			for (int i=0; i<count; i++) {
				fZec54[i] = tanf((fConst5 * min(fConst10, (45.254833995938995f * fRec3[i]))));
			}
			
			// LOOP 0xa1a0ee0
			// exec code
			for (int i=0; i<count; i++) {
				fZec60[i] = tanf((fConst5 * min(fConst10, (90.50966799187798f * fRec3[i]))));
			}
			
			// LOOP 0xa1ab7f8
			// exec code
			for (int i=0; i<count; i++) {
				fZec66[i] = tanf((fConst5 * min(fConst10, fRec3[i])));
			}
			
			// LOOP 0xa1b5e58
			// exec code
			for (int i=0; i<count; i++) {
				fZec72[i] = tanf((fConst5 * min(fConst10, (1.9999999999999996f * fRec3[i]))));
			}
			
			// LOOP 0xa1c07f8
			// exec code
			for (int i=0; i<count; i++) {
				fZec78[i] = tanf((fConst5 * min(fConst10, (3.9999999999999987f * fRec3[i]))));
			}
			
			// LOOP 0xa1cb258
			// exec code
			for (int i=0; i<count; i++) {
				fZec84[i] = tanf((fConst5 * min(fConst10, (7.9999999999999964f * fRec3[i]))));
			}
			
			// LOOP 0xa1d5c80
			// exec code
			for (int i=0; i<count; i++) {
				fZec90[i] = tanf((fConst5 * min(fConst10, (15.999999999999988f * fRec3[i]))));
			}
			
			// LOOP 0xa1e06a8
			// exec code
			for (int i=0; i<count; i++) {
				fZec96[i] = tanf((fConst5 * min(fConst10, (31.999999999999968f * fRec3[i]))));
			}
			
			// LOOP 0xa1eb0e8
			// exec code
			for (int i=0; i<count; i++) {
				fZec102[i] = tanf((fConst5 * min(fConst10, (63.99999999999993f * fRec3[i]))));
			}
			
			// LOOP 0xa1f5bc8
			// exec code
			for (int i=0; i<count; i++) {
				fZec108[i] = tanf((fConst5 * min(fConst10, (127.99999999999984f * fRec3[i]))));
			}
			
			// SECTION : 9
			// LOOP 0xa1443f0
			// pre processing
			for (int i=0; i<4; i++) fRec17_tmp[i]=fRec17_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec17[i] = (fSlow4 + (0.999f * fRec17[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec17_perm[i]=fRec17_tmp[count+i];
			
			// LOOP 0xa145518
			// pre processing
			for (int i=0; i<4; i++) fRec18_tmp[i]=fRec18_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec18[i] = ((fRec18[i-1] * fZec10[i]) + (fRec20[i] * (1.0f - fZec10[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec18_perm[i]=fRec18_tmp[count+i];
			
			// LOOP 0xa1512f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec12[i] = (1.0f / fZec11[i]);
			}
			
			// LOOP 0xa153300
			// exec code
			for (int i=0; i<count; i++) {
				fZec14[i] = powf(10,(0.05f * fRec30[i]));
			}
			
			// LOOP 0xa15a730
			// exec code
			for (int i=0; i<count; i++) {
				fZec19[i] = floorf(fZec18[i]);
			}
			
			// LOOP 0xa160910
			// exec code
			for (int i=0; i<count; i++) {
				fZec24[i] = (1.0f / fZec23[i]);
			}
			
			// LOOP 0xa168140
			// exec code
			for (int i=0; i<count; i++) {
				fZec28[i] = powf((fRec42[i] / fRec32[i]),0.06666666666666667f);
			}
			
			// LOOP 0xa16cb98
			// exec code
			for (int i=0; i<count; i++) {
				fZec31[i] = (1.0f / fZec30[i]);
			}
			
			// LOOP 0xa1773e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec37[i] = (1.0f / fZec36[i]);
			}
			
			// LOOP 0xa181c70
			// exec code
			for (int i=0; i<count; i++) {
				fZec43[i] = (1.0f / fZec42[i]);
			}
			
			// LOOP 0xa18c4b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec49[i] = (1.0f / fZec48[i]);
			}
			
			// LOOP 0xa196ca0
			// exec code
			for (int i=0; i<count; i++) {
				fZec55[i] = (1.0f / fZec54[i]);
			}
			
			// LOOP 0xa1a1530
			// exec code
			for (int i=0; i<count; i++) {
				fZec61[i] = (1.0f / fZec60[i]);
			}
			
			// LOOP 0xa1abca8
			// exec code
			for (int i=0; i<count; i++) {
				fZec67[i] = (1.0f / fZec66[i]);
			}
			
			// LOOP 0xa1b64c8
			// exec code
			for (int i=0; i<count; i++) {
				fZec73[i] = (1.0f / fZec72[i]);
			}
			
			// LOOP 0xa1c0e68
			// exec code
			for (int i=0; i<count; i++) {
				fZec79[i] = (1.0f / fZec78[i]);
			}
			
			// LOOP 0xa1cb8b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec85[i] = (1.0f / fZec84[i]);
			}
			
			// LOOP 0xa1d62d8
			// exec code
			for (int i=0; i<count; i++) {
				fZec91[i] = (1.0f / fZec90[i]);
			}
			
			// LOOP 0xa1e0d00
			// exec code
			for (int i=0; i<count; i++) {
				fZec97[i] = (1.0f / fZec96[i]);
			}
			
			// LOOP 0xa1eb748
			// exec code
			for (int i=0; i<count; i++) {
				fZec103[i] = (1.0f / fZec102[i]);
			}
			
			// LOOP 0xa1f6288
			// exec code
			for (int i=0; i<count; i++) {
				fZec109[i] = (1.0f / fZec108[i]);
			}
			
			// SECTION : 10
			// LOOP 0xa150b20
			// exec code
			for (int i=0; i<count; i++) {
				fZec13[i] = (1 + ((0.2857142857142857f + fZec12[i]) / fZec11[i]));
			}
			
			// LOOP 0xa152820
			// exec code
			for (int i=0; i<count; i++) {
				fZec15[i] = (0.7071067811865476f * ((powf(min((float)1, max(1e-07f, sinf((1.5707963267948966f * (fRec17[i] * min((1.0f / fRec17[i]), fRec18[i])))))),(logf(fZec14[i]) / logf(sinf((1.5707963267948966f * (fRec17[i] * fZec14[i])))))) * (fZec8[i] + fZec7[i])) / fRec18[i]));
			}
			
			// LOOP 0xa15a6b8
			// exec code
			for (int i=0; i<count; i++) {
				fZec20[i] = (fZec18[i] - fZec19[i]);
			}
			
			// LOOP 0xa160168
			// exec code
			for (int i=0; i<count; i++) {
				fZec25[i] = (1 + ((0.2857142857142857f + fZec24[i]) / fZec23[i]));
			}
			
			// LOOP 0xa1680c8
			// pre processing
			for (int i=0; i<4; i++) fYec1_tmp[i]=fYec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec1[i] = (fRec32[i] * faustpower<2>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec1_perm[i]=fYec1_tmp[count+i];
			
			// LOOP 0xa16c4b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec32[i] = (1 + ((0.2857142857142857f + fZec31[i]) / fZec30[i]));
			}
			
			// LOOP 0xa173188
			// pre processing
			for (int i=0; i<4; i++) fYec2_tmp[i]=fYec2_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec2[i] = (fRec32[i] * faustpower<4>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec2_perm[i]=fYec2_tmp[count+i];
			
			// LOOP 0xa176cd0
			// exec code
			for (int i=0; i<count; i++) {
				fZec38[i] = (1 + ((0.2857142857142857f + fZec37[i]) / fZec36[i]));
			}
			
			// LOOP 0xa17d9a0
			// pre processing
			for (int i=0; i<4; i++) fYec3_tmp[i]=fYec3_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec3[i] = (fRec32[i] * faustpower<6>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec3_perm[i]=fYec3_tmp[count+i];
			
			// LOOP 0xa1814c8
			// exec code
			for (int i=0; i<count; i++) {
				fZec44[i] = (1 + ((0.2857142857142857f + fZec43[i]) / fZec42[i]));
			}
			
			// LOOP 0xa188260
			// pre processing
			for (int i=0; i<4; i++) fYec4_tmp[i]=fYec4_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec4[i] = (fRec32[i] * faustpower<8>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec4_perm[i]=fYec4_tmp[count+i];
			
			// LOOP 0xa18bda8
			// exec code
			for (int i=0; i<count; i++) {
				fZec50[i] = (1 + ((0.2857142857142857f + fZec49[i]) / fZec48[i]));
			}
			
			// LOOP 0xa192a70
			// pre processing
			for (int i=0; i<4; i++) fYec5_tmp[i]=fYec5_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec5[i] = (fRec32[i] * faustpower<10>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec5_perm[i]=fYec5_tmp[count+i];
			
			// LOOP 0xa1965b8
			// exec code
			for (int i=0; i<count; i++) {
				fZec56[i] = (1 + ((0.2857142857142857f + fZec55[i]) / fZec54[i]));
			}
			
			// LOOP 0xa19d290
			// pre processing
			for (int i=0; i<4; i++) fYec6_tmp[i]=fYec6_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec6[i] = (fRec32[i] * faustpower<12>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec6_perm[i]=fYec6_tmp[count+i];
			
			// LOOP 0xa1a0e68
			// exec code
			for (int i=0; i<count; i++) {
				fZec62[i] = (1 + ((0.2857142857142857f + fZec61[i]) / fZec60[i]));
			}
			
			// LOOP 0xa1a7b48
			// pre processing
			for (int i=0; i<4; i++) fYec7_tmp[i]=fYec7_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec7[i] = (fRec32[i] * faustpower<14>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec7_perm[i]=fYec7_tmp[count+i];
			
			// LOOP 0xa1ab780
			// exec code
			for (int i=0; i<count; i++) {
				fZec68[i] = (1 + ((0.2857142857142857f + fZec67[i]) / fZec66[i]));
			}
			
			// LOOP 0xa1b2328
			// pre processing
			for (int i=0; i<4; i++) fYec8_tmp[i]=fYec8_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec8[i] = (fRec32[i] * fZec28[i]);
			}
			// post processing
			for (int i=0; i<4; i++) fYec8_perm[i]=fYec8_tmp[count+i];
			
			// LOOP 0xa1b5de0
			// exec code
			for (int i=0; i<count; i++) {
				fZec74[i] = (1 + ((0.2857142857142857f + fZec73[i]) / fZec72[i]));
			}
			
			// LOOP 0xa1bcb70
			// pre processing
			for (int i=0; i<4; i++) fYec9_tmp[i]=fYec9_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec9[i] = (fRec32[i] * faustpower<3>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec9_perm[i]=fYec9_tmp[count+i];
			
			// LOOP 0xa1c0780
			// exec code
			for (int i=0; i<count; i++) {
				fZec80[i] = (1 + ((0.2857142857142857f + fZec79[i]) / fZec78[i]));
			}
			
			// LOOP 0xa1c7530
			// pre processing
			for (int i=0; i<4; i++) fYec10_tmp[i]=fYec10_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec10[i] = (fRec32[i] * faustpower<5>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec10_perm[i]=fYec10_tmp[count+i];
			
			// LOOP 0xa1cb1e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec86[i] = (1 + ((0.2857142857142857f + fZec85[i]) / fZec84[i]));
			}
			
			// LOOP 0xa1d1f58
			// pre processing
			for (int i=0; i<4; i++) fYec11_tmp[i]=fYec11_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec11[i] = (fRec32[i] * faustpower<7>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec11_perm[i]=fYec11_tmp[count+i];
			
			// LOOP 0xa1d5c08
			// exec code
			for (int i=0; i<count; i++) {
				fZec92[i] = (1 + ((0.2857142857142857f + fZec91[i]) / fZec90[i]));
			}
			
			// LOOP 0xa1dc980
			// pre processing
			for (int i=0; i<4; i++) fYec12_tmp[i]=fYec12_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec12[i] = (fRec32[i] * faustpower<9>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec12_perm[i]=fYec12_tmp[count+i];
			
			// LOOP 0xa1e0630
			// exec code
			for (int i=0; i<count; i++) {
				fZec98[i] = (1 + ((0.2857142857142857f + fZec97[i]) / fZec96[i]));
			}
			
			// LOOP 0xa1e73f8
			// pre processing
			for (int i=0; i<4; i++) fYec13_tmp[i]=fYec13_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec13[i] = (fRec32[i] * faustpower<11>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec13_perm[i]=fYec13_tmp[count+i];
			
			// LOOP 0xa1eb070
			// exec code
			for (int i=0; i<count; i++) {
				fZec104[i] = (1 + ((0.2857142857142857f + fZec103[i]) / fZec102[i]));
			}
			
			// LOOP 0xa1f1ed8
			// pre processing
			for (int i=0; i<4; i++) fYec14_tmp[i]=fYec14_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec14[i] = (fRec32[i] * faustpower<13>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec14_perm[i]=fYec14_tmp[count+i];
			
			// LOOP 0xa1f5b50
			// exec code
			for (int i=0; i<count; i++) {
				fZec110[i] = (1 + ((0.2857142857142857f + fZec109[i]) / fZec108[i]));
			}
			
			// LOOP 0xa1fc9b8
			// pre processing
			for (int i=0; i<4; i++) fYec15_tmp[i]=fYec15_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fYec15[i] = (fRec32[i] * faustpower<15>(fZec28[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fYec15_perm[i]=fYec15_tmp[count+i];
			
			// SECTION : 11
			// LOOP 0xa144160
			// pre processing
			for (int i=0; i<4; i++) fRec16_tmp[i]=fRec16_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec16[i] = (fZec15[i] - (((fRec16[i-2] * (1 + ((fZec12[i] - 0.2857142857142857f) / fZec11[i]))) + (2 * (fRec16[i-1] * (1 - (1.0f / faustpower<2>(fZec11[i])))))) / fZec13[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec16_perm[i]=fRec16_tmp[count+i];
			
			// LOOP 0xa15a640
			// exec code
			for (int i=0; i<count; i++) {
				iZec21[i] = int((fZec20[i] != 0));
			}
			
			// LOOP 0xa15d6b8
			// pre processing
			for (int i=0; i<4; i++) fRec34_tmp[i]=fRec34_perm[i];
			for (int i=0; i<4; i++) fRec35_tmp[i]=fRec35_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec22[i] = ((int((fRec32[i] != fRec32[i-1])))?fConst17:(fRec34[i-1] - 1));
				fRec34[i] = fZec22[i];
				fRec35[i] = ((int((fZec22[i] <= 0)))?fRec32[i]:(fRec35[i-1] + ((fRec32[i] - fRec35[i-1]) / fZec22[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec35_perm[i]=fRec35_tmp[count+i];
			for (int i=0; i<4; i++) fRec34_perm[i]=fRec34_tmp[count+i];
			
			// LOOP 0xa15ff20
			// pre processing
			for (int i=0; i<4; i++) fRec40_tmp[i]=fRec40_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec40[i] = (fZec15[i] - (((fRec40[i-2] * (1 + ((fZec24[i] - 0.2857142857142857f) / fZec23[i]))) + (2 * (fRec40[i-1] * (1 - (1.0f / faustpower<2>(fZec23[i])))))) / fZec25[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec40_perm[i]=fRec40_tmp[count+i];
			
			// LOOP 0xa169828
			// pre processing
			for (int i=0; i<4; i++) fRec44_tmp[i]=fRec44_perm[i];
			for (int i=0; i<4; i++) fRec45_tmp[i]=fRec45_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec29[i] = ((int((fYec1[i] != fYec1[i-1])))?fConst17:(fRec44[i-1] - 1));
				fRec44[i] = fZec29[i];
				fRec45[i] = ((int((fZec29[i] <= 0)))?fYec1[i]:(fRec45[i-1] + ((fYec1[i] - fRec45[i-1]) / fZec29[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec45_perm[i]=fRec45_tmp[count+i];
			for (int i=0; i<4; i++) fRec44_perm[i]=fRec44_tmp[count+i];
			
			// LOOP 0xa16c2b0
			// pre processing
			for (int i=0; i<4; i++) fRec50_tmp[i]=fRec50_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec50[i] = (fZec15[i] - (((fRec50[i-2] * (1 + ((fZec31[i] - 0.2857142857142857f) / fZec30[i]))) + (2 * (fRec50[i-1] * (1 - (1.0f / faustpower<2>(fZec30[i])))))) / fZec32[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec50_perm[i]=fRec50_tmp[count+i];
			
			// LOOP 0xa174120
			// pre processing
			for (int i=0; i<4; i++) fRec53_tmp[i]=fRec53_perm[i];
			for (int i=0; i<4; i++) fRec54_tmp[i]=fRec54_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec35[i] = ((int((fYec2[i] != fYec2[i-1])))?fConst17:(fRec53[i-1] - 1));
				fRec53[i] = fZec35[i];
				fRec54[i] = ((int((fZec35[i] <= 0)))?fYec2[i]:(fRec54[i-1] + ((fYec2[i] - fRec54[i-1]) / fZec35[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec54_perm[i]=fRec54_tmp[count+i];
			for (int i=0; i<4; i++) fRec53_perm[i]=fRec53_tmp[count+i];
			
			// LOOP 0xa176ad0
			// pre processing
			for (int i=0; i<4; i++) fRec59_tmp[i]=fRec59_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec59[i] = (fZec15[i] - (((fRec59[i-2] * (1 + ((fZec37[i] - 0.2857142857142857f) / fZec36[i]))) + (2 * (fRec59[i-1] * (1 - (1.0f / faustpower<2>(fZec36[i])))))) / fZec38[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec59_perm[i]=fRec59_tmp[count+i];
			
			// LOOP 0xa17e9b0
			// pre processing
			for (int i=0; i<4; i++) fRec62_tmp[i]=fRec62_perm[i];
			for (int i=0; i<4; i++) fRec63_tmp[i]=fRec63_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec41[i] = ((int((fYec3[i] != fYec3[i-1])))?fConst17:(fRec62[i-1] - 1));
				fRec62[i] = fZec41[i];
				fRec63[i] = ((int((fZec41[i] <= 0)))?fYec3[i]:(fRec63[i-1] + ((fYec3[i] - fRec63[i-1]) / fZec41[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec63_perm[i]=fRec63_tmp[count+i];
			for (int i=0; i<4; i++) fRec62_perm[i]=fRec62_tmp[count+i];
			
			// LOOP 0xa181280
			// pre processing
			for (int i=0; i<4; i++) fRec68_tmp[i]=fRec68_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec68[i] = (fZec15[i] - (((fRec68[i-2] * (1 + ((fZec43[i] - 0.2857142857142857f) / fZec42[i]))) + (2 * (fRec68[i-1] * (1 - (1.0f / faustpower<2>(fZec42[i])))))) / fZec44[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec68_perm[i]=fRec68_tmp[count+i];
			
			// LOOP 0xa1891f8
			// pre processing
			for (int i=0; i<4; i++) fRec71_tmp[i]=fRec71_perm[i];
			for (int i=0; i<4; i++) fRec72_tmp[i]=fRec72_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec47[i] = ((int((fYec4[i] != fYec4[i-1])))?fConst17:(fRec71[i-1] - 1));
				fRec71[i] = fZec47[i];
				fRec72[i] = ((int((fZec47[i] <= 0)))?fYec4[i]:(fRec72[i-1] + ((fYec4[i] - fRec72[i-1]) / fZec47[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec72_perm[i]=fRec72_tmp[count+i];
			for (int i=0; i<4; i++) fRec71_perm[i]=fRec71_tmp[count+i];
			
			// LOOP 0xa18bba8
			// pre processing
			for (int i=0; i<4; i++) fRec77_tmp[i]=fRec77_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec77[i] = (fZec15[i] - (((fRec77[i-2] * (1 + ((fZec49[i] - 0.2857142857142857f) / fZec48[i]))) + (2 * (fRec77[i-1] * (1 - (1.0f / faustpower<2>(fZec48[i])))))) / fZec50[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec77_perm[i]=fRec77_tmp[count+i];
			
			// LOOP 0xa193a08
			// pre processing
			for (int i=0; i<4; i++) fRec80_tmp[i]=fRec80_perm[i];
			for (int i=0; i<4; i++) fRec81_tmp[i]=fRec81_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec53[i] = ((int((fYec5[i] != fYec5[i-1])))?fConst17:(fRec80[i-1] - 1));
				fRec80[i] = fZec53[i];
				fRec81[i] = ((int((fZec53[i] <= 0)))?fYec5[i]:(fRec81[i-1] + ((fYec5[i] - fRec81[i-1]) / fZec53[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec81_perm[i]=fRec81_tmp[count+i];
			for (int i=0; i<4; i++) fRec80_perm[i]=fRec80_tmp[count+i];
			
			// LOOP 0xa1963b8
			// pre processing
			for (int i=0; i<4; i++) fRec86_tmp[i]=fRec86_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec86[i] = (fZec15[i] - (((fRec86[i-2] * (1 + ((fZec55[i] - 0.2857142857142857f) / fZec54[i]))) + (2 * (fRec86[i-1] * (1 - (1.0f / faustpower<2>(fZec54[i])))))) / fZec56[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec86_perm[i]=fRec86_tmp[count+i];
			
			// LOOP 0xa19e2a0
			// pre processing
			for (int i=0; i<4; i++) fRec89_tmp[i]=fRec89_perm[i];
			for (int i=0; i<4; i++) fRec90_tmp[i]=fRec90_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec59[i] = ((int((fYec6[i] != fYec6[i-1])))?fConst17:(fRec89[i-1] - 1));
				fRec89[i] = fZec59[i];
				fRec90[i] = ((int((fZec59[i] <= 0)))?fYec6[i]:(fRec90[i-1] + ((fYec6[i] - fRec90[i-1]) / fZec59[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec90_perm[i]=fRec90_tmp[count+i];
			for (int i=0; i<4; i++) fRec89_perm[i]=fRec89_tmp[count+i];
			
			// LOOP 0xa1a0c68
			// pre processing
			for (int i=0; i<4; i++) fRec95_tmp[i]=fRec95_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec95[i] = (fZec15[i] - (((fRec95[i-2] * (1 + ((fZec61[i] - 0.2857142857142857f) / fZec60[i]))) + (2 * (fRec95[i-1] * (1 - (1.0f / faustpower<2>(fZec60[i])))))) / fZec62[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec95_perm[i]=fRec95_tmp[count+i];
			
			// LOOP 0xa1a8b58
			// pre processing
			for (int i=0; i<4; i++) fRec98_tmp[i]=fRec98_perm[i];
			for (int i=0; i<4; i++) fRec99_tmp[i]=fRec99_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec65[i] = ((int((fYec7[i] != fYec7[i-1])))?fConst17:(fRec98[i-1] - 1));
				fRec98[i] = fZec65[i];
				fRec99[i] = ((int((fZec65[i] <= 0)))?fYec7[i]:(fRec99[i-1] + ((fYec7[i] - fRec99[i-1]) / fZec65[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec99_perm[i]=fRec99_tmp[count+i];
			for (int i=0; i<4; i++) fRec98_perm[i]=fRec98_tmp[count+i];
			
			// LOOP 0xa1ab590
			// pre processing
			for (int i=0; i<4; i++) fRec104_tmp[i]=fRec104_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec104[i] = (fZec15[i] - (((fRec104[i-2] * (1 + ((fZec67[i] - 0.2857142857142857f) / fZec66[i]))) + (2 * (fRec104[i-1] * (1 - (1.0f / faustpower<2>(fZec66[i])))))) / fZec68[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec104_perm[i]=fRec104_tmp[count+i];
			
			// LOOP 0xa1b31e0
			// pre processing
			for (int i=0; i<4; i++) fRec107_tmp[i]=fRec107_perm[i];
			for (int i=0; i<4; i++) fRec108_tmp[i]=fRec108_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec71[i] = ((int((fYec8[i] != fYec8[i-1])))?fConst17:(fRec107[i-1] - 1));
				fRec107[i] = fZec71[i];
				fRec108[i] = ((int((fZec71[i] <= 0)))?fYec8[i]:(fRec108[i-1] + ((fYec8[i] - fRec108[i-1]) / fZec71[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec108_perm[i]=fRec108_tmp[count+i];
			for (int i=0; i<4; i++) fRec107_perm[i]=fRec107_tmp[count+i];
			
			// LOOP 0xa1b5be0
			// pre processing
			for (int i=0; i<4; i++) fRec113_tmp[i]=fRec113_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec113[i] = (fZec15[i] - (((fRec113[i-2] * (1 + ((fZec73[i] - 0.2857142857142857f) / fZec72[i]))) + (2 * (fRec113[i-1] * (1 - (1.0f / faustpower<2>(fZec72[i])))))) / fZec74[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec113_perm[i]=fRec113_tmp[count+i];
			
			// LOOP 0xa1bdb90
			// pre processing
			for (int i=0; i<4; i++) fRec116_tmp[i]=fRec116_perm[i];
			for (int i=0; i<4; i++) fRec117_tmp[i]=fRec117_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec77[i] = ((int((fYec9[i] != fYec9[i-1])))?fConst17:(fRec116[i-1] - 1));
				fRec116[i] = fZec77[i];
				fRec117[i] = ((int((fZec77[i] <= 0)))?fYec9[i]:(fRec117[i-1] + ((fYec9[i] - fRec117[i-1]) / fZec77[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec117_perm[i]=fRec117_tmp[count+i];
			for (int i=0; i<4; i++) fRec116_perm[i]=fRec116_tmp[count+i];
			
			// LOOP 0xa1c0580
			// pre processing
			for (int i=0; i<4; i++) fRec122_tmp[i]=fRec122_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec122[i] = (fZec15[i] - (((fRec122[i-2] * (1 + ((fZec79[i] - 0.2857142857142857f) / fZec78[i]))) + (2 * (fRec122[i-1] * (1 - (1.0f / faustpower<2>(fZec78[i])))))) / fZec80[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec122_perm[i]=fRec122_tmp[count+i];
			
			// LOOP 0xa1c8550
			// pre processing
			for (int i=0; i<4; i++) fRec125_tmp[i]=fRec125_perm[i];
			for (int i=0; i<4; i++) fRec126_tmp[i]=fRec126_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec83[i] = ((int((fYec10[i] != fYec10[i-1])))?fConst17:(fRec125[i-1] - 1));
				fRec125[i] = fZec83[i];
				fRec126[i] = ((int((fZec83[i] <= 0)))?fYec10[i]:(fRec126[i-1] + ((fYec10[i] - fRec126[i-1]) / fZec83[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec126_perm[i]=fRec126_tmp[count+i];
			for (int i=0; i<4; i++) fRec125_perm[i]=fRec125_tmp[count+i];
			
			// LOOP 0xa1cafe0
			// pre processing
			for (int i=0; i<4; i++) fRec131_tmp[i]=fRec131_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec131[i] = (fZec15[i] - (((fRec131[i-2] * (1 + ((fZec85[i] - 0.2857142857142857f) / fZec84[i]))) + (2 * (fRec131[i-1] * (1 - (1.0f / faustpower<2>(fZec84[i])))))) / fZec86[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec131_perm[i]=fRec131_tmp[count+i];
			
			// LOOP 0xa1d2f78
			// pre processing
			for (int i=0; i<4; i++) fRec134_tmp[i]=fRec134_perm[i];
			for (int i=0; i<4; i++) fRec135_tmp[i]=fRec135_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec89[i] = ((int((fYec11[i] != fYec11[i-1])))?fConst17:(fRec134[i-1] - 1));
				fRec134[i] = fZec89[i];
				fRec135[i] = ((int((fZec89[i] <= 0)))?fYec11[i]:(fRec135[i-1] + ((fYec11[i] - fRec135[i-1]) / fZec89[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec135_perm[i]=fRec135_tmp[count+i];
			for (int i=0; i<4; i++) fRec134_perm[i]=fRec134_tmp[count+i];
			
			// LOOP 0xa1d5a08
			// pre processing
			for (int i=0; i<4; i++) fRec140_tmp[i]=fRec140_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec140[i] = (fZec15[i] - (((fRec140[i-2] * (1 + ((fZec91[i] - 0.2857142857142857f) / fZec90[i]))) + (2 * (fRec140[i-1] * (1 - (1.0f / faustpower<2>(fZec90[i])))))) / fZec92[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec140_perm[i]=fRec140_tmp[count+i];
			
			// LOOP 0xa1dd9a0
			// pre processing
			for (int i=0; i<4; i++) fRec143_tmp[i]=fRec143_perm[i];
			for (int i=0; i<4; i++) fRec144_tmp[i]=fRec144_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec95[i] = ((int((fYec12[i] != fYec12[i-1])))?fConst17:(fRec143[i-1] - 1));
				fRec143[i] = fZec95[i];
				fRec144[i] = ((int((fZec95[i] <= 0)))?fYec12[i]:(fRec144[i-1] + ((fYec12[i] - fRec144[i-1]) / fZec95[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec144_perm[i]=fRec144_tmp[count+i];
			for (int i=0; i<4; i++) fRec143_perm[i]=fRec143_tmp[count+i];
			
			// LOOP 0xa1e0430
			// pre processing
			for (int i=0; i<4; i++) fRec149_tmp[i]=fRec149_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec149[i] = (fZec15[i] - (((fRec149[i-2] * (1 + ((fZec97[i] - 0.2857142857142857f) / fZec96[i]))) + (2 * (fRec149[i-1] * (1 - (1.0f / faustpower<2>(fZec96[i])))))) / fZec98[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec149_perm[i]=fRec149_tmp[count+i];
			
			// LOOP 0xa1e8418
			// pre processing
			for (int i=0; i<4; i++) fRec152_tmp[i]=fRec152_perm[i];
			for (int i=0; i<4; i++) fRec153_tmp[i]=fRec153_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec101[i] = ((int((fYec13[i] != fYec13[i-1])))?fConst17:(fRec152[i-1] - 1));
				fRec152[i] = fZec101[i];
				fRec153[i] = ((int((fZec101[i] <= 0)))?fYec13[i]:(fRec153[i-1] + ((fYec13[i] - fRec153[i-1]) / fZec101[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec153_perm[i]=fRec153_tmp[count+i];
			for (int i=0; i<4; i++) fRec152_perm[i]=fRec152_tmp[count+i];
			
			// LOOP 0xa1eae70
			// pre processing
			for (int i=0; i<4; i++) fRec158_tmp[i]=fRec158_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec158[i] = (fZec15[i] - (((fRec158[i-2] * (1 + ((fZec103[i] - 0.2857142857142857f) / fZec102[i]))) + (2 * (fRec158[i-1] * (1 - (1.0f / faustpower<2>(fZec102[i])))))) / fZec104[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec158_perm[i]=fRec158_tmp[count+i];
			
			// LOOP 0xa1f2ef8
			// pre processing
			for (int i=0; i<4; i++) fRec161_tmp[i]=fRec161_perm[i];
			for (int i=0; i<4; i++) fRec162_tmp[i]=fRec162_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec107[i] = ((int((fYec14[i] != fYec14[i-1])))?fConst17:(fRec161[i-1] - 1));
				fRec161[i] = fZec107[i];
				fRec162[i] = ((int((fZec107[i] <= 0)))?fYec14[i]:(fRec162[i-1] + ((fYec14[i] - fRec162[i-1]) / fZec107[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec162_perm[i]=fRec162_tmp[count+i];
			for (int i=0; i<4; i++) fRec161_perm[i]=fRec161_tmp[count+i];
			
			// LOOP 0xa1f5950
			// pre processing
			for (int i=0; i<4; i++) fRec167_tmp[i]=fRec167_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec167[i] = (fZec15[i] - (((fRec167[i-2] * (1 + ((fZec109[i] - 0.2857142857142857f) / fZec108[i]))) + (2 * (fRec167[i-1] * (1 - (1.0f / faustpower<2>(fZec108[i])))))) / fZec110[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec167_perm[i]=fRec167_tmp[count+i];
			
			// LOOP 0xa1fd9d8
			// pre processing
			for (int i=0; i<4; i++) fRec170_tmp[i]=fRec170_perm[i];
			for (int i=0; i<4; i++) fRec171_tmp[i]=fRec171_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec113[i] = ((int((fYec15[i] != fYec15[i-1])))?fConst17:(fRec170[i-1] - 1));
				fRec170[i] = fZec113[i];
				fRec171[i] = ((int((fZec113[i] <= 0)))?fYec15[i]:(fRec171[i-1] + ((fYec15[i] - fRec171[i-1]) / fZec113[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec171_perm[i]=fRec171_tmp[count+i];
			for (int i=0; i<4; i++) fRec170_perm[i]=fRec170_tmp[count+i];
			
			// SECTION : 12
			// LOOP 0xa1554a0
			// exec code
			for (int i=0; i<count; i++) {
				fZec16[i] = fabsf((((fRec16[i-2] * (0 - fZec12[i])) + (fRec16[i] / fZec11[i])) / fZec13[i]));
			}
			
			// LOOP 0xa15d4f0
			// pre processing
			for (int i=0; i<4; i++) fRec33_tmp[i]=fRec33_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec33[i] = ((iZec21[i])?fRec35[i]:fRec33[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec33_perm[i]=fRec33_tmp[count+i];
			
			// LOOP 0xa162b80
			// exec code
			for (int i=0; i<count; i++) {
				fZec26[i] = fabsf((((fRec40[i-2] * (0 - fZec24[i])) + (fRec40[i] / fZec23[i])) / fZec25[i]));
			}
			
			// LOOP 0xa169660
			// pre processing
			for (int i=0; i<4; i++) fRec43_tmp[i]=fRec43_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec43[i] = ((iZec21[i])?fRec45[i]:fRec43[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec43_perm[i]=fRec43_tmp[count+i];
			
			// LOOP 0xa16ee10
			// exec code
			for (int i=0; i<count; i++) {
				fZec33[i] = fabsf((((fRec50[i-2] * (0 - fZec31[i])) + (fRec50[i] / fZec30[i])) / fZec32[i]));
			}
			
			// LOOP 0xa173f58
			// pre processing
			for (int i=0; i<4; i++) fRec52_tmp[i]=fRec52_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec52[i] = ((iZec21[i])?fRec54[i]:fRec52[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec52_perm[i]=fRec52_tmp[count+i];
			
			// LOOP 0xa179628
			// exec code
			for (int i=0; i<count; i++) {
				fZec39[i] = fabsf((((fRec59[i-2] * (0 - fZec37[i])) + (fRec59[i] / fZec36[i])) / fZec38[i]));
			}
			
			// LOOP 0xa17e7e8
			// pre processing
			for (int i=0; i<4; i++) fRec61_tmp[i]=fRec61_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec61[i] = ((iZec21[i])?fRec63[i]:fRec61[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec61_perm[i]=fRec61_tmp[count+i];
			
			// LOOP 0xa183ee8
			// exec code
			for (int i=0; i<count; i++) {
				fZec45[i] = fabsf((((fRec68[i-2] * (0 - fZec43[i])) + (fRec68[i] / fZec42[i])) / fZec44[i]));
			}
			
			// LOOP 0xa189030
			// pre processing
			for (int i=0; i<4; i++) fRec70_tmp[i]=fRec70_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec70[i] = ((iZec21[i])?fRec72[i]:fRec70[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec70_perm[i]=fRec70_tmp[count+i];
			
			// LOOP 0xa18e6f8
			// exec code
			for (int i=0; i<count; i++) {
				fZec51[i] = fabsf((((fRec77[i-2] * (0 - fZec49[i])) + (fRec77[i] / fZec48[i])) / fZec50[i]));
			}
			
			// LOOP 0xa193840
			// pre processing
			for (int i=0; i<4; i++) fRec79_tmp[i]=fRec79_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec79[i] = ((iZec21[i])?fRec81[i]:fRec79[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec79_perm[i]=fRec79_tmp[count+i];
			
			// LOOP 0xa198f18
			// exec code
			for (int i=0; i<count; i++) {
				fZec57[i] = fabsf((((fRec86[i-2] * (0 - fZec55[i])) + (fRec86[i] / fZec54[i])) / fZec56[i]));
			}
			
			// LOOP 0xa19e0d8
			// pre processing
			for (int i=0; i<4; i++) fRec88_tmp[i]=fRec88_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec88[i] = ((iZec21[i])?fRec90[i]:fRec88[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec88_perm[i]=fRec88_tmp[count+i];
			
			// LOOP 0xa1a37d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec63[i] = fabsf((((fRec95[i-2] * (0 - fZec61[i])) + (fRec95[i] / fZec60[i])) / fZec62[i]));
			}
			
			// LOOP 0xa1a8990
			// pre processing
			for (int i=0; i<4; i++) fRec97_tmp[i]=fRec97_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec97[i] = ((iZec21[i])?fRec99[i]:fRec97[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec97_perm[i]=fRec97_tmp[count+i];
			
			// LOOP 0xa1adf10
			// exec code
			for (int i=0; i<count; i++) {
				fZec69[i] = fabsf((((fRec104[i-2] * (0 - fZec67[i])) + (fRec104[i] / fZec66[i])) / fZec68[i]));
			}
			
			// LOOP 0xa1b3018
			// pre processing
			for (int i=0; i<4; i++) fRec106_tmp[i]=fRec106_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec106[i] = ((iZec21[i])?fRec108[i]:fRec106[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec106_perm[i]=fRec106_tmp[count+i];
			
			// LOOP 0xa1b8748
			// exec code
			for (int i=0; i<count; i++) {
				fZec75[i] = fabsf((((fRec113[i-2] * (0 - fZec73[i])) + (fRec113[i] / fZec72[i])) / fZec74[i]));
			}
			
			// LOOP 0xa1bd9c8
			// pre processing
			for (int i=0; i<4; i++) fRec115_tmp[i]=fRec115_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec115[i] = ((iZec21[i])?fRec117[i]:fRec115[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec115_perm[i]=fRec115_tmp[count+i];
			
			// LOOP 0xa1c30d8
			// exec code
			for (int i=0; i<count; i++) {
				fZec81[i] = fabsf((((fRec122[i-2] * (0 - fZec79[i])) + (fRec122[i] / fZec78[i])) / fZec80[i]));
			}
			
			// LOOP 0xa1c8388
			// pre processing
			for (int i=0; i<4; i++) fRec124_tmp[i]=fRec124_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec124[i] = ((iZec21[i])?fRec126[i]:fRec124[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec124_perm[i]=fRec124_tmp[count+i];
			
			// LOOP 0xa1cdb30
			// exec code
			for (int i=0; i<count; i++) {
				fZec87[i] = fabsf((((fRec131[i-2] * (0 - fZec85[i])) + (fRec131[i] / fZec84[i])) / fZec86[i]));
			}
			
			// LOOP 0xa1d2db0
			// pre processing
			for (int i=0; i<4; i++) fRec133_tmp[i]=fRec133_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec133[i] = ((iZec21[i])?fRec135[i]:fRec133[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec133_perm[i]=fRec133_tmp[count+i];
			
			// LOOP 0xa1d8558
			// exec code
			for (int i=0; i<count; i++) {
				fZec93[i] = fabsf((((fRec140[i-2] * (0 - fZec91[i])) + (fRec140[i] / fZec90[i])) / fZec92[i]));
			}
			
			// LOOP 0xa1dd7d8
			// pre processing
			for (int i=0; i<4; i++) fRec142_tmp[i]=fRec142_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec142[i] = ((iZec21[i])?fRec144[i]:fRec142[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec142_perm[i]=fRec142_tmp[count+i];
			
			// LOOP 0xa1e2f80
			// exec code
			for (int i=0; i<count; i++) {
				fZec99[i] = fabsf((((fRec149[i-2] * (0 - fZec97[i])) + (fRec149[i] / fZec96[i])) / fZec98[i]));
			}
			
			// LOOP 0xa1e8250
			// pre processing
			for (int i=0; i<4; i++) fRec151_tmp[i]=fRec151_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec151[i] = ((iZec21[i])?fRec153[i]:fRec151[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec151_perm[i]=fRec151_tmp[count+i];
			
			// LOOP 0xa1eda58
			// exec code
			for (int i=0; i<count; i++) {
				fZec105[i] = fabsf((((fRec158[i-2] * (0 - fZec103[i])) + (fRec158[i] / fZec102[i])) / fZec104[i]));
			}
			
			// LOOP 0xa1f2d30
			// pre processing
			for (int i=0; i<4; i++) fRec160_tmp[i]=fRec160_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec160[i] = ((iZec21[i])?fRec162[i]:fRec160[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec160_perm[i]=fRec160_tmp[count+i];
			
			// LOOP 0xa1f8538
			// exec code
			for (int i=0; i<count; i++) {
				fZec111[i] = fabsf((((fRec167[i-2] * (0 - fZec109[i])) + (fRec167[i] / fZec108[i])) / fZec110[i]));
			}
			
			// LOOP 0xa1fd810
			// pre processing
			for (int i=0; i<4; i++) fRec169_tmp[i]=fRec169_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec169[i] = ((iZec21[i])?fRec171[i]:fRec169[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec169_perm[i]=fRec169_tmp[count+i];
			
			// SECTION : 13
			// LOOP 0xa143f20
			// pre processing
			for (int i=0; i<4; i++) fRec15_tmp[i]=fRec15_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec15[i] = ((fConst11 * max(fZec16[i], fRec15[i-1])) + (fConst12 * fZec16[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec15_perm[i]=fRec15_tmp[count+i];
			
			// LOOP 0xa15fc78
			// pre processing
			for (int i=0; i<4; i++) fRec39_tmp[i]=fRec39_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec39[i] = ((fConst11 * max(fZec26[i], fRec39[i-1])) + (fConst12 * fZec26[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec39_perm[i]=fRec39_tmp[count+i];
			
			// LOOP 0xa16c050
			// pre processing
			for (int i=0; i<4; i++) fRec49_tmp[i]=fRec49_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec49[i] = ((fConst11 * max(fZec33[i], fRec49[i-1])) + (fConst12 * fZec33[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec49_perm[i]=fRec49_tmp[count+i];
			
			// LOOP 0xa176870
			// pre processing
			for (int i=0; i<4; i++) fRec58_tmp[i]=fRec58_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec58[i] = ((fConst11 * max(fZec39[i], fRec58[i-1])) + (fConst12 * fZec39[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec58_perm[i]=fRec58_tmp[count+i];
			
			// LOOP 0xa180fd8
			// pre processing
			for (int i=0; i<4; i++) fRec67_tmp[i]=fRec67_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec67[i] = ((fConst11 * max(fZec45[i], fRec67[i-1])) + (fConst12 * fZec45[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec67_perm[i]=fRec67_tmp[count+i];
			
			// LOOP 0xa18b948
			// pre processing
			for (int i=0; i<4; i++) fRec76_tmp[i]=fRec76_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec76[i] = ((fConst11 * max(fZec51[i], fRec76[i-1])) + (fConst12 * fZec51[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec76_perm[i]=fRec76_tmp[count+i];
			
			// LOOP 0xa196158
			// pre processing
			for (int i=0; i<4; i++) fRec85_tmp[i]=fRec85_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec85[i] = ((fConst11 * max(fZec57[i], fRec85[i-1])) + (fConst12 * fZec57[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec85_perm[i]=fRec85_tmp[count+i];
			
			// LOOP 0xa1a0a08
			// pre processing
			for (int i=0; i<4; i++) fRec94_tmp[i]=fRec94_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec94[i] = ((fConst11 * max(fZec63[i], fRec94[i-1])) + (fConst12 * fZec63[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec94_perm[i]=fRec94_tmp[count+i];
			
			// LOOP 0xa1ab330
			// pre processing
			for (int i=0; i<4; i++) fRec103_tmp[i]=fRec103_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec103[i] = ((fConst11 * max(fZec69[i], fRec103[i-1])) + (fConst12 * fZec69[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec103_perm[i]=fRec103_tmp[count+i];
			
			// LOOP 0xa1b5980
			// pre processing
			for (int i=0; i<4; i++) fRec112_tmp[i]=fRec112_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec112[i] = ((fConst11 * max(fZec75[i], fRec112[i-1])) + (fConst12 * fZec75[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec112_perm[i]=fRec112_tmp[count+i];
			
			// LOOP 0xa1c0320
			// pre processing
			for (int i=0; i<4; i++) fRec121_tmp[i]=fRec121_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec121[i] = ((fConst11 * max(fZec81[i], fRec121[i-1])) + (fConst12 * fZec81[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec121_perm[i]=fRec121_tmp[count+i];
			
			// LOOP 0xa1cad80
			// pre processing
			for (int i=0; i<4; i++) fRec130_tmp[i]=fRec130_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec130[i] = ((fConst11 * max(fZec87[i], fRec130[i-1])) + (fConst12 * fZec87[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec130_perm[i]=fRec130_tmp[count+i];
			
			// LOOP 0xa1d57a8
			// pre processing
			for (int i=0; i<4; i++) fRec139_tmp[i]=fRec139_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec139[i] = ((fConst11 * max(fZec93[i], fRec139[i-1])) + (fConst12 * fZec93[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec139_perm[i]=fRec139_tmp[count+i];
			
			// LOOP 0xa1e01d0
			// pre processing
			for (int i=0; i<4; i++) fRec148_tmp[i]=fRec148_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec148[i] = ((fConst11 * max(fZec99[i], fRec148[i-1])) + (fConst12 * fZec99[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec148_perm[i]=fRec148_tmp[count+i];
			
			// LOOP 0xa1eac10
			// pre processing
			for (int i=0; i<4; i++) fRec157_tmp[i]=fRec157_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec157[i] = ((fConst11 * max(fZec105[i], fRec157[i-1])) + (fConst12 * fZec105[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec157_perm[i]=fRec157_tmp[count+i];
			
			// LOOP 0xa1f56f0
			// pre processing
			for (int i=0; i<4; i++) fRec166_tmp[i]=fRec166_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec166[i] = ((fConst11 * max(fZec111[i], fRec166[i-1])) + (fConst12 * fZec111[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec166_perm[i]=fRec166_tmp[count+i];
			
			// LOOP 0xa1fffa0
			// exec code
			for (int i=0; i<count; i++) {
				fZec114[i] = fmodf(fRec169[i],1);
			}
			
			// LOOP 0xa201b28
			// exec code
			for (int i=0; i<count; i++) {
				fZec117[i] = fmodf(fRec160[i],1);
			}
			
			// LOOP 0xa203848
			// exec code
			for (int i=0; i<count; i++) {
				fZec120[i] = fmodf(fRec151[i],1);
			}
			
			// LOOP 0xa205528
			// exec code
			for (int i=0; i<count; i++) {
				fZec123[i] = fmodf(fRec142[i],1);
			}
			
			// LOOP 0xa207288
			// exec code
			for (int i=0; i<count; i++) {
				fZec126[i] = fmodf(fRec133[i],1);
			}
			
			// LOOP 0xa208fb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec129[i] = fmodf(fRec124[i],1);
			}
			
			// LOOP 0xa20acc8
			// exec code
			for (int i=0; i<count; i++) {
				fZec132[i] = fmodf(fRec115[i],1);
			}
			
			// LOOP 0xa20c9e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec135[i] = fmodf(fRec106[i],1);
			}
			
			// LOOP 0xa219350
			// exec code
			for (int i=0; i<count; i++) {
				fZec140[i] = fmodf(fRec97[i],1);
			}
			
			// LOOP 0xa21aef0
			// exec code
			for (int i=0; i<count; i++) {
				fZec143[i] = fmodf(fRec88[i],1);
			}
			
			// LOOP 0xa21cab0
			// exec code
			for (int i=0; i<count; i++) {
				fZec146[i] = fmodf(fRec79[i],1);
			}
			
			// LOOP 0xa21e670
			// exec code
			for (int i=0; i<count; i++) {
				fZec149[i] = fmodf(fRec70[i],1);
			}
			
			// LOOP 0xa220240
			// exec code
			for (int i=0; i<count; i++) {
				fZec152[i] = fmodf(fRec61[i],1);
			}
			
			// LOOP 0xa221e10
			// exec code
			for (int i=0; i<count; i++) {
				fZec155[i] = fmodf(fRec52[i],1);
			}
			
			// LOOP 0xa223ad0
			// exec code
			for (int i=0; i<count; i++) {
				fZec158[i] = fmodf(fRec43[i],1);
			}
			
			// LOOP 0xa225748
			// exec code
			for (int i=0; i<count; i++) {
				fZec161[i] = fmodf(fRec33[i],1);
			}
			
			// SECTION : 14
			// LOOP 0xa136ed0
			// pre processing
			for (int i=0; i<4; i++) fRec2_tmp[i]=fRec2_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec2[i] = (fSlow0 + (0.999f * fRec2[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec2_perm[i]=fRec2_tmp[count+i];
			
			// LOOP 0xa143cf0
			// pre processing
			for (int i=0; i<4; i++) fRec14_tmp[i]=fRec14_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec14[i] = ((fConst13 * fRec14[i-1]) + (fConst14 * fRec15[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec14_perm[i]=fRec14_tmp[count+i];
			
			// LOOP 0xa1591b0
			// pre processing
			for (int i=0; i<4; i++) fRec31_tmp[i]=fRec31_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec31[i] = ((iZec21[i])?fRec32[i]:fRec31[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec31_perm[i]=fRec31_tmp[count+i];
			
			// LOOP 0xa15fa18
			// pre processing
			for (int i=0; i<4; i++) fRec38_tmp[i]=fRec38_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec38[i] = ((fConst13 * fRec38[i-1]) + (fConst14 * fRec39[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec38_perm[i]=fRec38_tmp[count+i];
			
			// LOOP 0xa166c68
			// pre processing
			for (int i=0; i<4; i++) fRec41_tmp[i]=fRec41_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec41[i] = ((iZec21[i])?fYec1[i]:fRec41[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec41_perm[i]=fRec41_tmp[count+i];
			
			// LOOP 0xa16be60
			// pre processing
			for (int i=0; i<4; i++) fRec48_tmp[i]=fRec48_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec48[i] = ((fConst13 * fRec48[i-1]) + (fConst14 * fRec49[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec48_perm[i]=fRec48_tmp[count+i];
			
			// LOOP 0xa172ef8
			// pre processing
			for (int i=0; i<4; i++) fRec51_tmp[i]=fRec51_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec51[i] = ((iZec21[i])?fYec2[i]:fRec51[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec51_perm[i]=fRec51_tmp[count+i];
			
			// LOOP 0xa176680
			// pre processing
			for (int i=0; i<4; i++) fRec57_tmp[i]=fRec57_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec57[i] = ((fConst13 * fRec57[i-1]) + (fConst14 * fRec58[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec57_perm[i]=fRec57_tmp[count+i];
			
			// LOOP 0xa17d710
			// pre processing
			for (int i=0; i<4; i++) fRec60_tmp[i]=fRec60_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec60[i] = ((iZec21[i])?fYec3[i]:fRec60[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec60_perm[i]=fRec60_tmp[count+i];
			
			// LOOP 0xa180da0
			// pre processing
			for (int i=0; i<4; i++) fRec66_tmp[i]=fRec66_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec66[i] = ((fConst13 * fRec66[i-1]) + (fConst14 * fRec67[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec66_perm[i]=fRec66_tmp[count+i];
			
			// LOOP 0xa187fd0
			// pre processing
			for (int i=0; i<4; i++) fRec69_tmp[i]=fRec69_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec69[i] = ((iZec21[i])?fYec4[i]:fRec69[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec69_perm[i]=fRec69_tmp[count+i];
			
			// LOOP 0xa18b758
			// pre processing
			for (int i=0; i<4; i++) fRec75_tmp[i]=fRec75_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec75[i] = ((fConst13 * fRec75[i-1]) + (fConst14 * fRec76[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec75_perm[i]=fRec75_tmp[count+i];
			
			// LOOP 0xa1927e0
			// pre processing
			for (int i=0; i<4; i++) fRec78_tmp[i]=fRec78_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec78[i] = ((iZec21[i])?fYec5[i]:fRec78[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec78_perm[i]=fRec78_tmp[count+i];
			
			// LOOP 0xa195f68
			// pre processing
			for (int i=0; i<4; i++) fRec84_tmp[i]=fRec84_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec84[i] = ((fConst13 * fRec84[i-1]) + (fConst14 * fRec85[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec84_perm[i]=fRec84_tmp[count+i];
			
			// LOOP 0xa19d000
			// pre processing
			for (int i=0; i<4; i++) fRec87_tmp[i]=fRec87_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec87[i] = ((iZec21[i])?fYec6[i]:fRec87[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec87_perm[i]=fRec87_tmp[count+i];
			
			// LOOP 0xa1a0818
			// pre processing
			for (int i=0; i<4; i++) fRec93_tmp[i]=fRec93_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec93[i] = ((fConst13 * fRec93[i-1]) + (fConst14 * fRec94[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec93_perm[i]=fRec93_tmp[count+i];
			
			// LOOP 0xa1a78b8
			// pre processing
			for (int i=0; i<4; i++) fRec96_tmp[i]=fRec96_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec96[i] = ((iZec21[i])?fYec7[i]:fRec96[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec96_perm[i]=fRec96_tmp[count+i];
			
			// LOOP 0xa1ab120
			// pre processing
			for (int i=0; i<4; i++) fRec102_tmp[i]=fRec102_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec102[i] = ((fConst13 * fRec102[i-1]) + (fConst14 * fRec103[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec102_perm[i]=fRec102_tmp[count+i];
			
			// LOOP 0xa1b20a8
			// pre processing
			for (int i=0; i<4; i++) fRec105_tmp[i]=fRec105_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec105[i] = ((iZec21[i])?fYec8[i]:fRec105[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec105_perm[i]=fRec105_tmp[count+i];
			
			// LOOP 0xa1b5780
			// pre processing
			for (int i=0; i<4; i++) fRec111_tmp[i]=fRec111_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec111[i] = ((fConst13 * fRec111[i-1]) + (fConst14 * fRec112[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec111_perm[i]=fRec111_tmp[count+i];
			
			// LOOP 0xa1bc8e0
			// pre processing
			for (int i=0; i<4; i++) fRec114_tmp[i]=fRec114_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec114[i] = ((iZec21[i])?fYec9[i]:fRec114[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec114_perm[i]=fRec114_tmp[count+i];
			
			// LOOP 0xa1c0130
			// pre processing
			for (int i=0; i<4; i++) fRec120_tmp[i]=fRec120_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec120[i] = ((fConst13 * fRec120[i-1]) + (fConst14 * fRec121[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec120_perm[i]=fRec120_tmp[count+i];
			
			// LOOP 0xa1c72a0
			// pre processing
			for (int i=0; i<4; i++) fRec123_tmp[i]=fRec123_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec123[i] = ((iZec21[i])?fYec10[i]:fRec123[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec123_perm[i]=fRec123_tmp[count+i];
			
			// LOOP 0xa1cab90
			// pre processing
			for (int i=0; i<4; i++) fRec129_tmp[i]=fRec129_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec129[i] = ((fConst13 * fRec129[i-1]) + (fConst14 * fRec130[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec129_perm[i]=fRec129_tmp[count+i];
			
			// LOOP 0xa1d1cc8
			// pre processing
			for (int i=0; i<4; i++) fRec132_tmp[i]=fRec132_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec132[i] = ((iZec21[i])?fYec11[i]:fRec132[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec132_perm[i]=fRec132_tmp[count+i];
			
			// LOOP 0xa1d55b8
			// pre processing
			for (int i=0; i<4; i++) fRec138_tmp[i]=fRec138_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec138[i] = ((fConst13 * fRec138[i-1]) + (fConst14 * fRec139[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec138_perm[i]=fRec138_tmp[count+i];
			
			// LOOP 0xa1dc6f0
			// pre processing
			for (int i=0; i<4; i++) fRec141_tmp[i]=fRec141_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec141[i] = ((iZec21[i])?fYec12[i]:fRec141[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec141_perm[i]=fRec141_tmp[count+i];
			
			// LOOP 0xa1dffe0
			// pre processing
			for (int i=0; i<4; i++) fRec147_tmp[i]=fRec147_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec147[i] = ((fConst13 * fRec147[i-1]) + (fConst14 * fRec148[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec147_perm[i]=fRec147_tmp[count+i];
			
			// LOOP 0xa1e7168
			// pre processing
			for (int i=0; i<4; i++) fRec150_tmp[i]=fRec150_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec150[i] = ((iZec21[i])?fYec13[i]:fRec150[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec150_perm[i]=fRec150_tmp[count+i];
			
			// LOOP 0xa1eaa20
			// pre processing
			for (int i=0; i<4; i++) fRec156_tmp[i]=fRec156_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec156[i] = ((fConst13 * fRec156[i-1]) + (fConst14 * fRec157[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec156_perm[i]=fRec156_tmp[count+i];
			
			// LOOP 0xa1f1c48
			// pre processing
			for (int i=0; i<4; i++) fRec159_tmp[i]=fRec159_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec159[i] = ((iZec21[i])?fYec14[i]:fRec159[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec159_perm[i]=fRec159_tmp[count+i];
			
			// LOOP 0xa1f5500
			// pre processing
			for (int i=0; i<4; i++) fRec165_tmp[i]=fRec165_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec165[i] = ((fConst13 * fRec165[i-1]) + (fConst14 * fRec166[i]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec165_perm[i]=fRec165_tmp[count+i];
			
			// LOOP 0xa1fc728
			// pre processing
			for (int i=0; i<4; i++) fRec168_tmp[i]=fRec168_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec168[i] = ((iZec21[i])?fYec15[i]:fRec168[i-1]);
			}
			// post processing
			for (int i=0; i<4; i++) fRec168_perm[i]=fRec168_tmp[count+i];
			
			// LOOP 0xa1fff28
			// exec code
			for (int i=0; i<count; i++) {
				fZec115[i] = ((int((fRec169[i] > 0)))?fZec114[i]:(1 + fZec114[i]));
			}
			
			// LOOP 0xa201ab0
			// exec code
			for (int i=0; i<count; i++) {
				fZec118[i] = ((int((fRec160[i] > 0)))?fZec117[i]:(1 + fZec117[i]));
			}
			
			// LOOP 0xa2037d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec121[i] = ((int((fRec151[i] > 0)))?fZec120[i]:(1 + fZec120[i]));
			}
			
			// LOOP 0xa2054b0
			// exec code
			for (int i=0; i<count; i++) {
				fZec124[i] = ((int((fRec142[i] > 0)))?fZec123[i]:(1 + fZec123[i]));
			}
			
			// LOOP 0xa207210
			// exec code
			for (int i=0; i<count; i++) {
				fZec127[i] = ((int((fRec133[i] > 0)))?fZec126[i]:(1 + fZec126[i]));
			}
			
			// LOOP 0xa208f38
			// exec code
			for (int i=0; i<count; i++) {
				fZec130[i] = ((int((fRec124[i] > 0)))?fZec129[i]:(1 + fZec129[i]));
			}
			
			// LOOP 0xa20ac50
			// exec code
			for (int i=0; i<count; i++) {
				fZec133[i] = ((int((fRec115[i] > 0)))?fZec132[i]:(1 + fZec132[i]));
			}
			
			// LOOP 0xa20c968
			// exec code
			for (int i=0; i<count; i++) {
				fZec136[i] = ((int((fRec106[i] > 0)))?fZec135[i]:(1 + fZec135[i]));
			}
			
			// LOOP 0xa2192d8
			// exec code
			for (int i=0; i<count; i++) {
				fZec141[i] = ((int((fRec97[i] > 0)))?fZec140[i]:(1 + fZec140[i]));
			}
			
			// LOOP 0xa21ae78
			// exec code
			for (int i=0; i<count; i++) {
				fZec144[i] = ((int((fRec88[i] > 0)))?fZec143[i]:(1 + fZec143[i]));
			}
			
			// LOOP 0xa21ca38
			// exec code
			for (int i=0; i<count; i++) {
				fZec147[i] = ((int((fRec79[i] > 0)))?fZec146[i]:(1 + fZec146[i]));
			}
			
			// LOOP 0xa21e5f8
			// exec code
			for (int i=0; i<count; i++) {
				fZec150[i] = ((int((fRec70[i] > 0)))?fZec149[i]:(1 + fZec149[i]));
			}
			
			// LOOP 0xa2201c8
			// exec code
			for (int i=0; i<count; i++) {
				fZec153[i] = ((int((fRec61[i] > 0)))?fZec152[i]:(1 + fZec152[i]));
			}
			
			// LOOP 0xa221d98
			// exec code
			for (int i=0; i<count; i++) {
				fZec156[i] = ((int((fRec52[i] > 0)))?fZec155[i]:(1 + fZec155[i]));
			}
			
			// LOOP 0xa223a58
			// exec code
			for (int i=0; i<count; i++) {
				fZec159[i] = ((int((fRec43[i] > 0)))?fZec158[i]:(1 + fZec158[i]));
			}
			
			// LOOP 0xa2256d0
			// exec code
			for (int i=0; i<count; i++) {
				fZec162[i] = ((int((fRec33[i] > 0)))?fZec161[i]:(1 + fZec161[i]));
			}
			
			// SECTION : 15
			// LOOP 0xa136cd8
			// pre processing
			for (int i=0; i<4; i++) fRec0_tmp[i]=fRec0_perm[i];
			for (int i=0; i<4; i++) fRec1_tmp[i]=fRec1_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec0[i] = ((int((fRec2[i] != fRec2[i-1])))?fConst1:(fRec0[i-1] - 1));
				fRec0[i] = fZec0[i];
				fRec1[i] = ((int((fZec0[i] <= 0)))?fRec2[i]:(fRec1[i-1] + ((fRec2[i] - fRec1[i-1]) / fZec0[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec1_perm[i]=fRec1_tmp[count+i];
			for (int i=0; i<4; i++) fRec0_perm[i]=fRec0_tmp[count+i];
			
			// LOOP 0xa142a88
			// pre processing
			for (int i=0; i<4; i++) fRec11_tmp[i]=fRec11_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fRec11[i] = (fSlow3 + (0.999f * fRec11[i-1]));
			}
			// post processing
			for (int i=0; i<4; i++) fRec11_perm[i]=fRec11_tmp[count+i];
			
			// LOOP 0xa143a90
			// pre processing
			for (int i=0; i<4; i++) fRec12_tmp[i]=fRec12_perm[i];
			for (int i=0; i<4; i++) fRec13_tmp[i]=fRec13_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec17[i] = ((int((fRec14[i] != fRec14[i-1])))?fConst15:(fRec12[i-1] - 1));
				fRec12[i] = fZec17[i];
				fRec13[i] = ((int((fZec17[i] <= 0)))?fRec14[i]:(fRec13[i-1] + ((fRec14[i] - fRec13[i-1]) / fZec17[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec13_perm[i]=fRec13_tmp[count+i];
			for (int i=0; i<4; i++) fRec12_perm[i]=fRec12_tmp[count+i];
			
			// LOOP 0xa15f868
			// pre processing
			for (int i=0; i<4; i++) fRec36_tmp[i]=fRec36_perm[i];
			for (int i=0; i<4; i++) fRec37_tmp[i]=fRec37_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec27[i] = ((int((fRec38[i] != fRec38[i-1])))?fConst15:(fRec36[i-1] - 1));
				fRec36[i] = fZec27[i];
				fRec37[i] = ((int((fZec27[i] <= 0)))?fRec38[i]:(fRec37[i-1] + ((fRec38[i] - fRec37[i-1]) / fZec27[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec37_perm[i]=fRec37_tmp[count+i];
			for (int i=0; i<4; i++) fRec36_perm[i]=fRec36_tmp[count+i];
			
			// LOOP 0xa16bcb0
			// pre processing
			for (int i=0; i<4; i++) fRec46_tmp[i]=fRec46_perm[i];
			for (int i=0; i<4; i++) fRec47_tmp[i]=fRec47_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec34[i] = ((int((fRec48[i] != fRec48[i-1])))?fConst15:(fRec46[i-1] - 1));
				fRec46[i] = fZec34[i];
				fRec47[i] = ((int((fZec34[i] <= 0)))?fRec48[i]:(fRec47[i-1] + ((fRec48[i] - fRec47[i-1]) / fZec34[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec47_perm[i]=fRec47_tmp[count+i];
			for (int i=0; i<4; i++) fRec46_perm[i]=fRec46_tmp[count+i];
			
			// LOOP 0xa1764d0
			// pre processing
			for (int i=0; i<4; i++) fRec55_tmp[i]=fRec55_perm[i];
			for (int i=0; i<4; i++) fRec56_tmp[i]=fRec56_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec40[i] = ((int((fRec57[i] != fRec57[i-1])))?fConst15:(fRec55[i-1] - 1));
				fRec55[i] = fZec40[i];
				fRec56[i] = ((int((fZec40[i] <= 0)))?fRec57[i]:(fRec56[i-1] + ((fRec57[i] - fRec56[i-1]) / fZec40[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec56_perm[i]=fRec56_tmp[count+i];
			for (int i=0; i<4; i++) fRec55_perm[i]=fRec55_tmp[count+i];
			
			// LOOP 0xa180bf0
			// pre processing
			for (int i=0; i<4; i++) fRec64_tmp[i]=fRec64_perm[i];
			for (int i=0; i<4; i++) fRec65_tmp[i]=fRec65_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec46[i] = ((int((fRec66[i] != fRec66[i-1])))?fConst15:(fRec64[i-1] - 1));
				fRec64[i] = fZec46[i];
				fRec65[i] = ((int((fZec46[i] <= 0)))?fRec66[i]:(fRec65[i-1] + ((fRec66[i] - fRec65[i-1]) / fZec46[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec65_perm[i]=fRec65_tmp[count+i];
			for (int i=0; i<4; i++) fRec64_perm[i]=fRec64_tmp[count+i];
			
			// LOOP 0xa18b5a8
			// pre processing
			for (int i=0; i<4; i++) fRec73_tmp[i]=fRec73_perm[i];
			for (int i=0; i<4; i++) fRec74_tmp[i]=fRec74_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec52[i] = ((int((fRec75[i] != fRec75[i-1])))?fConst15:(fRec73[i-1] - 1));
				fRec73[i] = fZec52[i];
				fRec74[i] = ((int((fZec52[i] <= 0)))?fRec75[i]:(fRec74[i-1] + ((fRec75[i] - fRec74[i-1]) / fZec52[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec74_perm[i]=fRec74_tmp[count+i];
			for (int i=0; i<4; i++) fRec73_perm[i]=fRec73_tmp[count+i];
			
			// LOOP 0xa195db8
			// pre processing
			for (int i=0; i<4; i++) fRec82_tmp[i]=fRec82_perm[i];
			for (int i=0; i<4; i++) fRec83_tmp[i]=fRec83_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec58[i] = ((int((fRec84[i] != fRec84[i-1])))?fConst15:(fRec82[i-1] - 1));
				fRec82[i] = fZec58[i];
				fRec83[i] = ((int((fZec58[i] <= 0)))?fRec84[i]:(fRec83[i-1] + ((fRec84[i] - fRec83[i-1]) / fZec58[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec83_perm[i]=fRec83_tmp[count+i];
			for (int i=0; i<4; i++) fRec82_perm[i]=fRec82_tmp[count+i];
			
			// LOOP 0xa1a0668
			// pre processing
			for (int i=0; i<4; i++) fRec91_tmp[i]=fRec91_perm[i];
			for (int i=0; i<4; i++) fRec92_tmp[i]=fRec92_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec64[i] = ((int((fRec93[i] != fRec93[i-1])))?fConst15:(fRec91[i-1] - 1));
				fRec91[i] = fZec64[i];
				fRec92[i] = ((int((fZec64[i] <= 0)))?fRec93[i]:(fRec92[i-1] + ((fRec93[i] - fRec92[i-1]) / fZec64[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec92_perm[i]=fRec92_tmp[count+i];
			for (int i=0; i<4; i++) fRec91_perm[i]=fRec91_tmp[count+i];
			
			// LOOP 0xa1aaf20
			// pre processing
			for (int i=0; i<4; i++) fRec100_tmp[i]=fRec100_perm[i];
			for (int i=0; i<4; i++) fRec101_tmp[i]=fRec101_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec70[i] = ((int((fRec102[i] != fRec102[i-1])))?fConst15:(fRec100[i-1] - 1));
				fRec100[i] = fZec70[i];
				fRec101[i] = ((int((fZec70[i] <= 0)))?fRec102[i]:(fRec101[i-1] + ((fRec102[i] - fRec101[i-1]) / fZec70[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec101_perm[i]=fRec101_tmp[count+i];
			for (int i=0; i<4; i++) fRec100_perm[i]=fRec100_tmp[count+i];
			
			// LOOP 0xa1b55d0
			// pre processing
			for (int i=0; i<4; i++) fRec109_tmp[i]=fRec109_perm[i];
			for (int i=0; i<4; i++) fRec110_tmp[i]=fRec110_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec76[i] = ((int((fRec111[i] != fRec111[i-1])))?fConst15:(fRec109[i-1] - 1));
				fRec109[i] = fZec76[i];
				fRec110[i] = ((int((fZec76[i] <= 0)))?fRec111[i]:(fRec110[i-1] + ((fRec111[i] - fRec110[i-1]) / fZec76[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec110_perm[i]=fRec110_tmp[count+i];
			for (int i=0; i<4; i++) fRec109_perm[i]=fRec109_tmp[count+i];
			
			// LOOP 0xa1bff80
			// pre processing
			for (int i=0; i<4; i++) fRec118_tmp[i]=fRec118_perm[i];
			for (int i=0; i<4; i++) fRec119_tmp[i]=fRec119_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec82[i] = ((int((fRec120[i] != fRec120[i-1])))?fConst15:(fRec118[i-1] - 1));
				fRec118[i] = fZec82[i];
				fRec119[i] = ((int((fZec82[i] <= 0)))?fRec120[i]:(fRec119[i-1] + ((fRec120[i] - fRec119[i-1]) / fZec82[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec119_perm[i]=fRec119_tmp[count+i];
			for (int i=0; i<4; i++) fRec118_perm[i]=fRec118_tmp[count+i];
			
			// LOOP 0xa1ca9e0
			// pre processing
			for (int i=0; i<4; i++) fRec127_tmp[i]=fRec127_perm[i];
			for (int i=0; i<4; i++) fRec128_tmp[i]=fRec128_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec88[i] = ((int((fRec129[i] != fRec129[i-1])))?fConst15:(fRec127[i-1] - 1));
				fRec127[i] = fZec88[i];
				fRec128[i] = ((int((fZec88[i] <= 0)))?fRec129[i]:(fRec128[i-1] + ((fRec129[i] - fRec128[i-1]) / fZec88[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec128_perm[i]=fRec128_tmp[count+i];
			for (int i=0; i<4; i++) fRec127_perm[i]=fRec127_tmp[count+i];
			
			// LOOP 0xa1d5408
			// pre processing
			for (int i=0; i<4; i++) fRec136_tmp[i]=fRec136_perm[i];
			for (int i=0; i<4; i++) fRec137_tmp[i]=fRec137_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec94[i] = ((int((fRec138[i] != fRec138[i-1])))?fConst15:(fRec136[i-1] - 1));
				fRec136[i] = fZec94[i];
				fRec137[i] = ((int((fZec94[i] <= 0)))?fRec138[i]:(fRec137[i-1] + ((fRec138[i] - fRec137[i-1]) / fZec94[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec137_perm[i]=fRec137_tmp[count+i];
			for (int i=0; i<4; i++) fRec136_perm[i]=fRec136_tmp[count+i];
			
			// LOOP 0xa1dfe30
			// pre processing
			for (int i=0; i<4; i++) fRec145_tmp[i]=fRec145_perm[i];
			for (int i=0; i<4; i++) fRec146_tmp[i]=fRec146_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec100[i] = ((int((fRec147[i] != fRec147[i-1])))?fConst15:(fRec145[i-1] - 1));
				fRec145[i] = fZec100[i];
				fRec146[i] = ((int((fZec100[i] <= 0)))?fRec147[i]:(fRec146[i-1] + ((fRec147[i] - fRec146[i-1]) / fZec100[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec146_perm[i]=fRec146_tmp[count+i];
			for (int i=0; i<4; i++) fRec145_perm[i]=fRec145_tmp[count+i];
			
			// LOOP 0xa1ea870
			// pre processing
			for (int i=0; i<4; i++) fRec154_tmp[i]=fRec154_perm[i];
			for (int i=0; i<4; i++) fRec155_tmp[i]=fRec155_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec106[i] = ((int((fRec156[i] != fRec156[i-1])))?fConst15:(fRec154[i-1] - 1));
				fRec154[i] = fZec106[i];
				fRec155[i] = ((int((fZec106[i] <= 0)))?fRec156[i]:(fRec155[i-1] + ((fRec156[i] - fRec155[i-1]) / fZec106[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec155_perm[i]=fRec155_tmp[count+i];
			for (int i=0; i<4; i++) fRec154_perm[i]=fRec154_tmp[count+i];
			
			// LOOP 0xa1f5350
			// pre processing
			for (int i=0; i<4; i++) fRec163_tmp[i]=fRec163_perm[i];
			for (int i=0; i<4; i++) fRec164_tmp[i]=fRec164_perm[i];
			// exec code
			for (int i=0; i<count; i++) {
				fZec112[i] = ((int((fRec165[i] != fRec165[i-1])))?fConst15:(fRec163[i-1] - 1));
				fRec163[i] = fZec112[i];
				fRec164[i] = ((int((fZec112[i] <= 0)))?fRec165[i]:(fRec164[i-1] + ((fRec165[i] - fRec164[i-1]) / fZec112[i])));
			}
			// post processing
			for (int i=0; i<4; i++) fRec164_perm[i]=fRec164_tmp[count+i];
			for (int i=0; i<4; i++) fRec163_perm[i]=fRec163_tmp[count+i];
			
			// LOOP 0xa1ffeb0
			// exec code
			for (int i=0; i<count; i++) {
				fZec116[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec168[i] - fZec115[i]))));
			}
			
			// LOOP 0xa201a38
			// exec code
			for (int i=0; i<count; i++) {
				fZec119[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec159[i] - fZec118[i]))));
			}
			
			// LOOP 0xa203758
			// exec code
			for (int i=0; i<count; i++) {
				fZec122[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec150[i] - fZec121[i]))));
			}
			
			// LOOP 0xa205438
			// exec code
			for (int i=0; i<count; i++) {
				fZec125[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec141[i] - fZec124[i]))));
			}
			
			// LOOP 0xa207198
			// exec code
			for (int i=0; i<count; i++) {
				fZec128[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec132[i] - fZec127[i]))));
			}
			
			// LOOP 0xa208ec0
			// exec code
			for (int i=0; i<count; i++) {
				fZec131[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec123[i] - fZec130[i]))));
			}
			
			// LOOP 0xa20abd8
			// exec code
			for (int i=0; i<count; i++) {
				fZec134[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec114[i] - fZec133[i]))));
			}
			
			// LOOP 0xa20c8f0
			// exec code
			for (int i=0; i<count; i++) {
				fZec137[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec105[i] - fZec136[i]))));
			}
			
			// LOOP 0xa219260
			// exec code
			for (int i=0; i<count; i++) {
				fZec142[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec96[i] - fZec141[i]))));
			}
			
			// LOOP 0xa21ae00
			// exec code
			for (int i=0; i<count; i++) {
				fZec145[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec87[i] - fZec144[i]))));
			}
			
			// LOOP 0xa21c9c0
			// exec code
			for (int i=0; i<count; i++) {
				fZec148[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec78[i] - fZec147[i]))));
			}
			
			// LOOP 0xa21e580
			// exec code
			for (int i=0; i<count; i++) {
				fZec151[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec69[i] - fZec150[i]))));
			}
			
			// LOOP 0xa220150
			// exec code
			for (int i=0; i<count; i++) {
				fZec154[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec60[i] - fZec153[i]))));
			}
			
			// LOOP 0xa221d20
			// exec code
			for (int i=0; i<count; i++) {
				fZec157[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec51[i] - fZec156[i]))));
			}
			
			// LOOP 0xa2239e0
			// exec code
			for (int i=0; i<count; i++) {
				fZec160[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec41[i] - fZec159[i]))));
			}
			
			// LOOP 0xa225658
			// exec code
			for (int i=0; i<count; i++) {
				fZec163[i] = cosf((6.283185307179586f * (fZec20[i] * (fRec31[i] - fZec162[i]))));
			}
			
			// SECTION : 16
			// LOOP 0xa1ffe38
			// exec code
			for (int i=0; i<count; i++) {
				fZec138[i] = ((((((((fRec101[i] * (fZec137[i] + (fZec136[i] * (fZec137[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec137[i]) - fZec19[i]))))))) + (fRec110[i] * (fZec134[i] + (fZec133[i] * (fZec134[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec134[i]) - fZec19[i])))))))) + (fRec119[i] * (fZec131[i] + (fZec130[i] * (fZec131[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec131[i]) - fZec19[i])))))))) + (fRec128[i] * (fZec128[i] + (fZec127[i] * (fZec128[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec128[i]) - fZec19[i])))))))) + (fRec137[i] * (fZec125[i] + (fZec124[i] * (fZec125[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec125[i]) - fZec19[i])))))))) + (fRec146[i] * (fZec122[i] + (fZec121[i] * (fZec122[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec122[i]) - fZec19[i])))))))) + (fRec155[i] * (fZec119[i] + (fZec118[i] * (fZec119[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec119[i]) - fZec19[i])))))))) + (fRec164[i] * (fZec116[i] + (fZec115[i] * (fZec116[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec116[i]) - fZec19[i]))))))));
			}
			
			// LOOP 0xa218bc8
			// exec code
			for (int i=0; i<count; i++) {
				fZec139[i] = (1 - fRec11[i]);
			}
			
			// LOOP 0xa2191e8
			// exec code
			for (int i=0; i<count; i++) {
				fZec164[i] = ((((((((fRec13[i] * (fZec163[i] + (fZec162[i] * (fZec163[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec163[i]) - fZec19[i]))))))) + (fRec37[i] * (fZec160[i] + (fZec159[i] * (fZec160[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec160[i]) - fZec19[i])))))))) + (fRec47[i] * (fZec157[i] + (fZec156[i] * (fZec157[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec157[i]) - fZec19[i])))))))) + (fRec56[i] * (fZec154[i] + (fZec153[i] * (fZec154[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec154[i]) - fZec19[i])))))))) + (fRec65[i] * (fZec151[i] + (fZec150[i] * (fZec151[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec151[i]) - fZec19[i])))))))) + (fRec74[i] * (fZec148[i] + (fZec147[i] * (fZec148[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec148[i]) - fZec19[i])))))))) + (fRec83[i] * (fZec145[i] + (fZec144[i] * (fZec145[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec145[i]) - fZec19[i])))))))) + (fRec92[i] * (fZec142[i] + (fZec141[i] * (fZec142[i] - cosf((6.283185307179586f * ((fZec18[i] + fZec142[i]) - fZec19[i]))))))));
			}
			
			// LOOP 0xa22e850
			// exec code
			for (int i=0; i<count; i++) {
				fZec165[i] = (1 + fRec11[i]);
			}
			
			// LOOP 0xa22ede8
			// exec code
			for (int i=0; i<count; i++) {
				fZec166[i] = ftbl0[int((100 + (fRec1[i] * cosf((6.283185307179586f * ((0.5f * fZec20[i]) - 0.25f))))))];
			}
			
			// SECTION : 17
			// LOOP 0xa136bf8
			// exec code
			for (int i=0; i<count; i++) {
				output0[i] = (FAUSTFLOAT)(0.375f * (fZec166[i] * ((fZec165[i] * fZec164[i]) + (fZec139[i] * fZec138[i]))));
			}
			
			// LOOP 0xa231b18
			// exec code
			for (int i=0; i<count; i++) {
				output1[i] = (FAUSTFLOAT)(0.375f * (fZec166[i] * ((fZec165[i] * fZec138[i]) + (fZec139[i] * fZec164[i]))));
			}
			
		}
	}
};


float 	mydsp::ftbl0[200];

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/
					
mydsp		DSP;
std::list<GUI*>	GUI::fGuiList;

//-------------------------------------------------------------------------
// 									MAIN
//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	char appname[256];
	char rcfilename[256];
	char* home = getenv("HOME");
	
	snprintf(appname, 255, "%s", basename(argv[0]));
	snprintf(rcfilename, 255, "%s/.%src", home, appname);

	GUI* interface 	= new GTKUI (appname, &argc, &argv);
	FUI* finterface	= new FUI();
	DSP.buildUserInterface(interface);
	DSP.buildUserInterface(finterface);
    DSP.buildUserInterface(new PrintUI());

#ifdef HTTPCTRL
	httpdUI* httpdinterface = new httpdUI(appname, DSP.getNumInputs(), DSP.getNumOutputs(), argc, argv);
	DSP.buildUserInterface(httpdinterface);
 	std::cout << "HTTPD is on" << std::endl;
#endif

#ifdef OSCCTRL
	GUI* oscinterface = new OSCUI(appname, argc, argv);
	DSP.buildUserInterface(oscinterface);
#endif

#ifdef OCVCTRL
	std::cout<<"OCVCTRL defined"<<std::endl;
	OCVUI* ocvinterface = new OCVUI();
	DSP.buildUserInterface(ocvinterface);
#endif

	jackaudio audio;
	audio.init(appname, &DSP);
	finterface->recallState(rcfilename);	
	audio.start();
	
#ifdef HTTPCTRL
	httpdinterface->run();
#endif

#ifdef OSCCTRL
	oscinterface->run();
#endif

#ifdef OCVCTRL
	ocvinterface->run();
#endif

	interface->run();
	
	audio.stop();
	finterface->saveState(rcfilename);
    
    // desallocation
    delete interface;
    delete finterface;
#ifdef HTTPCTRL
	 delete httpdinterface;
#endif
#ifdef OSCCTRL
	 delete oscinterface;
#endif

#ifdef OCVCTRL
	 delete ocvinterface;
#endif

  	return 0;
}

		
/********************END ARCHITECTURE SECTION (part 2/2)****************/

