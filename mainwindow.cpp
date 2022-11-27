#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "equminer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MyJudger");

    ui->lineEdit->setPlaceholderText("请在这里输入INPUT文件夹的绝对路径");
    connect(ui->pushButton_compare,&QPushButton::clicked,[&](){
        //read path of INPUT
        input_path =  ui->lineEdit->text().toStdString();
        const char* input_path_name = input_path.c_str();
        DIR* input_dir = opendir(input_path_name);

        if(input_dir == NULL){
             QMessageBox::information(this, "Error", "找不到input文件夹！",QStringLiteral("返回"));
        }
        else{
            dirent* curdir;
            //Traverse subfolders of INPUT
            while((curdir = readdir(input_dir)) != NULL){
                curdir_name = curdir->d_name;
                if(curdir_name != ".vscode" && curdir_name != "." && curdir_name != ".."){
                    //generate Input and Stdin_format objects
                    curdir_path = input_path + "\\" + curdir_name;
                    //cout<<curdir_path<<endl;
                    generate_input();
                    generate_stdin_format();

                    //generate Equminer and Output objects
                    //then compare each Input object, get results
                    equminer_test();

                    //empty input and stdin vector, for next directory
                    input_vec.clear();
                    std_vec.clear();
                }
            }
            closedir(input_dir);
        }

        //output results
        output_csv();

        compareFinished = true;

        judge_process();

        connect(ui->pushButton_equal,&QPushButton::clicked,[&](){
            output_vec[cur_num]->set_result(1);
            output_csv();
            judge_process();
        });

        connect(ui->pushButton_ineuqal,&QPushButton::clicked,[&](){
            output_vec[cur_num]->set_result(0);
            output_csv();
            judge_process();
        });

        connect(ui->pushButton_doubt,&QPushButton::clicked,[&](){
            output_vec[cur_num]->set_result(2);
            output_csv();
            judge_process();
        });
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generate_input(){
    const char* curdir_path_name = curdir_path.c_str();
    DIR* curdir_dir = opendir(curdir_path_name);
    if(curdir_dir == NULL){
        cout<<"Fail to open current file directory!"<<endl;
    }
    else{
        dirent* curfile;
        //Traverse files of CURRENT directory
        while((curfile = readdir(curdir_dir)) != NULL){
            curfile_name = curfile->d_name;
            if(curfile_name != "." && curfile_name != ".."
            && curfile_name != "stdin_format.txt"
            && curfile_name != "input.txt"
            && curfile_name != "output1.txt" && curfile_name != "output2.txt"){
                curfile_absolute_path = curdir_path + "\\" + curfile_name;
                curfile_relative_path = "input\\" + curdir_name + "\\" + curfile_name;
                Input* curInput = new Input(curfile_name,curfile_absolute_path,curfile_relative_path,curdir_path);
                input_vec.push_back(curInput);
                //cout<<"Read file: "<<curfile_name<<' '<<curfile_absolute_path<<' '<<curfile_relative_path<<endl;
            }
        }
        closedir(curdir_dir);
    }
}

void MainWindow::generate_stdin_format(){
    curfile_absolute_path = curdir_path + "\\stdin_format.txt";
    ifstream in_file;
    in_file.open(curfile_absolute_path,ios::in);
    if(!in_file){
        cout<<"Fail to open stdin_format.txt!"<<endl;
    }
    else{
        char buffer[51];
        while(in_file>>buffer){
            int t = 0,l = 0,r = 0;
            switch(buffer[0]){
                case 'i':
                {
                    t = 1;
                    if(buffer[3] == '('){
                        string left_num = "",right_num = "";
                        for(int i = 4;i <= 50;++i){
                            if(buffer[i] == ',')
                                break;
                            else{
                                left_num += buffer[i];
                            }
                        }
                        l = stoi(left_num);
                        for(int i = 4 + left_num.size() + 1;i<=50;++i){
                            if(buffer[i] == ')')
                                break;
                            else{
                                right_num += buffer[i];
                            }
                        }
                        r = stoi(right_num);
                    }
                    else{
                        cout<<"Error in stdin_format.txt!"<<endl;
                    }
                    break;
                }
                case 'c':
                {
                    t = 2;
                    l = 1;
                    r = 1;
                    break;
                }
                case 's':
                {
                    t = 2;
                    if(buffer[6] == '('){
                        string left_num = "",right_num = "";
                        for(int i = 7;i <= 50;++i){
                            if(buffer[i] == ',')
                                break;
                            else{
                                left_num += buffer[i];
                            }
                        }
                        //cout<<left_num<<endl;
                        const char* ln = left_num.c_str();
                        l = atoi(ln);
                        for(int i = 7 + left_num.size() + 1;i<=50;++i){
                            if(buffer[i] == ')')
                                break;
                            else{
                                right_num += buffer[i];
                            }
                        }
                        //cout<<right_num<<endl;
                        const char* rn = right_num.c_str();
                        r = atoi(rn);
                    }
                    else{
                        cout<<"Error in stdin_format.txt!"<<endl;
                    }
                    break;
                }
                default:
                {
                    cout<<"Error in stdin_format.txt!"<<endl;
                    break;
                }
            }
            if(l <= r){
                Stdin_format* sf = new Stdin_format(t,l,r);
                std_vec.push_back(sf);
            }
            else{
                cout<<"Error in stdin_format.txt!"<<endl;
            }
            //cout<<"Read file /stdin_format.txt : "<<t<<' '<<l<<' '<<r<<endl;
        }
    }
    in_file.close();
}

void MainWindow::equminer_test(){
    for(int i = 0;i < input_vec.size()-1;++i){
        for(int j = i+1;j < input_vec.size();++j){
            //generate Output objects
            Output* op = new Output(input_vec[i],input_vec[j]);
            output_vec.push_back(op);

            //generate Equminer objects
            Equminer* eq = new Equminer(input_vec[i],input_vec[j],op,std_vec);
            equ_vec.push_back(eq);

            //random test
            eq->case_compare();
        }
    }
}

void MainWindow::output_csv(){
    //equal
    ofstream out_file;
    out_file.open("equal.csv",ios::out);
    if (!out_file){
        cout<<"fail to open equal"<<endl;
        //exit(-1);
    }
    out_file<<"file1"<<','<<"file2"<<endl;

    for(int i = 0;i < output_vec.size();++i){
        if(output_vec[i]->get_result() == 1){
            //equal
            out_file<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
            //cout<<"equal: "<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
        }
    }
    out_file.close();

    //inequal
    out_file.open("inequal.csv",ios::out);
    if (!out_file){
        cout<<"fail to open inequal"<<endl;
        //exit(-1);
    }
    out_file<<"file1"<<','<<"file2"<<endl;

    for(int i = 0;i < output_vec.size();++i){
        if(output_vec[i]->get_result() == 0){
            //inequal
            out_file<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
            //cout<<"inequal: "<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
        }
    }
    out_file.close();

    //doubt
    out_file.open("doubt.csv",ios::out);
    if (!out_file){
        cout<<"fail to open doubt"<<endl;
        //exit(-1);
    }
    out_file<<"file1"<<','<<"file2"<<endl;

    for(int i = 0;i < output_vec.size();++i){
        if(output_vec[i]->get_result() == 2){
            //equal
            out_file<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
            //cout<<"equal: "<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<','<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<endl;
        }
    }
    out_file.close();
}

//C:\Users\86189\Desktop\input
void MainWindow::judge_process(){
    /**
    for(int i = 0;i<output_vec.size();++i){
        cout<<output_vec[i]->get_input1()->get_relative_path_to_INPUT()<<' '<<output_vec[i]->get_input2()->get_relative_path_to_INPUT()<<' '<<output_vec[i]->get_result()<<endl;
    }
    cout<<cur_num<<endl;
    **/

    ui->listWidget_file1->clear();
    ui->listWidget_file2->clear();
    ui->label->setText("");
    ui->label_2->setText("");

    bool flag = false;
    for(int i = cur_num+1;i<output_vec.size();++i){
        if(output_vec[i]->get_result() == 1){
            flag = true;
            cur_num = i;
            break;
        }
    }

    if(!flag){
        if(!judgeFinished){
            connect(ui->pushButton_equal,&QPushButton::clicked,[&](){
                QMessageBox::information(this, "Tips", "您已完成对所有文件的人工确认！",QStringLiteral("确定"));
            });

            connect(ui->pushButton_ineuqal,&QPushButton::clicked,[&](){
                QMessageBox::information(this, "Tips", "您已完成对所有文件的人工确认！",QStringLiteral("确定"));
            });

            connect(ui->pushButton_doubt,&QPushButton::clicked,[&](){
                QMessageBox::information(this, "Tips", "您已完成对所有文件的人工确认！",QStringLiteral("确定"));
            });

            judgeFinished = true;
        }
        return;
    }

    fstream in_file1;
    QStringList qsl;
    in_file1.open(output_vec[cur_num]->get_input1()->get_absolute_path(),ios::in);
    if(!in_file1){
        cerr<<"Error to open file!"<<endl;
        return;
    }
    else{
        string line;
        while(getline(in_file1, line)){
            QString l = QString(QString::fromLocal8Bit(line.c_str()));
            qsl.append(l);
            //cout<<buf<<' ';
        }
        QString p = QString(QString::fromLocal8Bit(output_vec[cur_num]->get_input1()->get_relative_path_to_INPUT().c_str()));
        ui->label->setText(p);
    }
    in_file1.close();
    //cout<<endl;

    fstream in_file2;
    QStringList qsl2;
    in_file2.open(output_vec[cur_num]->get_input2()->get_absolute_path(),ios::in);
    if(!in_file2){
        cerr<<"Error to open file!"<<endl;
        return;
    }
    else{
        string line;
        while(getline(in_file2, line)){
            QString l = QString(QString::fromLocal8Bit(line.c_str()));
            qsl2.append(l);
            //cout<<buf<<' ';
        }
        QString p = QString(QString::fromLocal8Bit(output_vec[cur_num]->get_input2()->get_relative_path_to_INPUT().c_str()));
        ui->label_2->setText(p);
    }
    in_file2.close();

    QVector<pair<int,int>> same;
    int cur = 0;
    for(int i = 0;i<qsl.size();++i){
        for(int j = cur;j<qsl2.size();++j){
            if(qsl[i] == qsl2[j]){
                cur = j+1;
                same.append(make_pair(i,j));
                continue;
            }
        }
    }

    int curline = 0;
    QVector<int> redl,redr;
    for(int i = 0;i<same.size();++i){
        int gapl = 0,gapr = 0;
        if(i == 0){
            gapl = same[i].second;
            gapr = same[i].first;
        }
        else{
            gapl = same[i].second - same[i-1].second -1;
            gapr = same[i].first - same[i-1].first -1;
        }
        for(int j = curline;j<curline+gapr;++j){
            qsl2.insert(j," ");
            redl.append(j);
        }
        for(int j = curline;j<curline+gapl;++j){
            qsl.insert(j+gapr," ");
            redr.append(j+gapr);
        }
        curline += (gapl+gapr+1);
    }
    int sizel = qsl.size(),sizer = qsl2.size();
    for(int i = curline;i<sizel;++i){
        qsl2.insert(i," ");
        redl.append(i);
    }
    for(int i = curline;i<sizer;++i){
        qsl.insert(i+sizel-curline," ");
        redr.append(i+sizel-curline);
    }

    ui->listWidget_file1->insertItems(0,qsl);
    ui->listWidget_file2->insertItems(0,qsl2);
    for(int i = 0;i<redl.size();++i)
        ui->listWidget_file1->item(redl[i])->setForeground(QColor("red"));
    for(int i = 0;i<redr.size();++i)
        ui->listWidget_file2->item(redr[i])->setForeground(QColor("red"));

    QBrush br;
    br.setColor(QColor(190,190,190));
    br.setStyle(Qt::FDiagPattern);
    for(int i = 0;i<qsl.size();++i){
        if(qsl[i] == " ")
            ui->listWidget_file1->item(i)->setBackground(br);
    }
    for(int i = 0;i<qsl2.size();++i){
        if(qsl2[i] == " ")
            ui->listWidget_file2->item(i)->setBackground(br);
    }
}
