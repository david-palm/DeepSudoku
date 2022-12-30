package com.example.deepsudoku

import android.content.ContentResolver
import android.content.Context
import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.ImageDecoder
import android.net.Uri
import android.os.Bundle
import android.provider.MediaStore
import android.provider.MediaStore.Images.Media.getBitmap
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.ContextCompat
import androidx.core.net.toFile
import androidx.navigation.Navigation
import com.example.deepsudoku.databinding.FragmentImageViewBinding
import java.io.File
import java.net.URI


class ImageViewFragment : Fragment() {
    private var _viewBinding: FragmentImageViewBinding? = null
    private val viewBinding: FragmentImageViewBinding get() = _viewBinding!!
    private var imageUri: Uri? = null
    lateinit var assetManager : AssetManager
    lateinit var image: Bitmap


    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _viewBinding = FragmentImageViewBinding.inflate(inflater, container, false)

        //Displaying image
        imageUri = arguments?.getParcelable<Uri>("ImageUri")
        viewBinding.imageView.setImageURI(imageUri)

        assetManager = resources.assets

        //Set button listeners
        viewBinding.imageAcceptButton.setOnClickListener { solveSudoku() }
        viewBinding.imageDeleteButton.setOnClickListener { deleteImage() }

        identifySudoku();

        return viewBinding.root
    }

    fun identifySudoku() {
        //Loading image
        image = ImageDecoder.decodeBitmap(ImageDecoder.createSource(requireContext().contentResolver, imageUri!!))
        //Making mutable copies of input image in order to pass them to native code
        image = image.copy(Bitmap.Config.ARGB_8888, true)
        var output: Bitmap = image.copy(image.config, true)
        //Process image by calling native code
        identifySudoku(image, output)
        viewBinding.imageView.setImageBitmap(output)
    }

    private fun solveSudoku(){
        var output: Bitmap = image.copy(image.config, true)
        //Process image by calling native code
        solveSudoku(assetManager, image, output)
        //Process image by calling native code
        //solveSudoku(assetManager, image, output)
        viewBinding.imageView.setImageBitmap(output)
    }

    private fun deleteImage(){
        requireContext().contentResolver.delete(imageUri!!, null, null)
        Navigation.findNavController(requireView()).navigate(
            R.id.action_imageViewFragment_to_imageCaptureFragment, Bundle())
    }

    external fun identifySudoku(inputImage: Bitmap, outputImage: Bitmap)
    external fun solveSudoku(assetManager : AssetManager, inputImage: Bitmap, outputImage: Bitmap)

    companion object {

    }
}