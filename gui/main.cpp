// GUI module: starts the Qt window
#include <QApplication>
#include <QMessageBox>
#include <QStyle>
#include <cstdlib>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    srand(42);

    app.setStyle("Fusion");
    app.setPalette(app.style()->standardPalette());

    try {
        MainWindow window;
        window.show();
        return app.exec();
    } catch (string error) {
        QMessageBox::critical(NULL, "Error", QString::fromStdString(error));
        return 1;
    }
}
