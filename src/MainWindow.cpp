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

#include "MainWindow.h"
#include "About.h"
#include "DampingMode.h"
#include "Scheme/Scheme"
#include "ui_FitSetting.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), table(new QStandardItemModel(0, 2, this)), guide_dialog(this), fit_dialog(this) {
    ui->setupUi(this);

    table->setHorizontalHeaderLabels(QStringList({QString{"Frequency"}, QString{"Damping Ratio"}}));

    ui->omega->setMaximum(std::numeric_limits<double>().max());

    ui->controlPointTable->setModel(table);
    ui->controlPointTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->canvas->setNoAntialiasingOnDrag(true);
    ui->canvas->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);

    ui->canvas->xAxis->setLabel("Natural Frequency (rad/s)");
    ui->canvas->yAxis->setLabel("Damping Ratio");

    ui->canvas->xAxis->grid()->setSubGridVisible(true);
    ui->canvas->yAxis->grid()->setSubGridVisible(true);

    updateScale();

    plotDampingCurve();

    updateOptimizerModeList();

    connect(this, &MainWindow::finishFitting, this, &MainWindow::processFittingResult);
}

MainWindow::~MainWindow() {
    if(optimization_task.valid())
        optimization_task.get();

    delete table;
    delete ui;
}

void MainWindow::savePlot() {
    const auto width = ui->imageWidth->value();
    const auto height = ui->imageHeight->value();
    const auto type = ui->imageType->currentIndex();

    if(0 == type) {
        QString name = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("*.pdf"));

        if(name.isEmpty())
            return;

        ui->canvas->savePdf(name, width, height);
    }
    else if(1 == type) {
        QString name = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("*.png"));

        if(name.isEmpty())
            return;

        ui->canvas->savePng(name, width, height);
    }
    else if(2 == type) {
        QString name = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("*.jpg"));

        if(name.isEmpty())
            return;

        ui->canvas->saveJpg(name, width, height);
    }

    statusBar()->showMessage("Successfully write your plot to disk.");
}

void MainWindow::saveTypeInfo() {
    QString name = QFileDialog::getSaveFileName(this, tr("Save"), "", tr("All Files (*)"));

    if(name.isEmpty())
        return;

    QFile file(name);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    const auto type_info = damping_curve.getTypeInfo();

    for(const auto& I : qAsConst(type_info)) {
        file.write(I.toStdString().c_str());
        file.write("\n");
    }

    statusBar()->showMessage("Successfully write to file.");
}

void MainWindow::loadTypeInfo() {
    QString name = QFileDialog::getOpenFileName(this, tr("Load"), "", tr("All Files (*)"));

    if(name.isEmpty())
        return;

    QFile file(name);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"), file.errorString());
        return;
    }

    while(!file.atEnd())
        addType(QString(file.readLine()));

    statusBar()->showMessage("Successfully read from file.");
}

