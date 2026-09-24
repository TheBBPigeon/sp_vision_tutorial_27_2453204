#include <iostream>
#include <opencv2/opencv.hpp>

// ======================= 作业 =======================
// 1. 读取 ../assets/demo.jpg
// 2. 使用 cvtColor 把图像转为灰度图（颜色空间：BGR2GRAY）
// 3. 使用 imwrite 把灰度图保存为 gray.jpg
// 4. 在灰度图上用 circle 画一个圆，标记你要"瞄准"的位置
// 5. 显示灰度图，按任意键退出
// ====================================================

int main()
{
    // TODO: 在这里完成你的代码
    const std::string gray_path = "assets/gray.jpg";
    const std::string demo_path = "assets/demo.jpg";

    cv::Mat img = cv::imread(demo_path);
    if(img.empty())
    {
        std::cout<< " img is empty!? check the path !!! "<< std::endl;
        return -1;
    }
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::imwrite(gray_path, gray);

    cv::circle(gray, cv::Point(200, 200), 20, 1, 10);
    cv::imshow("gray", gray);

    cv::waitKey(0);
     

    return 0;
}
