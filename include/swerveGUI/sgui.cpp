#include "sgui.h"
#include <iostream>
#include <sys/time.h>

sguiApp::sguiApp()
: b1("Hello World"),b2("Wazzap earth"), b3("salutations planet")   // creates a new button with label "Hello World".
{
  gettimeofday(&pastTime, NULL);//Initial timestamp
  // Sets the border width of the window.
  set_border_width(10);

  // When the button receives the "clicked" signal, it will call the
  // on_button_clicked() method defined below.
  b1.signal_clicked().connect( sigc::bind(sigc::mem_fun(*this,
              &sguiApp::on_button_clicked), "B1"));

  b2.signal_clicked().connect( sigc::bind(sigc::mem_fun(*this,
              &sguiApp::on_button_clicked), "B2"));

  b3.signal_clicked().connect( sigc::bind(sigc::mem_fun(*this,
              &sguiApp::on_button_clicked), "B3"));

  // This packs the box into the Window (a container).


  
  add(Child_widgets);

  Child_widgets.set_orientation(Gtk::ORIENTATION_VERTICAL);
  Child_widgets.pack_start(m_note);
  Child_widgets.pack_start(buttons);
  

  buttons.pack_start(b1);
  buttons.pack_start(b2);
  buttons.pack_start(b3);

  refListStore = Gtk::ListStore::create(m_Columns); // Creates My model. Still need to add rows elsewhere
  controllerView.set_model(refListStore);

  controller_Box.add(controllerView);

  m_note.append_page(main_Box,"Main");
  m_note.append_page(motor_Box, "Motors");






  // The final step is to display this newly created widget...
  show_all_children();

}

int sguiApp::updateTime(){
  timeval tempVal;
  gettimeofday(&tempVal, NULL);
  delTime = tempVal.tv_usec/1000 - pastTime.tv_usec/1000;
  gettimeofday(&pastTime, NULL);
  return(delTime);
}

void sguiApp::setUpControllerView(int rows){
  for(int i = 0; i < rows; i++){
    controller_iter = refListStore->append();
    Gtk::TreeModel::Row row = *controller_iter;
    row[m_Columns.m_col_text] = std::to_string(i);
    row[m_Columns.m_col_number] = 0;
  }

  controllerView.append_column("Input", m_Columns.m_col_text);
  controllerView.append_column("Val", m_Columns.m_col_number);

  m_note.append_page(controller_Box, "Controller");
  m_note.show_all();


  //How to iterate through the view
  // type_children children = refListStore->children();
  // for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter){
  //    Gtk::TreeModel::Row row = *iter;
  //    std::cout<< row[m_Columns.m_col_text] << " ";
  //    std::cout<< row[m_Columns.m_col_number] << "\n";
  // }
}

void sguiApp::updateControllerView(int* packet){
  int i = 0;
  type_children children = refListStore->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter){
     Gtk::TreeModel::Row row = *iter;
     row[m_Columns.m_col_number] = packet[i];
     i++;
  }
  controllerView.show();

}


sguiApp::~sguiApp()
{
}

void sguiApp::on_button_clicked(std::string buttonLbl)
{
  std::cout << buttonLbl << std::endl;
}

void sguiApp::say(std::string input){
  sguiApp::b1.set_label(Glib::ustring(input));
}