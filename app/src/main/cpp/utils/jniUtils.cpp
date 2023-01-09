#include "jniUtils.h"

void jniUtils::ArrayToJintArray(JNIEnv* env, std::array<std::array<int, 9>, 9> &input, jintArray &output)
{
    for(int row = 0; row < 9; row++)
    {
        for(int col = 0; col < 9; col++)
        {
            jint elements[] = { input[row][col] };
            env->SetIntArrayRegion(output, row * 9 + col, 1,
                                   elements);
        }
    }
}