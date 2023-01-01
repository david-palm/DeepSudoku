package com.example.deepsudoku

import android.graphics.Bitmap
import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.NavController
import androidx.navigation.fragment.NavHostFragment
import androidx.navigation.ui.setupActionBarWithNavController
import org.opencv.android.OpenCVLoader

typealias LumaListener = (luma: Double) -> Unit

class MainActivity : AppCompatActivity(R.layout.activity_main) {
    private lateinit var navController: NavController

    // Used to load the 'deepsudoku' library on application startup.
    init {
        System.loadLibrary("deepsudoku")
        System.loadLibrary("opencv_java4");
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Retrieve NavController from the NavHostFragment
        val navHostFragment = supportFragmentManager
            .findFragmentById(R.id.nav_host_fragment) as NavHostFragment
        navController = navHostFragment.navController

        if (OpenCVLoader.initDebug()) {
            Log.d("Test", "OpenCV succesfully loaded!")
        }
        // Set up the action bar for use with the NavController
        setupActionBarWithNavController(navController)
    }

    /**
     * Handle navigation when the user chooses Up from the action bar.
     */
    override fun onSupportNavigateUp(): Boolean {
        return navController.navigateUp() || super.onSupportNavigateUp()
    }
    /**
     * A native method that is implemented by the 'deepsudoku' native library,
     * which is packaged with this application.
     */

    external fun initKerasModel() : Long


}