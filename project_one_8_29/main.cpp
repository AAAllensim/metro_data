//
//  main.cpp
//  std_test
//
//  Created by Allensim on 2018/6/13.
//  Copyright © 2018年 Allensim. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <string>
#include <list>
#include <vector>
#include <fstream>
#include <streambuf>
#include <string.h>
#include "json/json.h"
#include <time.h>
#include <stdio.h>


//省去每次声明都要加头部
using namespace std;


typedef struct {
    string JA;
    string EN;
}nameLanguage;

typedef struct {
    string year;
    string hour;
    string line_in;
    string station_in;
    string line_out;
    string station_out;
}customerData;


typedef struct {
    string departureTime;
    string destinationStation;
    string trainType;
    string isLast;
    string notes;
}stationMessage_json;

typedef struct{
    string type;
    string context;
    string sameAs;
    string station;
    string railway;
    string operator_s;
    string railDirection;
    string date;
    vector<vector<stationMessage_json>>  stationMs_jsons;
    string id;
} timeTable_json;

typedef struct sta_mess {
    //固定不变
    string  station_name;
    int time;
    string line_type;
    string connect_pre;
    string connect_next;
    
    //发生变化
    string dady_station;
    string dady_lineType;
    int isInOpenList;
    int isInCloseList;
    
    struct sta_mess *dady_link;
    
    //时间表的判断指标(方法2)
    string startTime;
    string arriveTime;

    //运行间隔的判断指标（方法1）,用间隔来查找最短路径,H总花费时间w当前电车平均等待时间
    int H;
    int w;
    
    
    
}stationMessage;




typedef struct{
    string start_station;
    string end_station;
    int time;
    string line_type;
    
} common_station;

bool sortRule(stationMessage s1, stationMessage s2)
{
    if (s1.H>  s2.H ) {
        return false;
    }
    else
        return true;
};
bool sortRuleTime(stationMessage s1, stationMessage s2)
{
    if (s1.arriveTime.compare(s2.arriveTime) > 0 ) {
        return false;
    }
    else
        return true;
};
bool sortRuleTimeAnti(stationMessage s1, stationMessage s2)
{
    if (s1.startTime.compare(s2.startTime) < 0 ) {
        return false;
    }
    else
        return true;
};




string Read_Str(string filepath)
{
    ifstream infile;
    infile.open(filepath);
    //打开失败，路径不正确
    if(!infile)
        cout << "Open File Fail!" << endl;
    //读取文本内容到字符串
    string readStr;
    readStr.assign((istreambuf_iterator<char>(infile)),istreambuf_iterator<char>());
    //    infile.close();
    
    
    return readStr;
}

vector<string> split(string str,string pattern);

const int station_number=1000;
int max_pass_count=0;


list<stationMessage> openList;
list<stationMessage> closeList;
vector<stationMessage> real_station;
vector<common_station> common_route;
double map_list[station_number][station_number];
int count_amount;
vector<timeTable_json> jsonList;
vector<vector<string>> languageChangeTable;
vector<nameLanguage> NameList;
vector<vector<string>> directList;
vector<stationMessage>finalyRoute;
vector<customerData> finalData;
vector<string>AllLineType;
string dateType;


stationMessage* ArrayWithPath(char* address);
int countArray(char * address);
void routeSearchWithStatTime(string start ,string start_lineType, string end ,string end_lineType, string startTime);
void routeSearchWithTime(string start ,string end ,string time);
void routeSearchWithTimeInDynamic(string start ,string end ,string time);
void antiRouteSearch(string start ,string start_lineType, string end ,string end_lineType, string time);
void antiRouteSearchByWayTwo(string start ,string start_lineType, string end ,string end_lineType, string time);
void dealWithStation(stationMessage station,int count);
void dealWithStationByDynamic(stationMessage station,int count);
void dealWithStationByDynamicAnti(stationMessage currentStation,int count);
void dealWithStationByDynamicAntiByWayTwo(stationMessage currentStation,int count);
void insertToOpenList(stationMessage station_forCheck, stationMessage currentstation);
void insertToOpenListByDanamic(stationMessage station_forCheck, stationMessage currentstation);
void insertToOpenListByDanamicAnti(stationMessage station_forCheck, stationMessage currentstation);
void insertToOpenListByDanamicAntiByWayTwo(stationMessage station_forCheck, stationMessage currentstation);
void addDataSource(char * address);
void clearData();
void readTimeTable();
void setStationMessage();
string retureTime( stationMessage start , stationMessage end , string time);
string retureTimeAnti( stationMessage start , stationMessage end , string time);
string retureTimeAntiByWayOne(stationMessage start , stationMessage end , string time);
string timePlus(string time , int span);
string languageChange(string sentence);
void  languageTableSet();
void lineDirSet();
string direction(stationMessage start, stationMessage end);
string startTimeOne(string startTime ,int timeSpan);
void dealWithFinalyRouteOne();
void dealWithFinalyRouteTwo();
void dealWithFinalyRouteThree();
int timeMInas(string startTime, string endTime);
string timeTakenFrom (string endtime,int span);
string timeThirty(string time);
int lineWait(string lineType);
void dealWithCustomerData();
void fillAllLineType();
string judgeDate(int year,int month,int day);

