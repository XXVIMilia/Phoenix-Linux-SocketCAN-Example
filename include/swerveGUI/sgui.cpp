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
//Spin buttons arent changing, nor updating
//Data Tree needs to be made and inserted
//Need to create relevant signals for the buttons to control various functions
//################################################################
void sguiApp::prepCorners(){
  motor_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);

  //Radio Button Toggle interface
  Gtk::RadioButton* rb1 = Gtk::make_managed<Gtk::RadioButton>(turn_drive_toggle, "Turning");
  Gtk::RadioButton* rb2 = Gtk::make_managed<Gtk::RadioButton>(turn_drive_toggle, "Driving");
  rb1->set_active();
  toggle_RD_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  toggle_RD_box.add(*rb1);
  toggle_RD_box.add(*rb2);

  motor_Box.add(toggle_RD_box);

  //Run Test Interface
  runTest.set_label("Run");
  test_type.set_label("Angle Test: ");
  fillerText.set_label(" to ");
  runTestBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  runTestBox.add(runTest);
  runTestBox.add(test_type);
  runTestBox.add(startVal);
  runTestBox.add(fillerText);
  runTestBox.add(stopVal);

  motor_Box.add(runTestBox);



  //Motor View Setup
  guint borderWidth = 10;
  motorView_frame.add(motorView);
  motorView.set_row_spacing(borderWidth);
  motorView.set_column_spacing(borderWidth);
  motorView.set_border_width(borderWidth);
  fl_frame.set_border_width(borderWidth);
  fr_frame.set_border_width(borderWidth);
  bl_frame.set_border_width(borderWidth);
  br_frame.set_border_width(borderWidth);
  fl_frame.add(frontLeft.MainHolder);
  fr_frame.add(frontRight.MainHolder);
  bl_frame.add(backLeft.MainHolder);
  br_frame.add(backRight.MainHolder);
  motorView.add(fl_frame);
  motorView.add(fr_frame);
  motorView.attach_next_to(bl_frame,fl_frame,Gtk::POS_BOTTOM,1,1);
  motorView.attach_next_to(br_frame,fr_frame,Gtk::POS_BOTTOM,1,1);


  //Front Left Setup
  frontLeft.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontLeft.P_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontLeft.I_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontLeft.D_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  frontLeft.P_name.set_label("P: ");
  frontLeft.I_name.set_label("I: ");
  frontLeft.D_name.set_label("D: ");

  frontLeft.P_pair.pack_start(frontLeft.P_name);
  frontLeft.P_pair.pack_start(frontLeft.P_spin);

  frontLeft.I_pair.pack_start(frontLeft.I_name);
  frontLeft.I_pair.pack_start(frontLeft.I_spin);

  frontLeft.D_pair.pack_start(frontLeft.D_name);
  frontLeft.D_pair.pack_start(frontLeft.D_spin);



  frontLeft.PID_box.pack_start(frontLeft.P_pair);
  frontLeft.PID_box.pack_start(frontLeft.I_pair);
  frontLeft.PID_box.pack_start(frontLeft.D_pair);


  frontLeft.cornerName.set_text("Front Left");
  frontLeft.corner_name_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontLeft.corner_name_box.add(frontLeft.cornerName);
  frontLeft.corner_name_box.add(frontLeft.Enabled);

  frontLeft.PID_Data_Pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontLeft.PID_Data_Pair.add(frontLeft.PID_box);

  frontLeft.MainHolder.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontLeft.MainHolder.add(frontLeft.corner_name_box);
  frontLeft.MainHolder.add(frontLeft.PID_Data_Pair);


  //Front Right Setup
  frontRight.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontRight.P_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontRight.I_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontRight.D_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  frontRight.P_name.set_label("P: ");
  frontRight.I_name.set_label("I: ");
  frontRight.D_name.set_label("D: ");

  frontRight.P_pair.pack_start(frontRight.P_name);
  frontRight.P_pair.pack_start(frontRight.P_spin);

  frontRight.I_pair.pack_start(frontRight.I_name);
  frontRight.I_pair.pack_start(frontRight.I_spin);

  frontRight.D_pair.pack_start(frontRight.D_name);
  frontRight.D_pair.pack_start(frontRight.D_spin);



  frontRight.PID_box.pack_start(frontRight.P_pair);
  frontRight.PID_box.pack_start(frontRight.I_pair);
  frontRight.PID_box.pack_start(frontRight.D_pair);


  frontRight.cornerName.set_text("Front Right");
  frontRight.corner_name_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontRight.corner_name_box.add(frontRight.cornerName);
  frontRight.corner_name_box.add(frontRight.Enabled);

  frontRight.PID_Data_Pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  frontRight.PID_Data_Pair.add(frontRight.PID_box);

  frontRight.MainHolder.set_orientation(Gtk::ORIENTATION_VERTICAL);
  frontRight.MainHolder.add(frontRight.corner_name_box);
  frontRight.MainHolder.add(frontRight.PID_Data_Pair);

  //Back Left Setup
  backLeft.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  backLeft.P_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backLeft.I_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backLeft.D_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  backLeft.P_name.set_label("P: ");
  backLeft.I_name.set_label("I: ");
  backLeft.D_name.set_label("D: ");

  backLeft.P_pair.pack_start(backLeft.P_name);
  backLeft.P_pair.pack_start(backLeft.P_spin);

  backLeft.I_pair.pack_start(backLeft.I_name);
  backLeft.I_pair.pack_start(backLeft.I_spin);

  backLeft.D_pair.pack_start(backLeft.D_name);
  backLeft.D_pair.pack_start(backLeft.D_spin);



  backLeft.PID_box.pack_start(backLeft.P_pair);
  backLeft.PID_box.pack_start(backLeft.I_pair);
  backLeft.PID_box.pack_start(backLeft.D_pair);


  backLeft.cornerName.set_text("Back Left");
  backLeft.corner_name_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backLeft.corner_name_box.add(backLeft.cornerName);
  backLeft.corner_name_box.add(backLeft.Enabled);

  backLeft.PID_Data_Pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backLeft.PID_Data_Pair.add(backLeft.PID_box);

  backLeft.MainHolder.set_orientation(Gtk::ORIENTATION_VERTICAL);
  backLeft.MainHolder.add(backLeft.corner_name_box);
  backLeft.MainHolder.add(backLeft.PID_Data_Pair);

  //Back Right Setup
  backRight.PID_box.set_orientation(Gtk::ORIENTATION_VERTICAL);
  backRight.P_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backRight.I_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backRight.D_pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  backRight.P_name.set_label("P: ");
  backRight.I_name.set_label("I: ");
  backRight.D_name.set_label("D: ");

  backRight.P_pair.pack_start(backRight.P_name);
  backRight.P_pair.pack_start(backRight.P_spin);

  backRight.I_pair.pack_start(backRight.I_name);
  backRight.I_pair.pack_start(backRight.I_spin);

  backRight.D_pair.pack_start(backRight.D_name);
  backRight.D_pair.pack_start(backRight.D_spin);



  backRight.PID_box.pack_start(backRight.P_pair);
  backRight.PID_box.pack_start(backRight.I_pair);
  backRight.PID_box.pack_start(backRight.D_pair);


  backRight.cornerName.set_text("Back Right");
  backRight.corner_name_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backRight.corner_name_box.add(backRight.cornerName);
  backRight.corner_name_box.add(backRight.Enabled);

  backRight.PID_Data_Pair.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  backRight.PID_Data_Pair.add(backRight.PID_box);

  backRight.MainHolder.set_orientation(Gtk::ORIENTATION_VERTICAL);
  backRight.MainHolder.add(backRight.corner_name_box);
  backRight.MainHolder.add(backRight.PID_Data_Pair);

  //Create Motors notebook tab
  motor_Box.add(motorView_frame);
  m_note.append_page(motor_Box, "Motors");

  //Recursively show all elements in m_note
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