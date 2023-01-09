package com.example.deepsudoku

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.graphics.Typeface
import android.net.Uri
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import com.example.deepsudoku.databinding.FragmentSolutionViewBinding
import java.lang.reflect.Type

class SolutionViewFragment : Fragment() {
    private var _viewBinding: FragmentSolutionViewBinding? = null
    private val viewBinding: FragmentSolutionViewBinding get() = _viewBinding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _viewBinding = FragmentSolutionViewBinding.inflate(inflater, container, false)
        viewBinding.sudokuBoard.startDigits = arguments?.getIntegerArrayList("Sudoku")!!.toIntArray()
        viewBinding.sudokuBoard.solvedDigits = arguments?.getIntegerArrayList("SolvedSudoku")!!.toIntArray()
        return viewBinding.root
    }
}