void MainWindow::updateParameterFields(int index) {
    ui->paLabel->setText("");
    ui->pbLabel->setText("");
    ui->pcLabel->setText("");
    ui->pdLabel->setText("");
    ui->peLabel->setText("");

    ui->paLabel->setDisabled(true);
    ui->pbLabel->setDisabled(true);
    ui->pcLabel->setDisabled(true);
    ui->pdLabel->setDisabled(true);
    ui->peLabel->setDisabled(true);
    ui->pa->setDisabled(true);
    ui->pb->setDisabled(true);
    ui->pc->setDisabled(true);
    ui->pd->setDisabled(true);
    ui->pe->setDisabled(true);

    if(1 == index) {
        ui->paLabel->setEnabled(true);

        ui->paLabel->setText(QString("n_p"));

        ui->pa->setEnabled(true);
    }
    else if(2 == index) {
        ui->paLabel->setEnabled(true);
        ui->pbLabel->setEnabled(true);

        ui->paLabel->setText(QString("n_pr"));
        ui->pbLabel->setText(QString("n_pl"));

        ui->pa->setEnabled(true);
        ui->pb->setEnabled(true);
    }
    else if(3 == index) {
        ui->peLabel->setEnabled(true);

        ui->peLabel->setText(QString("gamma"));

        ui->pe->setEnabled(true);
    }
    else if(4 == index) {
        ui->paLabel->setEnabled(true);
        ui->pbLabel->setEnabled(true);
        ui->pcLabel->setEnabled(true);
        ui->pdLabel->setEnabled(true);
        ui->peLabel->setEnabled(true);

        ui->paLabel->setText(QString("n_pr"));
        ui->pbLabel->setText(QString("n_pl"));
        ui->pcLabel->setText(QString("n_pk"));
        ui->pdLabel->setText(QString("n_pm"));
        ui->peLabel->setText(QString("gamma"));

        ui->pa->setEnabled(true);
        ui->pb->setEnabled(true);
        ui->pc->setEnabled(true);
        ui->pd->setEnabled(true);
        ui->pe->setEnabled(true);
    }
}

void MainWindow::addType() {
    const auto omega = ui->omega->value();
    const auto zeta = ui->zeta->value();

    if(abs(zeta) < 1E-4) {
        statusBar()->showMessage("Not adding this type since damping ratio is too small.");
        return;
    }

    switch(ui->typeList->currentIndex()) {
    case 0: {
        damping_curve.addMode(std::make_unique<DampingModeT0>(omega, zeta, std::vector<double>{}));
        break;
    }
    case 1: {
        const auto np = ui->pa->value();

        damping_curve.addMode(std::make_unique<DampingModeT1>(omega, zeta, std::vector<double>{np}));

        break;
    }
    case 2: {
        const auto npr = ui->pa->value();
        const auto npl = ui->pb->value();

        damping_curve.addMode(std::make_unique<DampingModeT2>(omega, zeta, std::vector<double>{npr, npl}));

        break;
    }
    case 3: {
        const auto gamma = ui->pe->value();

        damping_curve.addMode(std::make_unique<DampingModeT3>(omega, zeta, std::vector<double>{gamma}));

        break;
    }
    case 4: {
        const auto npr = ui->pa->value();
        const auto npl = ui->pb->value();
        const auto npk = ui->pc->value();
        const auto npm = ui->pd->value();
        const auto gamma = ui->pe->value();

        damping_curve.addMode(std::make_unique<DampingModeT4>(omega, zeta, std::vector<double>{npr, npl, npk, npm, gamma}));

        break;
    }
    default:
        break;
    }

    updateTypeList();
}

void MainWindow::removeSelectedType() {
    auto highlighted = ui->currentTypes->selectionModel()->selectedIndexes();

    std::sort(highlighted.begin(), highlighted.end(), std::less<QModelIndex>());
    for(auto I = highlighted.rbegin(); I != highlighted.rend(); ++I)
        damping_curve.removeMode(I->row());

    updateTypeList();
}

void MainWindow::clearAllTypes() {
    damping_curve.removeMode();

    updateTypeList();
}

void MainWindow::updateTypeList() {
    ui->currentTypes->clear();
    ui->currentTypes->addItems(damping_curve.getTypeInfo());

    plotDampingCurve();
}

void MainWindow::switchCurveScale() {
    if(ui->minX->text().toDouble() <= 0. || ui->maxX->text().toDouble() <= 0.) {
        ui->switchCurveScale->setCheckState(Qt::Unchecked);
        return;
    }

    plotDampingCurve();
}

