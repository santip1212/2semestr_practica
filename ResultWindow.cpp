#include "ResultWindow.h"
#include <QPixmap>
#include <QResizeEvent>

ResultWindow::ResultWindow(const QImage& image, const QString& info, QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("Результат декомпрессии");
    setMinimumSize(600, 500);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* layout = new QVBoxLayout(central);

    // Информационная панель
    infoLabel = new QLabel(info, this);
    infoLabel->setStyleSheet(
        "QLabel {"
        "   background-color: #f0f0f0;"
        "   padding: 10px;"
        "   border: 1px solid #cccccc;"
        "   border-radius: 5px;"
        "   font-size: 12px;"
        "}"
        );
    layout->addWidget(infoLabel);

    // Область прокрутки для изображения
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

    // Отображение изображения
    QPixmap pixmap = QPixmap::fromImage(image);
    QSize viewportSize = scrollArea->viewport()->size();

    if (viewportSize.width() > 0 && viewportSize.height() > 0) {
        QPixmap scaled = pixmap.scaled(
            viewportSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        imageLabel->setPixmap(scaled);
    } else {
        imageLabel->setPixmap(pixmap);
    }
    imageLabel->adjustSize();

    scrollArea->setWidget(imageLabel);
    layout->addWidget(scrollArea);

    // Кнопка закрытия
    QPushButton* closeButton = new QPushButton("Закрыть", this);
    closeButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 8px 20px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #d32f2f;"
        "}"
        );
    connect(closeButton, &QPushButton::clicked, this, &QMainWindow::close);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();
    layout->addLayout(buttonLayout);
}

void ResultWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);

    if (imageLabel && !imageLabel->pixmap().isNull()) {
        QSize viewportSize = scrollArea->viewport()->size();

        // Исправлено: pixmap() возвращает QPixmap по значению
        QPixmap currentPixmap = imageLabel->pixmap();
        QPixmap scaled = currentPixmap.scaled(
            viewportSize,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );
        imageLabel->setPixmap(scaled);
        imageLabel->adjustSize();
    }
}