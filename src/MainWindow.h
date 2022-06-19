#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <future>
#include "DampingCurve.h"
#include "FitSetting.h"
#include "Guide.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    QStandardItemModel* table;

    Guide guide_dialog;

    FitSetting fit_dialog;

    DampingCurve damping_curve;

    ControlPoint control_point;

    QVector<QColor> color_preset{QColor(228, 26, 28),
                                 QColor(55, 126, 184),
                                 QColor(77, 175, 74),
                                 QColor(152, 78, 163),
                                 QColor(255, 127, 0),
                                 QColor(255, 255, 51),
                                 QColor(166, 86, 40)};

    QVector<Qt::PenStyle> line_preset{Qt::SolidLine,
                                      Qt::DashLine,
                                      Qt::DotLine,
                                      Qt::DashDotLine,
                                      Qt::DashDotDotLine};

    std::atomic<bool> early_quit{false};
    std::future<void> optimization_task;

    void addType(const QString&);
    void addType(const QStringList&);
    void addControlPointToPlot();
    void updateScale();
    bool validateScheme();
private slots:
    void addControlPoint();
    void addType();
    void clearAllControlPoints();
    void clearAllTypes();
    void loadTypeInfo();
    void plotDampingCurve();
    void queryDampingRatio();
    void removeSelectedControlPoint();
    void removeSelectedType();
    void savePlot();
    void saveTypeInfo();
    void scatterControlPoint();
    void updateParameterFields(int);
    void updateTypeList();
    void switchCurveScale();
    void changeX();
    void about();
    void performFitting();
    void performFittingTask(const arma::mat&);
    void loadControlPoint();
    void updateOptimizerModeList();
    void processFittingResult(QStringList);
    void switchTheme();
    void changeLegend();
    void showGuidelines();
    void showFitSetting();
    void tidyUp();
    void commandSP();
    void commandOS();
signals:
    void finishFitting(QStringList);
};

#endif // MAINWINDOW_H
