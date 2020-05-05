/* INCLUDES FOR THIS PROJECT */
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <limits>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/nonfree.hpp>

#include "dataStructures.h"
#include "matching2D.hpp"

using namespace std;



/* MAIN PROGRAM */
int main(int argc, const char *argv[])
{

    /* INIT VARIABLES AND DATA STRUCTURES */

    // data location
    string dataPath = "../";

    // camera
    string imgBasePath = dataPath + "images/";
    string imgPrefix = "KITTI/2011_09_26/image_00/data/000000"; // left camera, color
    string imgFileType = ".png";
    int imgStartIndex = 0; // first file index to load (assumes Lidar and camera names have identical naming convention)
    int imgEndIndex = 9;   // last file index to load
    int imgFillWidth = 4;  // no. of digits which make up the file index (e.g. img-0001.png)

    //******************* Changing the number of images charged in memory *******************//
    int dataBufferSize =3;       // no. of images which are held in memory (ring buffer) at the same time
    vector<DataFrame> dataBuffer; // list of data frames which are held in memory at the same time
    
    //******************* OPTIONAL: To visualise the changes *******************//
    bool bVis = true;            // visualize results

    /* MAIN LOOP OVER ALL IMAGES */

    for (size_t imgIndex = 0; imgIndex <= imgEndIndex - imgStartIndex; imgIndex++)
    {
        /* LOAD IMAGE INTO BUFFER */

        // assemble filenames for current index
        ostringstream imgNumber;
        imgNumber << setfill('0') << setw(imgFillWidth) << imgStartIndex + imgIndex;
        string imgFullFilename = imgBasePath + imgPrefix + imgNumber.str() + imgFileType;

        // load image from file and convert to grayscale
        cv::Mat img, imgGray;
        img = cv::imread(imgFullFilename);
        cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

        //// STUDENT ASSIGNMENT

        // push image into data frame buffer
        //******************* Impose that the databuffer is lesser than three *******************//
        //******************* To avoid the memory collapse *******************//
        DataFrame frame;
        frame.cameraImg = imgGray;

        if  ( dataBuffer.size() >= dataBufferSize ) dataBuffer.erase ( dataBuffer.begin() );
        dataBuffer.push_back ( frame );

        //// EOF STUDENT ASSIGNMENT
        cout << "#1 : LOAD IMAGE INTO BUFFER done" << endl;

        /* DETECT IMAGE KEYPOINTS */

        // extract 2D keypoints from current image
        vector<cv::KeyPoint> keypoints; // create empty feature list for current image
        string detectorType = "SIFT";


        //// Addition of the following keypoint detectors in file matching2D.cpp and 
        //// enable string-based selection based on detectorType
        //// -> HARRIS, FAST, BRISK, ORB, AKAZE, SIFT

        //******************* Calling the alternative detectors *******************//
        if (detectorType.compare ("SHITOMASI") == 0) detKeypointsShiTomasi ( keypoints , imgGray , false );
        else if (detectorType.compare ("HARRIS") == 0) detKeypointsHarris ( keypoints , imgGray , false );
        else detKeypointsModern ( keypoints , imgGray , detectorType , false );
        
        //// Keeping only keypoints on the preceding vehicle

        // only keep keypoints on the preceding vehicle

        bool bFocusOnVehicle = true;

        //******************* Establishment of the keypoints  threshold  *******************//
        cv::Rect vehicleRect(535, 180, 180, 150);

        //******************* Array to store the points within the rectangle  *******************//
        vector<cv::KeyPoint> keypointsRect;

        if (bFocusOnVehicle)
        {
        //*********** Loop to verify whether the keypoint belong to the rectangle  ***********//
        //********************* If so, add it to the array  *********************//
            int suma = 0;
            for( auto iterador = keypoints.begin() ; iterador != keypoints.end() ; iterador++)
            {
                if ( vehicleRect.contains (iterador->pt) )
                {  
                    cv::KeyPoint newKeypoint;
                    newKeypoint.pt = cv::Point2f (iterador->pt) ;
                    newKeypoint.size = 1;
                    keypointsRect.push_back (newKeypoint) ;
                }
            }
            float aux = keypoints.size();
            cout << "There were " << aux << " total keypoints within the image" << endl;
            keypoints =  keypointsRect;

            cout << "There are " << keypoints.size()<<" keypoints ("<< ( aux / keypoints.size() ) <<"%) on the preceding vehicle "<< endl;
        
        }

        // optional : limit number of keypoints (helpful for debugging and learning)
        bool bLimitKpts = false;
        if (bLimitKpts)
        {
            int maxKeypoints = 25;

            if (detectorType.compare("FREAK") == 0)
            { // there is no response info, so keep the first 50 as they are sorted in descending quality order
                keypoints.erase(keypoints.begin() + maxKeypoints, keypoints.end());
            }
            cv::KeyPointsFilter::retainBest(keypoints, maxKeypoints);
            cout << " NOTE: Keypoints have been limited!" << endl;
        }

        // push keypoints and descriptor for current frame to end of data buffer
        (dataBuffer.end() - 1)->keypoints = keypoints;
        cout << "#2 : DETECT KEYPOINTS done" << endl;

        /* EXTRACT KEYPOINT DESCRIPTORS */

        //// Addition of the following descriptors in file matching2D.cpp and enable string-based selection based on descriptorType
        //// -> BRIEF, ORB, FREAK, AKAZE, SIFT

        cv::Mat descriptors;
        string descriptorType = "FREAK"; // Other options: BRIEF, ORB, FREAK, AKAZE, SIFT        

        //*********** This function is described at matching2D_Student.cpp***********//
        descKeypoints((dataBuffer.end() - 1)->keypoints, (dataBuffer.end() - 1)->cameraImg, descriptors, descriptorType);

        // push descriptors for current frame to end of data buffer
        (dataBuffer.end() - 1)->descriptors = descriptors;

        cout << "#3 : EXTRACT DESCRIPTORS done" << endl;

        if (dataBuffer.size() > 1) // wait until at least two images have been processed
        {

            /* MATCH KEYPOINT DESCRIPTORS */

            vector<cv::DMatch> matches;
            string matcherType = "MAT_BF";        // MAT_BF, MAT_FLANN
            string descriptorType = "DES_BINARY"; // DES_BINARY, DES_HOG
            string selectorType = "SEL_KNN";       // SEL_NN, SEL_KNN

            matchDescriptors((dataBuffer.end() - 2)->keypoints, (dataBuffer.end() - 1)->keypoints,
                             (dataBuffer.end() - 2)->descriptors, (dataBuffer.end() - 1)->descriptors,
                             matches, descriptorType, matcherType, selectorType);


            // store matches in current data frame
            (dataBuffer.end() - 1)->kptMatches = matches;

            cout << "#4 : MATCH KEYPOINT DESCRIPTORS done" << endl;

            // visualize matches between current and previous image
            bVis = true;
            if (bVis)
            {
                cv::Mat matchImg = ((dataBuffer.end() - 1)->cameraImg).clone();
                cv::drawMatches((dataBuffer.end() - 2)->cameraImg, (dataBuffer.end() - 2)->keypoints,
                                (dataBuffer.end() - 1)->cameraImg, (dataBuffer.end() - 1)->keypoints,
                                matches, matchImg,
                                cv::Scalar::all(-1), cv::Scalar::all(-1),
                                vector<char>(), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

                string windowName = "Matching keypoints between two camera images";
                cv::namedWindow(windowName, 7);
                cv::imshow(windowName, matchImg);
                cout << "Press key to continue to next image" << endl;
                cv::waitKey(0); // wait for key to be pressed
            }
            bVis = false;
        }

    } // eof loop over all images

    return 0;
}

