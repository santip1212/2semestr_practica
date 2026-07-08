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
    setWindowTitle("Image Compressor - Preview"); // настрока главного окна
    setMinimumSize(800, 600);
    
    centralWidget = new QWidget(this); // центальный виджет
    setCentralWidget(centralWidget);
    
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    
    openButton = new QPushButton("Открыть изображение (BMP/TIFF)", this); // создание кнопки
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
    
    buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(openButton);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    scrollArea = new QScrollArea(this); // прокрутка изображения
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
    imageLabel->setText("Изображение не загружено\n\nПоддерживаемые форматы: BMP, TIFF");
    
    scrollArea->setWidget(imageLabel);
    mainLayout->addWidget(scrollArea);

    resize(900, 700); // для окна
}

void ImageCompressorApp::openImage()
{
    QString filePath = QFileDialog::getOpenFileName( // выбор файла
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

    if (!isImageFormatSupported(filePath)) { // проверка формата
        QMessageBox::warning(
            this,
            "Ошибка формата",
            "Поддерживаются только форматы BMP и TIFF.\n"
            "Пожалуйста, выберите файл с расширением .bmp или .tiff"
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
    

    currentImage = loadedImage; // сохраняем текущее изображение
    currentFilePath = filePath;
    
    displayImage(loadedImage); // выводим
    
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
    
    QSize imageSize = image.size(); // размеры изображений
    QSize viewportSize = scrollArea->viewport()->size();
    
    if (imageSize.width() > viewportSize.width() || imageSize.height() > viewportSize.height()) { // масштабирование
        QPixmap pixmap = QPixmap::fromImage(image);
        QPixmap scaledPixmap = pixmap.scaled(
            viewportSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        );
        imageLabel->setPixmap(scaledPixmap);
    } else {
        imageLabel->setPixmap(QPixmap::fromImage(image)); // в ориг масштабе, если оно входит 
    }
    
    imageLabel->adjustSize();
}

bool ImageCompressorApp::isImageFormatSupported(const QString &filePath)
{
    QFileInfo fileInfo(filePath); 
    QString suffix = fileInfo.suffix().toLower(); 

    if (suffix == "bmp" || suffix == "tiff" || suffix == "tif") { // поддерживается ли формат
        return true;
    }
 
    QImageReader reader(filePath); // проверка через QImageReader
    QString format = reader.format().toLower();
    return (format == "bmp" || format == "tiff");
}