int main(int argc, const char * argv[]) {
    
    

    //车站信息填充
    fillAllLineType();

    //客户信息处理
    dealWithCustomerData();
    
    vector<customerData>testData;
    testData.clear();
    
    int maxNumber=31;
    
    for (int i=0; i<finalData.size(); i++) {
        bool in=0;
        bool out=0;
        
        //针对进入车站的查找
        for (int j=0; j<maxNumber; j++) {
            if (AllLineType[j].find(finalData[i].line_in) != AllLineType[j].npos) {
                in=1;
            }
        }
        //针对出去车站的查找
        for (int j=0; j<maxNumber; j++) {
            if (AllLineType[j].find(finalData[i].line_out)  != AllLineType[j].npos) {
                out=1;
            }
        }
        
        //进行符合查找要求车站的存储
        if (in==1 && out ==1) {
            testData.push_back(finalData[i]);
        }
    }
    
    lineDirSet();
    languageTableSet();
    setStationMessage();
    readTimeTable();
    
    //    for (int i=0; i<count_amount; i++) {
    //        for (int j=0; j<count_amount; j++) {
    //            if (real_station[i].station_name.compare(real_station[j].station_name) != 0) {
    //                routeSearch(real_station[i].station_name, real_station[j].station_name);
    //                //记得清空数据,否则下次查询的路径存在错误
    //                clearData();
    //            }
    //        }
    //    }
    //    printf("the most long pass route:  %d\n",max_pass_count);
    //
    
    
    //接下来读取数组数据，（起始点，开始时间）  进行线路分析
    for (int i=0; i<testData.size()-testData.size()+5; i++) {
        string InSta;
        string InSta_lineType;
        string OutSta;
        string OutSta_lineType;
        string OutTime;
        //删除\后处理数据
        InSta.assign(testData[i].station_in.substr(1,testData[i].station_in.length()-2));
        InSta_lineType.assign("line");
        InSta_lineType.append(testData[i].line_in.substr(1,testData[i].line_in.length()-2));
        OutSta.assign(testData[i].station_out.substr(1,testData[i].station_out.length()-2));
        OutSta_lineType.assign("line");
        OutSta_lineType.append(testData[i].line_out.substr(1,testData[i].line_out.length()-2));

        OutTime.assign(testData[i].hour.substr(1,2));
        OutTime.append(":");
        OutTime.append(testData[i].hour.substr(3,2));


        if (InSta.compare("北綾瀬") == 0 && InSta_lineType.compare("line千代田線") ==0 ) {
            InSta_lineType.append("B");
        }
        if (OutSta.compare("北綾瀬") == 0  && OutSta_lineType.compare("line千代田線") == 0) {
            OutSta_lineType.append("B");
        }
        if (InSta_lineType.compare("line丸ノ内線") == 0) {
            if (InSta.compare("中野新橋") ==0  ||InSta.compare("中野富士見町") ==0 ||InSta.compare("方南町") ==0) {
                InSta_lineType.append("B");
            }
        }
        if (OutSta_lineType.compare("line丸ノ内線") == 0) {
            if (OutSta.compare("中野新橋") ==0  ||OutSta.compare("中野富士見町") ==0 ||OutSta.compare("方南町") ==0) {
                OutSta_lineType.append("B");
            }
        }
        if (InSta.compare("押上") == 0 && InSta_lineType.compare("line伊勢崎線") ==0 ) {
            InSta_lineType.append("B");
        }
        if (OutSta.compare("押上") == 0  && OutSta_lineType.compare("line伊勢崎線") == 0) {
            OutSta_lineType.append("B");
        }

        if (InSta_lineType.compare("line大江戸線") == 0) {
            if (InSta.compare("新宿西口") ==0  ||InSta.compare("東新宿") ==0 ||InSta.compare("若松河田") ==0 ||InSta.compare("牛込柳町") ==0 ||InSta.compare("牛込神楽坂") ==0 ||InSta.compare("飯田橋") ==0 ||InSta.compare("春日") ==0 ||InSta.compare("本郷三丁目") ==0 ||InSta.compare("上野御徒町") ==0 ||InSta.compare("新御徒町") ==0 ||InSta.compare("蔵前") ==0 ||InSta.compare("両国") ==0 ||InSta.compare("森下") ==0 ||InSta.compare("清澄白河") ==0 ||InSta.compare("門前仲町") ==0||InSta.compare("月島") ==0) {
                InSta_lineType.append("B");
            }
        }
        if (OutSta_lineType.compare("line大江戸線") == 0) {
            if (OutSta.compare("新宿西口") ==0  ||OutSta.compare("東新宿") ==0 ||OutSta.compare("若松河田") ==0 ||OutSta.compare("牛込柳町") ==0 ||OutSta.compare("牛込神楽坂") ==0 ||OutSta.compare("飯田橋") ==0 ||OutSta.compare("春日") ==0 ||OutSta.compare("本郷三丁目") ==0 ||OutSta.compare("上野御徒町") ==0 ||OutSta.compare("新御徒町") ==0 ||OutSta.compare("蔵前") ==0 ||OutSta.compare("両国") ==0 ||OutSta.compare("森下") ==0 ||OutSta.compare("清澄白河") ==0 ||OutSta.compare("門前仲町") ==0||OutSta.compare("月島") ==0) {
                OutSta_lineType.append("B");
            }
        }
        if (InSta_lineType.compare("line常磐線") == 0) {
            if (InSta.compare("綾瀬") ==0  ||InSta.compare("亀有") ==0 ||InSta.compare("金町") ==0 ||InSta.compare("北松戸") ==0 ||InSta.compare("馬橋") ==0 ||InSta.compare("新松戸") ==0 ||InSta.compare("北小金") ==0 ||InSta.compare("南柏") ==0 ||InSta.compare("北柏") ==0) {
                InSta_lineType.append("各停");
            }
        }
        if (OutSta_lineType.compare("line常磐線") == 0) {
            if (OutSta.compare("綾瀬") ==0  ||OutSta.compare("亀有") ==0 ||OutSta.compare("金町") ==0 ||OutSta.compare("北松戸") ==0 ||OutSta.compare("馬橋") ==0 ||OutSta.compare("新松戸") ==0 ||OutSta.compare("北小金") ==0 ||OutSta.compare("南柏") ==0 ||OutSta.compare("北柏") ==0 ) {
                OutSta_lineType.append("各停");
            }
        }


        for (int j=0; j<directList[13].size(); j++) {
            if (InSta.compare(directList[13][j]) == 0) {
                InSta_lineType.assign("line西武新宿線");
            }
        }
        for (int j=0; j<directList[13].size(); j++) {
            if (OutSta.compare(directList[13][j]) == 0) {
                OutSta_lineType.assign("line西武新宿線");
            }
        }




        int year= stoi(testData[i].year.substr(1,4));
        int month=stoi(testData[i].year.substr(5,2));
        int day=stoi(testData[i].year.substr(7,2));
        dateType.assign("odpt:");
        dateType.append(judgeDate(year, month, day));

//        routeSearchWithStatTime(InSta,InSta_lineType ,OutSta, OutSta_lineType,OutTime);
//        antiRouteSearch(InSta,InSta_lineType ,OutSta, OutSta_lineType,OutTime);
        antiRouteSearchByWayTwo(InSta,InSta_lineType ,OutSta, OutSta_lineType,OutTime);

        clearData();
    }



    
    //方案1
    //    routeSearchWithStatTime("渋谷","line銀座線" ,"青山一丁目","line銀座線","10:41");
    
    //方案2
//    routeSearchWithTimeInDynamic("入谷", "稲荷町", "10:58");
    
    
    //"20170202","000049","銀座線","神田","東西線","西葛西"
    //    銀座(line銀座線) to 元町・中華街(lineみなとみらい線) has no route



    //方案3
//    antiRouteSearch("谷塚","line伊勢崎線", "日比谷","line千代田線","16:34");
    
    
    //方案4
    //要町 (line有楽町線) to 辰巳 (line有楽町線) has a route
    //本郷三丁目 (line大江戸線B) to 月島 (line大江戸線B) has a route


//            dateType.assign("odpt:");
//            dateType.append(judgeDate(2017, 2, 1));
//
//    antiRouteSearchByWayTwo("本郷三丁目","line大江戸線B", "月島","line大江戸線B","17:55");

    
    return 0;
    
}
/**************************反向推到路径      利用方案1*******************************************/
void antiRouteSearchByWayTwo(string start ,string start_lineType, string end ,string end_lineType, string time)
{
    //首先查找周边能去的车站
    int findRoute=0;
    stationMessage currentStation;
    
    //查找起点坐标
    int tempIndex;
    for (int i=0; i < count_amount; i++) {
        if (start.compare(real_station[i].station_name)==0 && start_lineType.compare(real_station[i].line_type)==0 )
        {
            tempIndex=i;
            break;
        }
    }
    
    
    //加入起点
    real_station[tempIndex].isInOpenList=1;
    real_station[tempIndex].startTime.assign(time);
    real_station[tempIndex].arriveTime.assign(time);
    openList.push_back(real_station[tempIndex]);
    
    tempIndex=0;
    
    
    while (1) {
        
        //整理listopen
        openList.sort(sortRuleTimeAnti);
        
        currentStation=openList.front();
        currentStation.isInCloseList=1;
        currentStation.isInOpenList=0;
        //放入closeList
        closeList.push_back(currentStation);
        
        for (int i=0; i<count_amount; i++) {
            //line 要相同别忘记
            if (currentStation.station_name.compare(real_station[i].station_name) == 0 &&
                currentStation.line_type.compare(real_station[i].line_type) == 0)
            {
                tempIndex=i;
                break;
            }
        }
        real_station[tempIndex].isInCloseList=1;
        real_station[tempIndex].isInOpenList=0;
        
        openList.pop_front();
        
        if (end.compare(currentStation.station_name)==0  && end_lineType.compare(currentStation.line_type)==0 ) {
            findRoute=1;
            break;
        }
        
        //查找当前车站边上能走的车站
        dealWithStationByDynamicAntiByWayTwo(currentStation,count_amount);
        
        if (openList.empty()) {
            findRoute=0;
            break;
        }
        
    }
    
    
    if (findRoute==0) {
        printf("%s (%s) to %s (%s) has no route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
        printf("\n");
        
    }
    else
    {
        //找到close列表中的终点，从而来查找路径
        stationMessage  temp;
        list<stationMessage>::iterator it;
        for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
            temp = *it;
            if (temp.station_name.compare(end)==0 ) {
                break;
            }
        }
        finalyRoute.push_back(temp);
        //记录经过多少个车站和最大经过车站数
        //finalroute用来存储找寻的最终路径
        int pass_count=0;
        //再close列表中回退父节点的信息
        while (temp.dady_station.empty()==false) {
            for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
                stationMessage ttt=*it;
                if (temp.dady_station.compare(ttt.station_name)==0 && temp.dady_lineType.compare(ttt.line_type) ==0) {
                    temp=ttt;
                    finalyRoute.push_back(ttt);
                    break;
                }
            }
            pass_count++;
            if (max_pass_count < pass_count) {
                max_pass_count=pass_count;
            }
        }
        
        
        
        printf("%s (%s) to %s (%s) has a route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        printf("the number of pass station :%d\n",max_pass_count);
        
        //对最后生成的路径进行时间的精确认定
        dealWithFinalyRouteThree();
        
        printf("\n");
        printf("\n");
        
        
    }
}


void dealWithStationByDynamicAntiByWayTwo(stationMessage currentStation,int count)
{
    int index=0;
    //找到当前车站的下表，从地图信息来寻找能去的车站
    for (int i=0; i<count ; i++) {
        if (real_station[i].station_name.compare(currentStation.station_name) == 0  &&
            real_station[i].line_type.compare(currentStation.line_type) == 0 ){
            index=i;
            break;
        }
    }
    
    //记录能取的车站坐标
    int nerbor_count=0;
    int nerbor_index[station_number];
    for (int i=0; i<count; i++) {
        if (map_list[index][i]!=9999) {
            nerbor_index[nerbor_count]=i;
            nerbor_count++;
        }
    }
    
    //判断车站是否能加入Open list
    for (int i=0; i<nerbor_count; i++) {
        insertToOpenListByDanamicAntiByWayTwo(real_station[nerbor_index[i]],currentStation);
    }
    
}

