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
