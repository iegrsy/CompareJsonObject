#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QString>
#include <QMessageBox>

#include <QJsonParseError>
#include <QDragEnterEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnjsonpath_clicked()
{
    QString fileName = QFileDialog::getOpenFileName (
                this,
                tr ("Open File"),
                ".",
                tr ("Open Document Text Files (*.json)"));

    if (fileName.isEmpty ())
        return;

    ui->lejsonpath->setText(fileName);

    jsonStr.clear();
    jsonStr = readFile(fileName);

    jsonSerials = getJsonSerialNumbers(jsonStr);
    ui->tejsonlist->clear();
    ui->tejsonlist->document()->setPlainText(listtoStr(jsonSerials));
}

void MainWindow::on_btnsearch_clicked()
{
    if(ui->teodtlist->toPlainText().isEmpty() || ui->tejsonlist->toPlainText().isEmpty())
        return;

    QStringList diff, jsonSerials, odsSerials;
    diff.clear();
    jsonSerials.clear();
    odsSerials.clear();

    jsonSerials = getListSerialNumbers(ui->tejsonlist->toPlainText());
    odsSerials = getListSerialNumbers(ui->teodtlist->toPlainText());

    int p = 0;
    int pa = odsSerials.size();
    foreach ( QString os , odsSerials){
        bool state = false;

        for(int i = 0; i < jsonSerials.size(); i++) {
            QString js = jsonSerials.at(i);

            if(os.compare(js) == 0){
                state = true;
                jsonSerials.removeAt(i);
                break;
            }
        }

        if (!state)
            diff.append(os);
        p++;
        ui->progressBar->setValue((int)((double)p / (double)pa) * 100);
    }

    ui->tedifflist->clear();
    ui->tedifflist->document()->setPlainText(listtoStr(diff));

    if(ui->tedifflist->toPlainText().isEmpty()){
        QMessageBox msgbox;
        msgbox.setText("Test edilmeyen kamera bulunamadı.");
        msgbox.exec();
    }
}

QString MainWindow::listtoStr(QStringList list){
    if(list.isEmpty())
        return "";

    QString result;
    foreach (QString s, list) {
        result += s.append("\n");
    }
    return result;
}

QString MainWindow::readFile(QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return 0;

    QString jdata = file.readAll().data();
    file.close();

    return jdata;
}

QStringList MainWindow::getJsonSerialNumbers(QString str){
    if(str.isEmpty())
        return QStringList();

    QStringList slist;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8());
    QJsonObject jsonObj = jsonDoc.object();

    foreach (QString s, jsonObj.keys()) {
        QJsonArray array = jsonObj[s].toArray();

        for (int i = 0 ; i < array.size() ; i++){
            QJsonObject obj = array.at(i).toObject();
            slist.append(obj.value("Serial").toString());
        }
    }

    return slist;
}

QStringList MainWindow::getListSerialNumbers(QString str){
    if(str.isEmpty())
        return QStringList();

    return str.split("\n");
}
