// widgets:
// container
// button
// text
// label
// slider
// entry
#include"xform.h"
int main() {
    xi_Window xiWin = xiCreateWindow("xi SDL Window", 800, 600);

    xi_Container mycontainer = createContainer(100, 100, 400, 300, COLOR_YELLOW, "me container",true);
    // button
    Button mybutton = CreateButton(10, 10, 40, 50, "click me", COLOR_WHITE, COLOR_GREEN, COLOR_RED, COLOR_BLUE);
    // text
     Text mytext = CreateText("this is a text",39, 22,COLOR_WHITE,16);
 //   // label
    Label mylabel = CreateLabel(12, 33, 50, 60, "this is a label", COLOR_WHITE, COLOR_RED); 
 //   // slider
    Slider myslider = CreateSlider(40, 300, 100, 50, 0, 100, 50);
 // // entry
 // TextEntry myentry = CreateTextEntry(88,33, 500,50,16, COLOR_BLACK, COLOR_WHITE);


    EventLoop();

  //   while (program_active) {
  //     SDL_Event event;
  //     while (SDL_PollEvent(&event)) {
  //         switch (event.type) {
  //             case SDL_QUIT:
  //                program_active = false;  // User closed the window
  //                 break;
  //             
  //                 break;
  //             case SDL_WINDOWEVENT:
  //                 if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
  //                     // Resize event triggers window size update
  //                 }
  //                 break;
  //             default:
  //                 break;
  //         }
  //         //buttons
  //       //  sw_render_all_button_states(&event);
  //         //drop down
  //       //  sw_render_all_drop_down_states(&event);
  //  //slider
  //  //sw_render_all_slider_states(&event);
  //        //entry
  //    //   sw_render_all_entry_states(&event);
  //    
  //     }
  //      //clear_screen(xiWindow.background_color);
  //      xi_ClearScreen(grenderer, COLOR_GRAY);
  //      render_container(&mycontainer);
  //      render_button(&mybutton);
  //      render_text(&mytext);
  //      render_label(&mylabel);
  //      render_slider(&myslider);
  //     // render_text_entry(&myentry);
  //     SDL_RenderPresent(grenderer);       // Present the rendered output
  // }
    xiDestroyWindow(&xiWin);
    return 0;
}
