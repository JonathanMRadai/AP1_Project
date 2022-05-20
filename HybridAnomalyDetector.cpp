
#include "HybridAnomalyDetector.h"

HybridAnomalyDetector::HybridAnomalyDetector() {
    circleThreshold = 0.5;
}

HybridAnomalyDetector::HybridAnomalyDetector(float threshold) : SimpleAnomalyDetector(threshold) {
    circleThreshold = 0.5;
}

HybridAnomalyDetector::~HybridAnomalyDetector() {
	for ( auto element : cf) {
        if(element.center != nullptr) {
            delete element.center;
        }
    }
}

void HybridAnomalyDetector::learnHelper(const TimeSeries& ts,float p/*pearson*/,string f1, string f2,Point** ps){
    if(p>threshold){
        size_t len=ts.getRowSize();
        correlatedFeatures c;
        c.feature1=f1;
        c.feature2=f2;
        c.corrlation=p;
        c.lin_reg=linear_reg(ps,len);
        c.threshold=findThreshold(ps,len,c.lin_reg)*1.1; // 10% increase
        c.center = nullptr;
        cf.push_back(c);
        return;
    }

    if(p>circleThreshold){
        size_t len=ts.getRowSize();
        correlatedFeatures c;
        c.feature1=f1;
        c.feature2=f2;
        c.corrlation = p;
        Circle circle = findMinCircle(ps,len);
        c.center = new Point (circle.center); //delete the point in the destructor.
        c.threshold=circle.radius * 1.1; // 10% increase
        cf.push_back(c);
    }
    return;
}

float distance(float x1, float y1, float x2, float y2)
{
    // Calculating distance
    return sqrt(pow(x2 - x1, 2) +
                pow(y2 - y1, 2) * 1.0);
}

bool HybridAnomalyDetector::isAnomalous(float x, float y, correlatedFeatures c) {
    if(c.center != nullptr) {
        return (distance(c.center->x, c.center->y, x,y)>c.threshold);
    }
    return (abs(y - c.lin_reg.f(x))>c.threshold);
}