<p align="center">
  <img src="https://user-images.githubusercontent.com/109848051/211549188-97909c12-d9c4-47bd-b7d6-0cb32d31131a.png" width=40%>
</p>

#### DeepSudoku is an Android application that allows the user to solve a Sudoku by taking a picture of it with their smartphone camera. It is based on a group project for the course _image processing_ at the _University of Applied Sciences Cologne_. The group project was written in _Python_ as a _Jupyter Notebook_ using the _Google Colab_ platform. I rewrote the project to object-oriented _C++_-code and optimized so that it runs as a stand-alone _Android_ app.

# Overview
# How it works
![DeepSudoku](https://user-images.githubusercontent.com/109848051/212676978-a31c9174-85ba-489e-bf2c-93a72d41b3a9.gif)

### Identifying and warping the sudoku
In order to process the image all irrelevant information needs to be removed from it. To achieve this the contour of the sudoku is found by looking for contour that can be approximated by 4 points and has a certain size. Then a padding is added to the contour as a safety measure to avoid cutting of numbers and lines at the sides of the image. The corner points of the padded contour are then used to perform a four-point transform and warp the image to the screen.
### Identifying lines
### Finding intersections
### Cutting cells
### Removing artifacts
### Classifying digits

