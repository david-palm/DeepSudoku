#pragma once

#include <iostream>

class ImageProcessingException : public std::exception {
protected:
    char * message;
    int type;
    ImageProcessingException(char * msg, int type) : message(msg), type(type) {}
    virtual char* what () = 0;
public:
    int getType() { return type; }
};

class ContourNotFoundException : public ImageProcessingException {
private:
    int numberOfContourPoints;
    int area;
    int circumference;

public:
    ContourNotFoundException(char * msg) : ImageProcessingException(msg, 1) {}
    char* what ();
};

class ImageNotWarpedException : public ImageProcessingException {

public:
    ImageNotWarpedException(char * msg) : ImageProcessingException(msg, 2) {}
    char* what ();
};

class LinesNotFoundException : public ImageProcessingException {
private:
    int numberOfLines;

public:
    LinesNotFoundException(char * msg) : ImageProcessingException(msg, 3) {}
    char* what ();
};

class IntersectionsNotFoundException : public ImageProcessingException {
private:
    int numberOfIntersections;

public:
    IntersectionsNotFoundException(char * msg) : ImageProcessingException(msg, 4) {}
    char* what ();
};

class CellsNotCutException : public ImageProcessingException {
private:
    char * message;

public:
    CellsNotCutException(char * msg) : ImageProcessingException(msg, 5) {}
    char* what ();
};

class DigitsNotExtractedException : public ImageProcessingException {
private:
    char * message;

public:
    DigitsNotExtractedException(char * msg) : ImageProcessingException(msg, 7) {}
    char* what ();
};