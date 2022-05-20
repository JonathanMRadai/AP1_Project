
#ifndef COMMANDS_H_
#define COMMANDS_H_
#include<iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include "HybridAnomalyDetector.h"
#include "timeseries.h"
#include "algorithm"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>


using namespace std;

class DefaultIO {
public:
	virtual string read()=0;
	virtual void write(string text)=0;
	virtual void write(float f)=0;
	virtual void read(float* f)=0;
	virtual ~DefaultIO(){}

    //my additions
    virtual string uploadFile(string filename) {
        ofstream out;
        out.open(filename);
        string line;
        while (true) {
            line = this->read();
            if (line != "done") {
                out<<line<<endl;
                continue;
            }
            break;
        }
        if(out.is_open()) {
            out.close();
        }
        this->write("Upload complete.");
        return filename;
    }
};

class ServerIO : public DefaultIO {
protected:
    int clientID;
public:
    ServerIO(int clientID) : DefaultIO(), clientID(clientID)  {}

    virtual string read(){
        string clientInput="";
        char c=0;
        ::read(clientID,&c,sizeof(char));
        while(c!='\n'){
            clientInput+=c;
            ::read(clientID,&c,sizeof(char));
        }
        return clientInput;
    }
    virtual void write(string text){
        ::write(clientID, text.c_str(), text.length());
        ::write(clientID,"\n",1);

    }
    virtual void write(float f) {
        string output = to_string(f);
        ::write(clientID, output.c_str(), output.length());
        ::write(clientID,"\n",1);


    }
    virtual void read(float* f) {
        string clientInput="";
        char c=0;
        ::read(clientID,&c,sizeof(char));
        while(c!='\n'){
            clientInput+=c;
            ::read(clientID,&c,sizeof(char));
        }
        *f = stof(clientInput);
        return;
    }
    virtual ~ServerIO(){}
};

// you may add here helper classes

class SharedData {
public:
    TimeSeries* trainTS;
    TimeSeries* testTS;
    HybridAnomalyDetector* detector;
    vector<AnomalyReport> anomaly;
    SharedData() : trainTS(nullptr), testTS(nullptr){
        detector = new HybridAnomalyDetector();
    }
    ~SharedData() {
        if(trainTS != nullptr) {
            delete(trainTS);
            trainTS = nullptr;
        }
        if (testTS != nullptr) {
            delete(testTS);
            testTS = nullptr;
        }
        if (detector != nullptr) {
            delete(detector);
            detector = nullptr;
        }
    }
};

// you may edit this class
class Command{
protected: //my addition.
    DefaultIO* dio;
    string description;
public:
	Command(DefaultIO* dio):dio(dio){}
    const void showDescription() const { // my addition
        dio->write(description);
    }
	virtual void execute()=0;
	virtual ~Command(){}
};
class CommandA : public Command {
protected:
    SharedData* sharedData;

public:
    CommandA(DefaultIO *dio, SharedData* data) : Command(dio) {
       sharedData = data;
    }
};
// implement here your command classes

class UploadTimeSeries : public CommandA {
public:

    UploadTimeSeries(DefaultIO *dio, SharedData* sharedData) : CommandA(dio,sharedData) {
        description = "1.upload a time series csv file";
    }

    void execute() override {

        dio->write("Please upload your local train CSV file.");
        sharedData->trainTS = new TimeSeries((dio->uploadFile("train.csv")).c_str());
        dio->write("Please upload your local test CSV file.");
        sharedData->testTS = new TimeSeries((dio->uploadFile("test.csv")).c_str());
    }
};

class ChangeThreshold : public CommandA {

public:
    ChangeThreshold(DefaultIO *pIo, SharedData *pData) : CommandA(pIo, pData) {
        description = "2.algorithm settings";
    }
    void execute() override {
        stringstream s;
        s<<sharedData->detector->getThreshold();
        dio->write("The current correlation threshold is " + s.str());
        dio->write("Type a new threshold");
         float t = std::stof(dio->read());
         while (((1 < t) || (t < 0))) {
             dio->write("choose a value between 0 and 1.");
             t = std::stof(dio->read());
         }
         sharedData->detector->setCorrelationThreshold(t);
         return;
    }
};

