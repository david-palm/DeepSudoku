package com.example.deepsudoku

import android.content.Context
import android.graphics.Canvas
import android.graphics.Paint
import android.graphics.Rect
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.View
import android.widget.Button
import kotlin.math.ceil
import kotlin.math.min


class SudokuBoard(context: Context, attrs: AttributeSet) : View(context, attrs) {
    private val outlineColorPaint: Paint = Paint()
    private val lineColorPaint: Paint = Paint()
    private val cellHighlightColorPaint = Paint()
    private val cellActivatedColorPaint = Paint()
    private val buttonHighlightColorPaint = Paint()
    private val digitPaint = Paint()
    private var cellSize: Int = 0
    private var selectedCell: Vec2 = Vec2(-1, -1)
    private var activatedCells: MutableList<Vec2> = mutableListOf<Vec2>()
    var button: Button = Button(context)

    var startDigits: IntArray = IntArray(81)
    var solvedDigits: IntArray = IntArray(81)

    private lateinit var displayDigits: IntArray
    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        var boardLength: Int = min(measuredWidth, measuredHeight)
        cellSize = boardLength / 9
        //Setting square view for sudoku board
        setMeasuredDimension(boardLength, boardLength)

        displayDigits = startDigits.clone()

        outlineColorPaint.color = resources.getColor(R.color.sudoku_outline)
        lineColorPaint.color = resources.getColor(R.color.sudoku_line)
        cellHighlightColorPaint.color = resources.getColor(R.color.cell_highlight_color)
        cellActivatedColorPaint.color = resources.getColor(R.color.cell_activated_color)
        buttonHighlightColorPaint.color = resources.getColor(R.color.button_highlight_color)
        digitPaint.color = resources.getColor(R.color.sudoku_outline)
        //digitPaint.typeface = resources.getFont(R.font.open_sans_bold)
    }

    override fun onDraw(canvas: Canvas?) {
        drawCells(canvas)
        drawNumbers(canvas)
        drawLines(canvas)
        drawOutline(canvas)
    }

    override fun onTouchEvent(event: MotionEvent?): Boolean {
        if(event!!.action == MotionEvent.ACTION_DOWN){
            var y = ceil((event.y / cellSize).toDouble()).toInt()
            var x = ceil((event.x / cellSize).toDouble()).toInt()

            if(displayDigits[(x -1) + (y - 1) * 9] == 0){
                selectedCell.y = y
                selectedCell.x = x
                invalidate()
                return true
            }
        }

        if(event!!.action == MotionEvent.ACTION_UP) {
            if(selectedCell.y != -1 && selectedCell.x != -1) {
                activatedCells.add(Vec2(selectedCell.x, selectedCell.y))
                var index = (selectedCell.x - 1) + (selectedCell.y - 1) * 9
                displayDigits[index] = solvedDigits[index]
                selectedCell.x = -1
                selectedCell.y = -1
                if(displayDigits.contentEquals(solvedDigits)) {
                    button.setText(R.string.hide_digits)
                }
                invalidate()
                return true
            }
        }
        return false
    }

    private fun drawNumbers(canvas: Canvas?) {
        digitPaint.textSize = 80.0f
        for(cell in 0 .. 80){
            if(displayDigits[cell] != 0) {
                var digitText = displayDigits[cell].toString()
                var digitBounds : Rect = Rect()
                digitPaint.getTextBounds(digitText, 0, digitText.length, digitBounds)
                var width: Float = digitBounds.width().toFloat()
                var height: Float = digitBounds.height().toFloat()

                var row: Int = cell / 9
                var column: Int = cell % 9
                Log.d("SudokuBoard", "Drawing digit " + cell.toString())
                canvas!!.drawText(digitText, column * cellSize + ((cellSize - width) / 2), (row * cellSize + cellSize) - ((cellSize - height) / 2), digitPaint)

            }
        }
    }

    private fun drawCells(canvas: Canvas?) {
        cellHighlightColorPaint.style = Paint.Style.FILL
        cellActivatedColorPaint.style = Paint.Style.FILL
        if(selectedCell.x != -1 && selectedCell.y != -1) {
            canvas!!.drawRect((selectedCell.x - 1) * cellSize.toFloat(), (selectedCell.y - 1) * cellSize.toFloat(),
                selectedCell.x * cellSize.toFloat(), selectedCell.y * cellSize.toFloat(), cellHighlightColorPaint)
        }

        for(activatedCell: Vec2 in activatedCells) {
            canvas!!.drawRect((activatedCell.x - 1) * cellSize.toFloat(), (activatedCell.y - 1) * cellSize.toFloat(),
                activatedCell.x * cellSize.toFloat(), activatedCell.y * cellSize.toFloat(), cellActivatedColorPaint)
        }
    }

    private fun drawOutline(canvas: Canvas?){
        outlineColorPaint.isAntiAlias = true
        outlineColorPaint.style = Paint.Style.STROKE
        outlineColorPaint.strokeWidth = 16.0f
        outlineColorPaint.isAntiAlias = true

        canvas!!.drawRect(0.0f, 0.0f, width.toFloat(), height.toFloat(), outlineColorPaint)
    }

    private fun drawLines(canvas: Canvas?){
        lineColorPaint.isAntiAlias = true
        for(column in 1 .. 9){
            if(column % 3 != 0){
                lineColorPaint.strokeWidth = 2.0f
                canvas!!.drawLine((cellSize * column).toFloat(), 0.0f, (cellSize * column).toFloat(), height.toFloat(), lineColorPaint)
            }
        }

        for(row in 1 .. 9){
            if(row % 3 != 0){
                lineColorPaint.strokeWidth = 2.0f
                canvas!!.drawLine(0.0f, (cellSize * row).toFloat(), width.toFloat(), (cellSize * row).toFloat(), lineColorPaint)
            }
        }

        for(column in 1 .. 2){
            outlineColorPaint.strokeWidth = 8.0f
            canvas!!.drawLine((cellSize * column * 3).toFloat(), 0.0f, (cellSize * column * 3).toFloat(), height.toFloat(), outlineColorPaint)
        }

        for(row in 1 .. 2){
            outlineColorPaint.strokeWidth = 8.0f
            canvas!!.drawLine(0.0f, (cellSize * row * 3).toFloat(), width.toFloat(), (cellSize * row * 3).toFloat(), outlineColorPaint)
        }
    }

    fun revealHideDigits() {
        if (!solvedDigits.contentEquals(displayDigits)) {
            revealAllDigits()
        }
        else {
            hideAllDigits()
        }
        invalidate()
    }
    fun revealAllDigits(){
        displayDigits = solvedDigits.clone()
        for(index in 0 .. 80) {
            if (startDigits[index] == 0) {
                activatedCells.add(Vec2((index % 9) + 1, index / 9 + 1))
            }
        }
        invalidate()
    }

    fun hideAllDigits(){
        Log.d("SudokuBoard", "Hiding all digits!")
        displayDigits = startDigits.clone()
        Log.d("SudokuBoard", "Hiding all digits!2")
        activatedCells = mutableListOf<Vec2>()
        invalidate()
    }

    class Vec2(x: Int, y: Int){
        public var x = x
        public var y = y
    }




}

