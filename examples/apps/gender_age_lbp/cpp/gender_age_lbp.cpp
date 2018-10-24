#include <iostream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <time.h>
#include <stdint.h>
extern "C"
{
#include <fp16.h>
#include <hs.h>
}

#define WINDOW_NAME "Sungem Gender Age"
#define CAM_SOURCE 0
#define XML_FILE "../../../misc/lbpcascade_frontalface_improved.xml"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 360

#define NETWORK_IMAGE_WIDTH 227
#define NETWORK_IMAGE_HEIGHT 227

#define GRAPH_DIR "../../../graphs/"
#define CAT_STAT_DIRECTORY "../../../misc/"
#define GENDER_CAT_DIR "gender_categories.txt"
#define AGE_CAT_DIR "age_categories.txt"

#define INFERENCE_INTERVAL 1

using namespace std;
using namespace cv;


const int FONT = cv::FONT_HERSHEY_PLAIN;
const cv::Scalar BLUE = cv::Scalar(255, 0, 0, 255);
const cv::Scalar GREEN = cv::Scalar(0, 255, 0, 255);
const cv::Scalar RED = cv::Scalar(0, 0, 255, 255);
const cv::Scalar PINK = Scalar(255, 80, 180, 255);
const cv::Scalar BLACK = Scalar(0, 0, 0, 255);

const unsigned int MAX_PATH = 256;

const int PADDING = 60;

const float MALE_GENDER_THRESHOLD = 0.60;
const float FEMALE_GENDER_THRESHOLD = 0.40;

double networkMean[3];
double networkStd[3];

int graph_id;

const int DEV_NAME_SIZE = 100;
char hs_dev_name[DEV_NAME_SIZE];
void *dev_handle;
void *graph_handle[2];

typedef unsigned short half_float;

std::vector<std::string> categories;
std::vector<std::string> categories_age;
hsStatus hsStat;

//*******************************************************************************
typedef struct networkResults
{
    int gender;
    float genderConfidence;
    string ageCategory;
    float ageConfidence;
} networkResults;

bool preprocess_image(const cv::Mat &src_image_mat, cv::Mat &preprocessed_image_mat)
{

    double width_ratio = (double)NETWORK_IMAGE_WIDTH / (double)src_image_mat.cols;
    double height_ratio = (double)NETWORK_IMAGE_HEIGHT / (double)src_image_mat.rows;

    double largest_ratio = (width_ratio > height_ratio) ? width_ratio : height_ratio;

    cv::resize(src_image_mat, preprocessed_image_mat, cv::Size(), largest_ratio, largest_ratio, CV_INTER_AREA);

    int mid_row = preprocessed_image_mat.rows / 2.0;
    int mid_col = preprocessed_image_mat.cols / 2.0;
    int x_start = mid_col - (NETWORK_IMAGE_WIDTH / 2);
    int y_start = mid_row - (NETWORK_IMAGE_HEIGHT / 2);
    cv::Rect roi(x_start, y_start, NETWORK_IMAGE_WIDTH, NETWORK_IMAGE_HEIGHT);
    preprocessed_image_mat = preprocessed_image_mat(roi);
    return true;
}

bool read_stat_txt(double *network_mean, double *network_std, const string NETWORK_DIR)
{
    char filename[MAX_PATH];
    strncpy(filename, NETWORK_DIR.c_str(), MAX_PATH);
    strncat(filename, "stat.txt", MAX_PATH);
    FILE *stat_file = fopen(filename, "r");
    if (stat_file == nullptr)
    {
        return false;
    }
    int num_read_std = 0;
    int num_read_mean = 0;
    num_read_mean = fscanf(stat_file, "%lf%lf%lf\n", &(network_mean[0]), &(network_mean[1]), &(network_mean[2]));
    if (num_read_mean == 3)
    {
        num_read_std = fscanf(stat_file, "%lf%lf%lf", &(network_std[0]), &(network_std[1]), &(network_std[2]));
    }
    fclose(stat_file);

    if (num_read_mean != 3 || num_read_std != 3)
    {
        return false;
    }

    for (int i = 0; i < 3; i++)
    {
        network_mean[i] = 255.0 * network_mean[i];
        network_std[i] = 1.0 / (255.0 * network_std[i]);
    }

    return true;
}

