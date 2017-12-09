#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <getopt.h>
using namespace std;
using namespace cv;

vector<Mat> readYUV(char *image_path, int width, int height)
{
    ifstream fin;
    fin.open(image_path, ios::binary);
    if (!fin.is_open())
    {
        cout << "open failed" << endl;
        exit(0);
    }

    Mat img_Y = Mat::zeros(height, width, CV_8UC1);

    char ch;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            fin.read(&ch, sizeof(ch));

            img_Y.at<uchar>(i, j) = (uchar)ch;
        }
    }

    Mat img_U = Mat::zeros(height / 2, width / 2, CV_8UC1);
    for (int i = 0; i < height / 2; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            fin.read(&ch, sizeof(ch));

            img_U.at<uchar>(i, j) = (uchar)ch;
        }
    }

    Mat img_V = Mat::zeros(height / 2, width / 2, CV_8UC1);
    for (int i = 0; i < height / 2; i++)
    {
        for (int j = 0; j < width / 2; j++)
        {
            fin.read(&ch, sizeof(ch));

            img_V.at<uchar>(i, j) = (uchar)ch;
        }
    }
    fin.close();

    vector<Mat> yuv;
    yuv.push_back(img_Y);
    yuv.push_back(img_U);
    yuv.push_back(img_V);

    return yuv;
}

Mat enlarge(Mat src)
{
    Mat enlarge = Mat::zeros(src.rows * 2, src.cols * 2, CV_8UC1);
    resize(src, enlarge, Size(), 2, 2, INTER_CUBIC);

    return enlarge;
}

Mat merge(vector<Mat> channels)
{
    int width = channels.at(0).cols;
    int height = channels.at(0).rows;

    Mat yuv = Mat::zeros(height, width, CV_8UC3);
    merge(channels, yuv);

    return yuv;
}

Mat cvtYUV2BGR(Mat yuv)
{
    int width = yuv.cols;
    int height = yuv.rows;

    Mat bgr = Mat::zeros(height, width, CV_8UC3);
    cvtColor(yuv, bgr, COLOR_YUV2BGR);

    return bgr;
}

static void print_usage(const char *prog)
{
    printf("Usage: %s -f {filename} -w {width} -h {hight} [-o {filename}]\n", prog);
    puts("  -f --file    yuv raw image file name\n"
         "  -w --width   yuv raw image width\n"
         "  -h --hight   yuv raw image hight\n"
         "  -o --output  converted jpeg file name\n");
}

static char *image_file = NULL;
static char *output_file = NULL;
static unsigned int image_width = 0;
static unsigned int image_height = 0;

static void parse_opts(int argc, char **argv)
{
    int c;
    static const struct option lopts[] = {
        {"file", 1, NULL, 'f'},
        {"width", 1, NULL, 'w'},
        {"hight", 1, NULL, 'h'},
        {"output", 0, NULL, 'o'},
        {NULL, 0, 0, 0},
    };

    while ((c = getopt_long(argc, argv, "f:w:h:o:", lopts, NULL)) != -1)
    {

        switch (c)
        {
        case 'f':
            image_file = optarg;
            break;
        case 'w':
            image_width = atoi(optarg);
            break;
        case 'h':
            image_height = atoi(optarg);
            break;
        case 'o':
            output_file = optarg;
            break;
        default:
            print_usage(argv[0]);
            exit(0);
        }
    }
}

int main(int argc, char *argv[])
{
    parse_opts(argc, argv);
    if (NULL == image_file || 0 == image_width || 0 == image_height)
    {
        std::cerr << "Invalid arguments, check it!" << std::endl;
        print_usage(argv[0]);
        return 0;
    }

    if (NULL == output_file)
    {
        output_file = (char *)"image.jpg";
    }

    std::cout << image_file << std::endl;
    std::cout << image_width << std::endl;
    std::cout << image_height << std::endl;

    vector<Mat> temp = readYUV(image_file, image_width, image_height);
    vector<Mat> channels;
    std::cout << "Read Image OK." << std::endl;

    channels.push_back(temp.at(0));
    channels.push_back(enlarge(temp.at(1)));
    channels.push_back(enlarge(temp.at(2)));

    Mat yuv = merge(channels);
    Mat bgr = cvtYUV2BGR(yuv);
    std::cout << "Convert Image Over." << std::endl;

    imwrite(output_file, bgr);
    std::cout << "Write Image Over." << std::endl;

    return 0;
}