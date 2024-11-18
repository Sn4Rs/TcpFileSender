#ifndef TCPFILESENDER_H
#define TCPFILESENDER_H

#include <QDialog>
#include <QtNetwork>
#include <QtWidgets>

class TcpFileSender : public QDialog
{
    Q_OBJECT
private:
    QProgressBar    *clientProgressBar;
    QLabel          *clientStatusLabel;
    QPushButton     *startBtn;
    QPushButton     *quitbtn;
    QPushButton     *openbtn;
    QTcpSocket      tcpClient;
    QDialogButtonBox *btnbox;

    qint64          totalBytes;
    qint64          bytesWritten;
    qint64          bytesToWrite;
    qint64          loadSize;
    QString         fileName;
    QFile           *localFile;
    QByteArray      outBlock;
public:
    TcpFileSender(QWidget *parent = nullptr);
    ~TcpFileSender();

public slots:
    void start();
    void startTransfer();
    void updateClientProgress(qint64 numBytes);
    void openFile();
};
#endif // TCPFILESENDER_H