void insertToOpenListByDanamicAntiByWayTwo(stationMessage station_forCheck, stationMessage currentstation)
{
    //两车站之间的运行时间
    int driveTime=abs(currentstation.time-station_forCheck.time);
    
    //相比于数组下标对其，还是取某一范围内的前后的近似时间进行模拟，这里假定为5作为全局变量，即两车站之间的行进时间
    if (station_forCheck.isInCloseList!=1) {
        
        //存在于openlist
        if (station_forCheck.isInOpenList) {
            
            //同一条路线
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //               已经存在的路线                               沿着新路线的走法所花的时间
                if (station_forCheck.startTime.compare((retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime)))) < 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.startTime.assign((retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime))));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                }
            }
            //换乘站
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                if (station_forCheck.startTime.compare(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1))) < 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.startTime.assign(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1)));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                }
            }
        }
        
        
        //加入到openlist
        else{
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //同一条线
                string tTiem;
                tTiem.assign(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime)));
                //对没有运行时间的车站不进行入栈
                if (tTiem.compare("30:00") !=0) {
                    //到达时间
                    station_forCheck.startTime.assign(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime)));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    station_forCheck.isInOpenList=1;
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                    
                    openList.push_back(station_forCheck);
                }
                
            }
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                //换乘站
                string tTiem;
                tTiem.assign(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1)));
                if (tTiem.compare("30:00") !=0) {
                    //到达时间
                    station_forCheck.startTime.assign(retureTimeAnti(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1)));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    station_forCheck.isInOpenList=1;
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                    
                    openList.push_back(station_forCheck);
                }
            }
        }
    }
}


/**************************反向推到路径      利用方案2*******************************************/
void antiRouteSearch (string start ,string start_lineType, string end ,string end_lineType, string time)
{
    //首先查找周边能去的车站
    int findRoute=0;
    stationMessage currentStation;
    
    //查找起点坐标
    int tempIndex;
    for (int i=0; i < count_amount; i++) {
        if (start.compare(real_station[i].station_name)==0 && start_lineType.compare(real_station[i].line_type)==0 )
        {
            tempIndex=i;
            break;
        }
    }
    
    
    //加入起点
    real_station[tempIndex].isInOpenList=1;
    real_station[tempIndex].startTime.assign(time);
    real_station[tempIndex].arriveTime.assign(time);
    openList.push_back(real_station[tempIndex]);
    
    tempIndex=0;
    
    
    while (1) {
        
        //整理listopen
        openList.sort(sortRuleTimeAnti);
        
        currentStation=openList.front();
        currentStation.isInCloseList=1;
        currentStation.isInOpenList=0;
        //放入closeList
        closeList.push_back(currentStation);
        
        for (int i=0; i<count_amount; i++) {
            //line 要相同别忘记
            if (currentStation.station_name.compare(real_station[i].station_name) == 0 &&
                currentStation.line_type.compare(real_station[i].line_type) == 0)
            {
                tempIndex=i;
                break;
            }
        }
        real_station[tempIndex].isInCloseList=1;
        real_station[tempIndex].isInOpenList=0;
        
        openList.pop_front();
        
        if (end.compare(currentStation.station_name)==0  && end_lineType.compare(currentStation.line_type)==0 ) {
            findRoute=1;
            break;
        }
        
        //查找当前车站边上能走的车站
        dealWithStationByDynamicAnti(currentStation,count_amount);
        
        if (openList.empty()) {
            findRoute=0;
            break;
        }
        
    }
    
    
    if (findRoute==0) {
        printf("%s(%s) to %s(%s) has no route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
        printf("\n");
        
    }
    else
    {
        printf("%s(%s)  to %s (%s) has a route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        //找到close列表中的终点，从而来查找路径
        stationMessage  temp;
        list<stationMessage>::iterator it;
        for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
            temp = *it;
            if (temp.station_name.compare(end)==0 ) {
                break;
            }
        }
        
        finalyRoute.push_back(temp);
        //记录经过多少个车站和最大经过车站数
        //finalroute用来存储找寻的最终路径
        int pass_count=0;
        //再close列表中回退父节点的信息
        while (temp.dady_station.empty()==false) {
            for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
                stationMessage ttt=*it;
                if (temp.dady_station.compare(ttt.station_name)==0 && temp.dady_lineType.compare(ttt.line_type) ==0) {
                    temp=ttt;
                    finalyRoute.push_back(ttt);
                    break;
                }
            }
            pass_count++;
            if (max_pass_count < pass_count) {
                max_pass_count=pass_count;
            }
        }
        printf("the number of pass station :%d\n",pass_count);
        
        //对最后生成的路径进行时间的精确认定
        dealWithFinalyRouteThree();
        
        
        
        printf("\n");
        printf("\n");
        
        
    }
}
void dealWithStationByDynamicAnti(stationMessage currentStation,int count)
{
    int index=0;
    //找到当前车站的下表，从地图信息来寻找能去的车站
    for (int i=0; i<count ; i++) {
        if (real_station[i].station_name.compare(currentStation.station_name) == 0  &&
            real_station[i].line_type.compare(currentStation.line_type) == 0 ){
            index=i;
            break;
        }
    }
    
    //记录能取的车站坐标
    int nerbor_count=0;
    int nerbor_index[station_number];
    for (int i=0; i<count; i++) {
        if (map_list[index][i]!=9999) {
            nerbor_index[nerbor_count]=i;
            nerbor_count++;
        }
    }
    
    //判断车站是否能加入Open list
    for (int i=0; i<nerbor_count; i++) {
        insertToOpenListByDanamicAnti(real_station[nerbor_index[i]],currentStation);
    }
    
}

void insertToOpenListByDanamicAnti(stationMessage station_forCheck, stationMessage currentstation)
{
    //两车站之间的运行时间
    int driveTime=abs(currentstation.time-station_forCheck.time);
    
    //相比于数组下标对其，还是取某一范围内的前后的近似时间进行模拟，这里假定为5作为全局变量，即两车站之间的行进时间
    if (station_forCheck.isInCloseList!=1) {
        
        //存在于openlist
        if (station_forCheck.isInOpenList) {
            
            //同一条路线
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //               已经存在的路线                               沿着新路线的走法所花的时间
                if (station_forCheck.startTime.compare((retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime)))) < 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.startTime.assign((retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime))));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                }
            }
            //换乘站
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                if (station_forCheck.startTime.compare(retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1))) < 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.startTime.assign(retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1)));
                    station_forCheck.arriveTime.assign(station_forCheck.startTime);
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                    
                    
                }
            }
        }
        
        
        //加入到openlist
        else{
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //同一条线
                //到达时间
                station_forCheck.startTime.assign(retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, driveTime)));
                station_forCheck.arriveTime.assign(station_forCheck.startTime);
                station_forCheck.isInOpenList=1;
                
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);
                
                openList.push_back(station_forCheck);
            }
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                //换乘站
                station_forCheck.startTime.assign(retureTimeAntiByWayOne(station_forCheck, currentstation,  timeTakenFrom(currentstation.arriveTime, 1)));
                station_forCheck.arriveTime.assign(station_forCheck.startTime);
                station_forCheck.isInOpenList=1;
                
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);
                
                openList.push_back(station_forCheck);
            }
        }
    }
}


void dealWithFinalyRouteThree()
{
    
    int span=1;
    int maxsize=finalyRoute.size();
    for (int i=0; i<maxsize; i++) {
        string arriveTime;
        if (finalyRoute[i].startTime.compare("29:00") >=0) {
            arriveTime.assign("该时间点已经没有电车");
        }else{
            arriveTime.assign(finalyRoute[i].startTime);
        }

        printf("%s(%s)%s====",finalyRoute[i].station_name.c_str(),finalyRoute[i].line_type.c_str(),arriveTime.c_str());
    }
}


string retureTimeAntiByWayOne(stationMessage start , stationMessage end , string time)
{
    
    return time;

}


