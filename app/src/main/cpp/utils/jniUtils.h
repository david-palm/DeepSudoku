#pragma once

#include <jni.h>
#include <array>
namespace jniUtils
{
    void ArrayToJintArray(JNIEnv *env, std::array<std::array<int, 9>, 9> &input, jintArray &output);
}