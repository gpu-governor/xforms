// widgets:
// container
// button
// text
// label
// slider
// entry
#include"xform.h"
const Color COLOR_BACKGROUND   = {30, 30, 30, 255};  // Dark background (VS Code dark)
const Color COLOR_FOREGROUND   = {220, 220, 220, 255}; // Light gray text

// Buttons
const Color COLOR_BUTTON       = {0, 122, 204, 255};  // VS Code blue
const Color COLOR_BUTTON_HOVER = {0, 102, 184, 255};  // Slightly darker blue
const Color COLOR_BUTTON_CLICK = {0, 92, 174, 255};   // Even darker blue

// Labels
const Color COLOR_LABEL        = {200, 200, 200, 255};  // Light gray

// Text Entry (Input Fields)
const Color COLOR_TEXT_ENTRY   = {50, 50, 50, 255};  // Darker than background
const Color COLOR_TEXT_ENTRY_BORDER = {70, 70, 70, 255};  // Slight border for visibility
const Color COLOR_TEXT_ENTRY_FOCUS  = {0, 122, 204, 255};  // Blue border when focused

// Slider
const Color COLOR_SLIDER_TRACK = {80, 80, 80, 255};   // Dark gray track
const Color COLOR_SLIDER_FILL  = {0, 122, 204, 255};  // Blue fill
const Color COLOR_SLIDER_KNOB  = {200, 200, 200, 255}; // Light gray knob

// Misc
const Color COLOR_BORDER       = {50, 50, 50, 255};  // General border color
const Color COLOR_HIGHLIGHT    = {0, 122, 204, 255}; // Highlighted elements (like selection)


int main() {
    xi_Window xiWin = xiCreateWindow("xi SDL Window", 1080, 600);

    xi_Container mycontainer = createContainer(500, 100, 500, 400, COLOR_BACKGROUND, "me container",true);
    xi_Container boxcontainer = createContainer(50, 100, 400, 300, COLOR_BACKGROUND, NULL ,false); // if null is passed for title it becomes a box con
    // button
    Button mybutton = CreateButton(10, 10, 70, 30, "click me", COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLUE);
    // text
     Text mytext = CreateText("this is a text",100, 22,COLOR_GREEN,16);
     Text youtext = CreateText("this is a text",500, 22,COLOR_BLUE,16);
 //   // label
    Label mylabel = CreateLabel(50, 50, 200, 40, "this is a label", COLOR_WHITE, COLOR_RED); 
 //   // slider
    Slider myslider = CreateSlider(40, 300, 100, 50, 0, 100, 50);
 // // entry
  TextEntry myentry = CreateTextEntry(88,33, 500,50,16, COLOR_BLACK, COLOR_WHITE);


   // PARENT 
   mybutton.parent=&mycontainer;
   mytext.parent=&mycontainer;
   mylabel.parent=&mycontainer;
   myslider.parent=&mycontainer;
   myentry.parent=&mycontainer;
  //  EventLoop();

    while (program_active) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
          switch (event.type) {
              case SDL_QUIT:
                 program_active = false;  // User closed the window
                  break;
              
                  break;
              case SDL_WINDOWEVENT:
                  if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                      // Resize event triggers window size update
                  }
                  break;
              default:
                  break;
          }
          //buttons
        //  sw_render_all_button_states(&event);
          //drop down
        //  sw_render_all_drop_down_states(&event);
   //slider
   //sw_render_all_slider_states(&event);
         //entry
     //   sw_render_all_entry_states(&event);
       // Handle container movement
       handleContainerMovement(&mycontainer, &event);
       handleContainerMovement(&boxcontainer, &event);
     
      }
       //clear_screen(xiWindow.background_color);
       xi_ClearScreen(grenderer, COLOR_GRAY);
       render_container(&mycontainer);
       render_container(&boxcontainer);
       render_button(&mybutton);
       render_text(&mytext);
       render_text(&youtext);
       render_label(&mylabel);
       render_slider(&myslider);
      render_text_entry(&myentry);
      SDL_RenderPresent(grenderer);       // Present the rendered output
  }
    xiDestroyWindow(&xiWin);
    return 0;
}
