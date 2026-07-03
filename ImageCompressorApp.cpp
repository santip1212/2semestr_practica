#include "ImageCompressorApp.h"
#include <QImageReader>
#include <QFileInfo>

ImageCompressorApp::ImageCompressorApp(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

ImageCompressorApp::~ImageCompressorApp()
{
}

void ImageCompressorApp::setupUI()
{
    // Настройка главного окна
    setWindowTitle("Image Compressor - Preview");
    setMinimumSize(800, 600);
    
    // Создание центрального виджета
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Главный вертикальный макет
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    // Создание кнопки
    openButton = new QPushButton("Открыть изображение (BMP/TIFF)", this);
    openButton->setMinimumHeight(40);
    openButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #3d8b40;"
        "}"
    );
    
    connect(openButton, &QPushButton::clicked, this, &ImageCompressorApp::openImage);
    
    // Макет для кнопки
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // Создание области прокрутки для изображения
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
    
    // Создание метки для отображения изображения
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
    imageLabel->setText("Изображение не загружено\n\nПоддерживаемые форматы: BMP, TIFF");
    
    scrollArea->setWidget(imageLabel);
    mainLayout->addWidget(scrollArea);
    
    // Установка размера окна
    resize(900, 700);
}

void ImageCompressorApp::openImage()
{
    // Открытие диалога выбора файла
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите изображение",
        QDir::homePath(),
        "Изображения (*.bmp *.BMP *.tiff *.TIFF *.tif *.TIF)"
    );
    
    if (!filePath.isEmpty()) {
        loadImage(filePath);
    }
}

void ImageCompressorApp::loadImage(const QString &filePath)
{
    // Проверка формата файла
    if (!isImageFormatSupported(filePath)) {
        QMessageBox::warning(
            this,
            "Ошибка формата",
            "Поддерживаются только форматы BMP и TIFF.\n"
            "Пожалуйста, выберите файл с расширением .bmp или .tiff"
        );
        return;
    }
    
    // Загрузка изображения
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
    
    // Сохранение текущего изображения
    currentImage = loadedImage;
    currentFilePath = filePath;
    
    // Отображение изображения
    displayImage(loadedImage);
    
    // Обновление заголовка окна
    QFileInfo fileInfo(filePath);
    setWindowTitle(QString("Image Compressor - Preview [%1]").arg(fileInfo.fileName()));
}

void ImageCompressorApp::displayImage(const QImage &image)
{
    if (image.isNull()) {
        imageLabel->setText("Изображение не загружено");
        imageLabel->setPixmap(QPixmap());
        return;
    }
    
    // Получение размеров изображения
    QSize imageSize = image.size();
    QSize viewportSize = scrollArea->viewport()->size();
    
    // Если изображение больше области просмотра, масштабируем с сохранением пропорций
    if (imageSize.width() > viewportSize.width() || imageSize.height() > viewportSize.height()) {
        // Масштабирование изображения, чтобы оно поместилось в область просмотра
        QPixmap pixmap = QPixmap::fromImage(image);
        QPixmap scaledPixmap = pixmap.scaled(
            viewportSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        imageLabel->setPixmap(scaledPixmap);
    } else {
        // Если изображение меньше или равно области просмотра, отображаем в оригинальном размере
        imageLabel->setPixmap(QPixmap::fromImage(image));
    }
    
    // Обновление размера метки в соответствии с изображением
    imageLabel->adjustSize();
}

bool ImageCompressorApp::isImageFormatSupported(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    // Проверка поддерживаемых расширений
    if (suffix == "bmp" || suffix == "tiff" || suffix == "tif") {
        return true;
    }

    // Если расширение не совпадает, пробуем проверить через QImageReader
    QImageReader reader(filePath);
    QString format = reader.format().toLower();
    return (format == "bmp" || format == "tiff");
}