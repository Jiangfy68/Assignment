#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> //保证DEPTH数据的输入没有精度损失
#include <cmath>
using namespace std;

template <class T>
inline T FromString(const string &str)
{
    istringstream is(str);
    T v;
    is >> v;
    return v;
}

int main()
{

    string title, yizhi1, yizhi2, yizhi3, yizhi4, yizhi5, yizhi6, yizhi7;
    double STDEP, ENDEP, RLEV;
    double DEPTH[300], SP[300], GR[300], AC[300], DEN[300];
    double dGR[300], Vsh[300], SH[300];
    double fai[300], POR[300];
    double PERM[300];

    ifstream infile;                                          //创建输入流对象
    infile.open("demo.txt", ios_base::in | ios_base::binary); //以输入和二进制方式打开文件
    if (!infile)                                              //文件打开失败处理方式
    {
        cerr << "打开文件出错！" << endl;
        return 1;
    }

    getline(infile, title); //读取demo.txt文件的第一行
    cout << title << endl;

    getline(infile, yizhi1, '2'); //读出STDEP字符
    //cout << yizhi1 << endl;

    infile.seekg(-1, ios::cur); //向前移动一个文件指针

    getline(infile, yizhi2); //以字符读出STDEP的值
    //cout << "yizhi2:" << yizhi2 << endl;

    STDEP = FromString<double>(yizhi2); //把STDEP的值转化为double型
    cout << "STDEP=" << STDEP << endl;

    getline(infile, yizhi3, '2');
    // cout<<"yizhi3:"<<yizhi3<<endl;
    infile.seekg(-1, ios::cur);

    getline(infile, yizhi4);
    ENDEP = FromString<double>(yizhi4);
    cout << "ENDEP=" << ENDEP << endl;

    getline(infile, yizhi5, '0');
    //cout<<"yizhi5:"<<yizhi5<<endl;
    infile.seekg(-1, ios::cur);

    getline(infile, yizhi6);
    RLEV = FromString<double>(yizhi6);
    cout << "RLEV=" << RLEV << endl;

    getline(infile, yizhi7, '2');

    infile.seekg(-1, ios::cur);

    //开始读取主要数据
    for (int i = 0; i <= 280; i++)
    {
        infile >> DEPTH[i];
        //cout << "DEPTH[" << i << "]:" << setprecision(8) << DEPTH[i] << endl;

        infile >> SP[i];
        //cout << "SP[" << i << "]:" << SP[i] << endl;

        infile >> GR[i];
        //cout << "GR[" << i << "]:" << GR[i] << endl;

        infile >> AC[i];
        //cout << "AC[" << i << "]:" << AC[i] << endl;

        infile >> DEN[i];
        //cout << "DEN[" << i << "]:" << DEN[i] << endl;
    }
    infile.close(); //关闭文件
    cout << "文件读取结束" << endl;

    //处理数据
    //泥质含量Vsh的确定
    double GRMAX, GRMIN;
    GRMAX = 129.41;
    GRMIN = 78.221;
    float GCUR = 3.7;
    /* cout << "请输入GRMAX和GRMIN的值：";
    cin >> GRMAX >> GRMIN >> endl; */
    for (int i = 0; i <= 280; i++)
    {
        dGR[i] = (GR[i] - GRMIN) / (GRMAX - GRMIN);
        Vsh[i] = (pow(2, (GCUR * dGR[i])) - 1) / (pow(2, GCUR) - 1);
        //cout << "Vsh[" << i << "]=" << Vsh[i] << endl;

        SH[i] = Vsh[i] * 100;
        //cout << "SH[" << i << "]=" << SH[i] << "%" << endl;
    }

    //孔隙度的计算
    int SHCT = 40; //泥质截止值，默认40
    /* cout << "请输入SHCT的值：";
    cin >> SHCT >> endl; */
    float Cp = 1.2;            //压实校正系数1.0~1.6
                               /* cout << "请输入压实校正系数：";
    cin >> Cp << endl; */
    int ACma = 182, ACf = 620; //骨架声波时差，默认182；流体声波时差，默认620；
    for (int i = 0; i <= 280; i++)
    {
        if (SH[i] > SHCT)
        {
            fai[i] = 0.005;
        }
        else
        {
            fai[i] = (AC[i] - ACma) / ((ACf - ACma) * Cp);
        }
        //cout << "fai[" << i << "]=" << fai[i] << endl;

        POR[i] = fai[i] * 100;
        //cout << "POR[" << i << "]=" << POR[i] << "%" << endl;
    }

    //渗透率的计算
    int Sirr = 25; //束缚水饱和度，默认为25
    for (int i = 0; i <= 280; i++)
    {
        PERM[i] = (0.136 * pow(POR[i], 4.4)) / pow(Sirr, 2);
        // cout << "PERM[" << i << "]=" << PERM[i] << endl;
    }

    //保存数理成果数据
    ofstream outfile("1801040316_result.txt", ios_base::out | ios_base::binary);
    outfile << setiosflags(ios_base::left) << setw(15) << "DEPTH";
    outfile << setw(15) << "SP";
    outfile << setw(18) << "GR";
    outfile << setw(20) << "AC";
    outfile << setw(20) << "DEN";
    outfile << setw(15) << "SH";
    outfile << setw(20) << "POR";
    outfile << setw(15) << "PERM" << endl;
    for (int i = 0; i <= 280; i++)
    {
        outfile << setiosflags(ios_base::left) << setw(15) << DEPTH[i];
        outfile << setw(15) << SP[i];
        outfile << setw(15) << GR[i];
        outfile << setw(15) << AC[i];
        outfile << setw(15) << DEN[i];
        outfile << setw(15) << SH[i];
        outfile << setw(15) << POR[i];
        outfile << setw(15) << PERM[i] << endl;
    }
    cout << "文件输出结束" << endl;
    outfile.close();

    //分层
    int store[281];                                             //临时存储区分孔隙度数据
    int Stadep[5], Enddep[5];                                   //起始，终止深度
    double Thickness[5];                                        //层厚
    double sp[5], ac[5], gr[5], den[5], sh[5], por[5], perm[5]; //每层平均数值
    double sum = 0;
    //找出分层
    for (int i = 0; i <= 280; i++)
    {
        if (POR[i] != 0.5)
        {
            store[i] = i + 1;
        }
        else
        {
            {
                store[i] = 0;
            }
        }
    }

    /* for (int i = 0; i <= 280; i++)
    {
        cout << store[i] << endl;
    } */
    //找出地层起始数据
    for (int i = 1, j = 0; i <= 280, j <= 4; i++)
    {
        if (store[i] - store[i - 1] > 1)
        {
            Stadep[j++] = i;
        }
    }
    //找出地层结束数据
    for (int i = 1, j = 0; i <= 280, j <= 3; i++)
    {
        if (store[i] - store[i - 1] < 0)
        {
            Enddep[j++] = i - 1;
        }
    }
    Enddep[4] = 280;
    //输出测试
    /* for (int i = 0; i <= 4; i++)
    {
        cout << "起始深度：" << Stadep[i] << endl;
        cout << "终止深度：" << Enddep[i] << endl;
    } */

    for (int i = 0; i <= 4; i++)
    {
        Thickness[i] = DEPTH[Enddep[i]] - DEPTH[Stadep[i]];
        //cout << "第" << i + 1 << "层层厚：" << Thickness[i] << endl;
    }

    //计算平均AC
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + AC[j];
        }
        ac[i] = sum / (Enddep[i] - Stadep[i] + 1);
        /* cout << "sum=" << sum << endl;
        cout << "Stadep=" << Stadep[i] << endl;
        cout << "Enddep=" << Enddep[i] << endl;
        cout << AC[Enddep[i]] << endl;
        cout << "第" << i + 1 << "层AC:" << ac[i] << endl; */
        sum = 0;
    }
    //计算平均SP
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + SP[j];
        }
        sp[i] = sum / (Enddep[i] - Stadep[i] + 1);
        //cout << "第" << i + 1 << "层SP:" << sp[i] << endl;
        sum = 0;
    }
    //计算平均GR
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + GR[j];
        }
        gr[i] = sum / (Enddep[i] - Stadep[i] + 1);
        //cout << "第" << i + 1 << "层GR:" << gr[i] << endl;
        sum = 0;
    }
    //计算平均DEN
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + DEN[j];
        }
        den[i] = sum / (Enddep[i] - Stadep[i] + 1);
        // cout << "第" << i + 1 << "层DEN:" << den[i] << endl;
        sum = 0;
    }
    //计算平均SH
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + SH[j];
        }
        sh[i] = sum / (Enddep[i] - Stadep[i] + 1);
        //cout << "第" << i + 1 << "层SH:" << sh[i] << endl;
        sum = 0;
    }
    //计算平均POR
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + POR[j];
        }
        por[i] = sum / (Enddep[i] - Stadep[i] + 1);
        // cout << "第" << i + 1 << "层POR:" << por[i] << endl;
        sum = 0;
    }
    //计算平均PERM
    for (int i = 0; i <= 4; i++)
    {
        for (int j = Stadep[i]; j <= Enddep[i]; j++)
        {
            sum = sum + PERM[j];
        }
        perm[i] = sum / (Enddep[i] - Stadep[i] + 1);
        //cout << "第" << i + 1 << "层PERM:" << perm[i] << endl;
        sum = 0;
    }

    //形成数据成果表文件
    string s(150, '-');
    ofstream outfile2("1801040316_table.txt", ios_base::out | ios_base::binary);
    outfile2 << setiosflags(ios_base::left) << setw(15) << "层号";
    outfile2 << setw(20) << "起始深度";
    outfile2 << setw(20) << "结束深度";
    outfile2 << setw(18) << "层厚";
    outfile2 << setw(20) << "SP";
    outfile2 << setw(20) << "GR";
    outfile2 << setw(20) << "AC";
    outfile2 << setw(20) << "DEN";
    outfile2 << setw(15) << "SH";
    outfile2 << setw(20) << "POR";
    outfile2 << setw(15) << "PERM" << endl;
    outfile2 << s << endl;

    for (int i = 0; i <= 4; i++)
    {
        outfile2 << setw(15) << i + 1;
        outfile2 << setprecision(6) << setw(16) << DEPTH[Stadep[i]];
        outfile2 << setprecision(6) << setw(16) << DEPTH[Enddep[i]];
        outfile2 << setprecision(3) << setw(16) << Thickness[i];
        outfile2 << setprecision(4) << setw(18) << sp[i];
        outfile2 << setprecision(5) << setw(18) << gr[i];
        outfile2 << setw(20) << ac[i];
        outfile2 << setprecision(3) << setw(16) << den[i];
        outfile2 << setprecision(4) << setw(16) << sh[i];
        outfile2 << setw(20) << por[i];
        outfile2 << setprecision(3) << setw(16) << perm[i] << endl;
        outfile2 << s << endl;
    }
    cout << "成果表文件输出结束" << endl;
    outfile2.close();
    return 0;
}