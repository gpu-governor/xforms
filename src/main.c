// widgets:
// container
// button
// text
// label
// slider
// entry
#include"xform.h"
int main() {
    xiWindow xiWin = xiCreateWindow("xi SDL Window", 800, 600);

    xiContainer mycontainer = createContainer(100, 100, 400, 300, COLOR_YELLOW, "me container",true);
    // button
    Button mybutton = CreateButton(10, 10, 40, 50, "click me", COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLUE);
    // text
    Text mytext = CreateText(39, 22,"this is a text",COLOR_WHITE,16);
   // label
   Label mylabel = CreateLabel(12, 33, 50, 60, "this is a label", COLOR_WHITE, COLOR_RED); 
   // slider
   Slider myslider = CreateSlider(40, 300, 100, 50, 0, 100, 50);
 // entry
 TextEntry myentry = CreateTextEntry(88,33, 500,50,16, COLOR_BLACK, COLOR_WHITE);

    EventLoop();
    xiDestroyWindow(&xiWin);
    return 0;
}
