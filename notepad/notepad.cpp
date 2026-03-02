#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif // QT_CONFIG(printdialog)
#include <QPrinter>
#endif // QT_CONFIG(printer)
#endif // QT_PRINTSUPPORT_LIB
#include <QFont>
#include <QFontDialog>

#include "notepad.h"
#include "ui_notepad.h"

Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &Notepad::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &Notepad::open);
    connect(ui->actionSave, &QAction::triggered, this, &Notepad::save);
    connect(ui->actionSave_as, &QAction::triggered, this, &Notepad::saveAs);
    //connect(ui->actionPrint, &QAction::triggered, this, &Notepad::print);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
#if QT_CONFIG(clipboard)
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
#endif
    connect(ui->textEdit, &QTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QTextEdit::undo);

// Disable menu actions for unavailable features
#if !defined(QT_PRINTSUPPORT_LIB) || !QT_CONFIG(printer)
    //ui->actionPrint->setEnabled(false);
#endif

    // #if !QT_CONFIG(clipboard)
    //     ui->actionCut->setEnabled(false);
    //     ui->actionCopy->setEnabled(false);
    //     ui->actionPaste->setEnabled(false);
    // #endif
    setupBackgroundMenu();  // 设置背景菜单
    addCustomButton();
    // 初始背景色
    QPalette palette = ui->textEdit->palette();
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::Text, Qt::black);
    ui->textEdit->setPalette(palette);
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::newDocument()
{
    currentFile.clear();
    ui->textEdit->setText(QString());
}

void Notepad::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
}

void Notepad::save()
{
    QString fileName;
    // If we don't have a filename from before, get one.
    if (currentFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "保存");
        if (fileName.isEmpty())
            return;
        currentFile = fileName;
    } else {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void Notepad::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void Notepad::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev, this);
    if (dialog.exec() == QDialog::Rejected)
        return;
#endif // QT_CONFIG(printdialog)
    ui->textEdit->print(&printDev);
#endif // QT_CONFIG(printer)
}

void Notepad::selectFont()
{
    bool fontSelected;
    QFont font = QFontDialog::getFont(&fontSelected, this);
    if (fontSelected)
        ui->textEdit->setFont(font);
}

void Notepad::setFontUnderline(bool underline)
{
    ui->textEdit->setFontUnderline(underline);
}

void Notepad::setFontItalic(bool italic)
{
    ui->textEdit->setFontItalic(italic);
}

void Notepad::setFontBold(bool bold)
{
    bold ? ui->textEdit->setFontWeight(QFont::Bold) :
        ui->textEdit->setFontWeight(QFont::Normal);
}

void Notepad::setupBackgroundMenu()
{
    QMenu *bgMenu = menuBar()->addMenu("背景");

    // 纯色背景
    QAction *colorAction = new QAction("纯色背景...", this);
    connect(colorAction, &QAction::triggered,
            this, &Notepad::setBackgroundColor);
    bgMenu->addAction(colorAction);

    // // 图片背景（新增）
    // QAction *imageAction = new QAction("图片背景...", this);
    // connect(imageAction, &QAction::triggered,
    //         this, &Notepad::setImageBackground);
    // bgMenu->addAction(imageAction);

    // bgMenu->addSeparator();  // 分隔线

    // 重置背景
    QAction *resetAction = new QAction("重置默认背景", this);
    connect(resetAction, &QAction::triggered,
            this, &Notepad::resetBackground);
    bgMenu->addAction(resetAction);
}
void Notepad::setImageBackground()
{
    // 打开文件选择对话框，只显示图片文件
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "选择背景图片",
                                                    QDir::homePath(),  // 从用户主目录开始
                                                    "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif *.svg);;所有文件 (*)");

    if (fileName.isEmpty())
        return;

    // 保存图片路径
    currentImagePath = fileName;

    // 使用样式表设置背景图片
    // 注意：需要将文件路径中的反斜杠替换为正斜杠
    QString filePath = fileName;
    filePath.replace("\\", "/");

    QString styleSheet =
        "QTextEdit {"
        "   background-image: url(\"" + filePath + "\");"
                     "   background-repeat: repeat;"
                     "   background-position: top left;"
                     "   background-attachment: fixed;"
                     "}";

    ui->textEdit->setStyleSheet(styleSheet);

    // 可选：在状态栏显示提示
    // statusBar()->showMessage("图片背景已设置: " + QFileInfo(fileName).fileName(), 3000);
}
void Notepad::resetBackground()
{
    // 清除样式表
    ui->textEdit->setStyleSheet("");

    // 恢复默认调色板
    ui->textEdit->setPalette(QPalette());

    // 重置为白色背景
    currentBgColor = Qt::white;
    currentImagePath = "";

    // 设置白色背景
    QPalette palette = ui->textEdit->palette();
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::Text, Qt::black);
    ui->textEdit->setPalette(palette);
}
void Notepad::setBackgroundColor()
{
    // 如果有图片背景，先清除
    if (!currentImagePath.isEmpty()) {
        ui->textEdit->setStyleSheet("");
        currentImagePath = "";
    }

    QColor color = QColorDialog::getColor(currentBgColor, this, "选择背景色");

    if (color.isValid()) {
        currentBgColor = color;

        QPalette palette = ui->textEdit->palette();
        palette.setColor(QPalette::Base, color);

        // 只有当用户没有自定义文字颜色时，才自动调整文字颜色
        if (!userDefinedTextColor) {
            // 自动调整文字颜色
            if (color.lightness() < 128) {
                palette.setColor(QPalette::Text, Qt::white);
                currentTextColor = Qt::white;
            } else {
                palette.setColor(QPalette::Text, Qt::black);
                currentTextColor = Qt::black;
            }
        } else {
            // 如果用户自定义了文字颜色，保持用户的颜色
            palette.setColor(QPalette::Text, currentTextColor);
        }

        ui->textEdit->setPalette(palette);
    }
}
void Notepad::setTextColor()  // 重命名原 setTextcolor 为 setTextColor
{
    QColor color = QColorDialog::getColor(currentTextColor, this, "选择字体颜色");

    if (color.isValid()) {
        currentTextColor = color;
        userDefinedTextColor = true;  // 标记用户已自定义文字颜色

        QPalette palette = ui->textEdit->palette();
        palette.setColor(QPalette::Text, color);
        ui->textEdit->setPalette(palette);
    }
}
void Notepad::addCustomButton()
{
    // 创建按钮
    QAction *customAction = new QAction("字体", this);
    //QPushButton *menuButton = new QPushButton("字体", this);

    // 设置按钮样式，让它看起来像菜单项
    //menuButton->setFlat(true);  // 扁平化样式
    //menuButton->setMaximumHeight(25);
    menuBar()->addAction(customAction);

    // 连接信号
    connect(customAction, &QAction::triggered,
            this, &Notepad::setTextColor);
}
void Notepad::about()
{
    QMessageBox::about(this, tr("About Notepad"),
                       tr("The <b>Notepad</b> example demonstrates how to code a basic "
                          "text editor using QtWidgets"));
}
