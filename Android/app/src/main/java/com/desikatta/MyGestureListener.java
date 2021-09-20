package com.desikatta;

import android.view.GestureDetector;
import android.view.MotionEvent;

/* Overriding gestures, we want to detect */
class MyGestureListener extends GestureDetector.SimpleOnGestureListener {
    @Override
    public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY){
        if(distanceY > 10){
            storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI & 0b11110111);
            storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI | 0b00100000);
        }
        else if(distanceY < -10){
            storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI & 0b11011111);
            storage.Nil_Nil_W_A_S_D_R_DPI = (byte) (storage.Nil_Nil_W_A_S_D_R_DPI | 0b00001000);
        }
        return true;
    }
}
