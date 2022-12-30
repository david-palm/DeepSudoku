package com.example.deepsudoku

import android.content.res.AssetManager
import android.graphics.Bitmap
import android.net.Uri
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import com.example.deepsudoku.databinding.FragmentSolutionViewBinding

class SolutionViewFragment : Fragment() {
    private var _viewBinding: FragmentSolutionViewBinding? = null
    private val viewBinding: FragmentSolutionViewBinding get() = _viewBinding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _viewBinding = FragmentSolutionViewBinding.inflate(inflater, container, false)

        viewBinding.cell00.text = "5";

        return viewBinding.root
    }
}