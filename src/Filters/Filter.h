#ifndef FILTER_H
#define FILTER_H

class Filter {
public:
    virtual ~Filter() {}

    // Define core interface
    virtual void initialize() = 0;
    virtual double* iterate(double dt, double* state, double* measurements, double* controlVars) = 0;
    
    // Provide interface to query filter dimensions
    virtual int getMeasurementSize() const = 0;
    virtual int getInputSize() const = 0;
};


#endif // FILTER_H