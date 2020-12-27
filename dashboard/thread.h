#ifndef THREAD_H
#define THREAD_H

#include <QThread>

class HelloThread : public QThread
{
    Q_OBJECT
public:
    QObject *spdm = NULL;
    //HelloThread() {};
    //~HelloThread() {};
private:
    void run();
};

#endif // THREAD_H
