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

void sguiApp::getPIDCoefs(double (&toStore)[24]){
  std::cout<< "Getting pids\n";
  for(int i = 0; i < 6; i++){
    toStore[i] = frontLeft.pids[i];
  }

  for(int i = 0; i < 6; i++){
    toStore[i + 6] = frontRight.pids[i];
  }

  for(int i = 0; i < 6; i++){
    toStore[i + 12] = backLeft.pids[i];
  }

  for(int i = 0; i < 6; i++){
    toStore[i + 18] = backRight.pids[i];
  }
}

void sguiApp::runMotorTest(){
  std::cout << "Run motor test commencing\n";

  std::cout << "T/D: " << current_td_radio << "\n";

  if(frontLeft.Enabled.get_active()){
    std::cout << "Testing Front Left\n";
  }

  if(frontRight.Enabled.get_active()){
    std::cout << "Testing Front Right\n";
  }

  if(backLeft.Enabled.get_active()){
    std::cout << "Testing Back Left\n";
  }

  if(backRight.Enabled.get_active()){
    std::cout << "Testing Back Right\n";
  }

  signal_detected.emit();
}

//################################################################
//Make prep corners create tab structure!!!!
//Data Tree needs to be made and inserted
//################################################################
void sguiApp::prepCorners(){
  motor_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);

  

  //Radio Button Toggle interface
  rb1 = Gtk::make_managed<Gtk::RadioButton>(turn_drive_toggle, "Turning");
  rb2 = Gtk::make_managed<Gtk::RadioButton>(turn_drive_toggle, "Driving");
  rb1->set_active();

  
  rb1->signal_toggled().connect( sigc::bind(sigc::mem_fun(*this,
              &sguiApp::on_td_radio_clicked), "Turning"));

  rb2->signal_toggled().connect( sigc::bind(sigc::mem_fun(*this,
              &sguiApp::on_td_radio_clicked), "Driving"));

  current_td_radio = "Turning";

  

  toggle_RD_box.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
  toggle_RD_box.add(*rb1);
  toggle_RD_box.add(*rb2);

  motor_Box.add(toggle_RD_box);

  //Run Test Interface
  runTest.set_label("Run");
  test_type.set_label("Angle Test: ");
  fillerText.set_label(" to ");
  runTestBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

  runTest.signal_clicked().connect(sigc::mem_fun(*this, &sguiApp::runMotorTest));
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

  frontLeft.P_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);//Use for spin button config
  frontLeft.I_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);
  frontLeft.D_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);

  frontLeft.P_spin.set_wrap();
  frontLeft.I_spin.set_wrap();
  frontLeft.D_spin.set_wrap();

  frontLeft.P_spin.set_digits(5);
  frontLeft.I_spin.set_digits(5);
  frontLeft.D_spin.set_digits(5);

  frontLeft.P_spin.set_adjustment(frontLeft.P_adjustmentVals);
  frontLeft.I_spin.set_adjustment(frontLeft.I_adjustmentVals);
  frontLeft.D_spin.set_adjustment(frontLeft.D_adjustmentVals);


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

  frontRight.P_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);//Use for spin button config
  frontRight.I_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);
  frontRight.D_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);

  frontRight.P_spin.set_wrap();
  frontRight.I_spin.set_wrap();
  frontRight.D_spin.set_wrap();

  frontRight.P_spin.set_digits(5);
  frontRight.I_spin.set_digits(5);
  frontRight.D_spin.set_digits(5);

  frontRight.P_spin.set_adjustment(frontRight.P_adjustmentVals);
  frontRight.I_spin.set_adjustment(frontRight.I_adjustmentVals);
  frontRight.D_spin.set_adjustment(frontRight.D_adjustmentVals);


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
  
  backLeft.P_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);//Use for spin button config
  backLeft.I_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);
  backLeft.D_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);

  backLeft.P_spin.set_wrap();
  backLeft.I_spin.set_wrap();
  backLeft.D_spin.set_wrap();

  backLeft.P_spin.set_digits(5);
  backLeft.I_spin.set_digits(5);
  backLeft.D_spin.set_digits(5);

  backLeft.P_spin.set_adjustment(backLeft.P_adjustmentVals);
  backLeft.I_spin.set_adjustment(backLeft.I_adjustmentVals);
  backLeft.D_spin.set_adjustment(backLeft.D_adjustmentVals);

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

  backRight.P_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);//Use for spin button config
  backRight.I_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);
  backRight.D_adjustmentVals = Gtk::Adjustment::create(0.0,0.0,1.0,0.0005,0.0005);

  backRight.P_spin.set_wrap();
  backRight.I_spin.set_wrap();
  backRight.D_spin.set_wrap();

  backRight.P_spin.set_digits(5);
  backRight.I_spin.set_digits(5);
  backRight.D_spin.set_digits(5);

  backRight.P_spin.set_adjustment(backRight.P_adjustmentVals);
  backRight.I_spin.set_adjustment(backRight.I_adjustmentVals);
  backRight.D_spin.set_adjustment(backRight.D_adjustmentVals);

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
      //std::cout<<"Current ctr Val: " << i << "\n";
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

