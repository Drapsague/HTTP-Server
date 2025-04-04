// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
//
#include <memory>  // for allocator, __shared_ptr_access
#include <string>  // for char_traits, operator+, string, basic_string
#include <vector>
#include <iostream>
 
#include "ftxui/component/captured_mouse.hpp"  // for ftxui
#include "ftxui/component/component.hpp"       // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"  // for ComponentBase
#include "ftxui/component/component_options.hpp"  // for InputOption
#include "ftxui/component/screen_interactive.hpp"  // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"  // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp"  // for Ref
 

int main() {
   using namespace ftxui;

   // The data:
   std::string username {};
   bool is_username {};

   Component input_username = Input(&username, "Enter your username...");

   // To create a screen in the terminal
   auto screen = ScreenInteractive::TerminalOutput();

   Component username_layout = Renderer(input_username, [&]{
                                      return vbox({
                                         hbox({text("Please enter your username : "),
                                              input_username->Render()}),
                                                 });
                                         }) | border | size(WIDTH, EQUAL, 100);


   // The CatchEvent feature allow us to listen for specific events
   Component username_layout_event = CatchEvent(username_layout, [&](Event event){
                             // Event::Return means the user press Enter 
                              if(event == Event::Return && !username.empty()) {
                                 is_username =  true;
                                 screen.Exit();
                                 return true;
                              }
                              return false;
                         });

   screen.Loop(username_layout_event);

      



   if (is_username) {

      // This is the main list for the current conv
      std::vector<std::string> message_list;
      std::string message {};
      Component input_message = Input(&message, "Enter your message...");


      Component message_layout = Renderer(input_message, [&]{
                                 // If we don't create another vector of elements, only the last
                                 // message will be displayed
                                 std::vector<Element> bubbles;
                                 for (auto &msg : message_list) {
                                    bubbles.push_back(text("[" + username + "] : " + msg) | color(Color::Green)); 
                                 }
                                         return vbox({
                                            // vscroll_indicator creates a scroll bar
                                            // And frame will lock the screen to the scroll bar view
                                            vbox(std::move(bubbles)) | border | vscroll_indicator | frame,
                                            separatorHeavy(),
                                            hbox({text("[Message] : ") , 
                                                 input_message->Render()
                                                }) | border,
                                             });
                                 }) | size(WIDTH, EQUAL, 100);


      // The CatchEvent feature allow us to listen for specific events
      Component message_layout_event = CatchEvent(message_layout, [&](Event event){
                                // Event::Return means the user press Enter 
                                 if(event == Event::Return && !message.empty()) {
                                    message_list.push_back(message); 
                                    message.clear();
                                    return true;
                                 }
                                 return false;
                            });

      screen.Loop(message_layout_event);

   }

   return 0;

}

