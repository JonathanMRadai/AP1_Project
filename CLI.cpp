#include "CLI.h"

CLI::CLI(DefaultIO* dio) {
    this->dio = dio;
    sharedData = new SharedData;
    commands.push_back(new UploadTimeSeries(dio,sharedData));
    commands.push_back(new ChangeThreshold(dio,sharedData));
    commands.push_back(new DetectAnomalies(dio,sharedData));
    commands.push_back(new DisplayResults(dio,sharedData));
    commands.push_back(new AnalyzeResults(dio, sharedData));
}

void CLI::start(){
    int key;
    do {
        dio->write("Welcome to the Anomaly Detection Server.");
        dio->write("Please choose an option:");
        dio->write("1.upload a time series csv file");
        dio->write("2.algorithm settings");
        dio->write("3.detect anomalies");
        dio->write("4.display results");
        dio->write("5.upload anomalies and analyze results");
        dio->write("6.exit");
        stringstream stream(dio->read());
        stream >> key;
        switch (key) {
            case 1:
                commands[0]->execute();
                break;
            case 2:
                commands[1]->execute();
                break;
            case 3:
                commands[2]->execute();
                break;
            case 4:
                commands[3]->execute();
                break;
            case 5:
                commands[4]->execute();
                break;
        }
    } while (key != 6);
}


CLI::~CLI() {
    if(sharedData != nullptr) {
        delete(sharedData);
        sharedData = nullptr;
    }
    for (auto item : commands) {
        if(item != nullptr) {
            delete(item);
            item = nullptr;
        }
    }
}







