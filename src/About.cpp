#include "About.h"
#include <QIcon>
#include "ui_About.h"

About::About(QWidget* parent)
    : QDialog(parent), ui(new Ui::About) {
    ui->setupUi(this);

    setWindowTitle("About");
    setWindowIcon(QIcon(":/images/dolphin.ico"));
}

About::~About() {
    delete ui;
}
