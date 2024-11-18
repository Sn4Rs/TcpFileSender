#include "tcpfilesender.h"

TcpFileSender::TcpFileSender(QWidget *parent)
    : QDialog(parent)
{
    loadSize = 4*1024;
    totalBytes=0;
    bytesWritten=0;
    bytesToWrite=0;
    clientProgressBar=new QProgressBar;
    clientStatusLabel=new QLabel(QStringLiteral("客戶端就緒"));
    openbtn=new QPushButton(QStringLiteral("開檔"));
    startBtn=new QPushButton(QStringLiteral("開始"));
    quitbtn=new QPushButton(QStringLiteral("退出"));
    startBtn->setEnabled(false);
    btnbox=new QDialogButtonBox;
    btnbox->addButton(startBtn,QDialogButtonBox::ActionRole);
    btnbox->addButton(openbtn,QDialogButtonBox::ActionRole);
    btnbox->addButton(quitbtn,QDialogButtonBox::RejectRole);

    QVBoxLayout *mainlay = new QVBoxLayout;
    mainlay->addWidget(clientProgressBar);
    mainlay->addWidget(clientStatusLabel);
    mainlay->addStretch(1);
    mainlay->addSpacing(10);
    mainlay->addWidget(btnbox);
    setLayout(mainlay);
    setWindowTitle(tr("檔案傳送"));

    connect(openbtn,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(startBtn,SIGNAL(clicked()),this,SLOT(start()));
    connect(&tcpClient,SIGNAL(connected()),this,SLOT(startTransfer()));
    connect(&tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProgress(qint64)));
    connect(quitbtn,SIGNAL(clicked()),this,SLOT(close()));
}

void TcpFileSender::start()
{
    QString hostadd="140.130.35.52";
    startBtn->setEnabled(false);
    bytesWritten = 0;
    clientStatusLabel->setText(tr("連接中..."));
    //tcpClient.connectToHost(QHostAddress::LocalHost,16998);
    tcpClient.connectToHost(hostadd,16998);
}
void TcpFileSender::startTransfer()
{
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,tr("應用程式"),
                             tr("無法讀取 %1 \n %2.").arg(fileName)
                                 .arg(localFile->errorString()));
        return;
    }

    totalBytes = localFile->size();
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFile = fileName.right(fileName.size() - fileName.lastIndexOf("/") - 1);

    sendOut<<qint64(0)<<qint64(0)<<currentFile;
    totalBytes+=outBlock.size();

    sendOut.device()->seek(0);
    sendOut<<totalBytes<<qint64((outBlock.size()) - sizeof(qint64)*2); //總長度寫出至outblock->filename長度寫出
    bytesToWrite = totalBytes - tcpClient.write(outBlock);//送出檔案標頭
    clientStatusLabel->setText(tr("已連接"));
    qDebug()<<currentFile<<totalBytes;
    outBlock.resize(0);
}
void TcpFileSender::updateClientProgress(qint64 numBytes)//送出後
{
    bytesWritten += (int)numBytes;
    if(bytesToWrite > 0)
    {
        outBlock = localFile->read(qMin(bytesToWrite, loadSize));//buffer vs 剩餘緩衝區 最小值
        //outBlock = localFile->read(loadSize); //不一定會完全送出的寫法 (4KB)
        bytesToWrite -= (int)tcpClient.write(outBlock);
        outBlock.resize(0);
    }
    else
    {
        localFile->close();
    }

    clientProgressBar->setMaximum(totalBytes);
    clientProgressBar->setValue(bytesWritten);
    clientStatusLabel->setText(tr("已傳送 %1 bytes").arg(bytesWritten));
}
void TcpFileSender::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())startBtn->setEnabled(true);
}

TcpFileSender::~TcpFileSender() {}