void MainWindow::changeX() {
    bool flag;

    auto x_min = ui->minX->text().toDouble(&flag);
    if(!flag)
        return;

    x_min = QInputDialog::getDouble(this, "Minimum", "Input minimum frequency of interest...", x_min, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 10, &flag);
    if(!flag)
        return;

    auto x_max = ui->maxX->text().toDouble(&flag);
    if(!flag)
        return;

    x_max = QInputDialog::getDouble(this, "Maximum", "Input maximum frequency of interest...", x_max, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), 10, &flag);
    if(!flag)
        return;

    if(x_min >= x_max) {
        statusBar()->showMessage("Minimum shall be smaller than maximum.");
        return;
    }

    if(ui->switchCurveScale->checkState() == Qt::Checked && x_min <= 0.) {
        QMessageBox::information(this, tr("Oops!"), tr("Only positive values are acceptable for log scale. Maybe switch it off first?"));
        return;
    }

    ui->minX->setText(QString::number(x_min));
    ui->maxX->setText(QString::number(x_max));

    plotDampingCurve();
}

void MainWindow::plotDampingCurve() {
    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(0, 0, 0));

    ui->samplesValue->setText(QString::number(ui->samples->value()));

    updateScale();

    const auto x_min = ui->minX->text().toDouble();
    const auto x_max = ui->maxX->text().toDouble();

    if(ui->switchCurveScale->checkState() == Qt::Unchecked)
        damping_curve.updateLinearDampingCurve(x_min, x_max, ui->samples->value());
    else if(ui->switchCurveScale->checkState() == Qt::Checked)
        damping_curve.updateLogarithmicDampingCurve(x_min, x_max, ui->samples->value());

    ui->canvas->clearGraphs();
    ui->canvas->xAxis->setRange(damping_curve.minFrequency(), damping_curve.maxFrequency());
    ui->canvas->yAxis->setRange(damping_curve.minDampingRatio() - .1, damping_curve.maxDampingRatio() + .1);

    ui->canvas->addGraph();
    ui->canvas->graph()->setName("Total Response");
    ui->canvas->graph()->setPen(pen);
    ui->canvas->graph()->setData(damping_curve.getFrequencyVector(), damping_curve.getDampingRatioVector());

    for(auto j = 0; j < damping_curve.count(); ++j) {
        ui->canvas->addGraph();
        pen = QPen(color_preset[j % color_preset.size()]);
        pen.setWidth(2);
        pen.setStyle(line_preset[j % line_preset.size()]);
        ui->canvas->graph()->setPen(pen);
        ui->canvas->graph()->setName(ui->currentTypes->item(j)->text());
        ui->canvas->graph()->setData(damping_curve.getFrequencyVector(), damping_curve.getDampingRatioVector(j));
    }

    ui->canvas->replot();
}

void MainWindow::queryDampingRatio() {
    bool flag;
    const auto omega = ui->queryInput->text().toDouble(&flag);

    if(!flag) {
        statusBar()->showMessage("Try a valid frequency.");
        ui->queryInput->setFocus();
        return;
    }

    const auto zeta = damping_curve.query(omega);

    statusBar()->showMessage("The queried damping ratio is: " + QString::number(zeta));

    ui->queryInput->setText(QString::number(zeta, 'e', 15));
}

void MainWindow::addControlPoint() {
    bool flag;

    const auto omega = ui->cpOmega->text().toDouble(&flag);
    if(!flag) {
        statusBar()->showMessage("Frequency has to be a valid float number.");
        ui->cpOmega->setFocus();
        return;
    }

    const auto zeta = ui->cpZeta->text().toDouble(&flag);
    if(!flag) {
        statusBar()->showMessage("Damping ratio has to be a valid float number.");
        ui->cpZeta->setFocus();
        return;
    }

    table->insertRow(table->rowCount(), QList<QStandardItem*>({new QStandardItem(ui->cpOmega->text()), new QStandardItem(ui->cpZeta->text())}));

    control_point.addPoint(omega, zeta);

    scatterControlPoint();
}

void MainWindow::removeSelectedControlPoint() {
    QModelIndexList indexes = ui->controlPointTable->selectionModel()->selectedRows();

    for(int i = indexes.count(); i > 0; i--) {
        const auto row_number = indexes.at(i - 1).row();
        table->removeRow(row_number);
        control_point.removePoint(row_number);
    }

    scatterControlPoint();
}

