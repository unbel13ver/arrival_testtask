#include "thread.h"
#include <unistd.h>

#include <QDebug>
#include "../libmyaio/inc/lib.h"

void HelloThread::run()
{
    qDebug() << "hello from worker thread " << thread()->currentThreadId();

    struct client_str client = {0, NULL};
    int val = 0;

    char addr[20] = {0};

    sprintf(addr, IPADDRESS);
    if(libaio_register_client(addr, PORTN, &client) != 0) {
        qDebug() << "DB: Unable to register on the server!";
        return;
    } else {
        qDebug() << "DB: We are registered on the server.\n";
    }

    if (NULL == client.callback) {
        qDebug() << "DB: Callback function pointer is NULL, terminating";
        return;
    }

    if (spdm) {
        while (true) {
            val = client.callback();
            if (val < 0) {
                spdm->setProperty("kph", val);
                qDebug() << "DB: Server is down, the application may be closed";
                break;
            }
            spdm->setProperty("kph", val);
            usleep(16000);
        }
    } else {
        qDebug() << "DB: Unable to get speedometer";
    }
}
