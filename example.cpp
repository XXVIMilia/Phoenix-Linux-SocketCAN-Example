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
#include <math.h>



using namespace ctre::phoenix;
using namespace ctre::phoenix::platform;
using namespace ctre::phoenix::motorcontrol;
using namespace ctre::phoenix::motorcontrol::can;




/* make some talons for drive train
	Swerve Naming Convention:
	[F,B][L,R][D,R]
	[Front,Back][Left,Right][Drive,Rotate]
	e.g. talFLD = tal Front Left Drive

	Additionally:
	Motor CAN IDs are decided clockwise with FRD == 0 and FLR == 7
*/
TalonFX talFRD(0);
TalonFX talFRR(1);
TalonFX talBRD(2);
TalonFX talBRR(3);
TalonFX talBLD(4);
TalonFX talBLR(5);
TalonFX talFLD(6);
TalonFX talFLR(7);

CANCoder canFR(0);
CANCoder canBR(1);
CANCoder canBL(2);
CANCoder canFL(3);


//Joystick Variables
bool controller_status;
SDL_Joystick *joy;
SDL_GameController *GC;
const char *name;
int num_axes;
int num_buttons;
int num_hats;
int num_Sensors;
int *joyVals;





void initDrive()
{
	/* both talons should blink green when driving forward */
	//talBLD.SetInverted(true);
	//talon uses can
}


float linearMap(float x,int in_min,int in_max, int out_min,int out_max){
	float temp = (((x-in_min)*(out_max-out_min)) / (in_max-in_min) ) + out_min;
	return(roundf(temp) * 100 / 100);
}

float getAngle(float x,float y){
	float pi = 3.14159;
	float temp = 0.0;
	if(x == 0 && y==0){
		return(0);
	}
	else if(x > 0.0 && y >= 0.0){
		temp  = atan(y/x) * (180.0 / pi);
		return(temp);
	}
	else if(x <= 0.0 && y > 0.0){
		temp = (atan(y/x) + pi) * (180.0 / pi);
		return(temp);
	}
	else if(x < 0.0 && y <= 0.0){
		temp = (atan(y/x) + pi) * (180.0 / pi);
		return(temp);
	}
	else if (x >= 0.0 && y < 0.0){
		temp = (atan(y/x) + 2*pi)  * (180.0 / pi);
		return(temp);
	}
	else{
		std::cout<<"Error on Get Angle!\n";
		return(temp);
	}
	{
		/* code */
	}
	
}


void drive(float x, float y)
{
	const double encMin = 0;
	const double encMax = 4096;
	const double angleMin = 0;
	const double angleMax = 360;
	float magnitude = sqrt(pow(x,2.0) + pow(y,2.0));
	float angle = getAngle(x,y); 

	double dangle = linearMap(angle,angleMin,angleMax,encMin,encMax);

	std::cout<< "Magnitude: " << magnitude << " enc angle: " << dangle << "\n";
	ctre::phoenix::unmanaged::FeedEnable(1000);
	//talBLR.Set(ControlMode::Position,dangle);


	// talBLR.set(ControlMode::Position,)
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
			joybuff[i] = SDL_GameControllerGetAxis(GC,(SDL_GameControllerAxis)i);
		}
		for(int i = 0; i < num_buttons; i++){
			joybuff[i + num_axes] = SDL_GameControllerGetButton(GC,(SDL_GameControllerButton)i);
		}

		try{
			app->updateControllerView(joybuff);
			return true;
		}
		catch(...){
			return false;
		}
}

bool driveMotorGuiTab(sguiApp *app){
	return(true);
}

int prepController(){

		
			/* SDL seems somewhat fragile, shut it down and bring it up */
			SDL_Quit();
            SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1"); //so Ctrl-C still works
    		SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
			// SDL_SetHint(SDL_HINT_JOYSTICK_HIDAPI_PS4, "1");
			SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_SENSOR);

			/* poll for gamepad */
			int res = SDL_NumJoysticks();
			
			if (res < 0) {
				std::cout<<"Joy Error\n"; 
				return -1; 
			}

			/* yield for a bit */
			usleep(20000);
		
				
		GC = SDL_GameControllerOpen(0);
		joy = SDL_JoystickFromPlayerIndex(0);

		if(SDL_GameControllerSetSensorEnabled(GC,SDL_SENSOR_GYRO,SDL_TRUE) < 0){
			std::cout<<"Sensor Error!\n";
		}

		if (GC == NULL) {
				/* back to top of while loop */
				std::cout<<"No Game Controller Found\n";
				return(-1);
		}

		// Get information about the joystick
		name = SDL_JoystickName(joy);

		num_axes = 0;
		SDL_GameControllerAxis temp;
		do{
			temp = (SDL_GameControllerAxis)num_axes;
			if(temp != SDL_CONTROLLER_AXIS_INVALID and temp != SDL_CONTROLLER_AXIS_MAX){
				num_axes++;
			}
		}while(temp != SDL_CONTROLLER_AXIS_MAX);


		num_buttons = 0;
		SDL_GameControllerButton temp2;
		do{
			temp2 = (SDL_GameControllerButton)num_buttons;
			if(temp2 != SDL_CONTROLLER_BUTTON_INVALID and temp2 != SDL_CONTROLLER_BUTTON_MAX){
				num_buttons++;
			}
		}while(temp2 != SDL_CONTROLLER_BUTTON_MAX);
		
		
	
		

		
		printf("Now reading from joystick '%s' with:\n"
			"%d axes\n"
			"%d buttons\n"
			"%d hats\n\n",
			name,
			num_axes,
			num_buttons);


		// Get information about the joystick
		name = SDL_GameControllerName(GC);
		//num_axes = SDL_GameControllerNumAxes(GC);
		//num_buttons = SDL_GameControllerNumButtons(GC);
		std::string map = SDL_GameControllerMapping(GC);
		

		
		printf("Now reading from Game Controller '%s' with:\n",
			name
			);

		std::cout<< "\n" <<map << "\n";

		
		//SDL_free();
		return(num_axes + num_buttons);
	
}

