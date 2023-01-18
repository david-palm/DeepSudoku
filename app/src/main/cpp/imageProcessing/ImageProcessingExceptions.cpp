#include "ImageProcessingExceptions.h"

char* ContourNotFoundException::what() { return message; }
char* ImageNotWarpedException::what() { return message; }
char* LinesNotFoundException::what() { return message; }
char* IntersectionsNotFoundException::what() { return message; }
char* CellsNotCutException::what() { return message; }
char* DigitsNotExtractedException::what() { return message; }