void sguiApp::updateMotorView(int* packet){

}

void sguiApp::loadPIDVals(){
  //write current pid to txt
  std::ifstream pidConfig;
  std::cout<<"Trying to read PID config\n";
  pidConfig.open( pid_config_directory + "/configs/pid_configs.txt");
  std::cout<<"We trying: " << pid_config_directory << "/configs/pid_configs.txt\n";
  if (pidConfig.is_open())
  {
    double val;
    std::string line;

    std::getline(pidConfig,line); val = std::stod(line); frontLeft.P_spin.set_value(val); frontLeft.pids[0] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontLeft.I_spin.set_value(val); frontLeft.pids[1] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontLeft.D_spin.set_value(val); frontLeft.pids[2] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontLeft.pids[3] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontLeft.pids[4] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontLeft.pids[5] = val;

    std::getline(pidConfig,line); val = std::stod(line); frontRight.P_spin.set_value(val); frontRight.pids[0] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontRight.I_spin.set_value(val); frontRight.pids[1] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontRight.D_spin.set_value(val); frontRight.pids[2] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontRight.pids[3] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontRight.pids[4] = val;
    std::getline(pidConfig,line); val = std::stod(line); frontRight.pids[5] = val; 
    
    std::getline(pidConfig,line); val = std::stod(line); backLeft.P_spin.set_value(val); backLeft.pids[0] = val;
    std::getline(pidConfig,line); val = std::stod(line); backLeft.I_spin.set_value(val); backLeft.pids[1] = val;
    std::getline(pidConfig,line); val = std::stod(line); backLeft.D_spin.set_value(val); backLeft.pids[2] = val;
    std::getline(pidConfig,line); val = std::stod(line); backLeft.pids[3] = val;
    std::getline(pidConfig,line); val = std::stod(line); backLeft.pids[4] = val;
    std::getline(pidConfig,line); val = std::stod(line); backLeft.pids[5] = val;

    std::getline(pidConfig,line); val = std::stod(line); backRight.P_spin.set_value(val); backRight.pids[0] = val; 
    std::getline(pidConfig,line); val = std::stod(line); backRight.I_spin.set_value(val); backRight.pids[1] = val;
    std::getline(pidConfig,line); val = std::stod(line); backRight.D_spin.set_value(val); backRight.pids[2] = val;
    std::getline(pidConfig,line); val = std::stod(line); backRight.pids[3] = val;
    std::getline(pidConfig,line); val = std::stod(line); backRight.pids[4] = val;
    std::getline(pidConfig,line); val = std::stod(line); backRight.pids[5] = val;
    
    pidConfig.close();
  }
  else{
    std::cout << "Unable to open file, try different directory\n";
  }

}


