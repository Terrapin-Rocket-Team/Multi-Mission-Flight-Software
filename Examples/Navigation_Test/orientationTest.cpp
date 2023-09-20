//test absolute orientation(see if it gets set or not)
//test euler angle transformatio(see if match axis that we used for calcualtions)
#include "BNO055.cpp"

//not sure how to set up object to call all the neccesary functions

void setup(){
    //setupBN055();
    //calibrate_bno();

}
void loop(){
    imu::Quaternion q = bno.getBNO055absoluteOrientation(); //function from BNO55.cpp
    //quatToXYZ();
    delay(200);
    //need to display both and compate
}