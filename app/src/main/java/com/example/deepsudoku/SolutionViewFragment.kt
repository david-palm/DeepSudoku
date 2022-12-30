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
        var solvedSudoku = arguments?.getIntegerArrayList("SolvedSudoku")!!

        viewBinding.cell00.text = solvedSudoku.get(0).toString();
        viewBinding.cell01.text = solvedSudoku.get(1).toString();
        viewBinding.cell02.text = solvedSudoku.get(2).toString();
        viewBinding.cell03.text = solvedSudoku.get(3).toString();
        viewBinding.cell04.text = solvedSudoku.get(4).toString();
        viewBinding.cell05.text = solvedSudoku.get(5).toString();
        viewBinding.cell06.text = solvedSudoku.get(6).toString();
        viewBinding.cell07.text = solvedSudoku.get(7).toString();
        viewBinding.cell08.text = solvedSudoku.get(8).toString();

        viewBinding.cell10.text = solvedSudoku.get(9).toString();
        viewBinding.cell11.text = solvedSudoku.get(10).toString();
        viewBinding.cell12.text = solvedSudoku.get(11).toString();
        viewBinding.cell13.text = solvedSudoku.get(12).toString();
        viewBinding.cell14.text = solvedSudoku.get(13).toString();
        viewBinding.cell15.text = solvedSudoku.get(14).toString();
        viewBinding.cell16.text = solvedSudoku.get(15).toString();
        viewBinding.cell17.text = solvedSudoku.get(16).toString();
        viewBinding.cell18.text = solvedSudoku.get(17).toString();

        viewBinding.cell20.text = solvedSudoku.get(18).toString();
        viewBinding.cell21.text = solvedSudoku.get(19).toString();
        viewBinding.cell22.text = solvedSudoku.get(20).toString();
        viewBinding.cell23.text = solvedSudoku.get(21).toString();
        viewBinding.cell24.text = solvedSudoku.get(22).toString();
        viewBinding.cell25.text = solvedSudoku.get(23).toString();
        viewBinding.cell26.text = solvedSudoku.get(24).toString();
        viewBinding.cell27.text = solvedSudoku.get(25).toString();
        viewBinding.cell28.text = solvedSudoku.get(26).toString();


        viewBinding.cell30.text = solvedSudoku.get(27).toString();
        viewBinding.cell31.text = solvedSudoku.get(28).toString();
        viewBinding.cell32.text = solvedSudoku.get(29).toString();
        viewBinding.cell33.text = solvedSudoku.get(30).toString();
        viewBinding.cell34.text = solvedSudoku.get(31).toString();
        viewBinding.cell35.text = solvedSudoku.get(32).toString();
        viewBinding.cell36.text = solvedSudoku.get(33).toString();
        viewBinding.cell37.text = solvedSudoku.get(34).toString();
        viewBinding.cell38.text = solvedSudoku.get(35).toString();

        viewBinding.cell40.text = solvedSudoku.get(36).toString();
        viewBinding.cell41.text = solvedSudoku.get(37).toString();
        viewBinding.cell42.text = solvedSudoku.get(38).toString();
        viewBinding.cell43.text = solvedSudoku.get(39).toString();
        viewBinding.cell44.text = solvedSudoku.get(40).toString();
        viewBinding.cell45.text = solvedSudoku.get(41).toString();
        viewBinding.cell46.text = solvedSudoku.get(42).toString();
        viewBinding.cell47.text = solvedSudoku.get(43).toString();
        viewBinding.cell48.text = solvedSudoku.get(44).toString();

        viewBinding.cell50.text = solvedSudoku.get(45).toString();
        viewBinding.cell51.text = solvedSudoku.get(46).toString();
        viewBinding.cell52.text = solvedSudoku.get(47).toString();
        viewBinding.cell53.text = solvedSudoku.get(48).toString();
        viewBinding.cell54.text = solvedSudoku.get(49).toString();
        viewBinding.cell55.text = solvedSudoku.get(50).toString();
        viewBinding.cell56.text = solvedSudoku.get(51).toString();
        viewBinding.cell57.text = solvedSudoku.get(52).toString();
        viewBinding.cell58.text = solvedSudoku.get(53).toString();


        viewBinding.cell60.text = solvedSudoku.get(54).toString();
        viewBinding.cell61.text = solvedSudoku.get(55).toString();
        viewBinding.cell62.text = solvedSudoku.get(56).toString();
        viewBinding.cell63.text = solvedSudoku.get(57).toString();
        viewBinding.cell64.text = solvedSudoku.get(58).toString();
        viewBinding.cell65.text = solvedSudoku.get(59).toString();
        viewBinding.cell66.text = solvedSudoku.get(60).toString();
        viewBinding.cell67.text = solvedSudoku.get(61).toString();
        viewBinding.cell68.text = solvedSudoku.get(62).toString();

        viewBinding.cell70.text = solvedSudoku.get(63).toString();
        viewBinding.cell71.text = solvedSudoku.get(64).toString();
        viewBinding.cell72.text = solvedSudoku.get(65).toString();
        viewBinding.cell73.text = solvedSudoku.get(66).toString();
        viewBinding.cell74.text = solvedSudoku.get(67).toString();
        viewBinding.cell75.text = solvedSudoku.get(68).toString();
        viewBinding.cell76.text = solvedSudoku.get(69).toString();
        viewBinding.cell77.text = solvedSudoku.get(70).toString();
        viewBinding.cell78.text = solvedSudoku.get(71).toString();

        viewBinding.cell80.text = solvedSudoku.get(72).toString();
        viewBinding.cell81.text = solvedSudoku.get(73).toString();
        viewBinding.cell82.text = solvedSudoku.get(74).toString();
        viewBinding.cell83.text = solvedSudoku.get(75).toString();
        viewBinding.cell84.text = solvedSudoku.get(76).toString();
        viewBinding.cell85.text = solvedSudoku.get(77).toString();
        viewBinding.cell86.text = solvedSudoku.get(78).toString();
        viewBinding.cell87.text = solvedSudoku.get(79).toString();
        viewBinding.cell88.text = solvedSudoku.get(80).toString();


        return viewBinding.root
    }
}