sguiApp::~sguiApp()
{
  //Print all the named buttons. Can be used to also update txt if i bother to implement that
  // type_children children = refListStore->children();
  // for(type_children::iterator iter = children.begin(); iter != children.end(); ++iter){
  //    Gtk::TreeModel::Row row = *iter;
  //    std::cout<<row[m_Columns.m_col_text] << "\n";
  // }
  // controllerView.show();

  //write current pid to txt
  std::ofstream pidConfig;
  std::cout<<"Trying to write output to PID config\n";
  pidConfig.open( pid_config_directory + "/configs/pid_configs.txt");
  std::cout<<"We trying: " << pid_config_directory << "/configs/pid_configs.txt\n";
  if (pidConfig.is_open())
  {
    double val;
    char charray[16] = {0};

    for(int i = 0; i < 6; i ++){
      val = frontLeft.pids[i]; sprintf(charray,"%2.13f\n",val); pidConfig << charray;
    }

    for(int i = 0; i < 6; i ++){
      val = frontRight.pids[i]; sprintf(charray,"%2.13f\n",val); pidConfig << charray;
    }

    for(int i = 0; i < 6; i ++){
      val = backLeft.pids[i]; sprintf(charray,"%2.13f\n",val); pidConfig << charray;
    }

    for(int i = 0; i < 6; i ++){
      val = backRight.pids[i]; sprintf(charray,"%2.13f\n",val); pidConfig << charray;
    }

    pidConfig.close();
  }
  else{
    std::cout << "Unable to open file, try different directory\n";
  }



  
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

void sguiApp::on_td_radio_clicked(std::string buttonLbl)
{
  current_td_radio = buttonLbl;
  if(rb1->get_active() && buttonLbl == "Driving"){
    return;
  }

  if(rb2->get_active() && buttonLbl == "Turning"){
    return;
  }


  if(buttonLbl == "Driving"){
    std::cout << "Drive\n";
    frontLeft.pids[3] = frontLeft.P_spin.get_value();
    frontLeft.pids[4] = frontLeft.I_spin.get_value();
    frontLeft.pids[5] = frontLeft.D_spin.get_value();
    frontLeft.P_spin.set_value(frontLeft.pids[0]);
    frontLeft.I_spin.set_value(frontLeft.pids[1]);
    frontLeft.D_spin.set_value(frontLeft.pids[2]);

    frontRight.pids[3] = frontRight.P_spin.get_value();
    frontRight.pids[4] = frontRight.I_spin.get_value();
    frontRight.pids[5] = frontRight.D_spin.get_value();
    frontRight.P_spin.set_value(frontRight.pids[0]);
    frontRight.I_spin.set_value(frontRight.pids[1]);
    frontRight.D_spin.set_value(frontRight.pids[2]);

    backLeft.pids[3] = backLeft.P_spin.get_value();
    backLeft.pids[4] = backLeft.I_spin.get_value();
    backLeft.pids[5] = backLeft.D_spin.get_value();
    backLeft.P_spin.set_value(backLeft.pids[0]);
    backLeft.I_spin.set_value(backLeft.pids[1]);
    backLeft.D_spin.set_value(backLeft.pids[2]);

    backRight.pids[3] = backRight.P_spin.get_value();
    backRight.pids[4] = backRight.I_spin.get_value();
    backRight.pids[5] = backRight.D_spin.get_value();
    backRight.P_spin.set_value(backRight.pids[0]);
    backRight.I_spin.set_value(backRight.pids[1]);
    backRight.D_spin.set_value(backRight.pids[2]);
  }
  else if(buttonLbl == "Turning"){
    std::cout << "Turn\n";
    frontLeft.pids[0] = frontLeft.P_spin.get_value();
    frontLeft.pids[1] = frontLeft.I_spin.get_value();
    frontLeft.pids[2] = frontLeft.D_spin.get_value();
    frontLeft.P_spin.set_value(frontLeft.pids[3]);
    frontLeft.I_spin.set_value(frontLeft.pids[4]);
    frontLeft.D_spin.set_value(frontLeft.pids[5]);

    frontRight.pids[0] = frontRight.P_spin.get_value();
    frontRight.pids[1] = frontRight.I_spin.get_value();
    frontRight.pids[2] = frontRight.D_spin.get_value();
    frontRight.P_spin.set_value(frontRight.pids[3]);
    frontRight.I_spin.set_value(frontRight.pids[4]);
    frontRight.D_spin.set_value(frontRight.pids[5]);

    backLeft.pids[0] = backLeft.P_spin.get_value();
    backLeft.pids[1] = backLeft.I_spin.get_value();
    backLeft.pids[2] = backLeft.D_spin.get_value();
    backLeft.P_spin.set_value(backLeft.pids[3]);
    backLeft.I_spin.set_value(backLeft.pids[4]);
    backLeft.D_spin.set_value(backLeft.pids[5]);

    backRight.pids[0] = backRight.P_spin.get_value();
    backRight.pids[1] = backRight.I_spin.get_value();
    backRight.pids[2] = backRight.D_spin.get_value();
    backRight.P_spin.set_value(backRight.pids[3]);
    backRight.I_spin.set_value(backRight.pids[4]);
    backRight.D_spin.set_value(backRight.pids[5]);
  }
}

void sguiApp::say(std::string input){
  sguiApp::b1.set_label(Glib::ustring(input));
}