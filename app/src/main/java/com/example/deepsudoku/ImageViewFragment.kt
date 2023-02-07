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
import com.google.android.material.snackbar.Snackbar
import kotlin.time.ExperimentalTime


class ImageViewFragment : Fragment() {
    private var _viewBinding: FragmentImageViewBinding? = null
    private val viewBinding: FragmentImageViewBinding get() = _viewBinding!!
    lateinit var assetManager : AssetManager
    lateinit var previewImage: Bitmap


    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?): View? {
        _viewBinding = FragmentImageViewBinding.inflate(inflater, container, false)
        assetManager = resources.assets

        //Displaying image
        previewImage = arguments?.getParcelable<Bitmap>("PreviewImage")!!
        viewBinding.imageView.setImageBitmap(previewImage)

        //Set button listeners
        viewBinding.solveSudoku.setOnClickListener { solveSudoku() }
        viewBinding.retakeImage.setOnClickListener { deleteImage() }
        (activity as AppCompatActivity?)!!.supportActionBar!!.hide()



        return viewBinding.root
    }


    private fun solveSudoku(){
        try {
            viewBinding.imageView.visibility = View.GONE
            viewBinding.solveSudoku.visibility = View.INVISIBLE
            viewBinding.retakeImage.visibility = View.GONE
            var sudoku: IntArray = IntArray(81);
            var solvedSudoku: IntArray = IntArray(81);

            //Process image by calling native code
            solveSudoku(aiModelPointer, imageProcessorPointer, sudoku, solvedSudoku)

            val bundle = Bundle()
            bundle.putIntegerArrayList("Sudoku", sudoku.toCollection(ArrayList()))
            bundle.putIntegerArrayList("SolvedSudoku", solvedSudoku.toCollection(ArrayList()))
            Navigation.findNavController(requireView()).navigate(R.id.action_imageViewFragment_to_solutionViewFragment, bundle)
        } catch (exception: Exception) {
            Log.e("SudokuSolver", "Sudoku could not be solved!")
            Snackbar.make(viewBinding.root, "Sudoku could not be solved!", 2500).show()
        }
    }

    private fun deleteImage(){
        Navigation.findNavController(requireView()).navigate(R.id.action_imageViewFragment_to_imageCaptureFragment, Bundle())
    }

    external fun solveSudoku(aiModelPointer : Long, imageProcessorPointer: Long, sudoku: IntArray, solvedSudoku: IntArray)
}