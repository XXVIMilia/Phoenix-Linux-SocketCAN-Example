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
  //For pausing and restarting sigc
  sigc::slot<bool> joystickSlot;
  sigc::connection joystickHandler;

  class ModelColumns : public Gtk::TreeModelColumnRecord{
    public:

    ModelColumns()
      { add(m_col_text); add(m_col_number); }

    Gtk::TreeModelColumn<Glib::ustring> m_col_text;
    Gtk::TreeModelColumn<int> m_col_number;
  };

protected:
  //Signal handlers:
  virtual void on_button_clicked(std::string buttonLbl);

  //Member widgets:
  Gtk::Notebook m_note;
  Gtk::Box motor_Box,main_Box,controller_Box;
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
