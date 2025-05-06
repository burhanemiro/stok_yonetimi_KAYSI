#ifndef TUTANAK_H
#define TUTANAK_H


#include <QDialog>
#include <QString>


namespace Ui {
class Tutanak;
}


class Tutanak : public QDialog
{
    Q_OBJECT


public:
    explicit Tutanak(QWidget *parent, const QString &adi, const QString &yol);
    ~Tutanak();

    void setProjeAdi(const QString &adi);
    void setProjeYolu(const QString &yol);

private slots:
    void on_pushButtonMek_clicked();
    void on_pushButtonKayit_clicked();
    void on_pushButtonYaz_clicked();


private:
    Ui::Tutanak *ui;


    QString projeAdi;
    QString projeYolu;
    void malzemeListesiniYukle();
    void tutanagiKaydet();
    void tutanagiYazdir();
    QString cikisDosyaAdi;
    QString stokDosyaAdi;
};


#endif // TUTANAK_H
