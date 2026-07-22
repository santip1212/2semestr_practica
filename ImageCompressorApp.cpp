#include "ImageCompressorApp.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QImageReader>
#include <iostream>
#include <fstream>

ImageCompressorApp::ImageCompressorApp(QWidget *parent)
    : QMainWindow(parent)
    , dctCompressor(80)
    , fractalCompressor(8, 16, 4) {
    setupUI();
}

ImageCompressorApp::~ImageCompressorApp() {
}

void ImageCompressorApp::setupUI() {
    setWindowTitle("Image Compressor");
    setMinimumSize(800, 600);
    
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Верхняя панель
    topLayout = new QHBoxLayout();
    
    // Кнопка открытия
    openButton = new QPushButton("Открыть изображение", this);
    openButton->setMinimumHeight(35);
    openButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 12px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
    );
    connect(openButton, &QPushButton::clicked, this, &ImageCompressorApp::openImage);
    topLayout->addWidget(openButton);
    
    // Выбор алгоритма
    QLabel* algorithmLabel = new QLabel("Алгоритм:", this);
    topLayout->addWidget(algorithmLabel);
    
    algorithmComboBox = new QComboBox(this);
    algorithmComboBox->addItem("RLE (без потерь)");
    algorithmComboBox->addItem("Huffman (без потерь)");
    algorithmComboBox->addItem("LZW (без потерь)");
    algorithmComboBox->addItem("DCT (с потерями)");
    algorithmComboBox->addItem("Fractal (с потерями)");
    algorithmComboBox->setMinimumWidth(180);
    algorithmComboBox->setStyleSheet(
        "QComboBox {"
        "   border: 1px solid #cccccc;"
        "   border-radius: 4px;"
        "   padding: 5px 10px;"
        "   background: white;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "}"
    );
    topLayout->addWidget(algorithmComboBox);
    
    // Кнопка сжатия
    compressButton = new QPushButton("Сжать", this);
    compressButton->setMinimumHeight(35);
    compressButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 12px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}"
    );
    connect(compressButton, &QPushButton::clicked, this, &ImageCompressorApp::compressImage);
    topLayout->addWidget(compressButton);
    
    // Кнопка сохранения
    saveButton = new QPushButton("Сохранить результат", this);
    saveButton->setMinimumHeight(35);
    saveButton->setEnabled(false);
    saveButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #FF9800;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 12px;"
        "   padding: 8px 16px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #F57C00;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #E65100;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "}"
    );
    connect(saveButton, &QPushButton::clicked, this, &ImageCompressorApp::saveResult);
    topLayout->addWidget(saveButton);
    
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);
    
    // Информационная панель
    infoText = new QTextEdit(this);
    infoText->setReadOnly(true);
    infoText->setMaximumHeight(100);
    infoText->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #cccccc;"
        "   border-radius: 5px;"
        "   background-color: #fafafa;"
        "   font-family: monospace;"
        "   font-size: 12px;"
        "}"
    );
    infoText->setText("Загрузите изображение для начала работы...");
    mainLayout->addWidget(infoText);
    
    // Область изображения
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: 2px solid #cccccc;"
        "   border-radius: 5px;"
        "   background-color: #f0f0f0;"
        "}"
    );
    
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #f0f0f0;"
        "   border: 1px solid #dddddd;"
        "   border-radius: 3px;"
        "}"
    );
    imageLabel->setMinimumSize(400, 300);
    imageLabel->setText("Изображение не загружено\n\nПоддерживаемые форматы: BMP, TIFF, JPG, PNG");
    
    scrollArea->setWidget(imageLabel);
    mainLayout->addWidget(scrollArea);
    
    resize(900, 700);
}

void ImageCompressorApp::openImage() {
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение",
        QDir::homePath(),
        "Изображения (*.bmp *.BMP *.tiff *.TIFF *.tif *.TIF *.jpg *.jpeg *.png)"
    );
    
    if (!filePath.isEmpty()) {
        loadImage(filePath);
    }
}

void ImageCompressorApp::loadImage(const QString &filePath) {
    // Проверка формата через расширение
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    
    if (suffix == "bmp" || suffix == "tiff" || suffix == "tif" ||
        suffix == "jpg" || suffix == "jpeg" || suffix == "png") {
        // Поддерживаемый формат
    } else {
        QMessageBox::warning(
            this,
            "Ошибка формата",
            "Поддерживаются форматы: BMP, TIFF, JPG, PNG.\n"
            "Пожалуйста, выберите файл с одним из этих расширений."
        );
        return;
    }
    
    QImage loadedImage;
    if (!loadedImage.load(filePath)) {
        QMessageBox::warning(
            this,
            "Ошибка загрузки",
            "Не удалось загрузить изображение.\n"
            "Убедитесь, что файл не поврежден."
        );
        return;
    }
    
    currentImage = loadedImage;
    currentFilePath = filePath;
    currentDecompressedData.clear();
    currentCompressedData.clear();
    saveButton->setEnabled(false);
    
    displayImage(loadedImage);
    
    QFileInfo info(filePath);
    setWindowTitle(QString("Image Compressor - [%1]").arg(info.fileName()));
    
    // Информация об изображении
    updateInfo(QString("Загружено: %1 (%2 x %3 px)")
               .arg(info.fileName())
               .arg(currentImage.width())
               .arg(currentImage.height()));
}

