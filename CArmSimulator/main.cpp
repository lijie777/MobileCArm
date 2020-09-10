#include "UEGMainWindow.h"
#include "UEGApplication.h"

#include <log4cplus/logger.h>

log4cplus::Logger g_logger;

int main(int argc, char *argv[])
{
    UEGApplication a(argc, argv);
    UEGMainWindow w;
    QObject::connect(&a, &UEGApplication::displayMessage, &w, &UEGMainWindow::displayMessage);

    if (!a.init() || !w.Init())
    {
        //return EXIT_FAILURE;
    }

    w.show();

    return a.exec();
}
