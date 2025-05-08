//MainWindow.h (Başlık Dosyası):
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QMap> // Formları saklamak için QMap ekledik

    QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setProjeYolu(const QString &yol);
    QString getAktifProjeAdi() const;
    void cikisYap();

private slots:
    void on_pushButton_giris_clicked();
    void on_pushButton_cikis_clicked();
    void on_pushButton_stok_clicked();
    void yeniProje();
    void projeYukle();
    void projeyiKaydet();

    void malzemeGirisi();
    void malzemeCikisi();
    void malzemeStok();
    void malzemeTutanak();

private:
    Ui::MainWindow *ui;

    QMap<QString, QDialog*> formlar; // Formları saklamak için QMap

    QString projeYolu;
    QString projeAdi;

    void formlaraProjeYoluAta();
    QDialog* getForm(const QString &formAdi); // Formu QMap'ten al
    void addForm(const QString &formAdi, QDialog *form); // Formu QMap'e ekle
};

#endif // MAINWINDOW_H
