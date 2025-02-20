#include "ols-linear-series.h"

OLSLinearSeries::OLSLinearSeries(const unsigned char _maxSeriesLength) : maxSeriesLength(_maxSeriesLength),
                                                                         sumX(_maxSeriesLength),
                                                                         sumXSquare(_maxSeriesLength),
                                                                         sumY(_maxSeriesLength),
                                                                         sumYSquare(_maxSeriesLength),
                                                                         sumXY(_maxSeriesLength) {}

void OLSLinearSeries::reset()
{
    sumX.reset();
    sumXSquare.reset();
    sumY.reset();
    sumYSquare.reset();
    sumXY.reset();
}

void OLSLinearSeries::push(const Configurations::precision pointX, const Configurations::precision pointY)
{
    sumX.push(pointX);
    sumXSquare.push(pointX * pointX);
    sumY.push(pointY);
    sumYSquare.push(pointY * pointY);
    sumXY.push(pointX * pointY);
}

Configurations::precision OLSLinearSeries::yAtSeriesBegin() const
{
    return sumY[0];
}

Configurations::precision OLSLinearSeries::yAtSeriesEnd() const
{
    if (sumY.size() > 0) {
      return sumY[sumY.size() - 1];
    } 
    else 
    {
      return 0;
    }
}

Configurations::precision OLSLinearSeries::slope() const
{
    if (sumX.size() >= 2 && sumX.sum() > 0)
    {
        return ((Configurations::precision)sumX.size() * sumXY.sum() - sumX.sum() * sumY.sum()) / ((Configurations::precision)sumX.size() * sumXSquare.sum() - sumX.sum() * sumX.sum());
    }

    return 0.0;
}

Configurations::precision OLSLinearSeries::intercept() const
{
    if (sumX.size() >= 2 && sumX.sum() > 0 && sumY.size() >= 2 && sumY.sum() > 0)
    {
        return ((sumY.sum() - (slope() * sumX.sum())) / sumX.size());
    }

    return 0.0;
}


Configurations::precision OLSLinearSeries::goodnessOfFit() const
{
    // This function returns the R^2 as a goodness of fit indicator
    if (sumX.size() >= 2 && sumX.sum() > 0)
    {
        auto const slope = ((Configurations::precision)sumX.size() * sumXY.sum() - sumX.sum() * sumY.sum()) / ((Configurations::precision)sumX.size() * sumXSquare.sum() - sumX.sum() * sumX.sum());
        auto const intercept = (sumY.sum() - (slope * sumX.sum())) / (Configurations::precision)sumX.size();
        auto const sse = sumYSquare.sum() - (intercept * sumY.sum()) - (slope * sumXY.sum());
        auto const sst = sumYSquare.sum() - (sumY.sum() * sumY.sum()) / (Configurations::precision)sumX.size();
        return 1 - (sse / sst);
    }

    return 0;
}

Configurations::precision OLSLinearSeries::projectX(Configurations::precision x) {
    if (sumX.size() >= 2) 
    {
      return (slope() * x) + intercept();
    } 
    else 
    {
      return 0;
    }
  }

unsigned char OLSLinearSeries::size() const
{
    return sumY.size();
}