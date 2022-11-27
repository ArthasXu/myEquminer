#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QStringList>
//directory
#include <sys/types.h>
#include <dirent.h>
#include <fstream>

#include <stdlib.h>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>

#include "equminer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //equminer
    void generate_input();
    void generate_stdin_format();
    void equminer_test();
    void output_csv();

    //judger
    void judge_process();

private:
    Ui::MainWindow *ui;

    //vector
    vector<Input*> input_vec;
    vector<Output*> output_vec;
    vector<Stdin_format*> std_vec;
    vector<Equminer*> equ_vec;

    string input_path;              //path of INPUT directory
    string curdir_path;             //path of current directory
    string curdir_name;             //name of current directory
    string curfile_absolute_path;   //absolute path of current file
    string curfile_relative_path;   //relative path of current file
    string curfile_name;            //name of current file

    bool compareFinished = false;
    bool judgeFinished = false;

    int cur_num = -1;
};
#endif // MAINWINDOW_H
