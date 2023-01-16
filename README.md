<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/211549188-97909c12-d9c4-47bd-b7d6-0cb32d31131a.png" width=40%>
</p>

#### DeepSudoku is an Android application that allows the user to solve a Sudoku by taking a picture of it with their smartphone camera. It is based on a group project for the course _image processing_ at the _University of Applied Sciences Cologne_. The group project was written in _Python_ as a _Jupyter Notebook_ using the _Google Colab_ platform. I rewrote the project to object-oriented _C++_-code and optimized it so that it runs as a stand-alone _Android_ app.


# How it works

<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/212706009-c62fad85-c83f-4ccf-ad80-06d3ba630cfc.gif" width=40%>
</p>

### Identifying and warping the sudoku
In order to process the image all irrelevant information needs to be removed from it. To achieve this the contour of the sudoku is found by looking for contour that can be approximated by 4 points and has a certain size. Then a padding is added to the contour as a safety measure to avoid cutting of numbers and lines at the sides of the image. The corner points of the padded contour are then used to perform a four-point transform and warp the image to the screen.
### Identifying lines using custom Hough transform algorithm
In order to extract the digits from the image, the lines making up the sudoku grid need to be identified first. A *Hough transform* algorithm written from scratch and optimized for sudoku images is used for line identification in the warped image. But before the algorithm can identify the lines the warped image needs to be converted into two gradient images: one in x and one in y direction. A gradient image displays the amount a gray value changes in a certain direction. Mathematically it is the first derivative of the image. By filtering the image using a *Sobel operator* as a Mask we create the two gradient images we need for our *Hough transform* algorithm. The image needs to be converted to grayscale and blurred using a *Gauß* filter first to minimize outlier values caused by noise.

<p align="center">
    <img src="https://user-images.githubusercontent.com/109848051/212722504-16a94180-62b5-45c2-aaf1-a8e8ba735d05.png" width=80%>
    <br/> 
      <em>Warped image on the left and gradient images in x and y direction in the center and on the right</em>
    </br>
</p>

The *Hough transform* algorithm is able to calculate the angle, distance and magnitude of every pixel in the image with the information of the two gradient images. The line equation as *Hesse normal form* of every pixel can be constructed with the angle and distance of a pixel. To find all lines a 2D table called the *Hough accumulator* is created. The x axis represents all angles from 0 to 2 pi and the y axis the distance to the upper left corner in pixels. By changing the number of cells in x or y direction the resolution/accuracy of the *Hough accumulator* can be set. The *Hough transform* algorithm iterates over all pixels, calculates angle, distance and magnitude of the pixel and increments the associated value in the *Hough accumulator*. It is possible to set a range or threshold for the angle, distance and magnitude values to filter the values.
### Finding intersections
### Cutting cells
### Removing artifacts
### Classifying digits

