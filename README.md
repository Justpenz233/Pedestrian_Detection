# Pedestrian-Detection
Pedestrian-Detection using C++ openCV

Consider using two ideas:

1. MOG for Detecting and CamShift/MeanShift for Tracking(Movement detection ways)
2. HOG and SVM (Machine learning ways)

## Movement detection ways

### STEP 1: Background Subtraction

In this section, we mainly implement background subtractor - Mixture Of Gaussian(MOG).  

After the MOG works, we may get an image showing below:  
![MOG](https://docs.opencv.org/3.4/Background_Subtraction_Tutorial_result_MOG2.jpg) 

As you can see, the MOG algorithm can help us handle the shadow of moving object(labeled as gray) and changing lighten environment.  


### STEP 2: Find Contours


At this section, we do implement Gaussian blur and morphological erosion to eliminate the noisy.  
Handle the shadow according to your purpose.  
At the demo, it has been exchanging to background by using the threshold method.

Find the contours, and use the marked restrained area of contour to determine whether it is humankind.

### STEP 3: Match contours

TODO

### STEP 4: Display

TODO

### Summary

Poor accuracy and stability.
I have to do add so many tolerance parameters to promote the reliability of the module.