void runPIDTest(sguiApp* myApp){
	std::cout << "I am running a PID test\n";
	double pidVals[12];

	myApp->getPIDCoefs(pidVals);

	std::cout<< "Printing PIDS\n";
	for(int i = 0; i < 12; i++){
		std::cout<< pidVals[i] << "\n";
	}

	talBLR.Set(ControlMode::PercentOutput, 0.1);
	ctre::phoenix::unmanaged::FeedEnable(1000);
	usleep(2000000);
	talBLR.Set(ControlMode::PercentOutput, 0.0);

	
	talBLR.Config_kF(0,0.0,0);
	talBLR.Config_kP(0,pidVals[7],0);
	talBLR.Config_kI(0,pidVals[8],0);
	talBLR.Config_kD(0,pidVals[9],0);
	talBLR.ConfigClosedloopRamp(0.5);

	for(double deg = 0; deg < 9000; deg+=90){
		float x = 0.15 * cos((deg * 3.1415)/180.0);
		float y = 0.15 * sin((deg * 3.1415)/180.0);
		std::cout << "Expected mag: 15 Expected deg: " << deg << "\n";
		drive(x,y);
		usleep(1000000);
	}
	

}


int main(int argc, char *argv[]) {
	/* don't bother prompting, just use can0 */
	//std::cout << "Please input the name of your can interface: ";
	std::string interface;
	//std::cin >> interface;
	interface = "can0";
	//ctre::phoenix::platform::can::SetCANInterface(interface.c_str());// Uncomment later
	
	// Comment out the call if you would rather use the automatically running diag-server, note this requires uninstalling diagnostics from Tuner. 
	// c_SetPhoenixDiagnosticsStartTime(-1); // disable diag server, instead we will use the diag server stand alone application that Tuner installs

	/* setup drive */
	//initDrive(); //Uncomment later
	//ctre::phoenix::music::Orchestra myOrc;
	std::string name = "/home/pi/sguiCode/Phoenix-Linux-SocketCAN-Example/reeeee.chrp";
	std::cout << name << "\n";
	

	// std::cout << "Adding motor\n";

	// if(myOrc.AddInstrument(talBLD)){
	// 	std::cout << "some thing\n";
	// }
	// sleep(1);

	// if(myOrc.LoadMusic(name)){
	// 	std::cout << "uh oh\n";
	// }
	// sleep(1);

	// if(myOrc.Play()){
	// 	std::cout << "went wrong\n";
	// }

	// while(myOrc.IsPlaying()){}

	

	while (false) { // Switch to true later
		/* we are looking for gamepad (first time or after disconnect),
			neutral drive until gamepad (re)connected. */
		//drive(0, 0);

		// wait for gamepad
		printf("Waiting for gamepad...\n");
		
	}
		


	//SDL_Quit();


	auto app =
    Gtk::Application::create(argc, argv,
      "org.gtkmm.examples.base");

	//sgui App
	sguiApp sgui;

	std::cout << "Trying to set up controller\n";
	int num = prepController();//Uses sdl to look at plugged in controller and find number of buttons
	if(num == -1){
		controller_status = false;
	}
	else{
		controller_status = true;
	}


	//Tries to construct the motor view tab in the window
	std::cout << "Setting up window\n";
	sgui.prepCorners();

	char tmp[256];
    getcwd(tmp, 256);
	std::cout<< "Current Directory: " << tmp << "\n";
	sgui.pid_config_directory = tmp;

	sgui.loadPIDVals();

	//Only load crontroller values if theres a valid controller found
	if(controller_status){
		sgui.setUpControllerView(num);//Sets up controller tab in view
		
		
		sgui.loadConfigs(tmp);
		std::cout<<"I am gonna run\n";
		joyVals = new int[num];
	
		//used to create a reoccurring signal for controller updating
		sigc::slot<bool> my_slot = bind(sigc::ptr_fun(driveController), joyVals ,&sgui);
  		sigc::connection updater = Glib::signal_timeout().connect(my_slot,20);

		//Gives information to sgui class to destroy and recreate the reoccuring signal
		sgui.joystickSlot = my_slot;
		sgui.joystickHandler = updater;
	}

	sigc::slot<void> motorTestSlot = bind(sigc::ptr_fun(runPIDTest),&sgui);
	sgui.signal_detected.connect(motorTestSlot);

	talBLR.ConfigSelectedFeedbackSensor(TalonFXFeedbackDevice::IntegratedSensor,0,10);
	talBLR.SetSensorPhase(true);

	std::cout << "Gonna run\n";
	app->run(sgui);
	SDL_JoystickClose(joy);
	if(controller_status){
		delete joyVals;
	}
	

	return 0;
}