bool read_cat_txt(std::vector<std::string> *categories, const string NETWORK_DIR,const string CAT_DIR)
{
    char filename[MAX_PATH];
    strncpy(filename, NETWORK_DIR.c_str(), MAX_PATH);
    strncat(filename, CAT_DIR.c_str(), MAX_PATH);
    FILE *cat_file = fopen(filename, "r");
    if (cat_file == nullptr)
    {
        return false;
    }

    char cat_line[100];
    fgets(cat_line, 100, cat_file); // skip the first line
    while (fgets(cat_line, 100, cat_file) != NULL)
    {
        if (cat_line[strlen(cat_line) - 1] == '\n')
            cat_line[strlen(cat_line) - 1] = '\0';
        categories->push_back(std::string(cat_line));
    }
    fclose(cat_file);

    if (categories->size() < 1)
    {
        return false;
    }

    return true;
}

bool read_graph_from_file(const char *graph_filename, unsigned int *length_read, void **graph_buf)
{
    FILE *graph_file_ptr;

    *graph_buf = nullptr;

    graph_file_ptr = fopen(graph_filename, "rb");
    if (graph_file_ptr == nullptr)
    {
        return false;
    }

    *length_read = 0;
    fseek(graph_file_ptr, 0, SEEK_END);
    *length_read = ftell(graph_file_ptr);
    rewind(graph_file_ptr);

    if (!(*graph_buf = malloc(*length_read)))
    {
        // couldn't allocate buffer
        fclose(graph_file_ptr);
        return false;
    }

    size_t to_read = *length_read;
    size_t read_count = fread(*graph_buf, 1, to_read, graph_file_ptr);

    if (read_count != *length_read)
    {
        fclose(graph_file_ptr);
        free(*graph_buf);
        *graph_buf = nullptr;
        return false;
    }
    fclose(graph_file_ptr);

    return true;
}

