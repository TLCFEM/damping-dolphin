/*******************************************************************************
 * Copyright (C) 2022-2026 Theodore Chang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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

Ui::FitSetting* FitSetting::getUi() const {
    return ui;
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
