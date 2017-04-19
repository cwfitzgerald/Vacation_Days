#include "aboutdialogue.hpp"
#include "ui_aboutdialogue.h"

AboutDialogue::AboutDialogue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialogue)
{
    ui->setupUi(this);
}

AboutDialogue::~AboutDialogue()
{
    delete ui;
}
