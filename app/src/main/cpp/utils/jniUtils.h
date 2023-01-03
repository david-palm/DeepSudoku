#pragma once

#include <jni.h>

namespace jniUtils
{
    void ArrayToJintArray(JNIEnv *env, int (&input)[9][9], jintArray &output);
}