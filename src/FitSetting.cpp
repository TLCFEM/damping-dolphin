#include "FitSetting.h"
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include "ui_FitSetting.h"

FitSetting::FitSetting(QWidget* parent)
    : QDialog(parent), ui(new Ui::FitSetting) {
    ui->setupUi(this);

    setWindowTitle("Advanced Settings");
    setWindowIcon(QIcon(":/images/dolphin.ico"));
}

FitSetting::~FitSetting() {
    delete ui;
}

Ui::FitSetting* FitSetting::getUi() {
    return ui;
}

void FitSetting::on_changeBatchSize_clicked() {
    bool flag;
    const auto batchSize = QInputDialog::getText(this, "Batch Size", "Input batch size...").toInt(&flag);
    if(!flag || batchSize < 0) {
        QMessageBox::information(this, tr("Oops!"), tr("The batch size needs to be a positive integer."));
        return;
    }

    ui->batchSize->setText(QString::number(batchSize));
}

void FitSetting::on_changeWeight_clicked() {
    bool flag;
    const auto weight = QInputDialog::getText(this, "Weight", "Input weight...").toDouble(&flag);
    if(!flag || weight < 0.) {
        QMessageBox::information(this, tr("Oops!"), tr("The weight needs to be a non-negative float number."));
        return;
    }

    ui->weight->setText(QString::number(weight));
}

void FitSetting::on_changeStepSize_clicked() {
    bool flag;
    const auto stepSize = QInputDialog::getText(this, "Step Size", "Input step size...").toDouble(&flag);
    if(!flag || stepSize <= 0.) {
        QMessageBox::information(this, tr("Oops!"), tr("The step size needs to be a positive float number."));
        return;
    }

    ui->stepSize->setText(QString::number(stepSize));
}

void FitSetting::on_changeTolerance_clicked() {
    bool flag;
    const auto tolerance = QInputDialog::getText(this, "Tolerance", "Input tolerance...").toDouble(&flag);
    if(!flag || tolerance <= 0.) {
        QMessageBox::information(this, tr("Oops!"), tr("The tolerance needs to be a positive float number."));
        return;
    }

    ui->tolerance->setText(QString::number(tolerance));
}

void FitSetting::on_changeMaxOrder_clicked() {
    bool flag;
    const auto maxOrder = QInputDialog::getText(this, "Max Order", "Input maximum order...").toInt(&flag);
    if(!flag || maxOrder <= 0) {
        QMessageBox::information(this, tr("Oops!"), tr("The tolerance needs to be a positive integer number."));
        return;
    }

    ui->maxOrder->setText(QString::number(maxOrder));
}

void FitSetting::on_changeMaxIter_clicked() {
    bool flag;
    const auto maxIter = QInputDialog::getText(this, "Max Iteration", "Input maximum iteration...").toInt(&flag);
    if(!flag || maxIter <= 0) {
        QMessageBox::information(this, tr("Oops!"), tr("The tolerance needs to be a positive integer number."));
        return;
    }

    ui->maxIter->setText(QString::number(maxIter));
}
