
// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
//
#include "client.h"
#include <thread>

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
//


using namespace ftxui;

 
Element RenderMessage(const std::string &message, const MessageOption& opt) {
   auto bubble = text(message) | color(opt.color);
   if(opt.is_me) {
      return vbox({
                  hbox({text("[You] : "), bubble, filler()}) | color(Color::Green)
                  });
   }
   else {
      return vbox({
                  hbox({text("[" + opt.username + "] : "), bubble, filler()}) | color(opt.color)
               });
   }
}

MessageOption recvMessage(std::string message) {
   MessageOption opt;
   // size_t username_pos = message.find("{\"username\":\"");
   // size_t message_pos = message.find("\",message\":\"");
   size_t pos = message.find("{\"message\":\"");
   if (pos  == std::string::npos) {
      return opt;
   }
   // std::string u_tmp = message.substr(username_pos);
   // std::string u_ser = u_tmp.substr(u_tmp.find(':') + 1);
   // std::string username = u_ser.substr(0, u_ser.find('"'));

   // std::string msg_tmp = message.substr(message_pos);
   // std::string msg_ser = msg_tmp.substr(msg_tmp.find(':') + 1);
   // std::string msg = msg_ser.substr(0, msg_ser.find('"'));

   std::string msg_tmp = message.substr(pos);
   std::string msg_ser = msg_tmp.substr(msg_tmp.find(':') + 2);
   std::string msg = msg_ser.substr(0, msg_ser.find('"'));


   opt.is_me = false;
   opt.message = msg;
   opt.username = "User";
   opt.color = Color::Cyan;

   return opt;
}

int main() {

   Client s = Client(8085, 4096);
   s.connection();

   std::string recv_msg {};
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


   // This is the main list for the current conv
   std::vector<MessageOption> message_list;

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
      s.m_username = username;

      std::string message {};
      Component input_message = Input(&message, "Enter your message...");
      
      Component message_layout = Renderer(input_message, [&]{

                                 // If we don't create another vector of elements, only the last
                                 // message will be displayed
                                 std::vector<Element> bubbles;
                                 for (auto &msg : message_list) {
                                    bubbles.push_back(RenderMessage(msg.message, msg)); 
                                    // bubbles.push_back(text("[" + username + "] : " + msg) | color(Color::Green)); 
                                 }
                                return vbox({
                                   // vscroll_indicator creates a scroll bar
                                   // And frame will lock the screen to the scroll bar view
                                   vbox(std::move(bubbles)) | border | vscroll_indicator | frame,
                                   separator(),
                                   hbox({text("[Message] : "), 
                                        input_message->Render()
                                       }) | border,
                                    });
                        }) | size(WIDTH, EQUAL, 100);


      Component message_layout_event = CatchEvent(message_layout, [&](Event event){
                                    
                                 recv_msg = s.recv_message;
                                   // Event::Return means the user press Enter 
                                 if(event == Event::Return && !message.empty()) {
                                    MessageOption opt {};
                                    opt.is_me = true;
                                    opt.message = message;
                                    message_list.push_back(opt); 

                                    // Create and send the message 
                                    s.create_response(message);
                                    s.send_response();
                                    message.clear();
                                    screen.Clear();
                                    return true;
                                 }
                                 return false;
                            });



      std::thread receiver_thread([&] {
         while (true) {
             std::string recv_msg = s.recv_message;

             if (!recv_msg.empty()) {
                 MessageOption opt_r = recvMessage(recv_msg);
                 if (!opt_r.message.empty()) {
                     message_list.push_back(opt_r);
                     screen.PostEvent(Event::Custom);
                 }
                 recv_msg.clear();
             }
            s.recv_message.clear();

             std::this_thread::sleep_for(std::chrono::milliseconds(50));
         }
      });

      // Start the connection to the server
      screen.Loop(message_layout_event);
      }



   return 0;

}
