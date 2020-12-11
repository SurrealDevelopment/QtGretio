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
    app.setApplicationName("Gretio J2534");
    this->result = app.exec();
}
