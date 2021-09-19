#include "controlboard.h"
#include "ui_controlboard.h"

#include <QIntValidator>
#include <QMessageBox>
#include <QDateTime>

#include <iostream>

#include "protocol.h"

using namespace Protocol;

ControlBoard::ControlBoard(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ControlBoard)
{
    ui->setupUi(this);
    ui->disconnect_button->setEnabled(false);
    server = new QTcpSocket( this );
    ui->server_port_line->setValidator( new QIntValidator(0, 100000, this) );
    connect(server, SIGNAL(disconnected()), SLOT(on_disconnect_button_clicked()));
    connect(ui->command_box, SIGNAL(currentTextChanged(const QString &)), this, SLOT(update_command_line()));
    connect(ui->channel_box, SIGNAL(currentTextChanged(const QString &)), this, SLOT(update_command_line()));
    connect(ui->parameters_line, SIGNAL(textChanged(const QString &)), this, SLOT(update_command_line()));
}

ControlBoard::~ControlBoard()
{
    disconnect(server, SIGNAL(disconnected()), 0, 0);
    delete server;
    delete ui;
}


void ControlBoard::on_connect_button_clicked()
{
    QString host = ui->server_address_line->text();
    QString port = ui->server_port_line->text();
    server->connectToHost(host, port.toInt());
    if (!server->waitForConnected(1000)){
        delete server;
        server = new QTcpSocket( this );
        QString error_msg;
        error_msg = QString("Failed to connect to server: ") + host + QString(" on port ") + port;
        QMessageBox::warning(this, "Connection error", error_msg);
        return ;
    }

    std::string cmd;
    QString res_str;
    Response res;
    cmd = GET_COMMAND_LIST + ENDER;
    server->write(cmd.c_str());
    res_str = readCommand();
    res = parse_response(res_str.toStdString());
    if (res.status == OK){
        for (std::string command_name : res.results){
            ui->command_box->addItem(QString(command_name.c_str()));
        }
    }

    cmd = GET_CHANNEL_NUMBER + ENDER;
    server->write(cmd.c_str());
    res_str = readCommand();
    res = parse_response(res_str.toStdString());
    if (res.status == OK){
        int ch_num = parse_channel(res.results[0]);
        for (int i = 0; i < ch_num; ++i){
            ui->channel_box->addItem(QString::number(i));
        }
    }

    ui->server_connection->setTitle("Server (connected)");
    ui->server_address_line->setEnabled(false);
    ui->server_port_line->setEnabled(false);
    ui->connect_button->setEnabled(false);
    ui->disconnect_button->setEnabled(true);
    ui->group_command_box->setEnabled(true);
}

void ControlBoard::on_disconnect_button_clicked()
{
    disconnect(server, SIGNAL(disconnected()), 0, 0);
    delete server;
    server = new QTcpSocket( this );

    ui->server_connection->setTitle("Server (disconnected)");
    ui->server_address_line->setEnabled(true);
    ui->server_port_line->setEnabled(true);
    ui->connect_button->setEnabled(true);
    ui->disconnect_button->setEnabled(false);
    ui->command_box->clear();
    ui->channel_box->clear();
    ui->parameters_line->clear();
    ui->command_line->clear();
    ui->group_command_box->setEnabled(false);
}

void ControlBoard::update_command_line()
{
    QString command = ui->command_box->currentText();
    QString channel = ui->channel_box->currentText();
    QString params  = ui->parameters_line->text();
    QString command_line_str;
    command_line_str = command + GAP.c_str() + CHANNEL.c_str() + channel;
    if (!params.isEmpty()){
        command_line_str += COMMA.c_str() + params;
    }
    command_line_str += ENDER.c_str();
    ui->command_line->setText(command_line_str);
}

QString ControlBoard::readCommand()
{
    while (true){
        server->waitForReadyRead(1000);
        buffer += server->readAll();
        if (buffer.contains('\n')){
            break;
        }
    }
    int ender_pos = buffer.indexOf('\n') + 1;
    QString cmd = buffer.mid(0, ender_pos);
    buffer.remove(0, ender_pos);
    return cmd;
}

void ControlBoard::command_box_log(QString str)
{
    QLocale testLocale = QLocale(QLocale::English, QLocale::UnitedStates);
    QString dateTimeText = testLocale.toString(QDateTime::currentDateTime(), "dd.MM.yyyy hh:mm:ss : ");
    if (str.back() == '\n'){
        str.remove(str.length() - 1, 1);
    }
    QString line = dateTimeText + str;
    ui->textBrowser->append(line);
}

void ControlBoard::on_send_button_clicked()
{
    server->write(ui->command_line->text().toStdString().c_str());
    command_box_log(ui->command_line->text());
    QString res_str = readCommand();
    command_box_log(res_str);
    Response res = parse_response(res_str.toStdString());
}
