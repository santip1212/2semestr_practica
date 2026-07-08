#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollArea>
#include <QPixmap>
#include <QImage>
#include <QString>
#include <QSize> 
#include <QDebug>

class ImageCompressorApp: public QMainWindow
{
    Q_OBJECT

public:
    ImageCompressorApp(QWidget *parent = nullptr);
    ~ImageCompressorApp();

private slots:
    void openImage();
    void loadImage(const QString &filePath); 

private:
    QWidget *centralWidget;
    QVBoxLayout *mainLayout; 
    QHBoxLayout *buttonLayout;
    QPushButton *openButton;
    QScrollArea *scrollArea; 
    QLabel *imageLabel;
     
    QImage currentImage;
    QString currentFilePath;
    
    void setupUI();
    void displayImage(const QImage &image);
    bool isImageFormatSupported(const QString &filePath);
};
