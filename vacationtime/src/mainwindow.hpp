#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "aboutdialogue.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    public slots:
        void OpenAboutWindow();

    private:
        Ui::MainWindow* ui;
        AboutDialogue* abdialogue;
};

#endif // MAINWINDOW_HPP
