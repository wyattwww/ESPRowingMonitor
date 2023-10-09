#pragma once

#include "configuration.h"
#include "series.h"

using std::vector;

class OLSLinearSeries
{
    unsigned char maxSeriesLength = 0;
    Series sumX;
    Series sumXSquare;
    Series sumY;
    Series sumYSquare;
    Series sumXY;

public:
    explicit OLSLinearSeries(unsigned char _maxSeriesLength = 0);

    Configurations::precision yAtSeriesBegin() const;
    Configurations::precision yAtSeriesEnd() const;
    Configurations::precision slope() const;
    Configurations::precision intercept() const;
    Configurations::precision goodnessOfFit() const;
    Configurations::precision projectX(Configurations::precision x);
    unsigned char size() const;


    void push(Configurations::precision pointX, Configurations::precision pointY);
    void reset();
};