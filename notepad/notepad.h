#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <QPixmap>
#include <QColorDialog>
#include <QAction>
QT_BEGIN_NAMESPACE
namespace Ui {
class Notepad;
}
QT_END_NAMESPACE

class Notepad : public QMainWindow
{
    Q_OBJECT

public:
    explicit Notepad(QWidget *parent = nullptr);
    ~Notepad();

private slots:
    void newDocument();
    void open();
    void save();
    void saveAs();
    void print();
    void selectFont();
    void setFontBold(bool bold);
    void setFontUnderline(bool underline);
    void setFontItalic(bool italic);
    void about();
    void setBackgroundColor();      // 原有的纯色背景
    void setImageBackground();      // 新增：设置图片背景
    void resetBackground();         // 原有的重置背景
    void setTextColor();
    void addCustomButton();
private:
    Ui::Notepad *ui;
    QString currentFile;
    QColor currentBgColor;
    QColor currentTextColor;
    QString currentImagePath;
    bool userDefinedTextColor;    // 新增：保存当前图片路径
    void setupBackgroundMenu();     // 更新这个函数
};

#endif // NOTEPAD_H

