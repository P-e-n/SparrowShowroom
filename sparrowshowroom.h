#ifndef SPARROWSHOWROOM_H
#define SPARROWSHOWROOM_H

#include <QMainWindow>

#include <QLocalSocket>
#include <QDataStream>

class QLocalServer;

namespace Ui {
class SparrowShowroom;
}

class SparrowShowroom : public QMainWindow
{
    Q_OBJECT

public:
    explicit SparrowShowroom(QWidget *parent = 0);
    ~SparrowShowroom();

public slots:
    void on_log_added(QString log);

signals:
    void add_log(QString log);

private:
    void clientConnected();
    void handleSocketStateChanged(QLocalSocket::LocalSocketState socketState);
    void readSocketData();


    Ui::SparrowShowroom *ui;
    QLocalServer* server;
    QLocalSocket* socket;
    QDataStream socketDataStream;
    quint32 blockSize;

};

#endif // SPARROWSHOWROOM_H
