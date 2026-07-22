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
#include <QComboBox>
#include <QTextEdit>
#include <QSize>

#include "ICompressor.h"
#include "RLECompressor.h"
#include "HuffmanCompressor.h"
#include "LZWCompressor.h"
#include "DCTCompressor.h"
#include "FractalCompressorAdapter.h"
#include "ResultWindow.h"

class ImageCompressorApp : public QMainWindow {
    Q_OBJECT

public:
    ImageCompressorApp(QWidget *parent = nullptr);
    ~ImageCompressorApp();

private slots:
    void openImage();
    void loadImage(const QString &filePath);
    void compressImage();
    void saveResult();

private:
    // UI элементы
    QWidget* centralWidget;
    QVBoxLayout* mainLayout;
    QHBoxLayout* topLayout;
    QPushButton* openButton;
    QComboBox* algorithmComboBox;
    QPushButton* compressButton;
    QPushButton* saveButton;
    QScrollArea* scrollArea;
    QLabel* imageLabel;
    QTextEdit* infoText;
    
    // Данные
    QImage currentImage;
    QString currentFilePath;
    std::vector<uint8_t> currentCompressedData;
    std::vector<uint8_t> currentDecompressedData;
    int currentWidth, currentHeight, currentChannels;
    QString currentAlgorithmName;
    ICompressor* currentCompressor;
    
    // Компрессоры
    RLECompressor rleCompressor;
    HuffmanCompressor huffmanCompressor;
    LZWCompressor lzwCompressor;
    DCTCompressor dctCompressor;
    FractalCompressorAdapter fractalCompressor;
    
    void setupUI();
    void displayImage(const QImage &image);
    bool isImageFormatSupported(const QString &filePath);
    void updateInfo(const QString& text);
    QString formatSize(size_t bytes);
    ICompressor* getCompressor(const QString& name);
};