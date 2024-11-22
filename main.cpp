#include "armor_detector.hpp"

int main()
{
    cout << "Choose team's color" << endl;
    cout << "BLUE = 0 || RED = 1" << endl;
    cin >> color;
    VideoCapture cap("blue.avi");
    if (!cap.isOpened()) {
        cerr << "error of opening a video" << endl;
        return -1;
    }
    while (true) 
    {
        Mat frame;
        cap >> frame;
        if (frame.empty()) {
            cout << "video end" << endl;
            break;
        }
        Mat doneframe = preprocessImg(frame);
        Mat doneframe1 = fcontours(doneframe, frame);
        process_section1(frame);
        debug_Info1();
        process_section2();
        debug_Info2();
        // debug_draw(doneframe1);
        draw_armor(doneframe1);
        imshow("video" ,doneframe1);
        lightPoints.clear();
        lightbars.clear();
        armors.clear();
        if (waitKey(10) == 'q') {
            break;
        }
    }
    cap.release();
    destroyAllWindows();
}