//查找最近的出发时间
string retureTimeAnti( stationMessage start , stationMessage end , string time)
{
    string retureString;
    string startStation;
    string endStation;
    string Linedirection;
    string typeOfDay;
    int minTimeSpan=9999;
    int flag=0; //用来记录当前时刻表是否存在能出发的时间
    
    //中转站的情况
    if (start.line_type.compare(end.line_type) != 0  && start.station_name.compare(end.station_name) == 0 ) {
        return time;
    }
    
    
    
    // line的判断
    if (start.line_type.compare("line銀座線")==0) {
        startStation.assign("odpt.Station:TokyoMetro.Ginza.");
        endStation.assign("odpt.Station:TokyoMetro.Ginza.");
    }else if (start.line_type.compare("line日比谷線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Hibiya.");
        endStation.assign("odpt.Station:TokyoMetro.Hibiya.");
    }else if (start.line_type.compare("line東西線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Tozai.");
        endStation.assign("odpt.Station:TokyoMetro.Tozai.");
    }else if (start.line_type.compare("line千代田線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
        endStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
    }else if (start.line_type.compare("line千代田線B")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
        endStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
    }else if (start.line_type.compare("line半蔵門線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Hanzomon.");
        endStation.assign("odpt.Station:TokyoMetro.Hanzomon.");
    }else if (start.line_type.compare("line南北線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Namboku.");
        endStation.assign("odpt.Station:TokyoMetro.Namboku.");
    }else if (start.line_type.compare("line有楽町線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Yurakucho.");
        endStation.assign("odpt.Station:TokyoMetro.Yurakucho.");
    }else if (start.line_type.compare("line丸ノ内線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        endStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        
    }else if (start.line_type.compare("line丸ノ内線B")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.MarunouchiBranch.");
        endStation.assign("odpt.Station:TokyoMetro.MarunouchiBranch.");
        //中野坂上是B线的中转站，标识是TokyoMetro.Marunouchi.NakanoSakaue
        if (start.station_name.compare("中野坂上") == 0 ) {
            startStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        }
        if (end.station_name.compare("中野坂上") == 0 ) {
            endStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        }
        
    }else if (start.line_type.compare("line副都心線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Fukutoshin.");
        endStation.assign("odpt.Station:TokyoMetro.Fukutoshin.");
        
    }else
    {
        return time;
    }
        
    startStation.append(languageChange(start.station_name));
    endStation.append(languageChange(end.station_name));
    Linedirection.assign("odpt.RailDirection:TokyoMetro.");
    Linedirection.append(direction(start,end));
    
    
    typeOfDay.assign(dateType);

    
    int count=jsonList.size();
    for (int i=0; i<count; i++) {
        if (startStation.compare(jsonList[i].station) == 0 && Linedirection.compare(jsonList[i].railDirection) == 0) {
            //找到该车站
            vector<stationMessage_json> temp;
            if (typeOfDay.compare(dateType) == 0) {
                //找到当天该站的时间表
                temp=jsonList[i].stationMs_jsons[0];
                for (int j=temp.size()-1; j>=0; j--) {
                    //确认出发时间
                    stationMessage_json tempjson;
                    tempjson=temp[j];
                    //进行30小时制的转化，方便进行比较
                    if (timeThirty(tempjson.departureTime).compare(timeThirty(time)) <= 0) {
                        if (abs(timeMInas(timeThirty(tempjson.departureTime),timeThirty(time))) < minTimeSpan) {
                            retureString.assign(tempjson.departureTime);
                            minTimeSpan=abs(timeMInas(timeThirty(tempjson.departureTime),timeThirty(time)));
                            flag=1;
                        }
                    }
                }
            }
        }
    }
    
    if (flag == 0) {
        retureString.assign("30:00");
    }
    
    return retureString;
}


/**************************时间表的路线查找模拟训练      方案2*******************************************/
void routeSearchWithTimeInDynamic(string start ,string end ,string time)
{
    //首先查找周边能去的车站
    int findRoute=0;
    stationMessage currentStation;
    
    //查找起点坐标
    int tempIndex;
    for (int i=0; i < count_amount; i++) {
        if (start.compare(real_station[i].station_name)==0)
        {
            tempIndex=i;
            break;
        }
    }
    
    
    //加入起点
    real_station[tempIndex].isInOpenList=1;
    real_station[tempIndex].startTime.assign(time);
    real_station[tempIndex].arriveTime.assign(time);
    openList.push_back(real_station[tempIndex]);
    
    tempIndex=0;
    
    
    while (1) {
        
        //整理listopen
        openList.sort(sortRuleTime);
        
        currentStation=openList.front();
        currentStation.isInCloseList=1;
        currentStation.isInOpenList=0;
        //放入closeList
        closeList.push_back(currentStation);
        
        for (int i=0; i<count_amount; i++) {
            //line 要相同别忘记
            if (currentStation.station_name.compare(real_station[i].station_name) == 0 &&
                currentStation.line_type.compare(real_station[i].line_type) == 0)
            {
                tempIndex=i;
                break;
            }
        }
        real_station[tempIndex].isInCloseList=1;
        real_station[tempIndex].isInOpenList=0;
        
        openList.pop_front();
        
        if (end.compare(currentStation.station_name)==0) {
            findRoute=1;
            break;
        }
        
        //查找当前车站边上能走的车站
        dealWithStationByDynamic(currentStation,count_amount);
        
        if (openList.empty()) {
            findRoute=0;
            break;
        }
        
    }
    
    
    if (findRoute==0) {
        printf("%s to %s has no route\n",start.c_str(),end.c_str());
        printf("\n");
        
    }
    else
    {
        printf("%s to %s has a route\n",start.c_str(),end.c_str());
        //找到close列表中的终点，从而来查找路径
        stationMessage  temp;
        list<stationMessage>::iterator it;
        for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
            temp = *it;
            if (temp.station_name.compare(end)==0 ) {
                break;
            }
        }
        
        finalyRoute.push_back(temp);
        //记录经过多少个车站和最大经过车站数
        //finalroute用来存储找寻的最终路径
        int pass_count=0;
        //再close列表中回退父节点的信息
        while (temp.dady_station.empty()==false) {
            for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
                stationMessage ttt=*it;
                if (temp.dady_station.compare(ttt.station_name)==0 && temp.dady_lineType.compare(ttt.line_type) ==0) {
                    temp=ttt;
                    finalyRoute.push_back(ttt);
                    break;
                }
            }
            pass_count++;
            if (max_pass_count < pass_count) {
                max_pass_count=pass_count;
            }
        }
        printf("the number of pass station :%d\n",pass_count);
        
        //对最后生成的路径进行时间的精确认定
        dealWithFinalyRouteTwo();
        
        
        
        printf("\n");
        printf("\n");

        
    }
}


void dealWithFinalyRouteTwo()
{
    
    //确定出发时间
    int max;
    string sTime;
    int span;
    max=finalyRoute.size()-1;
    
    
    printf("到达时间：\n");
    for (int i=max; i>0; i--) {
        string arriveTime;
        if (finalyRoute[i].arriveTime.compare("30:00") >=0) {
            arriveTime.assign("该时间点已经没有电车");
        }else{
            arriveTime.assign(finalyRoute[i].arriveTime);
        }
        printf("%s(%s)%s====",finalyRoute[i].station_name.c_str(),finalyRoute[i].line_type.c_str(),arriveTime.c_str());
    }
    if (finalyRoute[0].arriveTime.compare("30:00") >= 0) {
        printf("%s(%s)该时间点已经没有电车\n",finalyRoute[0].station_name.c_str(),finalyRoute[0].line_type.c_str());
    }else{
        printf("%s(%s)%s\n",finalyRoute[0].station_name.c_str(),finalyRoute[0].line_type.c_str(),finalyRoute[0].arriveTime.c_str());
    }

    
    printf("出发时间：\n");
    //根据到达时间和行驶路径来反推出发时间
    for (int i=max; i>0; i--) {
        finalyRoute[i].startTime.assign(retureTime(finalyRoute[i], finalyRoute[i-1], finalyRoute[i].arriveTime));
    }
    
    for (int i=max; i>0; i--) {
        string startTime;
        if (finalyRoute[i].startTime.compare("30:00") >=0) {
            startTime.assign("该时间点已经没有电车");
        }else{
            startTime.assign(finalyRoute[i].startTime);
        }
        printf("%s(%s)%s====",finalyRoute[i].station_name.c_str(),finalyRoute[i].line_type.c_str(),startTime.c_str());
    }
    if (finalyRoute[0].startTime.compare("30:00") >= 0) {
        printf("%s(%s)该时间点已经没有电车",finalyRoute[0].station_name.c_str(),finalyRoute[0].line_type.c_str());
    }else{
        printf("%s(%s)%s",finalyRoute[0].station_name.c_str(),finalyRoute[0].line_type.c_str(),finalyRoute[0].startTime.c_str());
    }


    
    
}

void dealWithStationByDynamic(stationMessage currentStation,int count)
{
    int index=0;
    //找到当前车站的下表，从地图信息来寻找能去的车站
    for (int i=0; i<count ; i++) {
        if (real_station[i].station_name.compare(currentStation.station_name) == 0  &&
            real_station[i].line_type.compare(currentStation.line_type) == 0 ){
            index=i;
            break;
        }
    }
    
    //记录能取的车站坐标
    int nerbor_count=0;
    int nerbor_index[station_number];
    for (int i=0; i<count; i++) {
        if (map_list[index][i]!=9999) {
            nerbor_index[nerbor_count]=i;
            nerbor_count++;
        }
    }
    
    //判断车站是否能加入Open list
    for (int i=0; i<nerbor_count; i++) {
        insertToOpenListByDanamic(real_station[nerbor_index[i]],currentStation);
    }
    
}


void insertToOpenListByDanamic(stationMessage station_forCheck, stationMessage currentstation)
{
    //两车站之间的运行时间
    int driveTime=abs(currentstation.time-station_forCheck.time);
    
    //相比于数组下标对其，还是取某一范围内的前后的近似时间进行模拟，这里假定为5作为全局变量，即两车站之间的行进时间
    if (station_forCheck.isInCloseList!=1) {
        
        //存在于openlist
        if (station_forCheck.isInOpenList) {
            
            //同一条路线
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //               已经存在的路线                               沿着新路线的走法所花的时间
                if (station_forCheck.arriveTime.compare(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), driveTime)) > 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.arriveTime.assign(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), driveTime));
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);
                }
            }
            //换乘站
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                if (station_forCheck.arriveTime.compare(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), 1)) > 0 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.arriveTime.assign(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), 1));
                    
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);

                    
                }
            }
        }
        
        
        //加入到openlist
        else{
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //同一条线
                
                //前一站的出发时间
//                retureTime(currentstation, station_forCheck, currentstation.arriveTime);
                
                //到达时间
                station_forCheck.arriveTime.assign(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), driveTime));
                
                station_forCheck.isInOpenList=1;
                
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);

                openList.push_back(station_forCheck);
            }
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                //换乘站
                station_forCheck.arriveTime.assign(timePlus(retureTime(currentstation, station_forCheck, currentstation.arriveTime), 1));

                station_forCheck.isInOpenList=1;
                
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);

                openList.push_back(station_forCheck);
            }
        }
    }
}



