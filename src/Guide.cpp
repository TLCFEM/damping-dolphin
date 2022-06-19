#include "Guide.h"
#include <QIcon>
#include "ui_Guide.h"

Guide::Guide(QWidget* parent)
    : QDialog(parent), ui(new Ui::Guide) {
    ui->setupUi(this);

    setWindowTitle("Guide");
    setWindowIcon(QIcon(":/images/dolphin.ico"));
}

Guide::~Guide() {
    delete ui;
}
