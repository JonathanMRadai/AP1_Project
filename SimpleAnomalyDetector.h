
#ifndef SIMPLEANOMALYDETECTOR_H_
#define SIMPLEANOMALYDETECTOR_H_

#include "anomaly_detection_util.h"
#include "AnomalyDetector.h"
#include <vector>
#include <algorithm>
#include <string.h>
#include <math.h>

struct correlatedFeatures{
	string feature1,feature2;  // names of the correlated features
	float corrlation;
	Line lin_reg;
	float threshold;
	Point* center;
};


class SimpleAnomalyDetector:public TimeSeriesAnomalyDetector{
protected:
	vector<correlatedFeatures> cf;
	float threshold;
public:
	SimpleAnomalyDetector();
    SimpleAnomalyDetector(float threshold);
	virtual ~SimpleAnomalyDetector();

	virtual void learnNormal(const TimeSeries& ts);
	virtual vector<AnomalyReport> detect(const TimeSeries& ts);
	vector<correlatedFeatures> getNormalModel(){
		return cf;
	}
	void setCorrelationThreshold(float threshold){
		this->threshold=threshold;
	}
    const float getThreshold() const {
        return threshold;
    }

	// helper methods
protected:
	virtual void learnHelper(const TimeSeries& ts,float p/*pearson*/,string f1, string f2,Point** ps);
	virtual bool isAnomalous(float x, float y,correlatedFeatures c);
	Point** toPoints(vector<float> x, vector<float> y);
	float findThreshold(Point** ps,size_t len,Line rl);
};



#endif
