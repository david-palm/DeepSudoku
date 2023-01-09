package com.example.deepsudoku

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.ImageDecoder
import android.net.Uri
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.Navigation
import com.example.deepsudoku.databinding.FragmentImageViewBinding
import kotlin.time.ExperimentalTime


class ImageViewFragment : Fragment() {
    private var _viewBinding: FragmentImageViewBinding? = null
    private val viewBinding: FragmentImageViewBinding get() = _viewBinding!!
    private var imageUri: Uri? = null
    lateinit var assetManager : AssetManager
    lateinit var image: Bitmap
    var imageProcessorPointer: Long = 0


    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        _viewBinding = FragmentImageViewBinding.inflate(inflater, container, false)
        assetManager = resources.assets

        //Displaying image
        imageUri = arguments?.getParcelable<Uri>("ImageUri")
        viewBinding.imageView.setImageURI(imageUri)

        //Set button listeners
        viewBinding.imageAcceptButton.setOnClickListener { solveSudoku() }
        viewBinding.imageDeleteButton.setOnClickListener { deleteImage() }
        (activity as AppCompatActivity?)!!.supportActionBar!!.hide()
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
        imageProcessorPointer = identifySudoku(image, output)
        viewBinding.imageView.setImageBitmap(output)
    }

    @OptIn(ExperimentalTime::class)
    private fun solveSudoku(){
        var sudoku: IntArray = IntArray(81);
        var solvedSudoku: IntArray = IntArray(81);

        //Process image by calling native code
        val time = kotlin.time.measureTimedValue { solveSudoku(aiModelPointer, imageProcessorPointer, sudoku, solvedSudoku) }


        Log.d("Timer", "Solving sudoku took: " + time.toString())
        val bundle = Bundle()
        bundle.putIntegerArrayList("Sudoku", sudoku.toCollection(ArrayList()))
        bundle.putIntegerArrayList("SolvedSudoku", solvedSudoku.toCollection(ArrayList()))
        Navigation.findNavController(requireView()).navigate(R.id.action_imageViewFragment_to_solutionViewFragment, bundle)
    }

    private fun deleteImage(){
        requireContext().contentResolver.delete(imageUri!!, null, null)
        Navigation.findNavController(requireView()).navigate(R.id.action_imageViewFragment_to_imageCaptureFragment, Bundle())
    }

    external fun identifySudoku(inputImage: Bitmap, outputImage: Bitmap) : Long
    external fun solveSudoku(aiModelPointer : Long, imageProcessorPointer: Long, sudoku: IntArray, solvedSudoku: IntArray)
}