//automatic triggers for certain UIs in game

#include "engine.h"

void notifywelcome()
{
    UI::hideui("servers");
}

struct change
{
    int type;
    const char *desc;

    change() {}
    change(int type, const char *desc) : type(type), desc(desc) {}
};
static vector<change> needsapply;

VARP(applydialog, 0, 1, 1);

//when 0: change UI shows up whenever a pending change is added
//when 1: change UI does not appear and applychanges must be used manually
VAR(hidechanges, 0, 0, 1);

//adds a change to the queue of settings changes,
//if applydialog = 0 then this function does nothing
//if hidechanges = 0 then this functin does not display changes UI at the end
void addchange(const char *desc, int type)
{
    if(!applydialog) return;
    loopv(needsapply) if(!strcmp(needsapply[i].desc, desc)) return;
    needsapply.add(change(type, desc));
    if(!hidechanges) UI::showui("changes");
}

//clears out pending changes added by addchange()
void clearchanges(int type)
{
    loopvrev(needsapply)
    {
        change &c = needsapply[i];
        if(c.type&type)
        {
            c.type &= ~type;
            if(!c.type) needsapply.remove(i);
        }
    }
    if(needsapply.empty()) UI::hideui("changes");
}

//goes through and applies changes that are enqueued
void applychanges()
{
    int changetypes = 0;
    loopv(needsapply) changetypes |= needsapply[i].type;
    if(changetypes&CHANGE_GFX) execident("resetgl");
    else if(changetypes&CHANGE_SHADERS) execident("resetshaders");
    if(changetypes&CHANGE_SOUND) execident("resetsound");
}

//executes applychanges()
COMMAND(applychanges, "");

//returns if there are pending changes or not enqueued
ICOMMAND(pendingchanges, "b", (int *idx), { if(needsapply.inrange(*idx)) result(needsapply[*idx].desc); else if(*idx < 0) intret(needsapply.length()); });

static int lastmainmenu = -1;

//used in main.cpp
void menuprocess()
{
    if(lastmainmenu != mainmenu)
    {
        lastmainmenu = mainmenu;
        execident("mainmenutoggled");
    }
    if(mainmenu && !isconnected(true) && !UI::hascursor()) UI::showui("main");
}

//toggles if the main menu is shown
VAR(mainmenu, 1, 1, 0);

void clearmainmenu()
{
    hidechanges = 0;
    if(mainmenu && isconnected())
    {
        mainmenu = 0;
        UI::hideui(NULL);
    }
}

