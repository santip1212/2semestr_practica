#include <QApplication>
#include "ImageCompressorApp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    ImageCompressorApp window; 
    window.show(); 
    
    return app.exec(); 
} 