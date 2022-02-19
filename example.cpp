#define Phoenix_No_WPI // remove WPI dependencies
#include "ctre/Phoenix.h"
#include "ctre/phoenix/platform/Platform.h"
#include "ctre/phoenix/unmanaged/Unmanaged.h"
#include "ctre/phoenix/cci/Unmanaged_CCI.h"
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <gtkmm/application.h>
#include "swerveGUI/sgui.h"
#include <sigc++/sigc++.h>
#include <glibmm.h>
#include "ctre/phoenix/music/Orchestra.h"
#include "ctre/phoenix/ErrorCode.h"



using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;




/* make some talons for drive train */
//TalonSRX talLeft(1);
TalonFX talRight(0);
TalonFX tal(3);


//Joystick Variables
SDL_Joystick *joy;
const char *name;
int num_axes;
int num_buttons;
int num_hats;
int *joyVals;





void initDrive()
{
	/* both talons should blink green when driving forward */
	talRight.SetInverted(true);
	//talon uses can
}

void drive(double fwd, double turn)
{
	double left = fwd - turn;
	double right = fwd + turn; /* positive turn means turn robot LEFT */

	//talLeft.Set(ControlMode::PercentOutput, left);
	//talright.Set(ControlMode::PercentOutput, right);
}
/** simple wrapper for code cleanup */


bool getFPS(sguiApp* myWindow){
	std::string val = std::to_string(myWindow->updateTime());
	myWindow->say(val);
	return true;
}

bool driveController(int *joybuff,sguiApp *app){
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) { return false; }
				if (event.jdevice.type == SDL_JOYDEVICEREMOVED) { return false; }
			}
		for(int i = 0; i < num_axes;i++){
			joybuff[i] = SDL_JoystickGetAxis(joy, i);
		}
		for(int i = 0; i < num_buttons; i++){
			joybuff[i + num_axes] = SDL_JoystickGetButton(joy,i);
		}

		try{
			app->updateControllerView(joybuff);
			return true;
		}
		catch(...){
			return false;
		}
}

int prepController(){
	while(true){
		while (true) {
			/* SDL seems somewhat fragile, shut it down and bring it up */
			SDL_Quit();
            SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
    		SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
			SDL_Init(SDL_INIT_JOYSTICK);

			/* poll for gamepad */
			int res = SDL_NumJoysticks();
			if (res > 0) { break; }
			if (res < 0) { printf("Err = %i\n", res); }

			/* yield for a bit */
			usleep(20000);
		}
		printf("Waiting for gamepad...Found one\n");
			// Open the joystick for reading and store its handle in the joy variable
		joy = SDL_JoystickOpen(0);
		if (joy == NULL) {
				/* back to top of while loop */
				continue;
		}

		// Get information about the joystick
		name = SDL_JoystickName(joy);
		num_axes = SDL_JoystickNumAxes(joy);
		num_buttons = SDL_JoystickNumButtons(joy);
		num_hats = SDL_JoystickNumHats(joy);

		
		printf("Now reading from joystick '%s' with:\n"
			"%d axes\n"
			"%d buttons\n"
			"%d hats\n\n",
			name,
			num_axes,
			num_buttons,
			num_hats);

		
		
		return(num_axes + num_buttons);

	}
	
}


int main(int argc, char *argv[]) {
	/* don't bother prompting, just use can0 */
	//std::cout << "Please input the name of your can interface: ";
	std::string interface;
	//std::cin >> interface;
	interface = "can0";
	ctre::phoenix::platform::can::SetCANInterface(interface.c_str());// Uncomment later
	
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	// c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	/* setup drive */
	initDrive(); //Uncomment later
	ctre::phoenix::music::Orchestra myOrc;
	myOrc.AddInstrument(talRight);
	std::string filePath = "Something";
	myOrc.LoadMusic(filePath);
	myOrc.Play();


	

	while (false) { // Switch to true later
		/* we are looking for gamepad (first time or after disconnect),
			neutral drive until gamepad (re)connected. */
		drive(0, 0);

		// wait for gamepad
		printf("Waiting for gamepad...\n");
		
	}

	//SDL_Quit();


	auto app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");

	//sgui App
	sguiApp sgui;
	int num = prepController();
	sgui.setUpControllerView(num);
	char tmp[256];
    getcwd(tmp, 256);
	std::cout<< "Current Directory: " << tmp << "\n";
	sgui.loadConfigs(tmp);
	std::cout<<"I am gonna run\n";
	joyVals = new int[num];


	//used to create a reoccurring signal
	sigc::slot<bool> my_slot = bind(sigc::ptr_fun(driveController), joyVals ,&sgui);
  	sigc::connection updater = Glib::signal_timeout().connect(my_slot,20);

	sgui.joystickSlot = my_slot;
	sgui.joystickHandler = updater;

	app->run(sgui);
	SDL_JoystickClose(joy);
	delete joyVals;

	return 0;
}
