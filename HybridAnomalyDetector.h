

#ifndef HYBRIDANOMALYDETECTOR_H_
#define HYBRIDANOMALYDETECTOR_H_

#include "SimpleAnomalyDetector.h"
#include "minCircle.h"

class HybridAnomalyDetector:public SimpleAnomalyDetector {
protected:
    float circleThreshold;
public:
	HybridAnomalyDetector();
    HybridAnomalyDetector(float threshold);
	virtual ~HybridAnomalyDetector();
    //helper method.
protected:
    virtual void learnHelper(const TimeSeries& ts,float p/*pearson*/,string f1, string f2,Point** ps) override;
    virtual bool isAnomalous(float x, float y,correlatedFeatures c) override;

};

#endif
