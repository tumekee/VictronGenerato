# Automatic-Generator
automatically start and stop a generator with a simple on off signal

This code has been specifically designed to work with Victron a inverter that has programmable I/O pins and a Cerbo GX.


![info](https://github.com/tumekee/VictronGenerator/assets/20364311/0082b7a7-cda3-472e-838d-efcc58ef6d83)



This system is designed for generators that don’t have a “2 PIN” start switch. These systems normally work by simply bridging 2 pins or wires to tell the the generator to start or stop via its own starting and stopping conditions and procedure. This works well with a Victron Cerbo as the generator START/STOP function only opens and closes a relay.
The problem is most generators aren’t that smart and will only have the same controls as a normal electric start engine; ON, START , OFF ( like a key start ), often via an external starting plug or “ATS” plug.  simply opening and closing a relay is not enough to start these types of generators as there are a lot of factors to consider when starting and stopping a generator.
•	Turning the engine to “on”
•	Starting times.
•	Generator running detection. 
•	Disconnecting loads before shutting down 
•	(and more ) 
 
This code has been specifically designed to work with Victron inverter that has programmable I/O pins and a Cerbo GX.
The Cerbo acts as the on / off switch which is a requesting signal to START or STOP the generator. 
The Quattro is used to disconnect the loads before shutdown (assuming the generator is wired into the quattro) and provide a signal to say that AC voltage is present i.e. the generator is running. 

You will need to be able to program the quattro’s “assistants” either via USB or VRM 


general flag user (size:5)
*)	Use general flag to ignore AC input 1

programmable relay (size:176)
*)	Use General Flag
*)	Set relay off
*)	when Auxiliary input 1 is open for 2 seconds.

programmable relay (1) (size:37)
*)	Use General Flag
*)	Set relay on
*)	when Auxiliary input 1 is closed for 2 seconds.

programmable relay (2) (size:35)
*)	Use secondary programmable relay K2.
*)	Set relay on
*)	when AC1 available for 1 seconds.

programmable relay (3) (size:38)
*)	Use secondary programmable relay K2.
*)	Set relay off
*)	when AC1 not available for 1 seconds.


what the script does 

Checking for AC Power:
 
•	The system checks if there is AC power available or if the requesting signal to start the generator has been turned off.

•	If either of these conditions is true, the system performs additional checks.

AC Power Check:
 
•	If there is AC power present at the quattro

•	If the system is not already in the ON state, it changes the status to ON.

•	This indicates that the generator not be started because AC power is now present.

Requesting Signal Check:

•	If the requesting signal to start the generator has been turned off, it means the generator should not be started.

•	The system acknowledges that the requesting signal is off.

Start Attempts Check:

•	The system checks if the number of start attempts has exceeded three.

•	If it has, the system prevents further start attempts.

•	This avoids potential issues and ensures the generator is not started excessively.

Starting Error Check:

•	The system checks if there was a previous error during the starting process.

•	If there was, the system cancels further start attempts.

•	This prevents any potential issues caused by repeating a failed starting process.

AC Loss Check:

•	The system checks if AC power was previously detected but is no longer present.

•	If AC power is lost, it indicates that the generator might be running without producing power, which could cause damage.

Starting Error:

•	If there was a previous error during the starting process, the system enters the "starting_error" state.

•	This error indicates that the generator was unable to start successfully.

•	To clear this error, follow these steps:

•	After performing the physical checks, reset the system ONCE ( off then on again ) . This clears the error state.

•	Once the error is cleared, you can attempt to start the generator again.

AC Loss Error:

•	If AC power was previously detected but is no longer present, the system enters the " ACTUAL_ON_ERROR” state.

•	This error indicates that the generator may be running without producing power, which can cause serious damage.

•	To resolve this error, follow these steps:

•	Physically check the generator:

	Ensure that the generator is not running.
   
	Check if the breaker or any safety mechanisms are tripped.
   
	Verify that the diesel fuel level is sufficient.
   
•	Reset the system twice to clear the error state ( on off on  off )

•	After clearing the error, you can attempt to start the generator again.




Shutdown Sequence:

a. When the shutdown process is initiated, the system starts the shutdown sequence to safely turn off the generator.

b. The system performs disconnect the loads connected to the generator. This ensures that the power supply to the loads is interrupted before shutting down the generator.

c. The system handles this load disconnection automatically to prevent power surges or potential damage to connected devices.

Generator Shutdown:

a. Once the loads are safely disconnected, the system proceeds to shut down the generator.

b. It activates relay which goes to AUX1 on the quttro - this then triggers the general flag which disconnects all AC loads from the inverts AC1 input  

c. The generator's engine is turned off after 30 seconds as the AC load disconnect can take sometime ( somtimes )

Shutdown Completion:

a. Once the generator is completely shut down, the system confirms the shutdown process.

b. At this point, the generator is no longer running, and the system is in a standby state.
