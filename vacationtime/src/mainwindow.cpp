#include "mainwindow.hpp"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionAbout_VacationTime, SIGNAL(triggered()), this, SLOT(OpenAboutWindow()));
}

void MainWindow::OpenAboutWindow() {
    abdialogue = new AboutDialogue();
    abdialogue->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete abdialogue;
}
