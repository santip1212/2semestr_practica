#ifndef RESULTWINDOW_H
#define RESULTWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QImage>
#include <QResizeEvent>

class ResultWindow : public QMainWindow {
    Q_OBJECT

public:
    ResultWindow(const QImage& image, const QString& info, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    QLabel* imageLabel;
    QScrollArea* scrollArea;
    QLabel* infoLabel;
};

#endif // RESULTWINDOW_H