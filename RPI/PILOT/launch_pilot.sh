#!/bin/bash


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

waitButton ()
{
 echo "Waiting for button ..."
 while [ `gpio read $1` = 1 ]; do
   sleep 0.1
 done
 echo "Button pressed : Relaunching"

 stop_pilot

 sleep 0.5

 ./launch_pilot.sh;
}

function stop_pilot() {
        sudo pkill quad_pilot;
	set_off 15;
}


function ctrl_c() {
        echo "*** Trapped CTRL-C ***"
	sudo pkill quad_pilot;
	set_off 15;
	exit 0
}

#launching quad_pilot
rate_kp=1.4
rate_ki=0.01
rate_kd=0.02

stab_kp=3.0
stab_ki=0.01
stab_kd=0.5

yaw_rate=2.5
( sudo ./quad_pilot $rate_kp $rate_ki $rate_kd\
       $stab_kp $stab_ki $stab_kd $yaw_rate ) &


#Led and button control
LED=15
button=16

setup $LED $button
set_on $LED

waitButton $button

wait
