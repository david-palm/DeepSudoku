package com.example.deepsudoku

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.net.Uri
import androidx.fragment.app.Fragment
import com.example.deepsudoku.databinding.FragmentImageViewBinding

class SolutionViewFragment : Fragment() {
    private var _viewBinding: FragmentImageViewBinding? = null
    private val viewBinding: FragmentImageViewBinding get() = _viewBinding!!
    private var imageUri: Uri? = null
    lateinit var assetManager : AssetManager
    lateinit var image: Bitmap
}