void MainWindow::clearAllControlPoints() {
    table->clear();
    control_point.removePoint();

    scatterControlPoint();
}

void MainWindow::scatterControlPoint() {
    ui->canvas->clearGraphs();

    addControlPointToPlot();

    updateScale();

    if(control_point.count() > 0) {
        if(ui->switchCurveScale->checkState() == Qt::Unchecked)
            ui->canvas->xAxis->setRange(control_point.minFrequency() - 1., control_point.maxFrequency() + 1.);
        else if(ui->switchCurveScale->checkState() == Qt::Checked)
            ui->canvas->xAxis->setRange(.5 * control_point.minFrequency(), 2. * control_point.maxFrequency());
    }

    ui->canvas->yAxis->setRange(control_point.minDampingRatio() - .1, control_point.maxDampingRatio() + .1);

    ui->canvas->replot();
}

void MainWindow::about() {
    About about_dialog(this);

    about_dialog.exec();
}

void MainWindow::performFitting() {
    if(!validateScheme()) {
        QMessageBox::information(this, tr("Oops!"), tr("The selected scheme cannot be used to optimize negative region response."));
        return;
    }

    if(optimization_task.valid()) {
        using namespace std::chrono_literals;

        if(optimization_task.wait_for(0ms) != std::future_status::ready) {
            statusBar()->showMessage("The previous optimization is still running.");
            const int ret = QMessageBox::question(this, tr("Task is running."), tr("The optimization task is running, click OK to wait, or Abort to cancel"), QMessageBox::StandardButtons(QMessageBox::Ok | QMessageBox::Abort));
            if(QMessageBox::Abort == ret)
                early_quit = true;
            return;
        }
    }

    clearAllTypes();

    const auto& reference = control_point.getSampling();

    if(reference.empty() || reference(0) <= 0.) {
        statusBar()->showMessage("Only positive frequency ranges are supported.");
        return;
    }

    ui->minX->setText(QString::number(pow(10., log10(reference.col(0).min()) - .5)));
    ui->maxX->setText(QString::number(pow(10., log10(reference.col(0).max()) + .5)));

    statusBar()->showMessage("Optimizing...");

    optimization_task = std::async(std::launch::async, &MainWindow::performFittingTask, this, reference);
}

void MainWindow::performFittingTask(const mat& reference) {
    std::unique_ptr<ObjectiveFunction> f;

    if(ui->optimizationScheme->currentText() == "Zero Day")
        f = std::make_unique<ZeroDay>(ui->numberT0->value());
    else if(ui->optimizationScheme->currentText() == "Unicorn")
        f = std::make_unique<Unicorn>(ui->numberT1->value());
    else if(ui->optimizationScheme->currentText() == "Two Cities")
        f = std::make_unique<TwoCities>(ui->numberT2->value());
    else if(ui->optimizationScheme->currentText() == "Three Wise Men")
        f = std::make_unique<ThreeWiseMen>(ui->numberT3->value());

    const auto lower = log10(reference.col(0).min());
    const auto upper = log10(reference.col(0).max());

    const auto number_samples = ui->samples->value();

    OptimizerSetting opt_setting;

    opt_setting.stepSize = fit_dialog.getUi()->stepSize->text().toDouble();
    opt_setting.tolerance = fit_dialog.getUi()->tolerance->text().toDouble();
    opt_setting.weight = fit_dialog.getUi()->weight->text().toDouble();
    opt_setting.maxIter = fit_dialog.getUi()->maxIter->text().toInt();

    mat result, samples;

    if(reference.n_rows == 1)
        samples = reference;
    else {
        samples.set_size(number_samples, 2);
        samples.col(0) = logspace(lower, upper, number_samples);
        vec samples_y(samples.colptr(1), number_samples, false, true);
        if(ui->switchCurveScale->checkState() == Qt::Checked)
            interp1(log10(reference.col(0)), reference.col(1), log10(samples.col(0)), samples_y);
        else
            interp1(reference.col(0), reference.col(1), samples.col(0), samples_y);
    }

    f->initializeSampling(samples.t());

    early_quit = false;

    if(ui->optimizerList->currentText() == "LBFGS")
        result = run_optimizer<L_BFGS>(opt_setting, f.get(), &early_quit);
    else if(ui->optimizerList->currentText() == "Gradient Descent")
        result = run_optimizer<GradientDescent>(opt_setting, f.get(), &early_quit);
    else if(ui->optimizerList->currentText() == "AugLagrangian")
        result = run_optimizer<AugLagrangian>(opt_setting, f.get(), &early_quit);

    result.print("result");

    emit finishFitting(f->getTypeList(result));
}

