#ifndef EX5_STANDARDIO_H
#define EX5_STANDARDIO_H
#include "commands.h"

class standardIO : public DefaultIO {
    void write(string text) override {
        cout<<text;
    }
    void write(float f) override {
        cout<<f;
    }
    string read() override {
        string text;
        cin>>text;
        return text;
    }
    void read(float *f) override {
        cin>>*f;
    }

};


#endif //EX5_STANDARDIO_H
