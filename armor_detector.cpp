#include "armor_detector.hpp"

LightBar::LightBar(RotatedRect& rect) : RotatedRect(rect)
{
    height = rect.size.height;
    width = rect.size.width;
    ratio_of_height_and_width = height / width;
    s = height * width;
    center = rect.center;
    angle = rect.angle;
    y = center.y;
    rect.points(rp);
    upmid.x = (rp[0].x + rp[3].x) / 2;
    upmid.y = (rp[0].y + rp[3].y) / 2;
    downmid.x = (rp[2].x + rp[1].x) / 2;
    downmid.y = (rp[2].y + rp[1].y) / 2;

    // debuginfo
    // cout << "height:" << height << " "
    // << "width:" << width << " "
    // << "size:" << s << " "
    // << "angle:" << angle << " "
    // << "y:" << y << endl;
}

Armor::Armor(LightBar& lb, LightBar& rb, int& color) {
    leftbar = lb;
    rightbar = rb;
    armor_color = color;
}

Mat preprocessImg(Mat& frame)
{
    Mat kframe;
    GaussianBlur(frame, kframe, Size(5, 5), 0);
    Mat greyImg;
    cvtColor(kframe, greyImg, COLOR_RGB2GRAY);
    Mat threshImg;
    threshold(greyImg, threshImg, limits.threshnum, 255, THRESH_BINARY);
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    dilate(threshImg, threshImg, element);
    return threshImg;
}