void MainWindow::loadControlPoint() {
    QString name = QFileDialog::getOpenFileName(this, tr("Load"), "", tr("All Files (*)"));

    if(name.isEmpty())
        return;

    mat cp;
    cp.load(name.toStdString());

    clearAllControlPoints();

    for(auto I = 0llu; I < cp.n_rows; ++I) {
        table->insertRow(table->rowCount(), QList<QStandardItem*>({new QStandardItem(QString::number(cp(I, 0))), new QStandardItem(QString::number(cp(I, 1)))}));

        control_point.addPoint(cp(I, 0), cp(I, 1));
    }

    scatterControlPoint();

    statusBar()->showMessage("Successfully read from file.");
}

void MainWindow::updateOptimizerModeList() {
    ui->numberT0->setDisabled(true);
    ui->numberT1->setDisabled(true);
    ui->numberT2->setDisabled(true);
    ui->numberT3->setDisabled(true);
    ui->numberT4->setDisabled(true);

    if(ui->optimizationScheme->currentText() == "Zero Day")
        ui->numberT0->setEnabled(true);
    else if(ui->optimizationScheme->currentText() == "Unicorn")
        ui->numberT1->setEnabled(true);
    else if(ui->optimizationScheme->currentText() == "Two Cities")
        ui->numberT2->setEnabled(true);
    else if(ui->optimizationScheme->currentText() == "Three Wise Men")
        ui->numberT3->setEnabled(true);
}

void MainWindow::processFittingResult(QStringList result) {
    addType(result);

    addControlPointToPlot();

    statusBar()->showMessage("Finished!");
}

void MainWindow::switchTheme() {
    if(ui->darkMode->checkState() == Qt::Checked) {
        QFile file(":/utilities/stylesheet_francesco.qss");
        file.open(QFile::ReadOnly);
        setStyleSheet(QString::fromLatin1(file.readAll()));
    }
    else
        setStyleSheet("");
}

void MainWindow::changeLegend() {
    if(ui->changeLegend->checkState() == Qt::Checked)
        ui->canvas->legend->setVisible(true);
    else
        ui->canvas->legend->setVisible(false);

    ui->canvas->replot();
}

void MainWindow::showGuidelines() {
    guide_dialog.show();
}

void MainWindow::showFitSetting() {
    fit_dialog.exec();
}

void MainWindow::tidyUp() {
    damping_curve.tidyUp();
    updateTypeList();
}

void MainWindow::commandSP() {
    QString command = "integrator LeeNewmarkFull 1 .25 .5";

    const auto command_list = damping_curve.getCommand();

    for(const auto& I : qAsConst(command_list)) {
        command += ' ';
        command += I;
    }

    ui->commandOutput->setText(command);
}

void MainWindow::commandOS() {
    QString command = "integrator LeeNewmarkFullKC .5 .25";

    const auto command_list = damping_curve.getCommand();

    for(const auto& I : qAsConst(command_list)) {
        command += ' ';
        command += I;
    }

    ui->commandOutput->setText(command);
}

