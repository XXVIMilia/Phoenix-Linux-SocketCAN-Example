#include "sgui.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sys/time.h>




sguiApp::sguiApp()
: b1("Pause Updator"),b2("Resume Updator"), b3("salutations planet")   // creates a new button with label "Hello World".
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
  



  // The final step is to display this newly created widget...
  show_all_children();

}
//################################################################
//Make prep corners create tab structure!!!!
//Add Back corners
//Fix grid structure of each corner to have PID, Label, Sensor data, checkbox
//Spin buttons arent changing, nor updating
//################################################################
void sguiApp::prepCorners(){
  motorView.add(frontLeft.MainHolder);
  motorView.add(frontRight.MainHolder);
  motorView.attach_next_to(backLeft.MainHolder,frontLeft.MainHolder,Gtk::POS_BOTTOM,1,1);
  motorView.attach_next_to(backRight.MainHolder,frontRight.MainHolder,Gtk::POS_BOTTOM,1,1);

  frontLeft.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontLeft.PID_box.pack_start(frontLeft.P_spin);
  frontLeft.PID_box.pack_start(frontLeft.I_spin);
  frontLeft.PID_box.pack_start(frontLeft.D_spin);
  frontLeft.cornerName.set_text("Front Left");
  frontLeft.MainHolder.add(frontLeft.cornerName);
  frontLeft.MainHolder.add(frontLeft.PID_box);

  frontRight.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontRight.PID_box.pack_start(frontRight.P_spin);
  frontRight.PID_box.pack_start(frontRight.I_spin);
  frontRight.PID_box.pack_start(frontRight.D_spin);
  frontRight.cornerName.set_text("Front Right");
  frontRight.MainHolder.add(frontRight.cornerName);
  frontRight.MainHolder.add(frontRight.PID_box);

  motor_Box.add(motorView);
  m_note.append_page(motor_Box, "Motors");

  m_note.show_all();


}

void sguiApp::loadConfigs(std::string directory){
  std::string line;

  //Load Controller Button Names
  std::ifstream controllerConfig;
  std::cout<<"Opening Controller\n";
  controllerConfig.open( directory + "/configs/controller_map.txt");
  std::cout<<"We in\n";
  if (controllerConfig.is_open())
  {
    int i = 0;
    type_children children = refListStore->children();
    type_children::iterator iter = children.begin();
    while(std::getline(controllerConfig,line)){
      std::cout<<"Current ctr Val: " << i << "\n";
      Gtk::TreeModel::Row row = *iter;
      row[m_Columns.m_col_text] = line;
      iter++;
      i++;
    }
    controllerConfig.close();
  }
  else{
    std::cout << "Unable to open file, default values loaded";
  }

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

  controllerView.append_column_editable("Input", m_Columns.m_col_text);
  Gtk::CellRendererText *cell = (Gtk::CellRendererText*)controllerView.get_column_cell_renderer(0);
  cell->property_editable() = true;
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
  type_children children = refListStore->children();
  for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter){
     Gtk::TreeModel::Row row = *iter;
     std::cout<<row[m_Columns.m_col_text] << "\n";
  }
  controllerView.show();
  
}

void sguiApp::on_button_clicked(std::string buttonLbl)
{
  std::cout << buttonLbl << std::endl;
  if(buttonLbl == "B1" && signalPaused == false){
    joystickHandler.disconnect();
    signalPaused = true;
  }
  else if(buttonLbl == "B2" && signalPaused == true){
    joystickHandler = Glib::signal_timeout().connect(joystickSlot,20);
    signalPaused = false;
  }
}

void sguiApp::say(std::string input){
  sguiApp::b1.set_label(Glib::ustring(input));
}