#include "equminer.h"

//Input
Input::Input(){}

Input::Input(string n,string ap,string rp,string dp)
:name(n),absolute_path(ap),relative_path_to_INPUT(rp),dir_path(dp){}

string Input::get_name(){
    return name;
}

string Input::get_absolute_path(){
    return absolute_path;
}

string Input::get_relative_path_to_INPUT(){
    return relative_path_to_INPUT;
}

string Input::get_dir_path(){
    return dir_path;
}

//Output
Output::Output(){}

Output::Output(Input* i1,Input* i2):input1(i1),input2(i2){}

Input* Output::get_input1(){
    return input1;
}

Input* Output::get_input2(){
    return input2;
}

int Output::get_result(){
    return result;
}

void Output::set_result(int r){
    result = r;
}

//Stdin_format
Stdin_format::Stdin_format(){}

Stdin_format::Stdin_format(int t,int l,int r):type(t),left(l),right(r){}

int Stdin_format::get_type(){
    return type;
}
int Stdin_format::get_left(){
    return left;
}
int Stdin_format::get_right(){
    return right;
}

//Equminer
Equminer::Equminer(){}

Equminer::Equminer(Input* i1,Input* i2,Output* o,vector<Stdin_format*> &st)
:input1(i1),input2(i2),output(o),stf(st){}

string Equminer::random_case(int n){
    int t = stf[n]->get_type();
    int rand_num = stf[n]->get_left() + rand() % (stf[n]->get_right() - stf[n]->get_left() + 1);
    if(t == 1){ //int
        return to_string(rand_num);
    }
    else if(t == 2){ //char or string
        string res = "";
        char rand_char;
        for(int i = 0;i<rand_num;++i){
            rand_char = 'a' + rand() % 26;
            res += rand_char;
        }
        return res;
    }
    else{
        cout<<"Error in stdin_format.txt!"<<endl;
        return "";
    }

}

void Equminer::case_compare(){
    //cout<<input1->get_relative_path_to_INPUT()<<' '<<input2->get_relative_path_to_INPUT()<<endl;
    //create file  1.out  and  2.out  representing result of file compilation
    string creator1 = "g++ -w -o a.out ";
    creator1 += input1->get_absolute_path();
    system(creator1.c_str());
    /**
    string cr1 = "cmd /c ";
    string crea1 = creator1;
    cr1 += crea1;
    WinExec(cr1.c_str(), SW_HIDE);
    **/
    //cout<<creator1<<endl;

    string creator2 = "g++ -w -o b.out ";
    creator2 += input2->get_absolute_path();
    system(creator2.c_str());
    /**
    string cr2 = "cmd /c ";
    string crea2 = creator2;
    cr2 += crea2;
    WinExec(cr2.c_str(), SW_HIDE);
    **/

    //randomly generate samples several times and compare the output results
    for(int i = 0;i < 10;++i){  //10 times
        string inputfile_path = input1->get_dir_path() + "\\input.txt";
        string outputfile_path1 = input1->get_dir_path() + "\\output1.txt";
        string outputfile_path2 = input2->get_dir_path() + "\\output2.txt";

        ofstream out_file;
        out_file.open(inputfile_path,ios::out);

        //a group of randomly generated samples
        for(int j = 0;j<stf.size();++j){
            out_file << random_case(j) << ' ';
            cout<<random_case(j)<<' ';
        }
        out_file<<endl;
        out_file.close();

        //run random tests above with a.out and b.out,then compare results
        //a.out <input.txt >ouput.txt #在a.out所在目录下执行该命令
        char runner1[1001] = ".\\a.out <";
        strcat(runner1,inputfile_path.c_str());
        strcat(runner1," >");
        strcat(runner1,outputfile_path1.c_str());
        int r1 = system(runner1);
        /*string ru1 = "cmd /c ";
        string run1 = runner1;
        ru1 += run1;
        int r1 = WinExec(ru1.c_str(), SW_HIDE);*/

        char runner2[1001] = ".\\b.out <";
        strcat(runner2,inputfile_path.c_str());
        strcat(runner2," >");
        strcat(runner2,outputfile_path2.c_str());
        int r2 = system(runner2);
        /*string ru2 = "cmd /c ";
        string run2 = runner2;
        ru2 += run2;
        int r2 = WinExec(ru2.c_str(), SW_HIDE);*/

        //Error occurred while the program was running,skip this comparison
        if(r1 != 0 || r2 != 0){
            continue;
        }

        //read output of each file and compare
        fstream in_file1;
        vector<string> res1;
        in_file1.open(outputfile_path1,ios::in);
        if(!in_file1){
            cout<<"Fail to open output1.txt!"<<endl;
        }
        else{
            char buffer[51];
            while(in_file1>>buffer){
                string buf = buffer;
                res1.push_back(buf);
                //cout<<buf<<' ';
            }
        }
        in_file1.close();
        //cout<<endl;

        fstream in_file2;
        vector<string> res2;
        in_file2.open(outputfile_path2,ios::in);
        if(!in_file2){
            cout<<"Fail to open output2.txt!"<<endl;
        }
        else{
            char buffer[51];
            while(in_file2>>buffer){
                string buf = buffer;
                res2.push_back(buf);
                //cout<<buf<<' ';
            }
        }
        in_file2.close();
        //cout<<endl;


        //compare
        if(res1.size() != res2.size()){
            output->set_result(0);
            //cout<<"inequal!"<<endl;
            return;
        }
        else{
            for(int j = 0;j < res1.size();++j){
                if(res1[j] != res2[j]){
                    output->set_result(0);
                    //cout<<"inequal!"<<endl;
                    return;
                }
            }
        }
    }
    //same output for ten times,then equal
    output->set_result(1);
    //cout<<"equal"<<endl;
}
