#include "damping-dolphin.h"

#include <QApplication>
#include <QFile>
#include <QFontDatabase>
#include <QStyle>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    QApplication::setApplicationName("Damping Dolphin");
    QApplication::setApplicationDisplayName("Damping Dolphin");
    QApplication::setOrganizationName("University of Canterbury");
    QApplication::setWindowIcon(QIcon(":/images/dolphin.ico"));

    MainWindow win;

    win.setWindowTitle("Damping Dolphin");
    win.show();

    return QApplication::exec();
}
