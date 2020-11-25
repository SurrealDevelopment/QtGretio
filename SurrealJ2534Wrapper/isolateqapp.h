#ifndef ISOLATEQAPP_H
#define ISOLATEQAPP_H

#include <QApplication>
#include <QThread>



/**
 * @brief The IsolateQApp class makes a new q application
 * for our DLL to run out of.
 */
class IsolateQApp: public QThread
{
protected:
    int argc = 0;
    char * argv = "Arg1";

    int result;

public:
    QApplication app;
    IsolateQApp();
    ~IsolateQApp();

    void run();

};

#endif // ISOLATEQAPP_H