static float *result_data;
int sort_results(const void *index_a, const void *index_b)
{
    int *a = (int *)index_a;
    int *b = (int *)index_b;
    float diff = result_data[*b] - result_data[*a];
    if (diff < 0)
    {
        return -1;
    }
    else if (diff > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//*******************************************************************************
void initHS()
{
    hsStat = hsGetDeviceName(0, hs_dev_name, DEV_NAME_SIZE);
    if (hsStat != HS_OK)
    {
        if (hsStat == HS_DEVICE_NOT_FOUND)
        {
            cout << "Error - no device" << endl;
        }
        else
        {
            cout << "Error - hsGetDeviceName failed: " << hsStat << endl;
        }
        exit(1);
    }
    else
    {
        cout << "hs device "
             << " name: " << hs_dev_name << endl;
    }
    hsStat = hsOpenDevice(hs_dev_name, &dev_handle);

    if (hsStat != HS_OK)
    {
        cout << "Error - hsOpenDevice failed: " << hsStat << endl;
    }
    else
    {
        cout << "Successfully opened HS device" << hs_dev_name << endl;
    }
}
//*******************************************************************************

//*******************************************************************************
int initGenderNetwork()
{
    int gender_graph_id;

    if (!read_stat_txt(networkMean, networkStd, CAT_STAT_DIRECTORY))
    {
        cout << "Error - Failed to read stat.txt file for gender network." << endl;
        exit(1);
    }
    // Read the gender cat file
    if (!read_cat_txt(&categories, CAT_STAT_DIRECTORY,GENDER_CAT_DIR))
    {
        cout << "Error - Failed to read categories.txt file for gender network." << endl;
        exit(1);
    }

    char gender_graph_filename[MAX_PATH];
    strncpy(gender_graph_filename, GRAPH_DIR, MAX_PATH);
    strncat(gender_graph_filename, "gender_graph", MAX_PATH);
    unsigned int graph_len = 0;
    void *gender_graph_buf;
    if (!read_graph_from_file(gender_graph_filename, &graph_len, &gender_graph_buf))
    {
        // error reading graph
        cout << "Error - Could not read graph file from disk: " << gender_graph_filename << endl;
        hsCloseDevice(dev_handle);
        exit(1);
    }

    hsStat = hsAllocateGraph(dev_handle, &graph_handle[0], gender_graph_buf, graph_len);
    if (hsStat != HS_OK)
    {
        cout << "Error - hsAllocateGraph failed:" << hsStat << endl;
        exit(1);
    }
    else
    {
        cout << "Successfully Allocated Gender graph for HS device." << endl;
    }

    unsigned int graph_id_len = sizeof(gender_graph_id);

    hsStat = hsGetGraphOption(graph_handle[0], HS_GRAPH_ID, (void *)&gender_graph_id, &graph_id_len);

    if (hsStat != HS_OK)
    {
        cout << "Error: status" << (int)hsStat << endl;
    }
    else
    {
        cout << "gender_graph_id == " << gender_graph_id << endl;
    }

    return gender_graph_id;
}

//*******************************************************************************

int initAgeNetwork()
{
    int age_graph_id;

    if (!read_stat_txt(networkMean, networkStd, CAT_STAT_DIRECTORY))
    {
        cout << "Error - Failed to read stat.txt file for age network." << endl;
        exit(1);
    }

    if (!read_cat_txt(&categories_age, CAT_STAT_DIRECTORY,AGE_CAT_DIR))
    {
        cout << "Error - Failed to read categories_age.txt file for age network." << endl;
        exit(1);
    }

    char age_graph_filename[MAX_PATH];
    strncpy(age_graph_filename, GRAPH_DIR, MAX_PATH);
    strncat(age_graph_filename, "age_graph", MAX_PATH);
    unsigned int age_graph_len = 0;
    void *age_graph_buf;
    if (!read_graph_from_file(age_graph_filename, &age_graph_len, &age_graph_buf))
    {
        // error reading graph
        cout << "Error - Could not read graph file from disk: " << age_graph_filename << endl;
        hsCloseDevice(dev_handle);
        exit(1);
    }

    hsStat = hsAllocateGraph(dev_handle, &graph_handle[1], age_graph_buf, age_graph_len);
    if (hsStat != HS_OK)
    {
        cout << "Error - hsAllocateGraph failed: %d\n"
             << hsStat << endl;
        exit(1);
    }
    else
    {
        cout << "Successfully Allocated Age graph for HS device." << endl;
    }
    unsigned int graph_id_len = sizeof(graph_id);

    hsStat = hsGetGraphOption(graph_handle[1], HS_GRAPH_ID, (void *)&age_graph_id, &graph_id_len);

    if (hsStat != HS_OK)
    {
        cout << "Error: status" << (int)hsStat << endl;
    }
    else
    {
        cout << "age_graph_id == " << age_graph_id << endl;
    }
    return age_graph_id;
}
//*******************************************************************************

bool loadTensor(cv::Mat inputMat, void *graphHandle, int graph_id)
{
    cv::Mat preprocessed_image_mat;
    preprocess_image(inputMat, preprocessed_image_mat);
    if (preprocessed_image_mat.rows != NETWORK_IMAGE_HEIGHT ||
        preprocessed_image_mat.cols != NETWORK_IMAGE_WIDTH)
    {
        cout << "Error - preprocessed image is unexpected size!" << endl;
        return false;
    }

    float_t tensor32[3];
    half_float tensor16[NETWORK_IMAGE_WIDTH * NETWORK_IMAGE_HEIGHT * 3];

    uint8_t *image_data_ptr = (uint8_t *)preprocessed_image_mat.data;
    int chan = preprocessed_image_mat.channels();

    int tensor_index = 0;
    for (int row = 0; row < preprocessed_image_mat.rows; row++)
    {
        for (int col = 0; col < preprocessed_image_mat.cols; col++)
        {

            int pixel_start_index = row * (preprocessed_image_mat.cols + 0) * chan + col * chan; // TODO: don't hard code

            uint8_t blue = image_data_ptr[pixel_start_index + 0];
            uint8_t green = image_data_ptr[pixel_start_index + 1];
            uint8_t red = image_data_ptr[pixel_start_index + 2];

            tensor32[0] = (((float_t)blue - networkMean[0]) * networkStd[0]);
            tensor32[1] = (((float_t)green - networkMean[1]) * networkStd[1]);
            tensor32[2] = (((float_t)red - networkMean[2]) * networkStd[2]);

            tensor16[tensor_index++] = float2half(*((unsigned *)(&(tensor32[0]))));
            tensor16[tensor_index++] = float2half(*((unsigned *)(&(tensor32[1]))));
            tensor16[tensor_index++] = float2half(*((unsigned *)(&(tensor32[2]))));
        }
    }

    hsStatus hsStatusInfo = hsLoadTensor(graphHandle, tensor16, graph_id, NETWORK_IMAGE_HEIGHT * NETWORK_IMAGE_WIDTH * 3 * sizeof(half_float), nullptr);
    if (hsStatusInfo != HS_OK)
    {
        cout << "Error! - LoadTensor failed: " << hsStatusInfo << endl;
        return false;
    }
    return true;
}

// ******************************************************************************

cv::Mat getImage(void *graphHandle, bool truthy, int graph_id)
{
    uint8_t *image_data;
    unsigned int length;
    cv::Mat mat;
    void *user_param;
    int ret = hsGetImage(graphHandle, reinterpret_cast<void **>(&image_data),
                         graph_id, user_param, networkStd[0], networkMean[0], truthy);

    if (ret != HS_OK)
    {
        cout << "hsGetImage return error == " << ret << endl;
    }

    if (truthy)
    {
        length = 360 * 640 * 3;
    }
    else
    {
        length = 1920 * 1080 * 3;
    }
    std::vector<uint8_t> result_vector(reinterpret_cast<uint8_t *>(image_data),
                                       reinterpret_cast<uint8_t *>(image_data) + length);
    if (truthy)
    {
        cv::Mat A(1, length, CV_8UC1, image_data);
        mat = A.reshape(3, 360);
    }
    else
    {
        cv::Mat R(1, 1920 * 1080, CV_8UC1, image_data);
        cv::Mat G(1, 1920 * 1080, CV_8UC1, image_data + 1920 * 1080);
        cv::Mat B(1, 1920 * 1080, CV_8UC1, image_data + 1920 * 1080 * 2);
        std::vector<cv::Mat> channels;
        channels.push_back(B);
        channels.push_back(G);
        channels.push_back(R);
        cv::Mat bgr;
        cv::merge(channels, bgr);
        mat = bgr.reshape(3, 1080);
    }
    return mat;
}
// ******************************************************************************

networkResults getInferenceResults(cv::Mat inputMat, std::vector<std::string> networkCategories, hsStatus hss, void *graphHandle)
{
    void *result_buf;
    unsigned int res_length;
    void *user_data;
    hss = hsGetResult(graphHandle, &result_buf, &res_length, &user_data);

    if (hss != HS_OK)
    {
        cout << "Error! - GetResult failed: " << hss << endl;
        networkResults error = {-1, -1, "-1", -1};
        return error;
    }

    res_length /= sizeof(unsigned short);

    float result_fp32[8];

    fp16tofloat(result_fp32, (unsigned char *)result_buf, res_length);

    int indexes[res_length];
    for (unsigned int i = 0; i < res_length; i++)
    {
        indexes[i] = i;
    }
    result_data = result_fp32;

    networkResults personInferenceResults;

    if (strcmp(networkCategories[indexes[0]].c_str(), "Male") == 0)
    {
        personInferenceResults.gender = indexes[0];
        personInferenceResults.genderConfidence = result_fp32[indexes[0]];
    }
    if (strcmp(networkCategories[indexes[0]].c_str(), "0-2") == 0)
    {
        qsort(indexes, res_length, sizeof(*indexes), sort_results);
        personInferenceResults.ageCategory = networkCategories[indexes[0]].c_str();
        personInferenceResults.ageConfidence = result_fp32[indexes[0]];
    }

    return personInferenceResults;
}

//*******************************************************************************
int main(int argc, char **argv)
{
    Mat imgIn;
    VideoCapture capture;
    Mat croppedFaceMat;
    Scalar textColor = BLACK;
    Point topLeftRect[5];
    Point bottomRightRect[5];
    Point winTextOrigin;
    CascadeClassifier faceCascade;

    vector<Rect> faces;
    String genderText;
    String ageText;
    String rectangle_text;
    clock_t start_time, elapsed_time;
    bool start_inference_timer = true;
    int key;
    networkResults currentInferenceResult;
    networkResults currentInferenceResult_age;

    namedWindow(WINDOW_NAME, WINDOW_NORMAL);
    resizeWindow(WINDOW_NAME, WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowProperty(WINDOW_NAME, CV_WND_PROP_ASPECTRATIO, CV_WINDOW_KEEPRATIO);

    moveWindow(WINDOW_NAME, 0, 0);

    winTextOrigin.x = 0;
    winTextOrigin.y = 20;

    faceCascade.load(XML_FILE);

    initHS();
    int gender_graph_id = initGenderNetwork();
    int age_graph_id = initAgeNetwork();

    while (true)
    {

        imgIn = getImage(graph_handle[0], true, gender_graph_id);

        flip(imgIn, imgIn, 1);

        key = waitKey(1);

        if (key == 27)
            break;

        faceCascade.detectMultiScale(imgIn, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

        if (start_inference_timer)
        {
            start_time = clock();
            start_inference_timer = false;
        }

        for (int i = 0; i < faces.size(); i++)
        {

            topLeftRect[i].x = faces[i].x - PADDING;
            topLeftRect[i].y = faces[i].y - PADDING;
            bottomRightRect[i].x = faces[i].x + faces[i].width + PADDING;
            bottomRightRect[i].y = faces[i].y + faces[i].height + PADDING;

            if (topLeftRect[i].x > 0 && topLeftRect[i].y > 0 && bottomRightRect[i].x < WINDOW_WIDTH && bottomRightRect[i].y < WINDOW_HEIGHT)
            {
                // draw a rectangle around the detected face
                rectangle(imgIn, topLeftRect[i], bottomRightRect[i], textColor, 2, 8, 0);

                elapsed_time = clock() - start_time;

                Rect croppedFaceRect(topLeftRect[i], bottomRightRect[i]);

                croppedFaceMat = imgIn(croppedFaceRect);

                currentInferenceResult = getInferenceResults(croppedFaceMat, categories, hsStat, graph_handle[0]);
                if (currentInferenceResult.genderConfidence >= MALE_GENDER_THRESHOLD)
                {
                    genderText = categories.front().c_str();
                    textColor = BLUE;
                }
                else if (currentInferenceResult.genderConfidence <= FEMALE_GENDER_THRESHOLD)
                {
                    genderText = categories.back().c_str();
                    textColor = PINK;
                }
                else
                {
                    genderText = "Unknown";
                    textColor = BLACK;
                }

                loadTensor(croppedFaceMat, graph_handle[1], age_graph_id);
                currentInferenceResult_age = getInferenceResults(croppedFaceMat, categories_age, hsStat, graph_handle[1]);

                ageText = currentInferenceResult_age.ageCategory;

                start_inference_timer = true;
                rectangle_text = genderText + " " + ageText;
                putText(imgIn, rectangle_text, topLeftRect[i], FONT, 3, textColor, 3);
            }
        }
        putText(imgIn, "Press ESC to exit", winTextOrigin, FONT, 2, GREEN, 2);
        imshow(WINDOW_NAME, imgIn);
    }

    destroyAllWindows();

    hsDeallocateGraph(graph_handle[0]);
    hsDeallocateGraph(graph_handle[1]);
    hsCloseDevice(dev_handle);
    return 0;
}