class DetectAnomalies : public CommandA {
public:
    DetectAnomalies(DefaultIO *pIo, SharedData *pData) : CommandA(pIo, pData){
        description = "3.detect anomalies";
    }
    void execute() override {
        sharedData->detector->learnNormal(*(sharedData->trainTS));
        sharedData->anomaly = sharedData->detector->detect(*(sharedData->testTS));
        dio->write("anomaly detection complete.");
    }
};
class DisplayResults : public CommandA {
public:
    DisplayResults(DefaultIO *pIo, SharedData *pData) : CommandA(pIo, pData){
        description = "4.display results";
    }
    void execute() override {
        for (auto item : sharedData->anomaly) {
            dio->write(to_string(item.timeStep) + "  " + item.description);
        }
        dio->write("Done.");
    }
};

class AnalyzeResults : public CommandA {
public:
    AnalyzeResults(DefaultIO *pIo, SharedData *pData) : CommandA(pIo, pData){
        description = "5.upload anomalies and analyze results";
    }
    void execute() override {
        dio->write("Please upload your local anomalies file.");
        vector<pair<long,long>> anomalies = arrangeTimeRanges(dio->uploadFile("timeRanges.csv"));
        vector<pair<long,long>> reports = helpAnalyzeResultsExecute(sharedData->anomaly);
        long P = anomalies.size();
        long N = sharedData->testTS->getRowSize() - someD(&anomalies);
        long TP = 0;
        long FP = 0;
        for(auto report : reports) {
            bool reportHaveAnomaly = false;
           for(auto anomaly : anomalies ) {
               if(checkTP(report, anomaly)){
                   ++TP;
                   if(!reportHaveAnomaly) {
                       reportHaveAnomaly = true;
                   }
               }
           }
           if(!reportHaveAnomaly) {
               ++FP;
           }
        }
        float TPR = (float)TP / (float)P;
        float FPR = (float)FP / (float)N;

        //deal with round up.
        int temp = TPR * 1000;
        TPR = (float) temp / (float)1000;

        temp = FPR * 1000;
        FPR = (float)temp / (float)1000;

        //send the result.
        stringstream s1;
        s1<<setprecision(2)<<TPR;
        stringstream s2;
        s2<<setprecision(2)<<FPR;
        dio->write("True Positive Rate: " + s1.str());
        dio->write("False Positive Rate: " + s2.str());
        return;
    }
private:
    vector<pair<long,long>> arrangeTimeRanges(string fileName) {
        vector<pair<long,long>> result;
        ifstream in(fileName);
        while(!in.eof()){
            string line;
            in>>line;
            string val;
            stringstream lss(line);
            vector<long> temp;
            while(getline(lss,val,',')) {
                temp.push_back(stol(val));
            }
            if(temp.size() == 2) {
                result.push_back(make_pair(temp[0],temp[1]));
            }
        }
        in.close();
        return result;
    }
    vector<pair<long,long>> helpAnalyzeResultsExecute(vector<AnomalyReport> anomaly) {
        vector<pair<long,long>> result;
        if(anomaly.empty()) {
            return result;
        }
        int size = anomaly.size();
        string desc = anomaly[0].description;
        long step = anomaly[0].timeStep;
        long counter = 0;
        string prev;
        for(int i = 1; i < size; ++i) {
            if ((desc == anomaly[i].description) && (step == anomaly[i].timeStep - 1)) {
                step = anomaly[i].timeStep;
                ++counter;
                continue;
            }
            result.push_back(make_pair(step - counter, step));
            prev = desc;
            desc = anomaly[i].description;
            step = anomaly[i].timeStep;
            counter = 0;
        }
        if (prev != desc) {
            result.push_back(make_pair(step - counter, step));
        }
        return result;
    }
    long someD(vector<pair<long, long>>* v){
        long result = 0;
        for (auto item : *v){
            result += ((item.second - item.first) + 1);
        }
        return result;
    }
    bool checkTP(pair<long, long> r, pair<long, long> a) {
        if ((a.first <= r.second) && (r.second <= a.second)) {
            return true;
        }
        if(((a.first <= r.first) && (r.first <= a.second))) {
            return true;
        }
        if ((r.first <= a.second) && (a.second <= r.second)) {
            return true;
        }
        if(((r.first <= a.first) && (a.first <= r.second))) {
            return true;
        }
        return false;
    }
};


#endif

