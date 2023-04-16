/*******************************************************************************
 * Copyright (C) 2022-2023 Theodore Chang
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

#ifndef FITSETTING_H
#define FITSETTING_H

#include <QDialog>

namespace Ui {
    class FitSetting;
}

class FitSetting : public QDialog {
    Q_OBJECT

public:
    explicit FitSetting(QWidget* parent = nullptr);
    ~FitSetting() override;

    Ui::FitSetting* getUi();

private slots:
    void on_changeBatchSize_clicked();

    void on_changeWeight_clicked();

    void on_changeStepSize_clicked();

    void on_changeTolerance_clicked();

    void on_changeMaxOrder_clicked();
    void on_changeMaxIter_clicked();

private:
    Ui::FitSetting* ui;
};

#endif // FITSETTING_H