void ImageCompressorApp::displayImage(const QImage &image) {
    if (image.isNull()) {
        imageLabel->setText("Изображение не загружено");
        imageLabel->setPixmap(QPixmap());
        return;
    }
    
    QSize imageSize = image.size();
    QSize viewportSize = scrollArea->viewport()->size();
    
    if (imageSize.width() > viewportSize.width() || imageSize.height() > viewportSize.height()) {
        QPixmap pixmap = QPixmap::fromImage(image);
        QPixmap scaledPixmap = pixmap.scaled(
            viewportSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        imageLabel->setPixmap(scaledPixmap);
    } else {
        imageLabel->setPixmap(QPixmap::fromImage(image));
    }
    
    imageLabel->adjustSize();
}

bool ImageCompressorApp::isImageFormatSupported(const QString &filePath) {
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    return (suffix == "bmp" || suffix == "tiff" || suffix == "tif" ||
            suffix == "jpg" || suffix == "jpeg" || suffix == "png");
}

void ImageCompressorApp::updateInfo(const QString& text) {
    infoText->setText(text);
}

QString ImageCompressorApp::formatSize(size_t bytes) {
    const double kb = static_cast<double>(bytes) / 1024.0;
    if (kb < 1024) {
        return QString::number(kb, 'f', 2) + " KB";
    }
    return QString::number(kb / 1024.0, 'f', 2) + " MB";
}

ICompressor* ImageCompressorApp::getCompressor(const QString& name) {
    if (name == "RLE (без потерь)") return &rleCompressor;
    if (name == "Huffman (без потерь)") return &huffmanCompressor;
    if (name == "LZW (без потерь)") return &lzwCompressor;
    if (name == "DCT (с потерями)") return &dctCompressor;
    if (name == "Fractal (с потерями)") return &fractalCompressor;
    return nullptr;
}

void ImageCompressorApp::compressImage() {
    if (currentImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала загрузите изображение!");
        return;
    }
    
    QString algorithmName = algorithmComboBox->currentText();
    ICompressor* compressor = getCompressor(algorithmName);
    if (!compressor) {
        QMessageBox::warning(this, "Ошибка", "Неизвестный алгоритм!");
        return;
    }
    
    // Конвертация QImage в RGB
    QImage rgbImage = currentImage.convertToFormat(QImage::Format_RGB888);
    int width = rgbImage.width();
    int height = rgbImage.height();
    int channels = 3;
    
    std::vector<uint8_t> imageData(width * height * channels);
    memcpy(imageData.data(), rgbImage.bits(), imageData.size());
    
    // Сжатие
    updateInfo("Выполняется сжатие алгоритмом " + algorithmName + "...");
    QCoreApplication::processEvents();
    
    currentCompressedData = compressor->compress(imageData, width, height, channels);
    
    if (currentCompressedData.empty()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось выполнить сжатие!");
        updateInfo("Ошибка сжатия");
        return;
    }
    
    // Декомпрессия
    int decWidth, decHeight, decChannels;
    currentDecompressedData = compressor->decompress(currentCompressedData, decWidth, decHeight, decChannels);
    
    if (currentDecompressedData.empty()) {
        QMessageBox::warning(this, "Ошибка", "Не удалось выполнить декомпрессию!");
        updateInfo("Ошибка декомпрессии");
        return;
    }
    
    currentWidth = decWidth;
    currentHeight = decHeight;
    currentChannels = decChannels;
    currentAlgorithmName = algorithmName;
    
    // Вычисление размеров
    size_t originalSize = imageData.size();
    size_t compressedSize = currentCompressedData.size();
    double ratio = static_cast<double>(originalSize) / compressedSize;
    
    // Формирование информации
    QString info = QString(
        "Алгоритм: %1\n"
        "Исходный размер: %2\n"
        "Сжатый размер (JPEG): %3\n"
        "Коэффициент сжатия: %4x"
    ).arg(algorithmName)
     .arg(formatSize(originalSize))
     .arg(formatSize(compressedSize))
     .arg(ratio, 0, 'f', 2);
    
    updateInfo(info);
    saveButton->setEnabled(true);
    
    // Отображение результата в отдельном окне
    QImage resultImage(currentDecompressedData.data(), decWidth, decHeight, decWidth * decChannels, QImage::Format_RGB888);
    ResultWindow* resultWindow = new ResultWindow(resultImage, info, this);
    resultWindow->setAttribute(Qt::WA_DeleteOnClose);
    resultWindow->show();
}

void ImageCompressorApp::saveResult() {
    if (currentDecompressedData.empty()) {
        return;
    }
    
    // Получаем путь к папке bin
    QString binPath = QCoreApplication::applicationDirPath();
    QDir binDir(binPath);
    
    // Создаем имя файла
    QFileInfo fileInfo(currentFilePath);
    QString baseName = fileInfo.completeBaseName();
    QString algorithmName = algorithmComboBox->currentText().split(" ").first();
    QString fileName = baseName + "_" + algorithmName + ".jpg";
    QString filePath = binDir.filePath(fileName);
    
    // Сохраняем JPEG
    QImage resultImage(currentDecompressedData.data(), currentWidth, currentHeight, currentWidth * currentChannels, QImage::Format_RGB888);
    
    if (resultImage.save(filePath, "JPG", 80)) {
        QMessageBox::information(this, "Сохранено", 
            QString("Изображение сохранено:\n%1\n\nРазмер: %2")
            .arg(filePath)
            .arg(formatSize(currentCompressedData.size())));
        updateInfo(QString("Сохранено: %1 (%2)").arg(fileName).arg(formatSize(currentCompressedData.size())));
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл!");
    }
}