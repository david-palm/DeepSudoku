# DeepSudoku - Image-based sudoku solver
<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/211549188-97909c12-d9c4-47bd-b7d6-0cb32d31131a.png" width=30%>
</p>

#### DeepSudoku is an Android application that allows the user to solve a Sudoku by taking a picture of it with their smartphone camera. It is based on a group project for the course _image processing_ at the _University of Applied Sciences Cologne_. The group project was written in _Python_ as a _Jupyter Notebook_ using the _Google Colab_ platform and _OpenCV_. I rewrote the project to object-oriented _C++_-code and optimized it so that it runs as a stand-alone _Android_ app.

<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/212972617-bf64bc62-d40f-44b4-ad00-ee966f0fa26d.png" width=70%>
</p>

#### Download the current APK [here](https://github.com/david-palm/DeepSudoku/releases/download/unstable/deepsudoku_unstable_v0.1.apk)!
Known issues:
- app crashes when the image cannot be processed because native C++ code does not throw exceptions
- app needs to be restarted after giving camera permission on most devices

# How it works!


<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/212706009-c62fad85-c83f-4ccf-ad80-06d3ba630cfc.gif" width=40%>
</p>

### Identifying and warping the sudoku
In order to process the image all irrelevant information needs to be removed from it. To achieve this, a contour that can be approximated by 4 points and has a certain size is searched for. Then padding is added to the contour as a safety measure to avoid cutting off numbers and lines at the sides of the image. The corner points of the padded contour are then used to perform a four-point transform and warp the image to the screen.

### Identifying lines using custom Hough transform algorithm
In order to extract the digits from the image, the lines making up the sudoku grid need to be identified first. A *Hough transform* algorithm written from scratch and optimized for sudoku images is used for line identification in the warped image. But before the algorithm can identify the lines the warped image needs to be converted into two gradient images: one in x and one in y direction. A gradient image displays the amount a gray value change in a certain direction. Mathematically it is the first derivative of the image. The two gradient images are created by filtering the image using a *Sobel operator* as a mask. The image needs to be converted to grayscale and blurred using a *Gauß* filter first to minimize outlier values caused by noise.

<p align="center">
    <img src="https://user-images.githubusercontent.com/109848051/212722504-16a94180-62b5-45c2-aaf1-a8e8ba735d05.png" width=80%>
    <br/> 
      <em>Warped image on the left and gradient images in x and y direction in the center and on the right</em>
    </br>
</p>

The *Hough transform* algorithm is able to calculate the angle, distance, and magnitude of every pixel in the image with the information of the two gradient images. Every pixel's line equation as *Hesse normal form* can be constructed with the angle and distance. A 2D table called the *Hough accumulator* is created to store the angle and distance of every pixel. The x-axis represents all angles from **0** to **2 pi** and the y-axis the distance to the upper left corner in pixels. The resolution/accuracy of the *Hough accumulator* can be set by changing the number of cells in the x or y direction. The *Hough transform* algorithm iterates over all pixels, calculates the pixel's angle, distance, and magnitude and increments the associated value in the *Hough accumulator*. It is possible to set a range or threshold for these values to filter outliers or other undesired values. Because it is a two-dimensional array the *Hough accumulator* can be treated like an image. I binarized the *Hough accumulator* and used *ternary operators* to remove outliers to further improve the recognition of lines. To get the lines' equations the center of the resulting "white islands" is calculated. The coordinate of the center represents the angle and distance of the line. With it the *Hesse normal form* of the line can be constructed.
### Finding intersections cutting cells
With the equations of the lines, the intersections can be easily calculated using simple Algebra. The coordinates of the intersections are then stored in a two-dimensional array and used to cut the image into smaller images, each containing a single cell.
### Removing artifacts
After cutting the image into cells remains of lines and other interferences may still be present in the image. This could prevent the neural network to classify the digits correctly. To avoid this, we look at all contours in the image and remove those that have unfitting dimensions e.g. are too small, thin or long to be the contour of a digit. Subsequently, the image is resized to 28 by 28 pixels so that it can passed to the neural network.

<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/212741770-a182df8c-e770-474e-827f-484089b9d7fd.png" width=10%>
  <img src="https://user-images.githubusercontent.com/109848051/212741816-2ac00607-8760-4da4-b676-0175d8d00ea7.png" width=10%>
  <br/> 
      <em>Cell with artifacts on the left and without on the right</em>
    </br>
</p>

### Classifying digits with the neural network
The neural network was originally build using *TensorFlow* and *Keras* containing *Dense* layers, 2D convolution layers, max-pooling layers, and dropout layers. The network was converted using [frugally-deep](https://github.com/Dobiasd/frugally-deep) to integrate it into the native C++ code. All cells are fed to the model as a tensor. A vector with 9 floats is returned, each representing the probability of the digit. If the value is over 0.7 the digit is viewed as recognized otherwise it is viewed as an empty cell.