Mat fcontours(Mat& frame, Mat& frame1)
{
    findContours(frame.clone(), lightPoints, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    Mat debugImg = frame1.clone();
    // for (size_t i = 0; i < lightPoints.size(); i++) {
    //     drawContours(debugImg, lightPoints, i, Scalar(0, 255, 0), 1, 8);
    // }
    return debugImg;
}

void adjustRect(RotatedRect& rect)
{
    float& width = rect.size.width;
    float& height = rect.size.height;
    float& angle = rect.angle;
    while(angle >= 90.0) angle -= 180.0;
    while(angle < -90.0) angle += 180.0;
    if(angle >= 45.0)
    {
        swap(width, height);
        angle -= 90.0;
    }
    else if(angle < -45.0)
    {
        swap(width, height);
        angle += 90.0;
    }
}

int colordetector(vector<Point> cor, Mat frame)
{
    int red = 0;
    int blue = 0;
    for (const auto& point : cor) {
        red += frame.at<Vec3b>(point.y, point.x)[2];
        blue += frame.at<Vec3b>(point.y, point.x)[0];
    }
    if (red > blue) {
        return RED;
    }
    else return BLUE;
}

bool checkt2size(LightBar& lightbar)
{
    if (lightbar.s > limits.limit_s) {
        return true;
    }
    else return false;
}

bool checkt2ratio(LightBar& lightbar)
{
    if (lightbar.ratio_of_height_and_width < limits.limit_max_ratiohw &&
        lightbar.ratio_of_height_and_width > limits.limit_min_ratiohw) {
            return true;
    }
    else return false;
}

bool checkt2y(LightBar& lightbar1, LightBar& lightbar2)
{
    double ratio_y = abs(lightbar1.y - lightbar2.y);
    if (ratio_y > limits.limit_min_ratioy && 
        ratio_y < limits.limit_max_ratioy) {
        return true;
    }
    else return false;
}

bool checkt2angle(LightBar& lightbar1, LightBar& lightbar2)
{
    Point2f p1, p2;
    p1 = lightbar1.center;
    p2 = lightbar2.center;
    double angle = atan2(abs(p1.y - p2.y), abs(p1.x - p2.x));
    angle = angle / CV_PI * 180;
    if (angle < limits.limit_max_ratioa) return true;
    else return false;
}

bool checkt2ratioh(LightBar& lightbar1, LightBar& lightbar2)
{
    double ratio_h = lightbar1.height / lightbar2.height;
    if (ratio_h < limits.limit_max_ratioh && 
        ratio_h > limits.limit_min_ratioh) {
            return true;
        }
    else return false;
}

bool checkt2distanceofx(LightBar& lightbar1, LightBar& lightbar2)
{
    double x1 = lightbar1.center.x;
    double x2 = lightbar2.center.x;
    double distance = abs(x1 - x2);
    if (distance > limits.limit_min_distance_of_x) {
        return true;
    }
    else return false;
}

void process_section1(Mat& frame)
{
    for (const auto& cor : lightPoints) {
        if (cor.size() < 6) {
            continue;
        }
        RotatedRect rect;
        rect = minAreaRect(cor);
        adjustRect(rect);
        LightBar lb = LightBar(rect);
        if (checkt2size(lb) == true && checkt2ratio(lb) == true &&
            colordetector(cor, frame) == color) {
            lightbars.push_back(lb);
        }
    }
}

void debug_Info1()
{
    for (const auto& lb : lightbars) {
        cout << "height:" << lb.height << " "
        << "width:" << lb.width << " "
        << "size:" << lb.s << " "
        << "angle:" << lb.angle << " "
        << "y:" << lb.y << endl;
    }
}

void process_section2()
{
    for (size_t i = 0; i < lightbars.size(); i++) {
        for (size_t j = i + 1; j < lightbars.size(); j++) {
            LightBar leftbar = lightbars[i];
            LightBar rightbar = lightbars[j];
            if (checkt2y(leftbar, rightbar) == true &&
                checkt2angle(leftbar, rightbar) == true &&
                checkt2ratioh(leftbar, rightbar) == true &&
                checkt2distanceofx(leftbar, rightbar) == true) {
                    Armor armor = Armor(leftbar, rightbar, color);
                    armors.push_back(armor);
            }
        }
    }
}

void debug_Info2()
{
    for (const auto& armor : armors) {
        cout << "l_height:" << armor.leftbar.height << " "
        << "l_width:" << armor.leftbar.width << " "
        << "l_size:" << armor.leftbar.s << " "
        << "l_angle:" << armor.leftbar.angle << " "
        << "l_y:" << armor.leftbar.y << endl;
        cout << "r_height:" << armor.rightbar.height << " "
        << "r_width:" << armor.rightbar.width << " "
        << "r_size:" << armor.rightbar.s << " "
        << "r_angle:" << armor.rightbar.angle << " "
        << "r_y:" << armor.rightbar.y << endl;
        cout << armors.size() << endl;
        cout << "\n" << endl;
    }
}

void debug_draw(Mat& frame)
{
    Point2f rp1[4];
    Point2f rp2[4];
    for (const auto& armor : armors) {
        armor.leftbar.points(rp1);
        armor.rightbar.points(rp2);
    }
    line(frame, rp1[0], rp1[1], Scalar(0, 255, 0), 2, 8);
    line(frame, rp1[0], rp1[3], Scalar(0, 255, 0), 2, 8);
    line(frame, rp1[1], rp1[2], Scalar(0, 255, 0), 2, 8);
    line(frame, rp1[2], rp1[3], Scalar(0, 255, 0), 2, 8);

    line(frame, rp2[0], rp2[1], Scalar(0, 255, 0), 2, 8);
    line(frame, rp2[0], rp2[3], Scalar(0, 255, 0), 2, 8);
    line(frame, rp2[1], rp2[2], Scalar(0, 255, 0), 2, 8);
    line(frame, rp2[2], rp2[3], Scalar(0, 255, 0), 2, 8);
}

void draw_armor(Mat& frame)
{
    for (const auto& armor : armors) {
        line(frame, armor.leftbar.upmid, armor.rightbar.upmid, Scalar(0, 255, 0), 2, 8);
        line(frame, armor.leftbar.downmid, armor.rightbar.downmid, Scalar(0, 255, 0), 2, 8);
        line(frame, armor.leftbar.upmid, armor.leftbar.downmid, Scalar(0, 255, 0), 2, 8);
        line(frame, armor.rightbar.upmid, armor.rightbar.downmid, Scalar(0, 255, 0), 2, 8);
    }
}