string languageChange(string sentence)
{
    string result ;
    //JA TO EN
    //文件参照
    for (int i = 0; i<NameList.size() ; i++) {
        if (NameList[i].JA.compare(sentence) == 0) {
            result.assign(NameList[i].EN);
            break;
        }
    }
    return result;
    
}


//查找最近的出发时间
string retureTime( stationMessage start , stationMessage end , string time)
{
    string retureString;
    string startStation;
    string endStation;
    string Linedirection;
    string typeOfDay;
    int minTimeSpan=9999;
    int flag=0;

    //中转站的情况
    if (start.line_type.compare(end.line_type) != 0  && start.station_name.compare(end.station_name) == 0 ) {
        return time;

    }

    
    
    // 仅针对有时间表的METRO电车进行判断
    if (start.line_type.compare("line銀座線")==0) {
        startStation.assign("odpt.Station:TokyoMetro.Ginza.");
        endStation.assign("odpt.Station:TokyoMetro.Ginza.");
    }else if (start.line_type.compare("line日比谷線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Hibiya.");
        endStation.assign("odpt.Station:TokyoMetro.Hibiya.");
    }else if (start.line_type.compare("line東西線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Tozai.");
        endStation.assign("odpt.Station:TokyoMetro.Tozai.");
    }else if (start.line_type.compare("line千代田線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
        endStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
    }else if (start.line_type.compare("line千代田線B")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
        endStation.assign("odpt.Station:TokyoMetro.Chiyoda.");
    }else if (start.line_type.compare("line半蔵門線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Hanzomon.");
        endStation.assign("odpt.Station:TokyoMetro.Hanzomon.");
    }else if (start.line_type.compare("line南北線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Namboku.");
        endStation.assign("odpt.Station:TokyoMetro.Namboku.");
    }else if (start.line_type.compare("line有楽町線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Yurakucho.");
        endStation.assign("odpt.Station:TokyoMetro.Yurakucho.");
    }else if (start.line_type.compare("line丸ノ内線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        endStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        
    }else if (start.line_type.compare("line丸ノ内線B")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        endStation.assign("odpt.Station:TokyoMetro.Marunouchi.");
        
    }else if (start.line_type.compare("line副都心線")==0)
    {
        startStation.assign("odpt.Station:TokyoMetro.Fukutoshin.");
        endStation.assign("odpt.Station:TokyoMetro.Fukutoshin.");
        
    }else
    {
        return time;
    }
    
    startStation.append(languageChange(start.station_name));
    endStation.append(languageChange(end.station_name));
    Linedirection.assign("odpt.RailDirection:TokyoMetro.");
    Linedirection.append(direction(start,end));
    
    typeOfDay.assign(dateType);
//    typeOfDay.assign("odpt:weekdays");
    
    int count=jsonList.size();
    for (int i=0; i<count; i++) {
        if (startStation.compare(jsonList[i].station) == 0 && Linedirection.compare(jsonList[i].railDirection) == 0) {
            //找到该车站
            vector<stationMessage_json> temp;
            if (typeOfDay.compare(dateType) == 0) {
                //找到当天该站的时间表
                temp=jsonList[i].stationMs_jsons[0];
                for (int j=0; j<temp.size(); j++) {
                    //确认出发时间
                    stationMessage_json tempjson;
                    tempjson=temp[j];
                    //进行30小时制时间的转化，方便进行比较
                    if (timeThirty(tempjson.departureTime).compare(timeThirty(time)) >= 0) {
                        if (abs(timeMInas(timeThirty(tempjson.departureTime),timeThirty(time))) < minTimeSpan) {
                            retureString.assign(tempjson.departureTime);
                            minTimeSpan=abs(timeMInas(timeThirty(tempjson.departureTime),timeThirty(time)));
                            flag=1;
                        }
                    }
                }
            }
        }
    }
    
    
    if (flag == 0) {
        retureString.assign("30:00");
    }
    
    return retureString;
}

//车站的启动时间加运行时间的理论值
string timePlus(string time , int span)
{
    
    string  tempTime;
    string hour,tempHour;
    string minite,tempMinite;
    
    hour.assign(time.substr(0,2));
    minite.assign(time.substr(3,2));
    
    int h=stoi(hour);
    int m=stoi(minite);
    
    if (m+span>=60) {
        m=m+span-60;
        h=h+1;
    }else
    {
        m=m+span;
    }
    
    tempHour.assign(to_string(h)) ;
    tempMinite.assign(to_string(m));
    
    if (m<10) {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":0");
        tempTime.append(tempMinite);
    }else
    {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":");
        tempTime.append(tempMinite);
    }
    
    
    return tempTime;
}


