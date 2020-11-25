#include "isolateqapp.h"

IsolateQApp::IsolateQApp(): app(argc, (char **)&argv)
{

}

IsolateQApp::~IsolateQApp()
{
    // stop thread
    quit();
    wait();
}

void IsolateQApp::run()
{
    this->result = app.exec();
}
