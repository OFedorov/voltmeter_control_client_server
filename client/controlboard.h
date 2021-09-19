#ifndef CONTROLBOARD_H
#define CONTROLBOARD_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class ControlBoard; }
QT_END_NAMESPACE

class ControlBoard : public QMainWindow
{
    Q_OBJECT

public:
    ControlBoard(QWidget *parent = nullptr);
    ~ControlBoard();

private slots:
    void on_connect_button_clicked();

    void on_disconnect_button_clicked();

    void update_command_line();

    void on_send_button_clicked();

private:
    Ui::ControlBoard *ui;
    QTcpSocket *server;
    QString buffer;
    QString readCommand();
    void command_box_log(QString str);
};
#endif // CONTROLBOARD_H