string direction(stationMessage start, stationMessage end)
{
    string resultstring;
    int Fist;
    int Last;
    
    // KitaSenju  NakaMeguro
    
    //仅针对有时间表的metro的电车进行操作
    if (start.line_type.compare("line銀座線")==0 && end.line_type.compare("line銀座線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[0].size(); i++) {
            if (start.station_name.compare(directList[0][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[0][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line日比谷線")==0 && end.line_type.compare("line日比谷線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[1].size(); i++) {
            if (start.station_name.compare(directList[1][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[1][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line東西線")==0 && end.line_type.compare("line東西線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[2].size(); i++) {
            if (start.station_name.compare(directList[2][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[2][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line千代田線")==0 && end.line_type.compare("line千代田線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[3].size(); i++) {
            if (start.station_name.compare(directList[3][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[3][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line千代田線B")==0 && end.line_type.compare("line千代田線B")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[4].size(); i++) {
            if (start.station_name.compare(directList[4][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[4][i]) == 0)
            {
                Last=i;
            }
        }
    }

    else if (start.line_type.compare("line半蔵門線")==0 && end.line_type.compare("line半蔵門線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[5].size(); i++) {
            if (start.station_name.compare(directList[5][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[5][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line南北線")==0 && end.line_type.compare("line南北線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[6].size(); i++) {
            if (start.station_name.compare(directList[6][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[6][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line有楽町線")==0 && end.line_type.compare("line有楽町線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[7].size(); i++) {
            if (start.station_name.compare(directList[7][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[7][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line丸ノ内線")==0 && end.line_type.compare("line丸ノ内線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[8].size(); i++) {
            if (start.station_name.compare(directList[8][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[8][i]) == 0)
            {
                Last=i;
            }
        }
    }    else if (start.line_type.compare("line丸ノ内線B")==0 && end.line_type.compare("line丸ノ内線B")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[9].size(); i++) {
            if (start.station_name.compare(directList[9][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[9][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.line_type.compare("line副都心線")==0 && end.line_type.compare("line副都心線")==0) {
        //        根据数组下表进行方向判断
        for (int i=0; i<directList[10].size(); i++) {
            if (start.station_name.compare(directList[10][i]) == 0) {
                Fist=i;
            }
            if (end.station_name.compare(directList[10][i]) == 0)
            {
                Last=i;
            }
        }
    }
    else if (start.station_name.compare(end.station_name) == 0)
    {
        resultstring.assign(start.line_type);
        resultstring.append(end.line_type);
        return resultstring;
    }
    
    
    
    if (Last-Fist >0) {
        if (start.line_type.compare("line銀座線")==0 && end.line_type.compare("line銀座線")==0)
        {resultstring.assign("Shibuya");}
        else if (start.line_type.compare("line日比谷線")==0 && end.line_type.compare("line日比谷線")==0)
        {resultstring.assign("NakaMeguro");}
        else if (start.line_type.compare("line東西線")==0 && end.line_type.compare("line東西線")==0)
        {resultstring.assign("NishiFunabashi");}
        else if (start.line_type.compare("line千代田線")==0 && end.line_type.compare("line千代田線")==0)
        {resultstring.assign("YoyogiUehara");}
        else if (start.line_type.compare("line千代田線B")==0 && end.line_type.compare("line千代田線B")==0)
        {resultstring.assign("KitaAyase");}
        else if (start.line_type.compare("line半蔵門線")==0 && end.line_type.compare("line半蔵門線")==0)
        {resultstring.assign("Oshiage");}
        else if (start.line_type.compare("line南北線")==0 && end.line_type.compare("line南北線")==0)
        {resultstring.assign("Meguro");}
        else if (start.line_type.compare("line有楽町線")==0 && end.line_type.compare("line有楽町線")==0)
        {resultstring.assign("ShinKiba");}
        
        else if (start.line_type.compare("line丸ノ内線")==0 && end.line_type.compare("line丸ノ内線")==0)
        {resultstring.assign("Ogikubo");}
        else if (start.line_type.compare("line丸ノ内線B")==0 && end.line_type.compare("line丸ノ内線B")==0)
        {resultstring.assign("Honancho");}
        else if (start.line_type.compare("line副都心線")==0 && end.line_type.compare("line副都心線")==0)
        {resultstring.assign("Shibuya");}
    
        
    }else if (Last-Fist <0)
    {
        if (start.line_type.compare("line銀座線")==0 && end.line_type.compare("line銀座線")==0)
        {resultstring.assign("Asakusa");}
        else if (start.line_type.compare("line日比谷線")==0 && end.line_type.compare("line日比谷線")==0)
        {resultstring.assign("KitaSenju");}
        else if (start.line_type.compare("line東西線")==0 && end.line_type.compare("line東西線")==0)
        {resultstring.assign("Nakano");}
        else if (start.line_type.compare("line千代田線")==0 && end.line_type.compare("line千代田線")==0)
        {resultstring.assign("Ayase");}
        else if (start.line_type.compare("line千代田線B")==0 && end.line_type.compare("line千代田線B")==0)
        {resultstring.assign("Ayase");}
        else if (start.line_type.compare("line半蔵門線")==0 && end.line_type.compare("line半蔵門線")==0)
        {resultstring.assign("Shibuya");}
        else if (start.line_type.compare("line南北線")==0 && end.line_type.compare("line南北線")==0)
        {resultstring.assign("AkabaneIwabuchi");}
        else if (start.line_type.compare("line有楽町線")==0 && end.line_type.compare("line有楽町線")==0)
        {resultstring.assign("Wakoshi");}
        
        else if (start.line_type.compare("line丸ノ内線")==0 && end.line_type.compare("line丸ノ内線")==0)
        {resultstring.assign("Ikebukuro");}
        else if (start.line_type.compare("line丸ノ内線B")==0 && end.line_type.compare("line丸ノ内線B")==0)
        {resultstring.assign("NakanoSakaue");}
        else if (start.line_type.compare("line副都心線")==0 && end.line_type.compare("line副都心線")==0)
        {resultstring.assign("Wakoshi");}
        
    }
    
    
    
    return resultstring;
}

/**************************路线查找        方案1*******************************************/
void routeSearchWithStatTime(string start ,string start_lineType, string end ,string end_lineType, string startTime)
{
    
    
    int findRoute=0;
    stationMessage currentStation;
    
    //查找起点坐标
    int tempIndex;
    for (int i=0; i < count_amount; i++) {
        if (start.compare(real_station[i].station_name)==0 && start_lineType.compare(real_station[i].line_type)==0 )
        {
            tempIndex=i;
            break;
        }
    }
    
    
    //加入起点
    real_station[tempIndex].isInOpenList=1;
    real_station[tempIndex].startTime.assign(startTime);
    real_station[tempIndex].H=0;
    openList.push_back(real_station[tempIndex]);
    tempIndex=0;
    
    
    while (1) {
        
        //整理listopen
        openList.sort(sortRule);
        
        currentStation=openList.front();
        currentStation.isInCloseList=1;
        currentStation.isInOpenList=0;
        //放入closeList
        closeList.push_back(currentStation);
        
        for (int i=0; i<count_amount; i++) {
            //line 要相同别忘记
            if (currentStation.station_name.compare(real_station[i].station_name) == 0 &&
                currentStation.line_type.compare(real_station[i].line_type) == 0)
            {
                tempIndex=i;
                break;
            }
        }
        real_station[tempIndex].isInCloseList=1;
        real_station[tempIndex].isInOpenList=0;
        
        openList.pop_front();
        
        if (end.compare(currentStation.station_name)==0  && end_lineType.compare(currentStation.line_type)==0 ) {
            findRoute=1;
            break;
        }
        
        //查找当前车站边上能走的车站
        dealWithStation(currentStation,count_amount);
        
        if (openList.empty()) {
            findRoute=0;
            break;
        }
        
    }
    
    
    if (findRoute==0) {
        printf("%s(%s)  to %s (%s) has no route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        printf("\n");
        printf("eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee");
        
    }
    else
    {
        printf("%s(%s)  to %s (%s) has a route\n",start.c_str(),start_lineType.c_str(),end.c_str(),end_lineType.c_str());
        //找到close列表中的终点，从而来查找路径
        stationMessage  temp;
        list<stationMessage>::iterator it;
        for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
            temp = *it;
            if (temp.station_name.compare(end)==0 ) {
                break;
            }
        }
        finalyRoute.push_back(temp);
        //记录经过多少个车站和最大经过车站数
        //finalroute用来存储找寻的最终路径
        int pass_count=0;
        //再close列表中回退父节点的信息
        while (temp.dady_station.empty()==false) {
            for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
                stationMessage ttt=*it;
                if (temp.dady_station.compare(ttt.station_name)==0 && temp.dady_lineType.compare(ttt.line_type) ==0) {
                    temp=ttt;
                    finalyRoute.push_back(ttt);
                    break;
                }
            }
            pass_count++;
            if (max_pass_count < pass_count) {
                max_pass_count=pass_count;
            }
        }
        printf("the number of pass station :%d\n",pass_count);
        
        //对最后生成的路径进行时间的精确认定
         dealWithFinalyRouteOne();


        
        printf("\n");
        printf("\n");
        
    }
}


void dealWithStation(stationMessage currentStation,int count)
{
    int index=0;
    //找到当前车站的下表，从地图信息来寻找能去的车站
    for (int i=0; i<count ; i++) {
        if (real_station[i].station_name.compare(currentStation.station_name) == 0  &&
            real_station[i].line_type.compare(currentStation.line_type) == 0 ){
            index=i;
            break;
        }
    }
    
    //记录能取的车站
    int nerbor_count=0;
    int nerbor_index[station_number];
    for (int i=0; i<count; i++) {
        if (map_list[index][i]!=9999) {
            nerbor_index[nerbor_count]=i;
            nerbor_count++;
        }
    }
    
    //判断车站是否能加入Open list
    for (int i=0; i<nerbor_count; i++) {
        insertToOpenList(real_station[nerbor_index[i]],currentStation);
    }
    
}

void insertToOpenList(stationMessage station_forCheck, stationMessage currentstation)
{
    
    //检查的是能去的车站，一定是同一条线或者换乘站
    
    
    if (station_forCheck.isInCloseList!=1) {
        
        //已经存在于openlist
        if (station_forCheck.isInOpenList) {
            //同一条路线
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //               已经存在的路线                               沿着新路线的走法所花的时间
                if (station_forCheck.H >  currentstation.H+abs(currentstation.time-station_forCheck.time) ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.w=lineWait(station_forCheck.line_type);
                    station_forCheck.H=currentstation.H+abs(currentstation.time-station_forCheck.time);
                    station_forCheck.H=station_forCheck.H+station_forCheck.w;
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);

                }
            }
            //换乘站
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                if (station_forCheck.H > currentstation.H+1 ) {
                    //新的路比你当前的走过来的路要方便，则改变
                    station_forCheck.w=0;
                    station_forCheck.H=currentstation.H+1;
                    station_forCheck.H=station_forCheck.H+station_forCheck.w;
                    station_forCheck.dady_station.assign(currentstation.station_name);
                    station_forCheck.dady_lineType.assign(currentstation.line_type);

                }
                
            }
        }
        
        
        //加入到openlist
        else{
            if (station_forCheck.line_type.compare(currentstation.line_type) == 0) {
                //同一条线
                station_forCheck.w=lineWait(station_forCheck.line_type);
                station_forCheck.H=currentstation.H+abs(currentstation.time-station_forCheck.time);
                station_forCheck.H=station_forCheck.H+station_forCheck.w;
                station_forCheck.isInOpenList=1;
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);
                
                openList.push_back(station_forCheck);
            }
            else if (station_forCheck.station_name.compare(currentstation.station_name) == 0  && station_forCheck.line_type.compare(currentstation.line_type) != 0)
            {
                //换乘站
                station_forCheck.w=0;
                station_forCheck.H=currentstation.H+1;
                station_forCheck.H=station_forCheck.H+station_forCheck.w;
                station_forCheck.isInOpenList=1;
                station_forCheck.dady_station.assign(currentstation.station_name);
                station_forCheck.dady_lineType.assign(currentstation.line_type);

                openList.push_back(station_forCheck);
            }
        }
    }
}


int lineWait(string lineType)
{
    int waitTime=5;
    
    
    return waitTime;
}

string startTimeOne(string startTime ,int timeSpan)
{
    string  tempTime;
    string hour,tempHour;
    string minite,tempMinite;
    
    hour.assign(startTime.substr(0,2));
    minite.assign(startTime.substr(3,2));
    
    int h=stoi(hour);
    int m=stoi(minite);
    
    if (m+timeSpan>=60) {
        m=m+timeSpan-60;
        h=h+1;
    }else
    {
        m=m+timeSpan;
    }
    
    tempHour.assign(to_string(h)) ;
    tempMinite.assign(to_string(m));
    
    if (m<10) {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":0");
        tempTime.append(tempMinite);
    }else
    {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":");
        tempTime.append(tempMinite);
    }
    
    
    return tempTime;
}




void dealWithFinalyRouteOne()
{
    //方案1
    //设立平均等待时间T=1min
    int span=1;
    int maxsize=finalyRoute.size();
    for (int i=0; i<maxsize; i++) {
        printf("%s(%s)%s====",finalyRoute[i].station_name.c_str(),finalyRoute[i].line_type.c_str(),
              timePlus(finalyRoute[maxsize-1].startTime, finalyRoute[i].H).c_str());
    }
}



string timeTakenFrom (string endtime,int span)
{
    string  tempTime;
    string hour,tempHour;
    string minite,tempMinite;
    
    hour.assign(endtime.substr(0,2));
    minite.assign(endtime.substr(3,2));
    
    int h=stoi(hour);
    int m=stoi(minite);
    
    if (m-span < 0) {
        m=m+60-span;
        if (h==0) {
            h=23;
        }else
        {
            h=h-1;
        }
    }else
    {
        m=m-span;
    }
    
    tempHour.assign(to_string(h)) ;
    tempMinite.assign(to_string(m));
    
    if (m<10) {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":0");
        tempTime.append(tempMinite);
    }else
    {
        if (h<10) {
            tempTime.append("0");
        }
        tempTime.append(tempHour);
        tempTime.append(":");
        tempTime.append(tempMinite);
    }
    
    
    return tempTime;
}


int timeMInas(string startTime, string endTime)
{
    int  tempTime;
    string startHour,endHour;
    string startMinite,endMinite;
    
    startHour.assign(startTime.substr(0,2));
    startMinite.assign(startTime.substr(3,2));
    
    endHour.assign(endTime.substr(0,2));
    endMinite.assign(endTime.substr(3,2));
    
    int Sh=stoi(startHour);
    int Sm=stoi(startMinite);
    
    int Eh=stoi(endHour);
    int Em=stoi(endMinite);

    int m ;
    int h;
    m=Em-Sm;
    h=Eh-Sh;
    if (m>=0) {
        if (h>=0) {
            tempTime=h*60+m;
        }
    }else{
        tempTime=(h-1)*60+m+60;
    }
    return tempTime;
}

/*************************LINE信息*******************************************/
void fillAllLineType()
{
    int maxNumber=31;
    AllLineType.clear();
    for (int i=0; i<maxNumber; i++) {
        string tempS="sfd";
        AllLineType.push_back(tempS);
    }
    //metro
    AllLineType[0].assign("\"銀座線\"");
    AllLineType[1].assign("\"日比谷線\"");
    AllLineType[2].assign("\"東西線\"");
    AllLineType[3].assign("\"千代田線\"");
    AllLineType[4].assign("\"千代田線B\"");
    AllLineType[5].assign("\"半蔵門線\"");
    AllLineType[6].assign("\"南北線\"");
    AllLineType[7].assign("\"有楽町線\"");
    AllLineType[8].assign("\"丸ノ内線\"");
    AllLineType[9].assign("\"丸ノ内線B\"");
    AllLineType[10].assign("\"副都心線\"");
    //其他
    AllLineType[11].assign("\"新宿線\"");
    AllLineType[12].assign("\"小田原線\"");
    AllLineType[13].assign("\"西武新宿線\"");
    AllLineType[14].assign("\"池袋線\"");
    AllLineType[15].assign("\"伊勢崎線\"");
    AllLineType[16].assign("\"伊勢崎線B\"");
    AllLineType[17].assign("\"大江戸線\"");
    AllLineType[18].assign("\"大江戸線B\"");
    AllLineType[19].assign("\"浅草線\"");
    AllLineType[20].assign("\"三田線\"");
    AllLineType[21].assign("\"東武東上線\"");
    AllLineType[22].assign("\"常磐線\"");
    AllLineType[23].assign("\"田園都市線\"");
    AllLineType[24].assign("\"東横線\"");
    AllLineType[25].assign("\"みなとみらい線\"");
    AllLineType[26].assign("\"目黒線\"");
    AllLineType[27].assign("\"大井町線\"");
    AllLineType[28].assign("\"池上線\"");
    AllLineType[29].assign("\"東急多摩川線\"");
    AllLineType[30].assign("\"常磐線各停\"");

}

/*************************获取suica客户信息*******************************************/
void dealWithCustomerData()
{
    /*客户信息的文件处理*/
    string customerDataString;
    vector<string> customerVector;
    vector<customerData> csData;
    
    finalData.clear();
    
    customerDataString=Read_Str("/Users/allensim/Desktop/sample_od_20170201.txt");
    customerVector=split(customerDataString, "\n");
    vector<string> temp;
    for (int i=0; i<customerVector.size()-1; i++) {
        temp.clear();
        customerData tempData;
        temp=split(customerVector[i], ",");
        tempData.year.assign(temp[0]);
        tempData.hour.assign(temp[1]);
        tempData.line_in.assign(temp[2]);
        tempData.station_in.assign(temp[3]);
        tempData.line_out.assign(temp[4]);
        tempData.station_out.assign(temp[5]);
        
        finalData.push_back(tempData);
    }
}


/**************************重置清空数据*******************************************/
void clearData()
{
    list<stationMessage>::iterator it;
    stationMessage  temp;
    for ( it=closeList.begin()  ; it!=closeList.end(); it++) {
        temp=*it;
        temp.isInOpenList=0;
        temp.isInCloseList=0;
    }
    for ( it=openList.begin()  ; it!=openList.end(); it++) {
        temp=*it;
        temp.isInOpenList=0;
        temp.isInCloseList=0;
    }
    openList.clear();
    closeList.clear();
    
    for (int k=0; k<count_amount; k++) {
        real_station[k].isInCloseList=0;
        real_station[k].isInOpenList=0;
        real_station[k].dady_station.clear();
        real_station[k].dady_lineType.clear();
    }
    
    
    finalyRoute.clear();
}


//没新加一条线需要改动的地方  setStationMessage  ，中英文对照表 ， 方向表  ，fillAllLineType  ，retureTimeAnti   ，retureTime  ,direction
/**************************设置车站信息*******************************************/
void setStationMessage()
{
    double List[station_number][station_number];
    
    //reset
    for (int i=0; i<station_number; i++) {
        for (int j=0; j<station_number; j++) {
            List[i][j]=9999;
        }
    }
    
//    addDataSource("/Users/allensim/Desktop/text26.txt");

    
    //METRO
        addDataSource("/Users/allensim/Desktop/text1.txt");
        addDataSource("/Users/allensim/Desktop/text2.txt");
        addDataSource("/Users/allensim/Desktop/text3.txt");
        addDataSource("/Users/allensim/Desktop/text4.txt");
        addDataSource("/Users/allensim/Desktop/text5.txt");
        addDataSource("/Users/allensim/Desktop/text6.txt");
        addDataSource("/Users/allensim/Desktop/text7.txt");
        addDataSource("/Users/allensim/Desktop/text8.txt");
        addDataSource("/Users/allensim/Desktop/text9.txt");
        addDataSource("/Users/allensim/Desktop/text10.txt");
        addDataSource("/Users/allensim/Desktop/text11.txt");
    
    //OTHER
    addDataSource("/Users/allensim/Desktop/text12.txt");
    addDataSource("/Users/allensim/Desktop/text13.txt");
    addDataSource("/Users/allensim/Desktop/text14.txt");
    addDataSource("/Users/allensim/Desktop/text15.txt");
    addDataSource("/Users/allensim/Desktop/text16.txt");
    addDataSource("/Users/allensim/Desktop/text17.txt");
    addDataSource("/Users/allensim/Desktop/text18.txt");
    addDataSource("/Users/allensim/Desktop/text19.txt");
    addDataSource("/Users/allensim/Desktop/text20.txt");
    addDataSource("/Users/allensim/Desktop/text21.txt");
    addDataSource("/Users/allensim/Desktop/text22.txt");
    addDataSource("/Users/allensim/Desktop/text23.txt");
    addDataSource("/Users/allensim/Desktop/text24.txt");
    addDataSource("/Users/allensim/Desktop/text25.txt");
    addDataSource("/Users/allensim/Desktop/text26.txt");
    addDataSource("/Users/allensim/Desktop/text27.txt");
    addDataSource("/Users/allensim/Desktop/text28.txt");
    addDataSource("/Users/allensim/Desktop/text29.txt");
    addDataSource("/Users/allensim/Desktop/text30.txt");
    addDataSource("/Users/allensim/Desktop/text31.txt");


    //进行二维数组的时间填充
    for (int i=0; i<count_amount; i++) {
        //线路相同的时间分配
        if (real_station[i+1].line_type.compare(real_station[i].line_type) ==0 ) {
            List[i][i+1]=abs(real_station[i+1].time-real_station[i].time);
            List[i+1][i]=abs(real_station[i+1].time-real_station[i].time);
        }
    }
    
    //中转站的判断
    for (int i=0; i<count_amount; i++) {
        for (int j=0; j<count_amount; j++) {
            //中转站所花费的时间
            if (real_station[i].station_name.compare(real_station[j].station_name)==0  &&
                real_station[i].line_type.compare(real_station[j].line_type)!=0) {
                List[i][j]=1;
                List[j][i]=1;
            }
        }
    }
    
    //作用域扩大
    for (int i=0; i<count_amount; i++) {
        for (int j=0; j<count_amount; j++) {
            map_list[i][j]=List[i][j];
        }
    }
    

    
    return;
}


/**************************解析Json数据Timetable*******************************************/

void readTimeTable()
{
    //json的读取
    string read_string=Read_Str("/Users/allensim/Desktop/timetable_2017_02_01.json");
    
    
    //添加元素，使其成为json数据来解析
    read_string.insert(0, "{\"stationTimeTableList\":\n");
    read_string.append("}");
    
    Json::Reader reader;
    Json::Value root;
    
    if (!reader.parse(read_string, root, false))
    {
        std::cout << "failed!\n";
        return ;
    }
    
    
    
    jsonList.clear();
    int count_list=root["stationTimeTableList"].size();
    for (int i = 0 ; i < count_list ; i++) {
        timeTable_json temp_table_json;
        //解析json構造体
        temp_table_json.type.assign(root["stationTimeTableList"][i]["@type"].asString());
        temp_table_json.context.assign(root["stationTimeTableList"][i]["@context"].asString());
        temp_table_json.sameAs.assign(root["stationTimeTableList"][i]["owl:sameAs"].asString());
        temp_table_json.station.assign(root["stationTimeTableList"][i]["odpt:station"].asString());
        temp_table_json.railway.assign(root["stationTimeTableList"][i]["odpt:railway"].asString());
        temp_table_json.operator_s.assign(root["stationTimeTableList"][i]["odpt:operator"].asString());
        temp_table_json.railDirection.assign(root["stationTimeTableList"][i]["odpt:railDirection"].asString());
        temp_table_json.date.assign(root["stationTimeTableList"][i]["dc:date"].asString());
        temp_table_json.id.assign(root["stationTimeTableList"][i]["@id"].asString());
        
        string arr[3] = {"odpt:weekdays", "odpt:saturdays","odpt:holidays"};
        //json中日付毎に数字を入れる
        for (int j=0; j<3; j++) {
            vector<stationMessage_json> temp_list;
            temp_list.clear();
            Json:: Value timeValue = root["stationTimeTableList"][i][arr[j]];
            int count= timeValue.size();
            for (int k=0; k<count; k++) {
                stationMessage_json temp;
                temp.departureTime.assign(timeValue[k]["odpt:departureTime"].asString());
                temp.destinationStation.assign(timeValue[k]["odpt:destinationStation"].asString());
                temp.trainType.assign(timeValue[k]["odpt:trainType"].asString());
                if (!timeValue[k]["odpt:isLast"].isNull() ) {
                    temp.isLast.assign(timeValue[k]["odpt:isLast"].asString());
                }
                if (!timeValue[k]["odpt:notes"].isNull() ) {
                    temp.notes.assign(timeValue[k]["odpt:notes"].asString());
                }
                temp_list.push_back(temp);
            }
            temp_table_json.stationMs_jsons.push_back(temp_list) ;
        }
        
        
        
        jsonList.push_back(temp_table_json);
    }
    
    return;
}


/**************************方向对照*******************************************/
void lineDirSet()
{
    //定制方向
    string Dsrting;
    vector<string> DataVector;
    
    
    Dsrting=Read_Str("/Users/allensim/Desktop/lineDirection.txt");
    DataVector=split(Dsrting, "\n");
    vector<string> temp;
    for (int i=0; i<DataVector.size(); i++) {
        temp.clear();
        temp=split(DataVector[i], ",");
        directList.push_back(temp);
    }
    
    //    for (int i=0; i<directList[1].size(); i++) {
    //        printf("%s, %s \n",directList[0][i].c_str(),directList[1][i].c_str());
    //    }
    
}

/****************************30小时制*****************************************/
string timeThirty(string time)
{
    string result;
    
    string Hour;
    string Minite;
    
    Hour.assign(time.substr(0,2));
    Minite.assign(time.substr(3,2));
    
    if (Hour.compare("00") == 0) {
        Hour.assign("24");
    }else if (Hour.compare("01") == 0){
        Hour.assign("25");
    }else if (Hour.compare("02")  == 0){
        Hour.assign("26");
    }else{
        return time;
    }
    
    result.append(Hour);
    result.append(":");
    result.append(Minite);

    return result;
}

/**************************判断周几的函数*******************************************/
string judgeDate(int year,int month,int day)
{
    // {"odpt:weekdays", "odpt:saturdays","odpt:holidays"};
    string date_result;
    if(month==1||month==2) //把一月和二月换算成上一年的十三月和是四月
    {
        month+=12;
        year--;
    }
    int Week=(day+2*month+3*(month+1)/5+year+year/4-year/100+year/400)%7;
    switch(Week)
    {
        case 0:  date_result.assign("weekdays"); break;
        case 1:  date_result.assign("weekdays"); break;
        case 2:  date_result.assign("weekdays"); break;
        case 3:  date_result.assign("weekdays"); break;
        case 4:  date_result.assign("weekdays"); break;
        case 5:  date_result.assign("saturdays"); break;
        case 6:  date_result.assign("saturdays"); break;
    }
    

    return date_result;
}


/**************************车站的日英对照*******************************************/
void  languageTableSet( )
{
    
    string DataString;
    vector<string> DataVector;
    
    DataString=Read_Str("/Users/allensim/Desktop/JAtoEN.txt");
    DataVector=split(DataString, "\n");
    vector<string> temp;
    for (int i=0; i<DataVector.size()-1; i++) {
        temp.clear();
        temp=split(DataVector[i], ",");
        nameLanguage tempData;
        tempData.JA.assign(temp[0]);
        tempData.EN.assign(temp[1]);
        NameList.push_back(tempData);
    }
    //    for (int i=0; i<NameList.size(); i++) {
    //        printf("%s, %s \n",NameList[i].JA.c_str(),NameList[i].EN.c_str());
    //    }
}

//字符串分割函数
vector<string> split(string str,string pattern)
{
    string::size_type pos;
    vector<string> result;
    str+=pattern;//加分隔符扩展字符串以方便操作，不遗漏最后一组数据等
    int size=str.length();
    
    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}




/**************************解析车站信息*******************************************/


void addDataSource(char * address)
{
    
    ArrayWithPath(address);
    
    return;
//
//
//    stationMessage *station;
//    int count=0;
//
//    station=ArrayWithPath(address);
//    count=countArray(address);
//    for (int i=0; i<count-1; i++) {
//        real_station.push_back(*(station+i+1));
//        count_amount++;
//    }
}



stationMessage* ArrayWithPath(char* address)
{
    int count=0;
    count=countArray(address);
    
    
    FILE *fp = NULL;
    char  buff[255];
    char  station[100][255];
    stationMessage STM[100];
    char line_type[255];
    
    fp = fopen(address, "r");
    
    fgets(buff, 255, (FILE*)fp);
    
    
    
    int line_index=0;
    for (int i=0; i<255; i++) {
        if(buff[i]=='\t' )
        {
            line_index=i;
            break;
        }
    }
    memset(line_type, 0, sizeof(line_type));
    strncpy(line_type,buff, line_index);
    
    
    
    
    int index=0;
    int  value=0;
    while (strstr(buff, "line_end")==NULL){
        strcpy(station[index],buff);
        fgets(buff, 255, (FILE*)fp);
        index++;
    }
    
    for (int i=0; i<index; i++) {
        int finded=0;
        int k=0;
        for (int j=0; j<255; j++) {
            if (station[i][j]=='\n') {
                break;
            }else{
                if (station[i][j] >='0' &&  station[i][j] <='9' ) {
                    value=value*10+station[i][j]-'0';
                }else if(station[i][j]=='\t' && finded==0)
                {
                    k=j;
                    finded=1;
                }
            }
        }
        
        STM[i].time=value;
        
        STM[i].station_name.assign(station[i],0,k);
        STM[i].line_type.assign(line_type);
        value=0;
    }
    
    
    //pre,next
    for (int i=2; i<index-1; i++) {
        STM[i].connect_next.assign(STM[i+1].station_name);
        STM[i].connect_pre.assign(STM[i-1].station_name);
    }
    STM[1].connect_next.assign(STM[2].station_name);
    STM[1].connect_pre.assign("NULL");
    STM[index-1].connect_next.assign("NULL");
    STM[index-1].connect_pre.assign(STM[index-2].station_name);
    
    
    
    
    fclose(fp);
    
    
    count=countArray(address);
    for (int i=0; i<count-1; i++) {
        real_station.push_back(STM[i+1]);
        count_amount++;
    }

    return STM;
}


int countArray(char * address)
{
    FILE *fp = NULL;
    char  buff[255];
    char  station[100][255];
    
    fp = fopen(address, "r");
    
    fgets(buff, 255, (FILE*)fp);
    
    int index=0;
    while (strstr(buff, "line_end")==NULL){
        strcpy(station[index],buff);
        fgets(buff, 255, (FILE*)fp);
        index++;
    }
    
    fclose(fp);
    return index;
    
}

