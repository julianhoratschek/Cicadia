#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Modles/subjectstreemodel.h"
#include "UI/datatab.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QSvgGenerator>

#include <QPen>

#include "UI/qcustomplot.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void insertDataTab(int index);

public slots:
    void plotData(CCDataSetPtr dataset, const QModelIndexList &list);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_actionImport_triggered();

    void on_actionExit_triggered();

    void on_tabWidget_tabBarClicked(int index);

    void ontabBar_tabMoved(int, int);

    void on_tabWidget_tabCloseRequested(int index);

    void on_subjectsTreeView_doubleClicked(const QModelIndex &index);

    void oncustomPlot_MousePress(QMouseEvent *event);
    void oncustomPlot_MouseWheel(QWheelEvent *event);

    void on_actionExport_Graph_triggered();

    void on_actionClear_Graph_triggered();

    void on_actionSave_triggered();

    void on_actionOpen_triggered();

private:
    Ui::MainWindow                  *ui;
    SubjectsTreeModel               *subjectsTreeModel;
    CCDataBase                      *dataBase;
    QMap<QCPGraph*, CCDataSetPtr>   graphRelations;
};

#endif // MAINWINDOW_H
