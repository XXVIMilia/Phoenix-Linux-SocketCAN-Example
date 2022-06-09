#ifndef SGUI_H
#define SGUI_H

#include <gtkmm.h>

class sguiApp : public Gtk::Window
{

public:
  sguiApp();
  virtual ~sguiApp();
  void loadConfigs(std::string directory);
  void say(std::string input);
  int updateTime();
  void setUpControllerView(int rows);
  void updateControllerView(int* packet);
  void updateMotorView(int* packet);
  void prepCorners();
  void loadPIDVals();
  void runMotorTest();
  
  //For pausing and restarting sigc
  sigc::slot<bool> joystickSlot;
  sigc::connection joystickHandler;

  sigc::slot<bool> motorViewSlot;
  sigc::connection motorViewHandler;

  std::string pid_config_directory;


  class ModelColumns : public Gtk::TreeModelColumnRecord{
    public:

    ModelColumns()
      { add(m_col_text); add(m_col_number);}

    Gtk::TreeModelColumn<Glib::ustring> m_col_text;
    Gtk::TreeModelColumn<int> m_col_number;
  };

 
protected:
  //Signal handlers:
  virtual void on_button_clicked(std::string buttonLbl);
  void on_td_radio_clicked(std::string buttonLbl);
  std::string current_td_radio;
  //Member widgets:
  Gtk::Notebook m_note;
  Gtk::Box motor_Box,main_Box,controller_Box,music_Box;//Box to hold each notebook tab

  //Components in main Tab
  Gtk::ToggleButton drive_btn;
  Gtk::Label ctr_status, motor_status, drive_status;

  //Components in motor tab
  Gtk::Grid motorView;

  Gtk::Frame fl_frame, fr_frame, bl_frame, br_frame, motorView_frame;

  Gtk::Box toggle_RD_box;
  Gtk::RadioButton::Group turn_drive_toggle;
  
  //Testing Interface
  Gtk::Box runTestBox;
  Gtk::Button runTest;
  Gtk::Label test_type, fillerText;
  Gtk::Entry startVal, stopVal;




  struct corner{
    Gtk::Label cornerName;

    //Struct Containers
    Gtk::Box MainHolder, PID_Data_Pair;
    Gtk::Box PID_box, P_pair, I_pair, D_pair, corner_name_box;
    Gtk::Box Telemetry;

    //Struct Interface
    Gtk::CheckButton Enabled;
    Gtk::SpinButton P_spin, I_spin, D_spin;
    Gtk::Label P_name, I_name, D_name;
    Glib::RefPtr<Gtk::Adjustment> P_adjustmentVals;
    Glib::RefPtr<Gtk::Adjustment> I_adjustmentVals;
    Glib::RefPtr<Gtk::Adjustment> D_adjustmentVals;

    //Struct Data
    Gtk::TreeView Tele;
    ModelColumns m_Columns;
    Glib::RefPtr<Gtk::ListStore> dataListStore;
    Gtk::TreeModel::iterator data_iter;
  };

  typedef struct corner corner;
  corner frontLeft, frontRight, backLeft, backRight;


  Gtk::Button b1,b2,b3;
  Gtk::Box buttons, Child_widgets;
  Gtk::TreeView controllerView;
  ModelColumns m_Columns;
  Glib::RefPtr<Gtk::ListStore> refListStore;
  Gtk::TreeModel::iterator controller_iter;
  typedef Gtk::TreeModel::Children type_children; 


  //For timing
  struct timeval pastTime;
  int delTime;

  bool signalPaused;


  

};

#endif // SGUI_H
