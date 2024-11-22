#ifndef _ARMOR_DETECTOR_
#define _ARMOR_DETECTOR_

#include<iostream>
#include<cmath>
#include<vector>
#include<opencv2/opencv.hpp>
#include<algorithm>

using namespace std;
using namespace cv;

const int RED = 0;
const int BLUE = 1;

class LightBar : public RotatedRect
{
    public:
        LightBar() = default;
        explicit LightBar(RotatedRect& rect);
        Point2f center;
        Point2f rp[4];
        Point2f upmid, downmid;
        double ratio_of_height_and_width;
        double height;
        double width;
        double angle;
        double y;
        double s;
};
//定义灯条的各个数值

class Armor
{
    public:
        explicit Armor(LightBar& lb, LightBar& rb, int& color);
        LightBar leftbar;
        LightBar rightbar;
        int armor_color;
};
//定义装甲板

class Limits
{
    public:
        int threshnum = 90;    //二值化阈值
        double limit_s = 140;   //最小旋转矩形面积
        double limit_min_ratiohw = 1.5;   //最小长宽比
        double limit_max_ratiohw = 5;   //最大长宽比
        double limit_min_ratioy = 0;  //最小y轴值之比
        double limit_max_ratioy = 100;  //最大y轴值之比
        double limit_max_ratioa = 10;   //最大角差
        double limit_min_ratioh = 0;  //最小高比
        double limit_max_ratioh = 100;  //最大高比
        double limit_min_distance_of_x = 30;    //最小灯条间距离
};
//设置限制值

Mat preprocessImg(Mat& frame);  //预处理当前帧图像
Mat fcontours(Mat& frame, Mat& frame1); //识别轮廓
void process_section1(Mat& frame);  //第一阶段处理
void process_section2();    //第二阶段处理
void draw_armor(Mat& frame);    //画出装甲板
//主要流程函数

void debug_Info1(); //第一阶段处理结束后所有灯条调试信息
void debug_Info2(); //第二阶段处理结束后armor类的灯条信息和识别装甲板的个数
void debug_draw(Mat& frame);    //框出灯条
//调试函数

void adjustRect(RotatedRect& rect); //纠正旋转矩形
int colordetector(vector<Point> cor, Mat frame);    //识别装甲板颜色
bool checkt2size(LightBar& lightbar);   //通过灯条大小筛选
bool checkt2ratio(LightBar& lightbar);  //通过灯条长宽比筛选
bool checkt2y(LightBar& lightbar1, LightBar& lightbar2);    //比较两灯条间y轴高度
bool checkt2angle(LightBar& lightbar1, LightBar& lightbar2);    //比较两灯条间的角差
bool checkt2ratioh(LightBar& lightbar1, LightBar& lightbar2);   //比较两灯条间的高度比值
bool checkt2distanceofx(LightBar& lightbar1, LightBar& lightbar2);  //比较两灯条间的距离
//工具类函数

vector<vector<Point>> lightPoints;  //储存轮廓
vector<LightBar> lightbars; //储存第一次过滤后灯条
vector<Armor> armors;   //储存识别到装甲板的数量
int color;
Limits limits;

#endif