void MainWindow::addType(const QString& type) {
    QStringList fields = type.split(" ");
    if(fields.size() < 2) return;

    if(fields[1] == "0") {
        if(fields.size() < 5) return;
        ui->typeList->setCurrentIndex(0);
        ui->omega->setValue(fields[3].toDouble());
        ui->zeta->setValue(fields[4].toDouble());
        addType();
    }
    else if(fields[1] == "1") {
        if(fields.size() < 6) return;
        ui->typeList->setCurrentIndex(1);
        ui->omega->setValue(fields[3].toDouble());
        ui->zeta->setValue(fields[4].toDouble());
        ui->pa->setValue(fields[5].toDouble());
        addType();
    }
    else if(fields[1] == "2") {
        if(fields.size() < 7) return;
        ui->typeList->setCurrentIndex(2);
        ui->omega->setValue(fields[3].toDouble());
        ui->zeta->setValue(fields[4].toDouble());
        ui->pa->setValue(fields[5].toDouble());
        ui->pb->setValue(fields[6].toDouble());
        addType();
    }
    else if(fields[1] == "3") {
        if(fields.size() < 6) return;
        ui->typeList->setCurrentIndex(3);
        ui->omega->setValue(fields[3].toDouble());
        ui->zeta->setValue(fields[4].toDouble());
        ui->pe->setValue(fields[5].toDouble());
        addType();
    }
    else if(fields[1] == "4") {
        if(fields.size() < 10) return;
        ui->typeList->setCurrentIndex(4);
        ui->omega->setValue(fields[3].toDouble());
        ui->zeta->setValue(fields[4].toDouble());
        ui->pa->setValue(fields[5].toDouble());
        ui->pb->setValue(fields[6].toDouble());
        ui->pc->setValue(fields[7].toDouble());
        ui->pd->setValue(fields[8].toDouble());
        ui->pe->setValue(fields[9].toDouble());
        addType();
    }
}

void MainWindow::addType(const QStringList& type_list) {
    for(const auto& I : type_list)
        addType(I);
}

void MainWindow::addControlPointToPlot() {
    QPen pen;
    pen.setWidth(5);
    pen.setColor(QColor(255, 0, 0));

    ui->canvas->addGraph();
    ui->canvas->graph()->setPen(pen);
    ui->canvas->graph()->setName("Control Point");
    ui->canvas->graph()->setLineStyle(QCPGraph::LineStyle::lsNone);
    ui->canvas->graph()->setScatterStyle(QCPScatterStyle::ssStar);
    ui->canvas->graph()->setData(control_point.getFrequencyVector(), control_point.getDampingRatioVector());

    ui->canvas->replot();
}

void MainWindow::updateScale() {
    if(ui->switchCurveScale->checkState() == Qt::Unchecked) {
        ui->canvas->xAxis->setScaleType(QCPAxis::stLinear);
        ui->canvas->xAxis->setTicker(QSharedPointer<QCPAxisTicker>(new QCPAxisTicker));
        ui->canvas->xAxis->setNumberFormat("g");
        ui->canvas->xAxis->setNumberPrecision(4);
    }
    else if(ui->switchCurveScale->checkState() == Qt::Checked) {
        ui->canvas->xAxis->setScaleType(QCPAxis::stLogarithmic);
        ui->canvas->xAxis->setTicker(QSharedPointer<QCPAxisTickerLog>(new QCPAxisTickerLog));
        ui->canvas->xAxis->setNumberFormat("eb");
        ui->canvas->xAxis->setNumberPrecision(0);
    }
}

bool MainWindow::validateScheme() {
    const auto min_x = ui->minX->text().toDouble();

    if(min_x > 0.) return true;

    if(ui->optimizationScheme->currentText() == "Zero Day") return false;
    if(ui->optimizationScheme->currentText() == "Unicorn") return false;
    if(ui->optimizationScheme->currentText() == "Three Wise Men") return false;

    return true;
}
