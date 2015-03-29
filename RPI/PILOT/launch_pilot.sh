#!/bin/bash

#Led and button control GPIO pins
LED=0
button=2


# trap ctrl-c and call ctrl_c()
trap ctrl_c INT

function setup(){
    gpio mode $1 out
    gpio mode $2 in
}

function set_off(){
    gpio write $1 0
}

function set_on(){
    gpio write $1 1
}


function Blink_fast()
{
   for i in {1..20}; do
	set_off $1
	sleep 0.05
	set_on $1
	sleep 0.05
    done
}

function Blink_slow()
{
   for i in {1..3}; do
	set_off $1
	sleep 0.5
	set_on $1
	sleep 0.5
    done
}

function waitButton ()
{

    Blink_fast $LED;
    echo ""
 echo "Waiting for button ..."
 while [ `gpio read $1` = 0 ]; do
   sleep 0.1
 done

 echo "Button pressed : stay pressed for shutdown"
 n=0
 while [ `gpio read $1` = 1 ]; do
   sleep 0.1
   n=`expr $n + 1`

 if [ "$n" -ge "10" ]; then
     echo "Bye Bye !"
     stop_pilot
     Blink_slow $LED
     sudo halt
     exit 0
 fi
 done

 echo "Restarting Pilot"
 stop_pilot
 sleep 0.5
 start_pilot;

 #recursive call
 waitButton $1;

}

function stop_pilot() {
        sudo pkill quad_pilot;
	set_off $LED;
}

function start_pilot() {
rate_kp=1.4
rate_ki=0.01
rate_kd=0.02

stab_kp=3.0
stab_ki=0.01
stab_kd=0.5

yaw_rate=2.5
( sudo /home/pi/QUADCOPTER_V2/RPI/PILOT/quad_pilot $rate_kp $rate_ki $rate_kd\
       $stab_kp $stab_ki $stab_kd $yaw_rate ) &
}


function ctrl_c() {
        echo "*** Trapped CTRL-C ***"
	stop_pilot;
	exit 0
}

#MAIN --------------------------

#setup LED and button to niputs
setup $LED $button

#launching quad_pilot
start_pilot

#start the button pressed watching
waitButton $button


wait
