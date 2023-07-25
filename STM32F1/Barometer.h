//
// Created by kking on 7/24/2023.
//

#ifndef MULTIMISSIONLIBRARY_BAROMETER_H
#define MULTIMISSIONLIBRARY_BAROMETER_H


class Barometer {
public:
    virtual ~Barometer() {}; //Virtual descructor. Very important
    virtual void calibrate() = 0; //Virtual functions set equal to zero are "pure virtual functions". Required
    virtual double get_pressure() = 0;
    virtual double get_temp() = 0;
    virtual double get_temp_f() = 0;
    virtual double get_pressure_atm() = 0;
    virtual double get_rel_alt_ft() = 0;
};


#endif //MULTIMISSIONLIBRARY_BAROMETER_H
